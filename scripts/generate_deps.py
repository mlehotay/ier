#!/usr/bin/env python3
"""
generate_deps.py

Generate:
  1) build/dependencies.yml (machine-readable derived dependency surface)
  2) build/IER-prerequisites.md (human-readable derived audit surface)

Inputs:
  --manifest  IER/IER-manifest.md
  --bundles   assets/bundles.yml

Outputs:
  --out-deps      build/dependencies.yml
  --out-prereqs   build/IER-prerequisites.md

Hard failures (exit != 0):
  - hard dependency cycles
  - unresolved dependency identifiers (after bundle expansion)
  - bundle token referenced but not defined
  - manifest lists missing chapter files (after resolution)
  - (optional) missing YAML front matter / missing ier: block in strict mode

Soft issues (warnings, build continues):
  - missing YAML front matter (unless --strict-metadata)
  - YAML front matter present but missing ier: key (unless --strict-metadata)
  - malformed/partial ier: blocks (treated as empty where possible, unless --strict-metadata)

Manifest parsing:
  - Extracts backticked `...` tokens ending in .md
  - Keeps only "chapter-like" items with basename starting with "IER-" (excludes IER-manifest.md itself)
  - Resolves bare filenames like `IER-dynamics.md` by searching:
        repo_root/IER/<file>, then repo_root/pub/<file>, then repo-wide unique hit
  - Resolves path tokens like `IER/IER-dynamics.md` or `pub/...` as repo-relative paths
"""

from __future__ import annotations

import argparse
import datetime as dt
import difflib
import re
import subprocess
import sys
from dataclasses import dataclass, field
from pathlib import Path
from typing import Any, Dict, List, Optional, Set, Tuple

try:
    import yaml  # PyYAML
except ImportError:
    print("ERROR: PyYAML is required (import yaml failed).", file=sys.stderr)
    raise


# Manifest parsing: treat backticked tokens as authoritative references.
BACKTICK_MD_RE = re.compile(r"`([^`\n]+?\.md)`")
FRONT_MATTER_RE = re.compile(r"\A---\s*\n(.*?)\n---\s*\n", re.DOTALL)


@dataclass(frozen=True)
class ChapterMeta:
    tier: str = ""
    role: str = ""
    status: str = ""
    requires_hard: List[str] = field(default_factory=list)
    requires_structural: List[str] = field(default_factory=list)
    requires_guardrails: List[str] = field(default_factory=list)
    provides: List[str] = field(default_factory=list)
    gates: List[str] = field(default_factory=list)
    has_yaml: bool = False
    has_ier_block: bool = False

    def normalized(self) -> "ChapterMeta":
        def norm_list(x: Optional[List[str]]) -> List[str]:
            if not x:
                return []
            cleaned = [s.strip() for s in x if isinstance(s, str) and s.strip()]
            seen: Set[str] = set()
            out: List[str] = []
            for s in cleaned:
                if s not in seen:
                    seen.add(s)
                    out.append(s)
            return sorted(out)

        return ChapterMeta(
            tier=str(self.tier or "").strip(),
            role=str(self.role or "").strip(),
            status=str(self.status or "").strip(),
            requires_hard=norm_list(self.requires_hard),
            requires_structural=norm_list(self.requires_structural),
            requires_guardrails=norm_list(self.requires_guardrails),
            provides=norm_list(self.provides),
            gates=norm_list(self.gates),
            has_yaml=bool(self.has_yaml),
            has_ier_block=bool(self.has_ier_block),
        )


def _utc_now_iso() -> str:
    return dt.datetime.now(dt.timezone.utc).replace(microsecond=0).isoformat()


def _git_commit_hash() -> str:
    try:
        r = subprocess.run(
            ["git", "rev-parse", "HEAD"],
            check=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.DEVNULL,
            text=True,
        )
        return r.stdout.strip()
    except Exception:
        return ""


def _read_text(path: Path) -> str:
    try:
        return path.read_text(encoding="utf-8")
    except UnicodeDecodeError:
        return path.read_text(encoding="utf-8", errors="replace")


def _repo_root_from_manifest(manifest_path: Path) -> Path:
    """
    If manifest lives at IER/IER-manifest.md, treat repo root as parent of IER/.
    Otherwise treat repo root as manifest parent directory.
    """
    if manifest_path.parent.name == "IER":
        return manifest_path.parent.parent
    return manifest_path.parent


def _is_chapter_like_md_token(token: str) -> bool:
    """
    Chapter-like: basename starts with IER- and ends with .md,
    excluding the manifest file itself.
    """
    base = Path(token).name
    if not base.endswith(".md"):
        return False
    if base == "IER-manifest.md":
        return False
    return base.startswith("IER-")


def _resolve_manifest_token(repo_root: Path, token: str) -> Path:
    """
    Resolution rules:
      - If token contains a slash/backslash, treat it as repo-relative path.
      - If bare filename:
           1) repo_root/IER/<file> if exists
           2) repo_root/pub/<file> if exists
           3) repo-wide unique hit by filename
           4) default to repo_root/IER/<file> (so missing checks are stable)
    """
    t = token.strip()
    t = t.strip().strip("()[]{}.,;:")

    # Path-ish tokens: honor as repo-relative
    if "/" in t or "\\" in t:
        return (repo_root / t).resolve()

    # Bare filename: search common roots first
    candidates = [
        (repo_root / "IER" / t),
        (repo_root / "pub" / t),
    ]
    for c in candidates:
        if c.exists():
            return c.resolve()

    # Repo-wide unique match by filename (helps reorganizations)
    hits = [p for p in repo_root.rglob(t) if p.is_file()]
    if len(hits) == 1:
        return hits[0].resolve()

    return (repo_root / "IER" / t).resolve()


def _suggest_close_paths(missing: Path, candidates: List[Path], n: int = 3) -> List[str]:
    """
    Suggest close matches for a missing path using filename similarity.
    candidates should be "best candidates" already (e.g., IER/ and pub/).
    Returns repo-relative-ish strings when possible.
    """
    missing_name = missing.name
    cand_names = [c.name for c in candidates]
    close = difflib.get_close_matches(missing_name, cand_names, n=n, cutoff=0.72)
    out: List[str] = []
    for name in close:
        for c in candidates:
            if c.name == name:
                out.append(str(c))
                break
    return out


def parse_manifest(manifest_path: Path) -> List[Path]:
    """
    Parse v10.x manifest style:

    - Parse backticked tokens `...`
    - Keep only chapter-like items: basename startswith "IER-" (excluding IER-manifest.md)
    - Resolve bare filenames by searching IER/, then pub/, then repo-wide unique hit
    - Preserve first-appearance order; stable-dedupe
    """
    text = _read_text(manifest_path)
    repo_root = _repo_root_from_manifest(manifest_path)

    tokens: List[str] = []
    for m in BACKTICK_MD_RE.finditer(text):
        tok = m.group(1).strip()
        if _is_chapter_like_md_token(tok):
            tokens.append(tok)

    # Stable dedupe preserving order
    seen: Set[str] = set()
    ordered_tokens: List[str] = []
    for t in tokens:
        if t not in seen:
            seen.add(t)
            ordered_tokens.append(t)

    return [_resolve_manifest_token(repo_root, t) for t in ordered_tokens]


def extract_front_matter_yaml(md_text: str) -> Tuple[Optional[dict], bool]:
    """
    Return (yaml_dict_or_none, has_yaml_front_matter).
    Only reads a leading YAML block:
      ---
      ...
      ---
    """
    m = FRONT_MATTER_RE.match(md_text)
    if not m:
        return None, False

    yml_text = m.group(1)
    try:
        data = yaml.safe_load(yml_text)
        if data is None:
            data = {}
        if not isinstance(data, dict):
            return {}, True
        return data, True
    except Exception:
        return {}, True


def _dig(d: Dict[str, Any], path: List[str]) -> Any:
    cur: Any = d
    for k in path:
        if not isinstance(cur, dict):
            return None
        cur = cur.get(k)
    return cur


def _coerce_list(x: Any) -> List[str]:
    if x is None:
        return []
    if isinstance(x, list):
        out: List[str] = []
        for item in x:
            if isinstance(item, str):
                out.append(item)
        return out
    if isinstance(x, str):
        s = x.strip()
        return [s] if s else []
    return []


def parse_chapter_ier_meta(chapter_path: Path) -> Tuple[str, ChapterMeta, List[str]]:
    """
    Returns (chapter_id, ChapterMeta, warnings).
    Chapter ID is filename stem (IER-foo).
    """
    warnings: List[str] = []
    chapter_id = chapter_path.stem

    if not chapter_path.exists():
        warnings.append(f"Missing chapter file on disk: {chapter_path}")
        return chapter_id, ChapterMeta(has_yaml=False, has_ier_block=False), warnings

    text = _read_text(chapter_path)
    yml, has_yaml = extract_front_matter_yaml(text)

    meta = ChapterMeta(
        has_yaml=has_yaml,
        has_ier_block=False,
    )

    if not has_yaml:
        warnings.append(f"{chapter_id}: no YAML front matter found (treated as empty metadata).")
        return chapter_id, meta.normalized(), warnings

    if not isinstance(yml, dict):
        warnings.append(f"{chapter_id}: YAML front matter not a mapping (treated as empty metadata).")
        return chapter_id, meta.normalized(), warnings

    ier = yml.get("ier", None)
    if ier is None:
        warnings.append(f"{chapter_id}: YAML present but no top-level 'ier:' key (treated as empty metadata).")
        return chapter_id, meta.normalized(), warnings

    if not isinstance(ier, dict):
        warnings.append(f"{chapter_id}: 'ier:' is not a mapping (treated as empty metadata).")
        return chapter_id, meta.normalized(), warnings

    meta = ChapterMeta(
        tier=str(ier.get("tier", "") or ""),
        role=str(ier.get("role", "") or ""),
        status=str(ier.get("status", "") or ""),
        requires_hard=_coerce_list(_dig(ier, ["requires", "hard"])),
        requires_structural=_coerce_list(_dig(ier, ["requires", "structural"])),
        requires_guardrails=_coerce_list(_dig(ier, ["requires", "guardrails"])),
        provides=_coerce_list(ier.get("provides", [])),
        gates=_coerce_list(ier.get("gates", [])),
        has_yaml=True,
        has_ier_block=True,
    )

    return chapter_id, meta.normalized(), warnings


def load_bundles(bundles_path: Path) -> Dict[str, List[str]]:
    if not bundles_path.exists():
        raise FileNotFoundError(f"Bundles file not found: {bundles_path}")

    data = yaml.safe_load(_read_text(bundles_path))
    if data is None:
        data = {}
    if not isinstance(data, dict):
        raise ValueError(f"Bundles file must be a YAML mapping: {bundles_path}")

    bundles = data.get("bundles", {})
    if bundles is None:
        bundles = {}
    if not isinstance(bundles, dict):
        raise ValueError(f"'bundles:' must be a mapping in {bundles_path}")

    out: Dict[str, List[str]] = {}
    for k, v in bundles.items():
        if not isinstance(k, str):
            continue
        if isinstance(v, list):
            items = [s.strip() for s in v if isinstance(s, str) and s.strip()]
        elif isinstance(v, str):
            items = [v.strip()] if v.strip() else []
        else:
            items = []
        out[k.strip()] = sorted(set(items))
    return out


def expand_requires(
    chapter_id: str,
    requires: List[str],
    bundles: Dict[str, List[str]],
    used_bundles: Set[str],
    errors: List[str],
) -> List[str]:
    expanded: List[str] = []
    for token in requires:
        t = token.strip()
        if not t:
            continue
        if t in bundles:
            used_bundles.add(t)
            expanded.extend(bundles[t])
        else:
            # If it "looks like" a bundle token (all caps + digits/underscore/hyphen)
            # but isn't a chapter ID, treat as missing bundle def.
            if re.fullmatch(r"[A-Z0-9_\-]+", t) and not t.startswith("IER-") and t not in bundles:
                errors.append(f"{chapter_id}: bundle token '{t}' referenced but not defined in bundles config.")
            else:
                expanded.append(t)
    return sorted(set(expanded))


def validate_identifiers(
    chapters_in_manifest: Set[str],
    chapter_id: str,
    expanded_requires: Dict[str, List[str]],
    errors: List[str],
) -> None:
    for kind, reqs in expanded_requires.items():
        for r in reqs:
            if r not in chapters_in_manifest:
                errors.append(f"{chapter_id}: unresolved identifier in requires.{kind}: '{r}' (not in manifest).")


def build_hard_graph(chapter_ids: List[str], hard_requires: Dict[str, List[str]]) -> Dict[str, Set[str]]:
    """
    Returns adjacency: prereq -> set(of dependents).
    hard_requires maps node -> list(prereq nodes)
    """
    adj: Dict[str, Set[str]] = {cid: set() for cid in chapter_ids}
    for node in chapter_ids:
        for prereq in hard_requires.get(node, []):
            adj.setdefault(prereq, set()).add(node)
    return adj


def topo_depth_order(
    chapter_ids: List[str],
    hard_requires: Dict[str, List[str]],
) -> Tuple[List[str], Optional[List[str]]]:
    """
    Compute:
      - topo ordering grouped by depth; alphabetical within depth
      - if cycle: return (partial_order, cycle_nodes_list)
    """
    indeg = {cid: 0 for cid in chapter_ids}
    for node in chapter_ids:
        for prereq in hard_requires.get(node, []):
            if prereq in indeg:
                indeg[node] += 1

    import heapq

    heap: List[str] = []
    depth: Dict[str, int] = {cid: 0 for cid in chapter_ids}

    for cid in chapter_ids:
        if indeg[cid] == 0:
            heapq.heappush(heap, cid)

    processed: List[str] = []
    adj = build_hard_graph(chapter_ids, hard_requires)

    while heap:
        u = heapq.heappop(heap)
        processed.append(u)
        for v in sorted(adj.get(u, [])):
            indeg[v] -= 1
            depth[v] = max(depth[v], depth[u] + 1)
            if indeg[v] == 0:
                heapq.heappush(heap, v)

    if len(processed) != len(chapter_ids):
        cycle_nodes = sorted([cid for cid, d in indeg.items() if d > 0])
        return processed, cycle_nodes

    max_d = max(depth.values()) if depth else 0
    by_depth: Dict[int, List[str]] = {d: [] for d in range(max_d + 1)}
    for cid in chapter_ids:
        by_depth[depth[cid]].append(cid)

    ordered: List[str] = []
    for d in range(max_d + 1):
        ordered.extend(sorted(by_depth[d]))
    return ordered, None


def write_yaml(path: Path, data: dict) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    text = yaml.safe_dump(
        data,
        sort_keys=True,
        allow_unicode=True,
        default_flow_style=False,
        width=88,
    )
    path.write_text(text, encoding="utf-8")


def write_prereqs_md(
    path: Path,
    *,
    manifest_path: Path,
    deps_yml_path: Path,
    bundles_path: Path,
    git_hash: str,
    generated_utc: str,
    chapters: Dict[str, ChapterMeta],
    hard_requires: Dict[str, List[str]],
    structural_requires: Dict[str, List[str]],
    guardrail_requires: Dict[str, List[str]],
    provides: Dict[str, List[str]],
    gates_opened: Dict[str, List[str]],
    topo_order: List[str],
    used_bundles: List[str],
    warnings: List[str],
    validation_pass: bool,
) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)

    def _fmt_list(xs: List[str]) -> str:
        if not xs:
            return "[]"
        return "[" + ", ".join(f"`{x}`" for x in xs) + "]"

    # Reverse indices
    rev_hard: Dict[str, List[str]] = {cid: [] for cid in topo_order}
    rev_struct: Dict[str, List[str]] = {cid: [] for cid in topo_order}
    rev_guard: Dict[str, List[str]] = {cid: [] for cid in topo_order}

    for cid in topo_order:
        for p in hard_requires.get(cid, []):
            if p in rev_hard:
                rev_hard[p].append(cid)
        for p in structural_requires.get(cid, []):
            if p in rev_struct:
                rev_struct[p].append(cid)
        for p in guardrail_requires.get(cid, []):
            if p in rev_guard:
                rev_guard[p].append(cid)

    for m in (rev_hard, rev_struct, rev_guard):
        for k in m:
            m[k] = sorted(set(m[k]))

    # Gate index (opened only; “required” is not inferable from current schema)
    gate_openers: Dict[str, List[str]] = {}
    for cid, gates in gates_opened.items():
        for g in gates:
            gate_openers.setdefault(g, []).append(cid)
    for g in gate_openers:
        gate_openers[g] = sorted(set(gate_openers[g]))

    # Counts for validation summary
    hard_edges = sum(len(hard_requires.get(cid, [])) for cid in topo_order)
    structural_edges = sum(len(structural_requires.get(cid, [])) for cid in topo_order)
    guardrail_edges = sum(len(guardrail_requires.get(cid, [])) for cid in topo_order)
    total_gates = len(gate_openers)

    with path.open("w", encoding="utf-8") as f:
        f.write("# IER-prerequisites.md\n\n")
        f.write("## Status Declaration\n\n")
        f.write("This document is **NON-CANONICAL**, **NON-CORPUS**, and **DERIVED**.\n\n")
        f.write("It is generated from chapter-local YAML front matter under the top-level key `ier:`.\n")
        f.write("Chapter-local YAML is the **single source of truth** for dependency metadata.\n\n")
        f.write("This file is an **audit surface** only and carries **no interpretive or adjudicative authority**.\n\n")

        f.write("## Generation Metadata\n\n")
        f.write("- Generator: `generate_deps.py`\n")
        if git_hash:
            f.write(f"- Git commit: `{git_hash}`\n")
        f.write(f"- Manifest: `{manifest_path.as_posix()}`\n")
        f.write(f"- Bundles: `{bundles_path.as_posix()}`\n")
        f.write(f"- Deps YAML: `{deps_yml_path.as_posix()}`\n")
        f.write(f"- Total chapters (manifest-derived): **{len(topo_order)}**\n")
        with_ier = sum(1 for c in chapters.values() if c.has_ier_block)
        missing_ier = len(topo_order) - with_ier
        f.write(f"- Chapters with `ier:` metadata: **{with_ier}**\n")
        f.write(f"- Chapters missing `ier:` metadata: **{missing_ier}**\n")
        f.write(f"- Bundles expanded: `{', '.join(used_bundles) if used_bundles else '—'}`\n\n")

        f.write("## Legend\n\n")
        f.write("- **Hard** prerequisites: semantic prerequisites; impose a strict partial order.\n")
        f.write("- **Structural** prerequisites: machinery/vocabulary prerequisites for precision.\n")
        f.write("- **Guardrail** prerequisites: misuse/interpretation constraints (not narrative inputs).\n")
        f.write("- `[]` means explicitly empty.\n")
        f.write("- Warnings indicate missing/partial metadata or non-fatal issues.\n\n")

        f.write("## Chapter Prerequisite Table\n\n")
        for cid in topo_order:
            cm = chapters[cid]
            f.write(f"### {cid}\n")
            f.write(f"- Tier: `{cm.tier or ''}`\n")
            f.write(f"- Role: `{cm.role or ''}`\n")
            f.write(f"- Status: `{cm.status or ''}`\n")
            f.write(f"- Hard Prerequisites: {_fmt_list(hard_requires.get(cid, []))}\n")
            f.write(f"- Structural Prerequisites: {_fmt_list(structural_requires.get(cid, []))}\n")
            f.write(f"- Guardrail Prerequisites: {_fmt_list(guardrail_requires.get(cid, []))}\n")
            f.write(f"- Provides: {_fmt_list(provides.get(cid, []))}\n")
            f.write(f"- Gates Opened: {_fmt_list(gates_opened.get(cid, []))}\n")
            f.write("- Gates Required: []\n\n")

        f.write("## Reverse Dependency Index\n\n")
        for cid in topo_order:
            f.write(f"### {cid}\n")
            f.write(f"- Hard dependents: {_fmt_list(rev_hard.get(cid, []))}\n")
            f.write(f"- Structural dependents: {_fmt_list(rev_struct.get(cid, []))}\n")
            f.write(f"- Guardrail dependents: {_fmt_list(rev_guard.get(cid, []))}\n\n")

        f.write("## Gate Index\n\n")
        if not gate_openers:
            f.write("*(No gates declared.)*\n\n")
        else:
            for g in sorted(gate_openers.keys()):
                f.write(f"### {g}\n")
                f.write(f"- Opened by: {_fmt_list(gate_openers[g])}\n")
                f.write("- Required by: []\n\n")

        f.write("## Validation Summary\n\n")
        f.write(f"- Total chapters processed: **{len(topo_order)}**\n")
        f.write(f"- Total hard dependency edges: **{hard_edges}**\n")
        f.write(f"- Total structural dependency edges: **{structural_edges}**\n")
        f.write(f"- Total guardrail dependency edges: **{guardrail_edges}**\n")
        f.write(f"- Total gates: **{total_gates}**\n")
        f.write(f"- Validation status: **{'PASS' if validation_pass else 'FAIL'}**\n")
        f.write(f"- Warnings: **{len(warnings)}**\n\n")
        if warnings:
            f.write("### Warnings\n\n")
            for w in warnings:
                f.write(f"- {w}\n")
            f.write("\n")

        f.write("## Generation Timestamp\n\n")
        f.write(f"- UTC: `{generated_utc}`\n\n")


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--manifest", required=True, help="Path to IER-manifest.md")
    ap.add_argument("--bundles", required=True, help="Path to bundles.yml (e.g., assets/bundles.yml)")
    ap.add_argument("--out-deps", required=False, help="Write derived deps YAML (e.g., build/dependencies.yml)")
    ap.add_argument("--out-prereqs", required=False, help="Write derived audit markdown (e.g., build/IER-prerequisites.md)")
    ap.add_argument(
        "--strict-metadata",
        action="store_true",
        help="Fail if any chapter lacks YAML front matter or ier: block (instead of warning).",
    )
    args = ap.parse_args()

    manifest_path = Path(args.manifest)
    bundles_path = Path(args.bundles)

    warnings: List[str] = []
    errors: List[str] = []

    if not manifest_path.exists():
        print(f"ERROR: manifest not found: {manifest_path}", file=sys.stderr)
        return 2

    bundles = load_bundles(bundles_path)

    chapter_paths = parse_manifest(manifest_path)
    if not chapter_paths:
        print(f"ERROR: no chapter-like entries found in manifest: {manifest_path}", file=sys.stderr)
        return 2

    repo_root = _repo_root_from_manifest(manifest_path)

    # Manifest is authoritative: missing chapter files are fatal.
    # Candidate pool for suggestions: prefer IER/ and pub/ only (avoids noise).
    ier_candidates = list((repo_root / "IER").glob("IER-*.md")) if (repo_root / "IER").exists() else []
    pub_candidates = list((repo_root / "pub").glob("IER-*.md")) if (repo_root / "pub").exists() else []
    suggestion_candidates = ier_candidates + pub_candidates

    missing_files = [p for p in chapter_paths if not p.exists()]
    if missing_files:
        for p in missing_files:
            sugg = _suggest_close_paths(p, suggestion_candidates, n=3)
            if sugg:
                errors.append(f"Manifest lists missing file: {p} (did you mean: {', '.join(sugg)})")
            else:
                errors.append(f"Manifest lists missing file: {p}")

    chapter_ids: List[str] = [p.stem for p in chapter_paths]
    chapters_in_manifest: Set[str] = set(chapter_ids)

    chapters: Dict[str, ChapterMeta] = {}
    raw_warns: List[str] = []
    for p in chapter_paths:
        cid, meta, ws = parse_chapter_ier_meta(p)
        chapters[cid] = meta
        raw_warns.extend(ws)

    if args.strict_metadata:
        for cid, cm in chapters.items():
            if not cm.has_yaml:
                errors.append(f"{cid}: missing YAML front matter (strict-metadata).")
            if not cm.has_ier_block:
                errors.append(f"{cid}: missing 'ier:' block (strict-metadata).")
    else:
        warnings.extend(raw_warns)

    used_bundles: Set[str] = set()

    hard_requires: Dict[str, List[str]] = {}
    structural_requires: Dict[str, List[str]] = {}
    guardrail_requires: Dict[str, List[str]] = {}
    provides: Dict[str, List[str]] = {}
    gates_opened: Dict[str, List[str]] = {}

    for cid in chapter_ids:
        cm = chapters[cid]
        hard = expand_requires(cid, cm.requires_hard, bundles, used_bundles, errors)
        structural = expand_requires(cid, cm.requires_structural, bundles, used_bundles, errors)
        guard = expand_requires(cid, cm.requires_guardrails, bundles, used_bundles, errors)

        expanded = {"hard": hard, "structural": structural, "guardrails": guard}
        validate_identifiers(chapters_in_manifest, cid, expanded, errors)

        hard_requires[cid] = hard
        structural_requires[cid] = structural
        guardrail_requires[cid] = guard
        provides[cid] = cm.provides
        gates_opened[cid] = cm.gates

    topo_order, cycle_nodes = topo_depth_order(chapter_ids, hard_requires)
    if cycle_nodes:
        errors.append("Hard dependency cycle detected (nodes with remaining indegree): " + ", ".join(cycle_nodes))

    if errors:
        print("DEPENDENCY GENERATION FAILED\n", file=sys.stderr)
        for e in errors:
            print(f"ERROR: {e}", file=sys.stderr)
        for w in warnings:
            print(f"WARNING: {w}", file=sys.stderr)
        return 1

    git_hash = _git_commit_hash()
    generated_utc = _utc_now_iso()

    hard_edges: List[List[str]] = []
    for node in chapter_ids:
        for prereq in hard_requires.get(node, []):
            hard_edges.append([prereq, node])
    hard_edges = sorted(hard_edges, key=lambda x: (x[0], x[1]))

    deps_doc: Dict[str, Any] = {
        "meta": {
            "generator": "generate_deps.py",
            "generated_utc": generated_utc,
            "git_commit": git_hash or "",
            "manifest": str(manifest_path),
            "bundles": str(bundles_path),
        },
        "bundles": {
            "defined": {k: bundles[k] for k in sorted(bundles.keys())},
            "expanded_in_run": sorted(used_bundles),
        },
        "chapters": {},
        "hard_edges": hard_edges,
        "warnings": warnings,
    }

    for cid in chapter_ids:
        cm = chapters[cid]
        deps_doc["chapters"][cid] = {
            "tier": cm.tier,
            "role": cm.role,
            "status": cm.status,
            "requires": {
                "hard": hard_requires.get(cid, []),
                "structural": structural_requires.get(cid, []),
                "guardrails": guardrail_requires.get(cid, []),
            },
            "provides": provides.get(cid, []),
            "gates": gates_opened.get(cid, []),
            "metadata": {
                "has_yaml_front_matter": cm.has_yaml,
                "has_ier_block": cm.has_ier_block,
            },
        }

    out_deps = Path(args.out_deps) if args.out_deps else None
    out_prereqs = Path(args.out_prereqs) if args.out_prereqs else None

    if out_deps:
        write_yaml(out_deps, deps_doc)

    if out_prereqs:
        write_prereqs_md(
            out_prereqs,
            manifest_path=manifest_path,
            deps_yml_path=out_deps if out_deps else Path("build/dependencies.yml"),
            bundles_path=bundles_path,
            git_hash=git_hash,
            generated_utc=generated_utc,
            chapters=chapters,
            hard_requires=hard_requires,
            structural_requires=structural_requires,
            guardrail_requires=guardrail_requires,
            provides=provides,
            gates_opened=gates_opened,
            topo_order=topo_order,
            used_bundles=sorted(used_bundles),
            warnings=warnings,
            validation_pass=True,
        )

    for w in warnings:
        print(f"WARNING: {w}", file=sys.stderr)

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
