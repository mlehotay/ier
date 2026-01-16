#!/usr/bin/env python3
from __future__ import annotations

import argparse
import re
import sys
from pathlib import Path


# -----------------------------
# Manifest parsing (matches your extraction style)
# -----------------------------

ENTRY_RE = re.compile(r"`([^`]+)`")

PART_I_RE = re.compile(r"^##\s+\*\*PART I\b")
PART_IV_RE = re.compile(r"^##\s+\*\*PART IV\b")


def die(msg: str) -> None:
    print(f"ERROR: {msg}", file=sys.stderr)
    sys.exit(1)


def normalize_path(token: str) -> str | None:
    token = token.strip()
    if not token.endswith(".md"):
        return None
    # Map bare canonical filenames to IER/
    if "/" not in token and token.startswith("IER-"):
        return f"IER/{token}"
    return token


def extract_corpus_parts_I_to_III(lines: list[str]) -> list[str]:
    in_book = False
    out: list[str] = []
    for line in lines:
        s = line.strip()

        if PART_I_RE.match(s):
            in_book = True
            continue

        if PART_IV_RE.match(s):
            in_book = False

        if not in_book:
            continue

        m = ENTRY_RE.search(line)
        if not m:
            continue

        p = normalize_path(m.group(1))
        # Only canonical chapter files for Parts I–III are IER/IER-*.md
        if p and p.startswith("IER/IER-") and p.endswith(".md"):
            out.append(p)

    return dedup_preserve_order(out)


def dedup_preserve_order(paths: list[str]) -> list[str]:
    seen: set[str] = set()
    ordered: list[str] = []
    for p in paths:
        if p not in seen:
            seen.add(p)
            ordered.append(p)
    return ordered


# -----------------------------
# Book-input loading
# -----------------------------

def load_book_input(booklist_path: Path) -> list[str]:
    raw: list[str] = []
    for line in booklist_path.read_text(encoding="utf-8", errors="strict").splitlines():
        s = line.strip()
        if not s:
            continue
        raw.append(s)
    return raw


# -----------------------------
# Structural Verification
# -----------------------------

def is_chapter_path(p: str) -> bool:
    return p.startswith("IER/IER-") and p.endswith(".md")


def find_preface_index(paths: list[str], pattern: re.Pattern[str]) -> int | None:
    for i, p in enumerate(paths):
        if pattern.search(p):
            return i
    return None


def structural_verify(
    manifest_path: Path,
    book_input_path: Path,
    require_preface: bool,
    preface_regex: str,
    allow_frontmatter: bool,
) -> int:
    """
    Returns number of structural errors found.
    """
    errors = 0

    manifest_lines = manifest_path.read_text(encoding="utf-8", errors="strict").splitlines()
    expected_chapters = extract_corpus_parts_I_to_III(manifest_lines)

    if not expected_chapters:
        print("ERROR: Could not extract any Parts I–III chapters from manifest.", file=sys.stderr)
        return 1

    actual_paths = load_book_input(book_input_path)

    if not actual_paths:
        print("ERROR: book-input.txt is empty.", file=sys.stderr)
        return 1

    # 1) Missing files (existence check)
    for p in actual_paths:
        if not Path(p).exists():
            print(f"ERROR: Listed file does not exist: {p}", file=sys.stderr)
            errors += 1

    # 2) Verify chapter subset against manifest, in order, with no extras
    actual_chapters = [p for p in actual_paths if is_chapter_path(p)]

    exp_set = set(expected_chapters)
    act_set = set(actual_chapters)

    missing = [p for p in expected_chapters if p not in act_set]
    extra = [p for p in actual_chapters if p not in exp_set]

    if missing:
        print("ERROR: Missing canonical chapter(s) from book list:", file=sys.stderr)
        for p in missing:
            print(f"  - {p}", file=sys.stderr)
        errors += len(missing)

    if extra:
        print("ERROR: Unexpected chapter(s) in book list (not in manifest Parts I–III):", file=sys.stderr)
        for p in extra:
            print(f"  - {p}", file=sys.stderr)
        errors += len(extra)

    # Order check: actual chapter sequence must equal expected sequence (after filtering)
    if actual_chapters != expected_chapters:
        print("ERROR: Chapter ordering mismatch against manifest Parts I–III.", file=sys.stderr)
        print("  Expected:", file=sys.stderr)
        for i, p in enumerate(expected_chapters, 1):
            print(f"    {i:02d}  {p}", file=sys.stderr)
        print("  Actual:", file=sys.stderr)
        for i, p in enumerate(actual_chapters, 1):
            print(f"    {i:02d}  {p}", file=sys.stderr)
        errors += 1

    # 3) Non-book file included (strictly for IER/ paths)
    if allow_frontmatter:
        # allow frontmatter from elsewhere (pub/ etc.)
        pass

    for p in actual_paths:
        if p.startswith("IER/") and not is_chapter_path(p):
            print(f"ERROR: Non-book IER/ file included in book list: {p}", file=sys.stderr)
            errors += 1

    # 4) Preface presence / placement (strict but configurable)
    if require_preface:
        preface_pat = re.compile(preface_regex, re.IGNORECASE)
        idx_preface = find_preface_index(actual_paths, preface_pat)

        if idx_preface is None:
            print(
                f"ERROR: Preface missing. Expected a path matching /{preface_regex}/ in book list.",
                file=sys.stderr,
            )
            errors += 1
        else:
            # must appear before first chapter
            try:
                first_chapter_idx = next(i for i, p in enumerate(actual_paths) if is_chapter_path(p))
            except StopIteration:
                first_chapter_idx = None

            if first_chapter_idx is not None and idx_preface > first_chapter_idx:
                print(
                    f"ERROR: Preface appears after first chapter ({actual_paths[first_chapter_idx]}).",
                    file=sys.stderr,
                )
                errors += 1

    return errors


# -----------------------------
# Glyph / math-mode verification
# -----------------------------

CHECKS = [
    (
        "Unicode arrows",
        re.compile(r"[\u2190-\u21FF]"),
        r"Use LaTeX operators (e.g. \Rightarrow, \Leftrightarrow) in math mode.",
    ),
    (
        "Unicode math operators",
        re.compile(r"[\u2200-\u22FF]"),
        r"Use LaTeX operators (e.g. \neq, \subseteq, \in) in math mode.",
    ),
    (
        "Checkmarks / X symbols",
        re.compile(r"[✔✘✓✗✅❌]"),
        "Replace with plain text (Yes/No, Has/Lacks).",
    ),
    (
        "Unicode minus sign",
        re.compile(r"−"),
        r"Replace with '-' (ASCII) or use '-' inside LaTeX math mode.",
    ),
    (
        "Unicode multiplication symbols",
        re.compile(r"[×·]"),
        r"Use \times or \cdot in LaTeX math mode.",
    ),
    (
        "Greek letters (Unicode)",
        re.compile(r"[\u0370-\u03FF]"),
        r"Use LaTeX Greek (e.g. \alpha) in math mode.",
    ),
]

INLINE_CODE_MATHLIKE = re.compile(
    r"`[^`]*[=<>¬≈≠≤≥→←↔⇒⇔∈∉⊂⊆⊃⊇∪∩×·÷±∑∏√∞∀∃∧∨¬][^`]*`"
)

CODE_BLOCK_MATHLIKE = re.compile(
    r"```[\s\S]*?[=<>¬≈≠≤≥→←↔⇒⇔∈∉⊂⊆⊃⊇∪∩×·÷±∑∏√∞∀∃∧∨¬][\s\S]*?```",
    re.MULTILINE,
)


def glyph_verify(book_input_path: Path) -> int:
    """
    Returns number of glyph/misuse errors found.

    Quiet mode: reports only file:line + rule + offending character (no Markdown excerpts).
    """
    errors = 0
    paths = load_book_input(book_input_path)

    for p in paths:
        path = Path(p)
        if not path.exists():
            # Structural validator already reports this; avoid duplicate noise.
            continue

        text = path.read_text(encoding="utf-8", errors="replace")

        # Single-character / regex checks (quiet: no excerpt)
        for label, pattern, advice in CHECKS:
            for m in pattern.finditer(text):
                lineno = text.count("\n", 0, m.start()) + 1
                char = m.group(0)
                print(
                    f"ERROR: {p}:{lineno}: {label} detected ({char}). {advice}",
                    file=sys.stderr,
                )
                errors += 1

        # Inline code containing math-like glyphs (quiet: no content dump)
        for m in INLINE_CODE_MATHLIKE.finditer(text):
            lineno = text.count("\n", 0, m.start()) + 1
            print(
                f"ERROR: {p}:{lineno}: Math-like content inside inline code. "
                "Use LaTeX math mode instead of backticks.",
                file=sys.stderr,
            )
            errors += 1

        # Fenced code blocks containing math-like glyphs (already quiet)
        for m in CODE_BLOCK_MATHLIKE.finditer(text):
            lineno = text.count("\n", 0, m.start()) + 1
            print(
                f"ERROR: {p}:{lineno}: Math-like content inside fenced code block. "
                "Use display math ($$ ... $$) or prose.",
                file=sys.stderr,
            )
            errors += 1

    return errors


# -----------------------------
# Authoring + Build-rule verification
# -----------------------------

# A1/A2 heading discipline
HEADING_RE = re.compile(r"^(#{1,6})\s+\S")

# A3: YAML front matter (disallowed in corpus chapters)
YAML_FM_LINE = re.compile(r"^---\s*$")

# A4/A10: raw LaTeX that implies preamble/layout/doc-structure (banlist)
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

# A5: indent-code blocks are disallowed (heuristic outside fences)
FENCE_RE = re.compile(r"^```")
INDENT_CODE_RE = re.compile(r"^(?:\t| {4,})\S")

# A7: HTML tables disallowed
HTML_TABLE_RE = re.compile(r"<\s*table\b", re.IGNORECASE)

# A8: bare URLs discouraged (treat as error unless allowed)
BARE_URL_RE = re.compile(r"(?<!\()(?<!\[)(https?://\S+)", re.IGNORECASE)

# A9: invisible unicode spaces that break determinism
BAD_SPACE_RE = re.compile(r"[\u00A0\u2007\u202F\u200B\u200C\u200D\u2060]")

# Build: divider page naming
DIVIDER_RE = re.compile(r"^build/_section_p\d{2}_s\d{2}[^/]*\.md$")

# Build: scaffold numbering (for known scaffold roots)
SCAFFOLD_ROOTS = ("pub/corpus-book/", "pub/tldr-book/", "pub/paper/")
SCAFFOLD_NAME_RE = re.compile(r"^\d{2}-[^/]+\.md$")


def _iter_lines_with_state(text: str):
    """
    Yields (lineno, line, in_fence) as we scan, toggling in/out on ``` fences.
    """
    in_fence = False
    for i, line in enumerate(text.splitlines(), 1):
        if FENCE_RE.match(line.strip()):
            in_fence = not in_fence
            yield i, line, in_fence
            continue
        yield i, line, in_fence


def _check_a1_one_h1_and_first(text: str, p: str) -> int:
    errors = 0
    lines = text.splitlines()

    # Find first non-empty line
    first_nonempty_idx = None
    for i, line in enumerate(lines):
        if line.strip():
            first_nonempty_idx = i
            break

    if first_nonempty_idx is None:
        print(f"ERROR: {p}:1: Empty file.", file=sys.stderr)
        return 1

    first = lines[first_nonempty_idx].rstrip("\n")

    # YAML front matter disallowed especially because A1 requires H1 first
    if YAML_FM_LINE.match(first.strip()):
        print(f"ERROR: {p}:{first_nonempty_idx+1}: YAML front matter detected. First non-empty line must be an H1.", file=sys.stderr)
        errors += 1

    # Must be exactly one H1 title at top
    if not first.startswith("# "):
        print(f"ERROR: {p}:{first_nonempty_idx+1}: A1 violation: first non-empty line must be an H1 ('# ...').", file=sys.stderr)
        errors += 1

    # No other H1 headings permitted
    for i, line in enumerate(lines, 1):
        if line.startswith("# "):
            if i != (first_nonempty_idx + 1):
                print(f"ERROR: {p}:{i}: A1 violation: multiple H1 headings (only one permitted).", file=sys.stderr)
                errors += 1
    return errors


def _check_a2_heading_no_skip(text: str, p: str) -> int:
    """
    Enforce: heading levels must not skip by more than 1 when moving downward.
    (Retreats can be any amount; only 'advance' is constrained.)
    """
    errors = 0
    prev_level: int | None = None
    for lineno, line, in_fence in _iter_lines_with_state(text):
        if in_fence:
            continue
        m = HEADING_RE.match(line)
        if not m:
            continue
        level = len(m.group(1))
        if prev_level is not None and level > prev_level + 1:
            print(
                f"ERROR: {p}:{lineno}: A2 violation: heading level jumps from H{prev_level} to H{level}.",
                file=sys.stderr,
            )
            errors += 1
        prev_level = level
    return errors


def _check_a4_a10_banned_latex(text: str, p: str) -> int:
    errors = 0
    for m in LATEX_BANNED_RE.finditer(text):
        lineno = text.count("\n", 0, m.start()) + 1
        frag = m.group(0)
        print(
            f"ERROR: {p}:{lineno}: A4/A10 violation: banned raw LaTeX directive detected ({frag}).",
            file=sys.stderr,
        )
        errors += 1
    return errors


def _check_a5_no_indent_code(text: str, p: str) -> int:
    errors = 0
    prev_blank = False
    for lineno, line, in_fence in _iter_lines_with_state(text):
        if in_fence:
            prev_blank = False
            continue

        if not line.strip():
            prev_blank = True
            continue

        # Heuristic: lines that "look like" indented code blocks.
        # We only flag when preceded by a blank line to reduce false positives in lists/quotes.
        if prev_blank and INDENT_CODE_RE.match(line):
            print(
                f"ERROR: {p}:{lineno}: A5 violation: indent-based code block detected. Use fenced code blocks (``` ... ```).",
                file=sys.stderr,
            )
            errors += 1

        prev_blank = False
    return errors


def _check_a7_no_html_tables(text: str, p: str) -> int:
    errors = 0
    for m in HTML_TABLE_RE.finditer(text):
        lineno = text.count("\n", 0, m.start()) + 1
        print(
            f"ERROR: {p}:{lineno}: A7 violation: HTML <table> detected. Use Pandoc-compatible Markdown tables.",
            file=sys.stderr,
        )
        errors += 1
    return errors


def _check_a8_no_bare_urls(text: str, p: str, allow_bare_urls: bool) -> int:
    if allow_bare_urls:
        return 0
    errors = 0
    for m in BARE_URL_RE.finditer(text):
        lineno = text.count("\n", 0, m.start()) + 1
        url = m.group(1)
        print(
            f"ERROR: {p}:{lineno}: A8 violation: bare URL detected ({url}). Prefer a Markdown link or a reference.",
            file=sys.stderr,
        )
        errors += 1
    return errors


def _check_a9_bad_spaces(text: str, p: str) -> int:
    errors = 0
    for m in BAD_SPACE_RE.finditer(text):
        lineno = text.count("\n", 0, m.start()) + 1
        ch = m.group(0)
        codepoint = f"U+{ord(ch):04X}"
        print(
            f"ERROR: {p}:{lineno}: A9 violation: invisible/unstable whitespace detected ({codepoint}). Replace with ASCII space.",
            file=sys.stderr,
        )
        errors += 1
    return errors


def _check_build_paths(paths: list[str]) -> int:
    """
    Build-system checks we can enforce from book-input.txt alone.
    """
    errors = 0
    for p in paths:
        # Divider pages must match deterministic naming
        if p.startswith("build/_section_"):
            if not DIVIDER_RE.match(p):
                print(
                    f"ERROR: {p}: Build violation: section divider page name must match build/_section_pPP_sSS*.md",
                    file=sys.stderr,
                )
                errors += 1

        # Scaffold files in known scaffold roots must be NN-*.md
        for root in SCAFFOLD_ROOTS:
            if p.startswith(root):
                name = Path(p).name
                if not SCAFFOLD_NAME_RE.match(name):
                    print(
                        f"ERROR: {p}: Build violation: SCAFFOLD file must be named NN-description.md (two-digit prefix).",
                        file=sys.stderr,
                    )
                    errors += 1
                break

    return errors


def authoring_verify(book_input_path: Path, allow_bare_urls: bool, scope: str) -> int:
    """
    Returns number of authoring/build-rule errors found.

    scope:
      - "all": check all .md files in book-input
      - "canonical": check only IER/IER-*.md
    """
    errors = 0
    paths = load_book_input(book_input_path)

    errors += _check_build_paths(paths)

    for p in paths:
        if not p.endswith(".md"):
            continue

        if scope == "canonical" and not is_chapter_path(p):
            continue

        path = Path(p)
        if not path.exists():
            continue

        text = path.read_text(encoding="utf-8", errors="replace")

        errors += _check_a1_one_h1_and_first(text, p)
        errors += _check_a2_heading_no_skip(text, p)
        errors += _check_a4_a10_banned_latex(text, p)
        errors += _check_a5_no_indent_code(text, p)
        errors += _check_a7_no_html_tables(text, p)
        errors += _check_a8_no_bare_urls(text, p, allow_bare_urls=allow_bare_urls)
        errors += _check_a9_bad_spaces(text, p)

    return errors


# -----------------------------
# Main
# -----------------------------

def main() -> None:
    ap = argparse.ArgumentParser(description="IER book verification: structure + glyph discipline + authoring/build rules.")
    ap.add_argument("manifest", help="IER-manifest.md path")
    ap.add_argument("book_input", help="build/book-input.txt path")

    ap.add_argument(
        "--skip-glyphs",
        action="store_true",
        help="Skip glyph/math-mode checks; only perform structural/authoring verification.",
    )
    ap.add_argument(
        "--skip-authoring",
        action="store_true",
        help="Skip canon authoring/build-rule checks; only perform structural (+ optional glyph) verification.",
    )
    ap.add_argument(
        "--authoring-scope",
        choices=["all", "canonical"],
        default="all",
        help="Which files to apply authoring checks to (default: all).",
    )
    ap.add_argument(
        "--allow-bare-urls",
        action="store_true",
        help="Do not error on bare URLs in prose (A8).",
    )
    ap.add_argument(
        "--no-preface",
        action="store_true",
        help="Do not require a Preface entry in the book list.",
    )
    ap.add_argument(
        "--preface-regex",
        default=r"(preface|IER-paper)",
        help=r"Regex used to locate the Preface entry in book-input (default: '(preface|IER-paper)').",
    )

    args = ap.parse_args()

    manifest_path = Path(args.manifest).resolve()
    book_input_path = Path(args.book_input).resolve()

    if not manifest_path.exists():
        die(f"Manifest not found: {manifest_path}")
    if not book_input_path.exists():
        die(f"Book input list not found: {book_input_path}")

    structural_errors = structural_verify(
        manifest_path=manifest_path,
        book_input_path=book_input_path,
        require_preface=(not args.no_preface),
        preface_regex=args.preface_regex,
        allow_frontmatter=True,
    )

    authoring_errors = 0
    if not args.skip_authoring:
        authoring_errors = authoring_verify(
            book_input_path=book_input_path,
            allow_bare_urls=args.allow_bare_urls,
            scope=args.authoring_scope,
        )

    glyph_errors = 0
    if not args.skip_glyphs:
        glyph_errors = glyph_verify(book_input_path)

    total = structural_errors + authoring_errors + glyph_errors
    if total:
        print(f"\nVERIFICATION FAILED: {total} issue(s) found.", file=sys.stderr)
        sys.exit(1)

    print("VERIFICATION PASSED: structure matches manifest; authoring/build rules and glyph discipline are clean.")
    sys.exit(0)


if __name__ == "__main__":
    main()
