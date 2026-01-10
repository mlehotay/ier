#!/usr/bin/env python3
from __future__ import annotations

import re
import sys
from pathlib import Path


# Matches a manifest entry like:
# 12. **[T2] `IER-panic.md`** — ...
# 69. **`IER-paper/IER-paper.md`** — ...
ENTRY_RE = re.compile(r"`([^`]+)`")


def die(msg: str) -> None:
    print(f"ERROR: {msg}", file=sys.stderr)
    sys.exit(1)


def main() -> None:
    if len(sys.argv) != 3:
        die("Usage: extract_book_list.py <IER-manifest.md> <out/book-input.txt>")

    manifest_path = Path(sys.argv[1]).resolve()
    out_path = Path(sys.argv[2]).resolve()

    if not manifest_path.exists():
        die(f"Manifest not found: {manifest_path}")

    text = manifest_path.read_text(encoding="utf-8", errors="strict").splitlines()

    in_book = False
    files: list[str] = []

    for line in text:
        # Enter the book chapter region
        if line.strip().startswith("## **PART I"):
            in_book = True

        # Stop once we hit non-book sections
        if line.strip().startswith("## **PART IV"):
            in_book = False

        if not in_book:
            continue

        # Extract any backticked path
        m = ENTRY_RE.search(line)
        if not m:
            continue

        path = m.group(1).strip()

        # We only want the actual chapter files (Parts I–III are in IER/)
        # Manifest currently lists them as `IER-foo.md` (no directory), so map to IER/.
        if path.startswith("IER-") and path.endswith(".md"):
            files.append(f"../IER/{path}")

    if not files:
        die("No chapter files found (expected backticked `IER-*.md` entries in Parts I–III).")

    # Optional: prepend preface (you can remove this if you want “manifest only”)
    preface = "../IER-paper/IER-paper.md"

    # De-dup while preserving order (defensive)
    seen: set[str] = set()
    ordered: list[str] = []
    for p in [preface, *files]:
        if p not in seen:
            seen.add(p)
            ordered.append(p)

    out_path.parent.mkdir(parents=True, exist_ok=True)
    out_path.write_text("\n".join(ordered) + "\n", encoding="utf-8")

    # Optional: write a numbered report for humans (not used by pandoc)
    numbered_path = out_path.with_suffix(".numbered.txt")
    numbered_lines = []
    chap_n = 0
    for p in ordered:
        if p == preface:
            numbered_lines.append(f"Preface\t{p}")
        else:
            chap_n += 1
            numbered_lines.append(f"{chap_n:02d}\t{p}")
    numbered_path.write_text("\n".join(numbered_lines) + "\n", encoding="utf-8")

    print(f"Wrote {len(ordered)} paths to {out_path}")
    print(f"Wrote numbered report to {numbered_path}")


if __name__ == "__main__":
    main()
