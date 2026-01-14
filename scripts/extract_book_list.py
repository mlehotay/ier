#!/usr/bin/env python3
from __future__ import annotations

import argparse
import re
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Dict, List, Optional

ENTRY_RE = re.compile(r"`([^`]+)`")

# Part markers: any H2 header containing the word "part" (case-insensitive)
PART_MARK_RE = re.compile(r"^##\s+.*\bpart\b", re.IGNORECASE)

# Optional: if a header includes an explicit digit after "part", we’ll use it (e.g., "PART 2")
PART_DIGIT_RE = re.compile(r"\bpart\b\s*([0-9]+)\b", re.IGNORECASE)

# Scaffold filename prefix: NN-description.md (two digits required)
SCAFFOLD_PREFIX_RE = re.compile(r"^(\d{2})-.*\.md$")


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


def dedup_preserve_order(paths: list[str]) -> list[str]:
    seen: set[str] = set()
    ordered: list[str] = []
    for p in paths:
        if p not in seen:
            seen.add(p)
            ordered.append(p)
    return ordered


def validate_paths_exist(paths: list[str], repo_root: Path) -> None:
    missing: list[str] = []
    for p in paths:
        if not (repo_root / p).exists():
            missing.append(p)
    if missing:
        msg = "Referenced Markdown files not found:\n" + "\n".join(f"  - {m}" for m in missing)
        die(msg)


@dataclass(frozen=True)
class ScaffoldFile:
    path: str          # repo-relative (preferred) or absolute (allowed)
    nn: int            # numeric prefix (00-99)
    part_index: int    # floor(nn/10)
    slot: int          # nn % 10


def parse_scaffold_file(path_str: str, repo_root: Path) -> ScaffoldFile:
    p = Path(path_str)

    # Convert to repo-relative if possible (pandoc-friendly + stable diffs)
    if p.is_absolute():
        try:
            rel = p.relative_to(repo_root)
            path_str = str(rel)
            p = Path(path_str)
        except ValueError:
            # Keep absolute if outside repo root; still valid input for pandoc
            pass

    name = p.name
    m = SCAFFOLD_PREFIX_RE.match(name)
    if not m:
        die(
            "SCAFFOLD filename must start with a two-digit prefix 'NN-' and end with .md:\n"
            f"  - {path_str}"
        )

    nn = int(m.group(1))
    part_index = nn // 10
    slot = nn % 10
    return ScaffoldFile(path=path_str, nn=nn, part_index=part_index, slot=slot)


def collect_scaffold_files(scaffold_dir: Path, repo_root: Path) -> List[ScaffoldFile]:
    if not scaffold_dir.exists():
        die(f"SCAFFOLD directory not found: {scaffold_dir}")
    if not scaffold_dir.is_dir():
        die(f"SCAFFOLD path is not a directory: {scaffold_dir}")

    files = sorted(p for p in scaffold_dir.glob("*.md") if p.is_file())
    parsed = [parse_scaffold_file(str(p), repo_root) for p in files]

    # Enforce lexicographic ordering by filename (numeric prefixes do the work)
    parsed.sort(key=lambda sf: Path(sf.path).name)
    return parsed


def next_part_index(current_part: int, header_line: str, have_seen_part_marker: bool) -> int:
    """
    When we hit a PART marker header:
      - If it contains "part <digit>", use that digit (e.g., PART 2 -> 2).
      - Otherwise:
          * if we haven't seen any part marker yet, jump to 1
          * else increment by 1
    """
    m = PART_DIGIT_RE.search(header_line)
    if m:
        return int(m.group(1))

    if not have_seen_part_marker:
        return 1
    return current_part + 1


def extract_chapters_by_part(lines: List[str]) -> Dict[int, List[str]]:
    """
    Extract all backticked .md entries.
    Group by "part" markers:
      - Anything before the first PART marker is Part 0.
      - Each subsequent PART marker advances the part index.
    """
    current_part = 0
    have_seen_part_marker = False
    out: Dict[int, List[str]] = {}

    for line in lines:
        s = line.strip()

        if PART_MARK_RE.match(s):
            current_part = next_part_index(current_part, s, have_seen_part_marker)
            have_seen_part_marker = True
            continue

        m = ENTRY_RE.search(line)
        if not m:
            continue

        p = normalize_path(m.group(1))
        if not p:
            continue

        out.setdefault(current_part, []).append(p)

    for k in list(out.keys()):
        out[k] = dedup_preserve_order(out[k])
    return out


def build_emit_list(
    scaffolds: List[ScaffoldFile],
    chapters_by_part: Dict[int, List[str]],
) -> List[str]:
    """
    Authoritative rule:

      part_index = floor(NN/10)

    For each part p:
      emit scaffolds with slot 0-5 (before)
      then chapters for p
      then scaffolds with slot 6-9 (after)

    Parts included are the union of:
      - parts present in scaffolding
      - parts present in chapter extraction
    """
    scaffolds_by_part: Dict[int, List[ScaffoldFile]] = {}
    for sf in scaffolds:
        scaffolds_by_part.setdefault(sf.part_index, []).append(sf)

    for p in list(scaffolds_by_part.keys()):
        scaffolds_by_part[p].sort(key=lambda sf: Path(sf.path).name)

    parts = sorted(set(scaffolds_by_part.keys()) | set(chapters_by_part.keys()))
    if not parts:
        return []

    emitted: List[str] = []
    for p in parts:
        sfs = scaffolds_by_part.get(p, [])
        before = [sf.path for sf in sfs if 0 <= sf.slot <= 5]
        after = [sf.path for sf in sfs if 6 <= sf.slot <= 9]

        emitted.extend(before)
        emitted.extend(chapters_by_part.get(p, []))
        emitted.extend(after)

    return dedup_preserve_order(emitted)


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("selection", help="selection markdown input (backticked .md paths)")
    ap.add_argument("scaffold_dir", help="directory containing scaffolding *.md")
    ap.add_argument("out", help="output path list file")
    args = ap.parse_args()

    # Assume script is run from repo root (Makefile does this).
    repo_root = Path.cwd().resolve()

    selection_path = Path(args.selection).resolve()
    scaffold_dir = Path(args.scaffold_dir).resolve()
    out_path = Path(args.out).resolve()

    if not selection_path.exists():
        die(f"Selection file not found: {selection_path}")

    scaffolds = collect_scaffold_files(scaffold_dir, repo_root)

    lines = selection_path.read_text(encoding="utf-8", errors="strict").splitlines()
    chapters_by_part = extract_chapters_by_part(lines)

    emitted = build_emit_list(scaffolds, chapters_by_part)
    if not emitted:
        die("No input files found (scaffold + chapters is empty).")

    # Fail fast if any referenced file doesn't exist relative to repo root.
    validate_paths_exist([p for p in emitted if not Path(p).is_absolute()], repo_root)

    out_path.parent.mkdir(parents=True, exist_ok=True)
    out_path.write_text("\n".join(emitted) + "\n", encoding="utf-8")

    numbered_path = out_path.with_suffix(".numbered.txt")
    numbered_lines = [f"{i+1:02d}\t{p}" for i, p in enumerate(emitted)]
    numbered_path.write_text("\n".join(numbered_lines) + "\n", encoding="utf-8")

    print(f"Wrote {len(emitted)} paths to {out_path}")
    print(f"Wrote numbered report to {numbered_path}")


if __name__ == "__main__":
    main()
