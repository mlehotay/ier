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


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("manifest", help="manifest-like markdown input")
    ap.add_argument("out", help="output path list file")
    ap.add_argument(
        "--mode",
        choices=["list", "corpus"],
        default="list",
        help="list=all backticked .md entries; corpus=IER-manifest Parts I–III only",
    )
    args = ap.parse_args()

    manifest_path = Path(args.manifest).resolve()
    out_path = Path(args.out).resolve()

    if not manifest_path.exists():
        die(f"Manifest not found: {manifest_path}")

    lines = manifest_path.read_text(encoding="utf-8", errors="strict").splitlines()

    if args.mode == "corpus":
        raw = extract_corpus_parts_I_to_III(lines)
    else:
        raw = extract_generic(lines)

    ordered = dedup_preserve_order(raw)

    if not ordered:
        die("No chapter files found for selected mode.")

    out_path.parent.mkdir(parents=True, exist_ok=True)
    out_path.write_text("\n".join(ordered) + "\n", encoding="utf-8")

    numbered_path = out_path.with_suffix(".numbered.txt")
    numbered_lines = [f"{i+1:02d}\t{p}" for i, p in enumerate(ordered)]
    numbered_path.write_text("\n".join(numbered_lines) + "\n", encoding="utf-8")

    print(f"Wrote {len(ordered)} paths to {out_path}")
    print(f"Wrote numbered report to {numbered_path}")


if __name__ == "__main__":
    main()
