#!/usr/bin/env python3
from __future__ import annotations

import argparse
import re
import sys
from pathlib import Path

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


def extract_generic(lines: list[str]) -> list[str]:
    out: list[str] = []
    for line in lines:
        m = ENTRY_RE.search(line)
        if not m:
            continue
        p = normalize_path(m.group(1))
        if p:
            out.append(p)
    return out


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

    return out


def dedup_preserve_order(paths: list[str]) -> list[str]:
    seen: set[str] = set()
    ordered: list[str] = []
    for p in paths:
        if p not in seen:
            seen.add(p)
            ordered.append(p)
    return ordered


def collect_scaffold_files(scaffold_dir: Path) -> list[str]:
    if not scaffold_dir.exists():
        die(f"SCAFFOLD directory not found: {scaffold_dir}")
    if not scaffold_dir.is_dir():
        die(f"SCAFFOLD path is not a directory: {scaffold_dir}")

    # Sorted lexicographically (your numeric prefixes then do the work)
    return sorted(str(p) for p in scaffold_dir.glob("*.md") if p.is_file())


def validate_paths_exist(paths: list[str], repo_root: Path) -> None:
    missing: list[str] = []
    for p in paths:
        # p may already be relative like "IER/..." or "pub/..."
        if not (repo_root / p).exists():
            missing.append(p)
    if missing:
        msg = "Referenced Markdown files not found:\n" + "\n".join(f"  - {m}" for m in missing)
        die(msg)


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("selection", help="selection/manifest-like markdown input")
    ap.add_argument("scaffold_dir", help="directory containing scaffolding *.md")
    ap.add_argument("out", help="output path list file")
    ap.add_argument(
        "--mode",
        choices=["list", "corpus"],
        default="list",
        help="list=all backticked .md entries; corpus=IER-manifest Parts I–III only",
    )
    args = ap.parse_args()

    # Assume script is run from repo root (Makefile does this).
    repo_root = Path.cwd().resolve()

    selection_path = Path(args.selection).resolve()
    scaffold_dir = Path(args.scaffold_dir).resolve()
    out_path = Path(args.out).resolve()

    if not selection_path.exists():
        die(f"Selection file not found: {selection_path}")

    scaffold_paths = collect_scaffold_files(scaffold_dir)

    lines = selection_path.read_text(encoding="utf-8", errors="strict").splitlines()
    if args.mode == "corpus":
        chapter_paths = extract_corpus_parts_I_to_III(lines)
    else:
        chapter_paths = extract_generic(lines)

    # Combine: scaffold first, then chapters.
    combined = dedup_preserve_order(scaffold_paths + chapter_paths)

    if not combined:
        die("No input files found (scaffold + chapters is empty).")

    # Fail fast if any referenced file doesn't exist relative to repo root.
    # (Scaffold paths are absolute here; chapters are usually relative.)
    # Normalize scaffold paths to repo-relative when possible for pandoc.
    normalized: list[str] = []
    for p in combined:
        pp = Path(p)
        if pp.is_absolute():
            try:
                p = str(pp.relative_to(repo_root))
            except ValueError:
                # Absolute but not under repo root; keep absolute (still valid to pandoc)
                p = str(pp)
        normalized.append(p)

    normalized = dedup_preserve_order(normalized)

    validate_paths_exist([p for p in normalized if not Path(p).is_absolute()], repo_root)

    out_path.parent.mkdir(parents=True, exist_ok=True)
    out_path.write_text("\n".join(normalized) + "\n", encoding="utf-8")

    numbered_path = out_path.with_suffix(".numbered.txt")
    numbered_lines = [f"{i+1:02d}\t{p}" for i, p in enumerate(normalized)]
    numbered_path.write_text("\n".join(numbered_lines) + "\n", encoding="utf-8")

    print(f"Wrote {len(normalized)} paths to {out_path}")
    print(f"Wrote numbered report to {numbered_path}")


if __name__ == "__main__":
    main()
