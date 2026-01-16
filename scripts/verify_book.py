#!/usr/bin/env python3
from __future__ import annotations

import argparse
import re
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Iterable, Literal

# ------------------------------------------------------------
# Error / warning reporting
# ------------------------------------------------------------

Rule = str


@dataclass(frozen=True)
class Issue:
    path: str
    line: int
    rule: Rule
    message: str
    is_warning: bool = False

    def render(self) -> str:
        prefix = "WARN" if self.is_warning else "ERROR"
        return f"{prefix}: {self.path}:{self.line}: {self.rule}: {self.message}"


class Reporter:
    def __init__(self) -> None:
        self.issues: list[Issue] = []

    def error(self, path: str, line: int, rule: Rule, message: str) -> None:
        self.issues.append(Issue(path=path, line=line, rule=rule, message=message, is_warning=False))

    def warn(self, path: str, line: int, rule: Rule, message: str) -> None:
        self.issues.append(Issue(path=path, line=line, rule=rule, message=message, is_warning=True))

    def emit(self, summarize: bool = True) -> int:
        # Deterministic order (insertion order), no extra noise on success.
        errors = 0
        counts: dict[str, int] = {}
        for it in self.issues:
            print(it.render(), file=sys.stderr)
            if not it.is_warning:
                errors += 1
                counts[it.rule] = counts.get(it.rule, 0) + 1

        if summarize and errors:
            # compact summary by rule
            parts = ", ".join(f"{k}={v}" for k, v in sorted(counts.items()))
            print(f"ERROR: verify:0: SUMMARY: {errors} error(s) [{parts}]", file=sys.stderr)

        return 1 if errors else 0


# ------------------------------------------------------------
# Import build logic (preferred approach)
# ------------------------------------------------------------

def _import_extractor() -> object:
    scripts_dir = Path(__file__).resolve().parent
    sys.path.insert(0, str(scripts_dir))
    import extract_book_list as ebl  # type: ignore

    return ebl


# ------------------------------------------------------------
# Booklist loading + integrity
# ------------------------------------------------------------

def load_booklist_lines(booklist_path: Path) -> list[tuple[int, str]]:
    """
    Returns [(lineno, raw_line_without_newline)], including blanks (caller decides).
    """
    out: list[tuple[int, str]] = []
    for i, line in enumerate(booklist_path.read_text(encoding="utf-8", errors="strict").splitlines(), 1):
        out.append((i, line))
    return out


def is_abs_path(s: str) -> bool:
    try:
        return Path(s).is_absolute()
    except Exception:
        return False


def path_exists(repo_root: Path, p: str) -> bool:
    pp = Path(p)
    if pp.is_absolute():
        return pp.exists()
    return (repo_root / pp).exists()


def verify_booklist_integrity(
    rep: Reporter,
    repo_root: Path,
    booklist_path: Path,
) -> list[str]:
    lines = load_booklist_lines(booklist_path)

    if not lines:
        rep.error(str(booklist_path), 0, "BOOKLIST-EMPTY", "booklist is empty")
        return []

    paths: list[str] = []
    first_line_by_path: dict[str, int] = {}

    for lineno, raw in lines:
        if raw.strip() == "":
            rep.error(str(booklist_path), lineno, "BOOKLIST-BLANK", "blank/whitespace-only entry")
            continue

        p = raw  # preserve exact string; equality check requires exact match
        paths.append(p)

        if p in first_line_by_path:
            rep.error(
                str(booklist_path),
                lineno,
                "BOOKLIST-DUP",
                f"duplicate path (first at line {first_line_by_path[p]}): {p}",
            )
        else:
            first_line_by_path[p] = lineno

        if not p.endswith(".md"):
            rep.error(str(booklist_path), lineno, "BOOKLIST-EXT", f"non-.md entry: {p}")

        if " " in p:
            rep.warn(str(booklist_path), lineno, "BOOKLIST-SPACES", f"path contains spaces: {p}")

        if not path_exists(repo_root, p):
            rep.error(str(booklist_path), lineno, "BOOKLIST-MISSING", f"path does not exist: {p}")

    if not paths:
        # All lines were blank -> treat as empty list
        rep.error(str(booklist_path), 0, "BOOKLIST-EMPTY", "booklist has no usable entries")
        return []

    return paths


# ------------------------------------------------------------
# Scaffold dir inference
# ------------------------------------------------------------

def infer_scaffold_dir_from_selection(repo_root: Path, selection_path: Path) -> Path | None:
    stem = selection_path.name.lower()
    if "corpus" in stem:
        return repo_root / "pub" / "corpus-book"
    if "tldr" in stem:
        return repo_root / "pub" / "tldr-book"
    return None


def infer_scaffold_dir_from_booklist(repo_root: Path, booklist_paths: list[str]) -> Path | None:
    """
    Scan for pub/<x>/*.md. If exactly one directory is implied, return it.
    """
    dirs: set[Path] = set()
    for p in booklist_paths:
        pp = Path(p)
        if pp.is_absolute():
            continue
        parts = pp.parts
        if len(parts) >= 3 and parts[0] == "pub" and parts[-1].endswith(".md"):
            dirs.add(repo_root / parts[0] / parts[1])
    if len(dirs) == 1:
        return next(iter(dirs))
    return None


def determine_scaffold_dir(
    rep: Reporter,
    repo_root: Path,
    selection_path: Path,
    booklist_paths: list[str],
    scaffold_dir_flag: str | None,
) -> Path | None:
    if scaffold_dir_flag:
        return (repo_root / scaffold_dir_flag).resolve() if not Path(scaffold_dir_flag).is_absolute() else Path(scaffold_dir_flag).resolve()

    inferred = infer_scaffold_dir_from_selection(repo_root, selection_path)
    if inferred is not None:
        return inferred.resolve()

    inferred2 = infer_scaffold_dir_from_booklist(repo_root, booklist_paths)
    if inferred2 is not None:
        return inferred2.resolve()

    rep.error("verify", 0, "INPUT-SCAFFOLD-DIR", "cannot determine scaffold dir; pass --scaffold-dir <dir>")
    return None


# ------------------------------------------------------------
# Selection checks
# ------------------------------------------------------------

SELECTION_ENTRY_RE = re.compile(r"`([^`]+)`")
H4_OR_DEEPER_RE = re.compile(r"^####\s+(?!#)")
YAML_FM_LINE_RE = re.compile(r"^---\s*$")


def verify_selection_hygiene(
    rep: Reporter,
    repo_root: Path,
    selection_path: Path,
    extractor: object,
) -> list[str]:
    """
    Returns normalized selection paths (in appearance order, duplicates allowed).
    """
    text = selection_path.read_text(encoding="utf-8", errors="strict")
    lines = text.splitlines()

    # Disallow YAML front matter in selection
    # (detect: first non-empty line is ---)
    for i, line in enumerate(lines, 1):
        if line.strip() == "":
            continue
        if YAML_FM_LINE_RE.match(line.strip()):
            rep.error(str(selection_path), i, "SEL-YAML", "YAML front matter is not allowed in selection")
        break

    # Warn on #### or deeper headings
    for i, line in enumerate(lines, 1):
        if H4_OR_DEEPER_RE.match(line.strip()):
            rep.warn(str(selection_path), i, "SEL-H4", "found '####' or deeper heading; build ignores these markers")

    normalize_path = getattr(extractor, "normalize_path")

    normalized: list[str] = []
    for i, line in enumerate(lines, 1):
        toks = SELECTION_ENTRY_RE.findall(line)
        if not toks:
            continue
        for tok in toks:
            p = normalize_path(tok)
            if not p:
                continue
            normalized.append(p)

            # Must exist (repo-relative)
            if Path(p).is_absolute():
                if not Path(p).exists():
                    rep.error(str(selection_path), i, "SEL-MISSING", f"backticked path does not exist: {p}")
            else:
                if not (repo_root / p).exists():
                    rep.error(str(selection_path), i, "SEL-MISSING", f"backticked path does not exist: {p}")

    return normalized


# ------------------------------------------------------------
# Equivalence (recompute expected emitted list)
# ------------------------------------------------------------

def find_first_mismatch(expected: list[str], actual: list[str]) -> int | None:
    n = min(len(expected), len(actual))
    for i in range(n):
        if expected[i] != actual[i]:
            return i
    if len(expected) != len(actual):
        return n
    return None


def write_numbered_list(path: Path, items: list[str]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    lines = [f"{i+1:04d}\t{p}" for i, p in enumerate(items)]
    path.write_text("\n".join(lines) + "\n", encoding="utf-8")


def report_compact_list_diff(
    rep: Reporter,
    booklist_path: Path,
    expected: list[str],
    actual: list[str],
    mismatch_index: int,
) -> None:
    # List-level error -> line 0
    ctx = 3
    lo = max(0, mismatch_index - ctx)
    hi = min(max(len(expected), len(actual)), mismatch_index + ctx + 1)

    rep.error(
        str(booklist_path),
        0,
        "LIST-MISMATCH",
        f"first mismatch at index {mismatch_index} (0-based); expected len={len(expected)} actual len={len(actual)}",
    )

    # Add helpful adjacent context as additional list-level errors (still rule LIST-MISMATCH-CONTEXT).
    for i in range(lo, hi):
        exp = expected[i] if i < len(expected) else "<missing>"
        act = actual[i] if i < len(actual) else "<missing>"
        mark = ">>" if i == mismatch_index else "  "
        rep.error(
            str(booklist_path),
            0,
            "LIST-MISMATCH-CONTEXT",
            f"{mark} [{i:04d}] expected: {exp}",
        )
        rep.error(
            str(booklist_path),
            0,
            "LIST-MISMATCH-CONTEXT",
            f"{mark} [{i:04d}] actual:   {act}",
        )


# ------------------------------------------------------------
# Build mechanics invariants
# ------------------------------------------------------------

PART_DIV_RE = re.compile(r"^build/_part_p(\d{2})\.md$")
SECT_DIV_RE = re.compile(r"^build/_section_p(\d{2})_s(\d{2})\.md$")
BREAK_RE = re.compile(r"^build/_break_ch_(\d{4})\.md$")

H1_RE = re.compile(r"^#\s+.+\S\s*$")
H2_RE = re.compile(r"^##\s+.+\S\s*$")


def first_nonempty_line(lines: list[str]) -> tuple[int, str] | None:
    for i, line in enumerate(lines, 1):
        if line.strip():
            return i, line
    return None


def verify_generated_file_contracts(
    rep: Reporter,
    repo_root: Path,
    path_str: str,
) -> None:
    """
    Enforce §3.1 contracts on build/_*.md files.
    """
    if Path(path_str).is_absolute():
        # generated files are expected to be repo-relative; still validate if absolute
        p = Path(path_str)
    else:
        p = repo_root / path_str

    if not p.exists():
        # already caught by booklist integrity, but keep quiet here
        return

    rel = path_str

    content = p.read_text(encoding="utf-8", errors="strict")
    lines = content.splitlines()

    m_part = PART_DIV_RE.match(rel)
    if m_part:
        part = int(m_part.group(1))
        if part == 0:
            rep.error(rel, 0, "GEN-PART0", "part divider for Part 0 is forbidden (build/_part_p00.md)")
        fn = first_nonempty_line(lines)
        if fn is None:
            rep.error(rel, 1, "GEN-PART", "empty part divider file")
            return
        lineno, line = fn
        if line.strip() != r"\cleardoublepage":
            rep.error(rel, lineno, "GEN-PART", r"first non-empty line must be '\cleardoublepage'")
        h1s = [i for i, l in enumerate(lines, 1) if l.startswith("# ")]
        if len(h1s) != 1:
            rep.error(rel, 0, "GEN-PART", f"must contain exactly one H1 ('# ...'); found {len(h1s)}")
        return

    m_sect = SECT_DIV_RE.match(rel)
    if m_sect:
        fn = first_nonempty_line(lines)
        if fn is None:
            rep.error(rel, 1, "GEN-SECTION", "empty section divider file")
            return
        lineno, line = fn
        if line.strip() != r"\clearpage":
            rep.error(rel, lineno, "GEN-SECTION", r"first non-empty line must be '\clearpage'")
        h2s = [i for i, l in enumerate(lines, 1) if l.startswith("## ")]
        if len(h2s) != 1:
            rep.error(rel, 0, "GEN-SECTION", f"must contain exactly one H2 ('## ...'); found {len(h2s)}")
        return

    m_break = BREAK_RE.match(rel)
    if m_break:
        if r"\clearpage" not in content:
            rep.error(rel, 0, "GEN-BREAK", r"chapter break must contain '\clearpage'")
        return


def verify_emitted_placement_invariants(
    rep: Reporter,
    expected_emitted: list[str],
    selection_paths_norm: list[str],
) -> None:
    """
    §3.2 placement invariants.

    Important: extractor emits:
      [section_divider]*, break, chapter_path
    so we treat "section divider appears before a chapter" as:
      section_divider -> break -> selection-chapter
    """
    selection_set = set(selection_paths_norm)

    def is_break(p: str) -> bool:
        return BREAK_RE.match(p) is not None

    def is_section_div(p: str) -> bool:
        return SECT_DIV_RE.match(p) is not None

    # A) Chapter breaks precede every selection chapter path
    for i, p in enumerate(expected_emitted):
        if p in selection_set:
            if i == 0 or not is_break(expected_emitted[i - 1]):
                rep.error(
                    "booklist",
                    0,
                    "INV-CH-BREAK",
                    f"selection chapter '{p}' is not immediately preceded by a chapter break (build/_break_ch_*.md)",
                )

    # B) Section divider insertion: must be followed by break then selection chapter
    for i, p in enumerate(expected_emitted):
        if is_section_div(p):
            if i + 2 >= len(expected_emitted):
                rep.error("booklist", 0, "INV-SECTION", f"section divider '{p}' is stranded at end of list")
                continue
            p1 = expected_emitted[i + 1]
            p2 = expected_emitted[i + 2]
            if not is_break(p1) or p2 not in selection_set:
                rep.error(
                    "booklist",
                    0,
                    "INV-SECTION",
                    f"section divider '{p}' must be followed by (break, selection chapter); got ({p1}, {p2})",
                )


def segment_expected_by_part(
    extractor: object,
    scaffolds: list[object],
    stream_by_part: dict[int, list[str]],
    auto_part_divider_by_part: dict[int, str],
) -> tuple[list[int], dict[int, list[str]]]:
    """
    Reconstruct the same per-part segments used by build_emit_list, so we can
    enforce scaffold-slot discipline and part-divider policy structurally.
    """
    # ScaffoldFile has attributes: part_index, slot, path
    scaffolds_by_part: dict[int, list[object]] = {}
    for sf in scaffolds:
        scaffolds_by_part.setdefault(int(sf.part_index), []).append(sf)
    for p in list(scaffolds_by_part.keys()):
        scaffolds_by_part[p].sort(key=lambda s: Path(str(s.path)).name)

    parts = sorted(set(scaffolds_by_part.keys()) | set(stream_by_part.keys()) | set(auto_part_divider_by_part.keys()))
    segs: dict[int, list[str]] = {}

    for p in parts:
        sfs = scaffolds_by_part.get(p, [])
        before = [str(sf.path) for sf in sfs if 0 <= int(sf.slot) <= 5]
        after = [str(sf.path) for sf in sfs if 6 <= int(sf.slot) <= 9]

        chunk: list[str] = []
        chunk.extend(before)

        if p >= 1 and not before:
            pd = auto_part_divider_by_part.get(p)
            if pd:
                chunk.append(pd)

        chunk.extend(stream_by_part.get(p, []))
        chunk.extend(after)
        segs[p] = chunk

    return parts, segs


def verify_part_divider_policy_and_scaffold_slots(
    rep: Reporter,
    extractor: object,
    scaffold_dir: Path,
    scaffolds: list[object],
    parts: list[int],
    segs: dict[int, list[str]],
    stream_by_part: dict[int, list[str]],
    auto_part_divider_by_part: dict[int, str],
) -> None:
    """
    §3.2C and §4.3
    """
    # Scaffold naming rule for *included* scaffold files
    scaffold_name_re = re.compile(r"^(\d{2})-[^/]+\.md$")

    for p in parts:
        seg = segs.get(p, [])

        # Which scaffolds belong to this part?
        sfs = [sf for sf in scaffolds if int(sf.part_index) == p]
        before_sc = [str(sf.path) for sf in sfs if 0 <= int(sf.slot) <= 5]
        after_sc = [str(sf.path) for sf in sfs if 6 <= int(sf.slot) <= 9]

        # 4.2 scaffold naming
        for sf in sfs:
            nm = Path(str(sf.path)).name
            if not scaffold_name_re.match(nm):
                rep.error(str(sf.path), 0, "SCAFFOLD-NAME", "scaffold must be named NN-description.md where NN is 00–99")

        # 4.3 slot discipline (structural) within the expected segment
        # In the expected segment, "before" scaffolds must occur before any stream content;
        # "after" scaffolds must occur after all stream content.
        stream = stream_by_part.get(p, [])
        if stream:
            # find first stream index within segment
            idxs = [i for i, x in enumerate(seg) if x in set(stream)]
            if idxs:
                first_stream = min(idxs)
                last_stream = max(idxs)
                for bp in before_sc:
                    try:
                        i = seg.index(bp)
                    except ValueError:
                        continue
                    if i > first_stream:
                        rep.error("booklist", 0, "SCAFFOLD-SLOT", f"before-scaffold '{bp}' appears after stream start in part {p:02d}")
                for ap in after_sc:
                    try:
                        i = seg.index(ap)
                    except ValueError:
                        continue
                    if i < last_stream:
                        rep.error("booklist", 0, "SCAFFOLD-SLOT", f"after-scaffold '{ap}' appears before stream end in part {p:02d}")

        # 3.2C part divider insertion policy
        if p == 0:
            continue
        pd = auto_part_divider_by_part.get(p)
        if pd is None:
            # If selection had a part heading, extractor created one; if not present we don't enforce here.
            continue

        if before_sc:
            # must not appear
            if pd in seg:
                rep.error("booklist", 0, "INV-PART-DIV", f"part divider '{pd}' must not appear for part {p:02d} when before-scaffolds exist")
        else:
            # must appear before stream (if any stream)
            if stream_by_part.get(p):
                if pd not in seg:
                    rep.error("booklist", 0, "INV-PART-DIV", f"part divider '{pd}' must appear for part {p:02d} (no before-scaffolds)")
                else:
                    pd_i = seg.index(pd)
                    # if stream exists, ensure pd comes before the first stream item
                    stream_set = set(stream_by_part.get(p, []))
                    stream_idxs = [i for i, x in enumerate(seg) if x in stream_set]
                    if stream_idxs and pd_i > min(stream_idxs):
                        rep.error("booklist", 0, "INV-PART-DIV", f"part divider '{pd}' must appear before part {p:02d} stream")


# ------------------------------------------------------------
# Authoring rules (scoped)
# ------------------------------------------------------------

HEADING_RE = re.compile(r"^(#{1,6})\s+\S")
FENCE_RE = re.compile(r"^```")

INDENT_CODE_RE = re.compile(r"^(?:\t| {4,})\S")
HTML_TABLE_RE = re.compile(r"<\s*table\b", re.IGNORECASE)

# Bare URL: do not flag inside (...) or [...] immediately before url (light heuristic)
BARE_URL_RE = re.compile(r"(?<!\()(?<!\[)(https?://\S+)", re.IGNORECASE)

BAD_SPACE_RE = re.compile(r"[\u00A0\u2007\u202F\u200B\u200C\u200D\u2060]")

LATEX_BANNED = [
    r"\\documentclass\b",
    r"\\usepackage\b",
    r"\\begin\{document\}",
    r"\\end\{document\}",
    r"\\input\{",
    r"\\include\{",
    r"\\includeonly\b",
    r"\\pagestyle\b",
    r"\\thispagestyle\b",
    r"\\geometry\b",
    r"\\setlength\b",
    r"\\addtolength\b",
    r"\\linespread\b",
    r"\\fontsize\b",
    r"\\newcommand\b",
    r"\\renewcommand\b",
    r"\\def\b",
]
LATEX_BANNED_RE = re.compile("|".join(LATEX_BANNED))

# Glyph discipline (same spirit as old verifier, but scoped + rule IDs)
GLYPH_CHECKS: list[tuple[str, re.Pattern[str], str]] = [
    ("GLYPH-ARROW", re.compile(r"[\u2190-\u21FF]"), r"Use LaTeX operators (e.g. \Rightarrow, \Leftrightarrow) in math mode."),
    ("GLYPH-OP", re.compile(r"[\u2200-\u22FF]"), r"Use LaTeX operators (e.g. \neq, \subseteq, \in) in math mode."),
    ("GLYPH-CHECK", re.compile(r"[✔✘✓✗✅❌]"), "Replace with plain text (Yes/No, Has/Lacks)."),
    ("GLYPH-MINUS", re.compile(r"−"), r"Replace with '-' (ASCII) or use '-' inside LaTeX math mode."),
    ("GLYPH-MUL", re.compile(r"[×·]"), r"Use \times or \cdot in LaTeX math mode."),
    ("GLYPH-GREEK", re.compile(r"[\u0370-\u03FF]"), r"Use LaTeX Greek (e.g. \alpha) in math mode."),
]

INLINE_CODE_MATHLIKE = re.compile(
    r"`[^`]*[=<>¬≈≠≤≥→←↔⇒⇔∈∉⊂⊆⊃⊇∪∩×·÷±∑∏√∞∀∃∧∨¬][^`]*`"
)

# Detect any of the above in fenced code blocks (if present)
CODE_BLOCK_MATHLIKE = re.compile(
    r"```[\s\S]*?[=<>¬≈≠≤≥→←↔⇒⇔∈∉⊂⊆⊃⊇∪∩×·÷±∑∏√∞∀∃∧∨¬][\s\S]*?```",
    re.MULTILINE,
)


def iter_lines_with_fence_state(text: str) -> Iterable[tuple[int, str, bool]]:
    in_fence = False
    for i, line in enumerate(text.splitlines(), 1):
        if FENCE_RE.match(line.strip()):
            in_fence = not in_fence
            yield i, line, in_fence
            continue
        yield i, line, in_fence


def check_unbalanced_fences(rep: Reporter, path: str, text: str) -> None:
    fence_count = sum(1 for line in text.splitlines() if FENCE_RE.match(line.strip()))
    if fence_count % 2 == 1:
        rep.error(path, 0, "A-FENCE", "unbalanced fenced code blocks (odd number of ``` fences)")


def check_a3_yaml_front_matter(rep: Reporter, path: str, text: str) -> None:
    lines = text.splitlines()
    for i, line in enumerate(lines, 1):
        if line.strip() == "":
            continue
        if YAML_FM_LINE_RE.match(line.strip()):
            rep.error(path, i, "A3", "YAML front matter is forbidden")
        break


def check_a1_canonical_h1_first(rep: Reporter, path: str, text: str) -> None:
    lines = text.splitlines()

    first_nonempty = None
    for i, line in enumerate(lines, 1):
        if line.strip():
            first_nonempty = (i, line)
            break
    if first_nonempty is None:
        rep.error(path, 1, "A1", "empty file")
        return

    lineno, line = first_nonempty
    if not line.startswith("# "):
        rep.error(path, lineno, "A1", "first non-empty line must be exactly one H1 ('# ...')")

    # Exactly one H1 total
    h1s = [i for i, l in enumerate(lines, 1) if l.startswith("# ")]
    if len(h1s) != 1:
        rep.error(path, 0, "A1", f"must contain exactly one H1; found {len(h1s)}")


def check_a2_heading_no_skip(rep: Reporter, path: str, text: str) -> None:
    prev_level: int | None = None
    for lineno, line, in_fence in iter_lines_with_fence_state(text):
        if in_fence:
            continue
        m = HEADING_RE.match(line)
        if not m:
            continue
        level = len(m.group(1))
        if prev_level is not None and level > prev_level + 1:
            rep.error(path, lineno, "A2", f"heading level jumps from H{prev_level} to H{level}")
        prev_level = level


def check_a4_banned_latex(rep: Reporter, path: str, text: str, allow_pagebreaks_only: bool) -> None:
    """
    Canonical: ban preamble/layout directives.
    Scaffolds: allow \clearpage and \cleardoublepage; still ban preamble/layout directives.
    """
    for m in LATEX_BANNED_RE.finditer(text):
        frag = m.group(0)
        # Allow pagebreak directives if allow_pagebreaks_only; they are not in this banlist anyway.
        lineno = text.count("\n", 0, m.start()) + 1
        rep.error(path, lineno, "A4", f"banned raw LaTeX directive detected ({frag})")


def check_a5_no_indent_code(rep: Reporter, path: str, text: str) -> None:
    prev_blank = False
    for lineno, line, in_fence in iter_lines_with_fence_state(text):
        if in_fence:
            prev_blank = False
            continue

        if line.strip() == "":
            prev_blank = True
            continue

        if prev_blank and INDENT_CODE_RE.match(line):
            rep.error(path, lineno, "A5", "indent-based code block detected; use fenced code blocks (``` ... ```)")
        prev_blank = False


def check_a7_no_html_table(rep: Reporter, path: str, text: str) -> None:
    for m in HTML_TABLE_RE.finditer(text):
        lineno = text.count("\n", 0, m.start()) + 1
        rep.error(path, lineno, "A7", "HTML <table> detected; use Pandoc-compatible Markdown tables")


BareUrlsMode = Literal["error", "warn", "ignore"]


def check_a8_bare_urls(rep: Reporter, path: str, text: str, mode: BareUrlsMode) -> None:
    if mode == "ignore":
        return
    for m in BARE_URL_RE.finditer(text):
        lineno = text.count("\n", 0, m.start()) + 1
        url = m.group(1)
        msg = f"bare URL detected ({url}); prefer a Markdown link"
        if mode == "warn":
            rep.warn(path, lineno, "A8", msg)
        else:
            rep.error(path, lineno, "A8", msg)


def check_a9_bad_spaces(rep: Reporter, path: str, text: str) -> None:
    for m in BAD_SPACE_RE.finditer(text):
        lineno = text.count("\n", 0, m.start()) + 1
        ch = m.group(0)
        codepoint = f"U+{ord(ch):04X}"
        rep.error(path, lineno, "A9", f"invisible/unstable whitespace detected ({codepoint}); replace with ASCII space")


def check_glyphs(rep: Reporter, path: str, text: str) -> None:
    for rule, pat, advice in GLYPH_CHECKS:
        for m in pat.finditer(text):
            lineno = text.count("\n", 0, m.start()) + 1
            ch = m.group(0)
            rep.error(path, lineno, rule, f"detected '{ch}'. {advice}")

    for m in INLINE_CODE_MATHLIKE.finditer(text):
        lineno = text.count("\n", 0, m.start()) + 1
        rep.error(path, lineno, "GLYPH-INLINE-CODE", "math-like glyphs inside inline code; use LaTeX math mode")

    for m in CODE_BLOCK_MATHLIKE.finditer(text):
        lineno = text.count("\n", 0, m.start()) + 1
        rep.error(path, lineno, "GLYPH-FENCE", "math-like glyphs inside fenced code block; use math mode or prose")


def classify_path(repo_root: Path, p: str, scaffold_dir: Path | None) -> Literal["generated", "canonical", "scaffold", "other"]:
    if p.startswith("build/_") and p.endswith(".md"):
        return "generated"
    if p.startswith("IER/IER-") and p.endswith(".md"):
        return "canonical"
    if scaffold_dir is not None:
        # Compare normalized absolute path if possible
        try:
            pp = (repo_root / p).resolve() if not Path(p).is_absolute() else Path(p).resolve()
            if scaffold_dir in pp.parents:
                return "scaffold"
        except Exception:
            pass
    return "other"


def verify_authoring(
    rep: Reporter,
    repo_root: Path,
    booklist_paths: list[str],
    scaffold_dir: Path | None,
    scope: Literal["canonical", "all"],
    bare_urls_mode: BareUrlsMode,
    skip_glyphs: bool,
) -> None:
    for p in booklist_paths:
        if not p.endswith(".md"):
            continue

        klass = classify_path(repo_root, p, scaffold_dir)

        # Scope gate
        if scope == "canonical" and klass != "canonical":
            continue

        # Generated files are validated by §3.1 only; do not apply authoring blanket rules
        if klass == "generated":
            continue

        # Resolve path
        fs_path = Path(p) if Path(p).is_absolute() else (repo_root / p)
        if not fs_path.exists():
            continue
        text = fs_path.read_text(encoding="utf-8", errors="replace")

        # Always enforce fence balance as a stability rule (low false positives)
        check_unbalanced_fences(rep, p, text)

        # Rule sets
        if klass == "canonical":
            check_a3_yaml_front_matter(rep, p, text)
            check_a1_canonical_h1_first(rep, p, text)
            check_a2_heading_no_skip(rep, p, text)
            check_a4_banned_latex(rep, p, text, allow_pagebreaks_only=False)
            check_a5_no_indent_code(rep, p, text)
            check_a7_no_html_table(rep, p, text)
            check_a8_bare_urls(rep, p, text, bare_urls_mode)
            check_a9_bad_spaces(rep, p, text)
            if not skip_glyphs:
                check_glyphs(rep, p, text)

        elif klass == "scaffold":
            # Partial strict: A3, A5, A7, A9; allow \clearpage/\cleardoublepage (not banned here)
            check_a3_yaml_front_matter(rep, p, text)
            # Optional A2 if headings present: just run it (low cost)
            check_a2_heading_no_skip(rep, p, text)
            check_a4_banned_latex(rep, p, text, allow_pagebreaks_only=True)
            check_a5_no_indent_code(rep, p, text)
            check_a7_no_html_table(rep, p, text)
            check_a9_bad_spaces(rep, p, text)
            # Glyph discipline is not required for scaffolds by spec; only do if scope==all AND user didn't skip
            if scope == "all" and not skip_glyphs:
                check_glyphs(rep, p, text)

        else:
            # "other": apply scaffold-like stability rules when scope=all
            check_a3_yaml_front_matter(rep, p, text)
            check_a5_no_indent_code(rep, p, text)
            check_a7_no_html_table(rep, p, text)
            check_a9_bad_spaces(rep, p, text)
            check_a8_bare_urls(rep, p, text, bare_urls_mode)
            if scope == "all" and not skip_glyphs:
                check_glyphs(rep, p, text)


# ------------------------------------------------------------
# Main
# ------------------------------------------------------------

def main() -> None:
    ap = argparse.ArgumentParser(
        description="IER book verifier: booklist integrity, build equivalence, invariants, and scoped authoring discipline."
    )
    ap.add_argument("selection", help="selection markdown input (backticked .md paths)")
    ap.add_argument("booklist", help="emitted booklist.txt produced by extract_book_list.py")

    ap.add_argument("--skip-glyphs", action="store_true", help="Skip glyph/maths unicode discipline checks.")
    ap.add_argument("--skip-authoring", action="store_true", help="Skip authoring checks (A-rules).")
    ap.add_argument(
        "--skip-structure",
        action="store_true",
        help="Skip structure checks (equivalence + invariants + scaffold structural discipline); run authoring only.",
    )
    ap.add_argument(
        "--scope",
        choices=["canonical", "all"],
        default="canonical",
        help="Scope for authoring checks (default: canonical).",
    )
    ap.add_argument(
        "--bare-urls",
        choices=["error", "warn", "ignore"],
        default="error",
        help="How to treat bare URLs (default: error).",
    )
    ap.add_argument("--scaffold-dir", default=None, help="Explicit scaffold directory (recommended).")
    ap.add_argument("--explain", action="store_true", help="Print computed summaries (counts).")
    ap.add_argument(
        "--diff-full",
        action="store_true",
        help="Write full numbered expected/actual lists under build/ on list mismatch.",
    )

    args = ap.parse_args()

    repo_root = Path.cwd().resolve()
    selection_path = Path(args.selection).resolve()
    booklist_path = Path(args.booklist).resolve()

    rep = Reporter()

    if not selection_path.exists():
        rep.error(str(selection_path), 0, "INPUT", "selection file not found")
        sys.exit(rep.emit())

    if not booklist_path.exists():
        rep.error(str(booklist_path), 0, "INPUT", "booklist file not found")
        sys.exit(rep.emit())

    extractor = _import_extractor()

    # 1) Booklist integrity (always-on)
    booklist_paths = verify_booklist_integrity(rep, repo_root, booklist_path)

    # 6) Selection hygiene (always-on)
    selection_paths_norm = verify_selection_hygiene(rep, repo_root, selection_path, extractor)

    # Determine scaffold dir early (needed for structure + scaffold authoring classification)
    scaffold_dir: Path | None = None
    if not args.skip_structure or (not args.skip_authoring and args.scope == "all"):
        scaffold_dir = determine_scaffold_dir(
            rep,
            repo_root=repo_root,
            selection_path=selection_path,
            booklist_paths=booklist_paths,
            scaffold_dir_flag=args.scaffold_dir,
        )

    # If integrity already yielded no usable list, stop early
    if not booklist_paths:
        sys.exit(rep.emit())

    expected_emitted: list[str] | None = None
    stream_by_part: dict[int, list[str]] | None = None
    auto_part_divider_by_part: dict[int, str] | None = None
    scaffolds: list[object] | None = None

    # 2) Recompute expected list and require exact equality (unless skip-structure)
    if not args.skip_structure and scaffold_dir is not None:
        collect_scaffold_files = getattr(extractor, "collect_scaffold_files")
        extract_stream_and_dividers = getattr(extractor, "extract_stream_and_dividers")
        build_emit_list = getattr(extractor, "build_emit_list")

        scaffolds = collect_scaffold_files(scaffold_dir, repo_root)
        sel_lines = selection_path.read_text(encoding="utf-8", errors="strict").splitlines()
        stream_by_part, auto_part_divider_by_part, _part_has = extract_stream_and_dividers(
            sel_lines,
            repo_root=repo_root,
            build_dir_rel="build",
        )
        expected_emitted = build_emit_list(scaffolds, stream_by_part, auto_part_divider_by_part)

        # Exact equality
        mismatch = find_first_mismatch(expected_emitted, booklist_paths)
        if mismatch is not None:
            report_compact_list_diff(rep, booklist_path, expected_emitted, booklist_paths, mismatch)
            if args.diff_full:
                build_dir = repo_root / "build"
                stem = selection_path.stem
                write_numbered_list(build_dir / f"{stem}-verify.expected.numbered.txt", expected_emitted)
                write_numbered_list(build_dir / f"{stem}-verify.actual.numbered.txt", booklist_paths)

    # 3) Build-mechanics invariants (generated files + placement + scaffold structure)
    if not args.skip_structure:
        # 3.1 generated structural files contracts (on actual list)
        for p in booklist_paths:
            if p.startswith("build/_") and p.endswith(".md"):
                verify_generated_file_contracts(rep, repo_root, p)

        # 3.2 placement invariants + scaffold slot discipline: only meaningful if we computed expected list
        if expected_emitted is not None and scaffolds is not None and stream_by_part is not None and auto_part_divider_by_part is not None and scaffold_dir is not None:
            verify_emitted_placement_invariants(rep, expected_emitted, selection_paths_norm)

            parts, segs = segment_expected_by_part(extractor, scaffolds, stream_by_part, auto_part_divider_by_part)
            verify_part_divider_policy_and_scaffold_slots(
                rep,
                extractor=extractor,
                scaffold_dir=scaffold_dir,
                scaffolds=scaffolds,
                parts=parts,
                segs=segs,
                stream_by_part=stream_by_part,
                auto_part_divider_by_part=auto_part_divider_by_part,
            )

            if args.explain:
                # Low-noise summary (goes to stderr so it doesn't pollute success stdout)
                total_chapters = sum(1 for p in expected_emitted if p in set(selection_paths_norm))
                total_breaks = sum(1 for p in expected_emitted if BREAK_RE.match(p) is not None)
                total_sections = sum(1 for p in expected_emitted if SECT_DIV_RE.match(p) is not None)
                total_parts = sum(1 for p in expected_emitted if PART_DIV_RE.match(p) is not None)
                total_scaffolds = sum(1 for p in expected_emitted if (not p.startswith("build/_")) and scaffold_dir in ((repo_root / p).resolve().parents if not Path(p).is_absolute() else Path(p).resolve().parents) if scaffold_dir is not None)
                rep.warn("verify", 0, "EXPLAIN", f"expected: parts={total_parts} sections={total_sections} breaks={total_breaks} chapters={total_chapters} scaffolds~={total_scaffolds}")

    # 5) Authoring rules (scoped)
    if not args.skip_authoring:
        verify_authoring(
            rep,
            repo_root=repo_root,
            booklist_paths=booklist_paths,
            scaffold_dir=scaffold_dir,
            scope=args.scope,
            bare_urls_mode=args.bare_urls,
            skip_glyphs=args.skip_glyphs,
        )

    exit_code = rep.emit()
    if exit_code == 0:
        # Low-noise on success
        sys.exit(0)
    sys.exit(1)


if __name__ == "__main__":
    main()
