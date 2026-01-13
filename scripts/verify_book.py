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
    # If something under IER/ is in book-input but not a canonical chapter, flag it.
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
# Main
# -----------------------------

def main() -> None:
    ap = argparse.ArgumentParser(description="IER book verification: structure + glyph discipline.")
    ap.add_argument("manifest", help="IER-manifest.md path")
    ap.add_argument("book_input", help="build/book-input.txt path")

    ap.add_argument(
        "--skip-glyphs",
        action="store_true",
        help="Skip glyph/math-mode checks; only perform structural verification.",
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

    glyph_errors = 0
    if not args.skip_glyphs:
        glyph_errors = glyph_verify(book_input_path)

    total = structural_errors + glyph_errors
    if total:
        print(f"\nVERIFICATION FAILED: {total} issue(s) found.", file=sys.stderr)
        sys.exit(1)

    print("VERIFICATION PASSED: structure matches manifest and glyph discipline checks are clean.")
    sys.exit(0)


if __name__ == "__main__":
    main()
