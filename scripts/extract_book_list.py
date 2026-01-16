#!/usr/bin/env python3
from __future__ import annotations

import argparse
import re
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Dict, List, Tuple

# Backticked tokens; accept multiple per line.
ENTRY_RE = re.compile(r"`([^`]+)`")

# Structural markers (explicit markdown levels)
H2_PART_RE = re.compile(r"^##\s+(?!#)(.+?)\s*$")   # "## ..." but not "###"
H3_SECT_RE = re.compile(r"^###\s+(?!#)(.+?)\s*$")  # "### ..." but not "####"

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


def ensure_build_dir(repo_root: Path, build_dir_rel: str) -> Path:
    build_dir = (repo_root / build_dir_rel).resolve()
    build_dir.mkdir(parents=True, exist_ok=True)
    return build_dir


def write_generated(repo_root: Path, out_path: Path, content: str) -> str:
    out_path.parent.mkdir(parents=True, exist_ok=True)
    out_path.write_text(content, encoding="utf-8")
    try:
        return str(out_path.relative_to(repo_root))
    except ValueError:
        return str(out_path)


def make_part_divider(
    repo_root: Path,
    build_dir_rel: str,
    part_index: int,
    part_title: str,
) -> str:
    """
    Part divider: MUST start on a right-hand (odd-numbered) page.
    LaTeX's \\cleardoublepage forces the next content to start on an odd page.
    """
    build_dir = ensure_build_dir(repo_root, build_dir_rel)
    fname = f"_part_p{part_index:02d}.md"
    out_path = build_dir / fname

    title = part_title.strip()
    content = "\n".join(
        [
            r"\cleardoublepage",
            "",
            f"# {title}" if title else "#",
            "",
        ]
    )
    return write_generated(repo_root, out_path, content)


def make_section_divider(
    repo_root: Path,
    build_dir_rel: str,
    part_index: int,
    section_index: int,
    section_title: str,
) -> str:
    """
    Section divider: MUST start on a new page (but does NOT enforce odd-page).
    """
    build_dir = ensure_build_dir(repo_root, build_dir_rel)
    fname = f"_section_p{part_index:02d}_s{section_index:02d}.md"
    out_path = build_dir / fname

    title = section_title.strip()
    # Use H2 so it nests under the Part H1 in outline.
    content = "\n".join(
        [
            r"\clearpage",
            "",
            f"## {title}" if title else "##",
            "",
        ]
    )
    return write_generated(repo_root, out_path, content)


def make_chapter_break(
    repo_root: Path,
    build_dir_rel: str,
    chapter_counter: int,
) -> str:
    """
    Chapter break: ensures each chapter file starts on a new page.
    Use \\clearpage to flush floats.
    """
    build_dir = ensure_build_dir(repo_root, build_dir_rel)
    fname = f"_break_ch_{chapter_counter:04d}.md"
    out_path = build_dir / fname
    content = "\n".join([r"\clearpage", ""])
    return write_generated(repo_root, out_path, content)


def extract_stream_and_dividers(
    lines: List[str],
    repo_root: Path,
    build_dir_rel: str = "build",
) -> Tuple[Dict[int, List[str]], Dict[int, str], Dict[int, bool]]:
    """
    Returns:
      - stream_by_part: per-part stream INCLUDING:
          * section divider files (page break + heading)
          * chapter break files (page break)
          * chapter paths
      - auto_part_divider_by_part: generated part divider file path for each Part (p>=1)
      - part_has_selection_content: whether part has any selection content

    Rules:
      - Part 0 = before first H2 (##)
      - Each H2 (## ...) increments part index by 1 (no keyword required)
      - Each H3 (### ...) generates a section divider inserted BEFORE the next chapter
      - Each chapter path is preceded by a generated chapter break file
    """
    current_part = 0
    section_index = 0
    chapter_counter = 0

    stream_by_part: Dict[int, List[str]] = {}
    auto_part_divider_by_part: Dict[int, str] = {}
    part_has_selection_content: Dict[int, bool] = {}

    pending_section_inserts: List[str] = []

    for line in lines:
        s = line.strip()

        m_part = H2_PART_RE.match(s)
        if m_part:
            current_part += 1
            section_index = 0
            pending_section_inserts.clear()

            title = m_part.group(1)
            auto_part_divider_by_part[current_part] = make_part_divider(
                repo_root=repo_root,
                build_dir_rel=build_dir_rel,
                part_index=current_part,
                part_title=title,
            )
            part_has_selection_content.setdefault(current_part, False)
            continue

        m_sect = H3_SECT_RE.match(s)
        if m_sect:
            section_index += 1
            title = m_sect.group(1)
            sect_rel = make_section_divider(
                repo_root=repo_root,
                build_dir_rel=build_dir_rel,
                part_index=current_part,
                section_index=section_index,
                section_title=title,
            )
            pending_section_inserts.append(sect_rel)
            part_has_selection_content[current_part] = True
            continue

        tokens = ENTRY_RE.findall(line)
        if not tokens:
            continue

        for tok in tokens:
            p = normalize_path(tok)
            if not p:
                continue

            part_has_selection_content[current_part] = True

            # Insert pending section divider(s) immediately before the next chapter
            if pending_section_inserts:
                stream_by_part.setdefault(current_part, []).extend(pending_section_inserts)
                pending_section_inserts.clear()

            # Ensure each chapter starts on a new page
            chapter_counter += 1
            brk = make_chapter_break(repo_root, build_dir_rel, chapter_counter)
            stream_by_part.setdefault(current_part, []).append(brk)

            stream_by_part.setdefault(current_part, []).append(p)

    for k in list(stream_by_part.keys()):
        stream_by_part[k] = dedup_preserve_order(stream_by_part[k])

    return stream_by_part, auto_part_divider_by_part, part_has_selection_content


def build_emit_list(
    scaffolds: List[ScaffoldFile],
    stream_by_part: Dict[int, List[str]],
    auto_part_divider_by_part: Dict[int, str],
) -> List[str]:
    """
    Authoritative SCAFFOLD rule:
      part_index = floor(NN/10)

    For each part p:
      emit scaffolds with slot 0-5 (before)
      possibly emit autogenerated part divider (odd-page) if no "before" scaffolds
      then stream for p (section dividers + chapter breaks + chapters)
      then scaffolds with slot 6-9 (after)

    Autogenerated part divider insertion policy (heuristic):
      - Part 0: never
      - Part p>=1: emit the autogenerated part divider *only if* there are no
        "before" scaffolds (slots 0-5) for that part.
    """
    scaffolds_by_part: Dict[int, List[ScaffoldFile]] = {}
    for sf in scaffolds:
        scaffolds_by_part.setdefault(sf.part_index, []).append(sf)

    for p in list(scaffolds_by_part.keys()):
        scaffolds_by_part[p].sort(key=lambda sf: Path(sf.path).name)

    parts = sorted(set(scaffolds_by_part.keys()) | set(stream_by_part.keys()) | set(auto_part_divider_by_part.keys()))
    if not parts:
        return []

    emitted: List[str] = []
    for p in parts:
        sfs = scaffolds_by_part.get(p, [])
        before = [sf.path for sf in sfs if 0 <= sf.slot <= 5]
        after = [sf.path for sf in sfs if 6 <= sf.slot <= 9]

        emitted.extend(before)

        # Autogenerated part divider (odd-page), only if no pre-scaffold exists
        if p >= 1 and not before:
            pd = auto_part_divider_by_part.get(p)
            if pd:
                emitted.append(pd)

        emitted.extend(stream_by_part.get(p, []))
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
    stream_by_part, auto_part_divider_by_part, _part_has_content = extract_stream_and_dividers(
        lines,
        repo_root=repo_root,
        build_dir_rel="build",
    )

    emitted = build_emit_list(scaffolds, stream_by_part, auto_part_divider_by_part)
    if not emitted:
        die("No input files found (scaffold + chapters is empty).")

    # Fail fast if any referenced file doesn't exist relative to repo root.
    # (Generated part/section/break files are repo-relative and should exist already.)
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
