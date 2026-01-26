#!/usr/bin/env python3
"""
lint_manifest.py — IER/IER-manifest.md linter

Contract (mechanical):
- Authoritative inventory entries are backticked tokens ending in `.md`
  that appear in "inventory contexts" (Markdown list items or table rows).
- IER corpus chapters are only those references that are either:
    * bare `IER-foo.md` (default resolves to IER/IER-foo.md), or
    * explicit `IER/IER-foo.md`
  and whose basename startswith "IER-" and endswith ".md", excluding "IER-manifest.md".
- Order is first appearance; duplicates ignored (warn).
- All resolved IER corpus chapter files must exist (error).
- Prose/comments may contain backticked `.md` tokens; they are ignored by default
  because they are not in inventory contexts.

Usage:
  python3 scripts/lint_manifest.py
  python3 scripts/lint_manifest.py --manifest IER/IER-manifest.md

Exit codes:
  0 = OK (warnings possible)
  2 = fatal errors
"""

from __future__ import annotations

import argparse
import os
import re
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Iterable, List, Optional, Set, Tuple

# ---- Inline code span extraction ----
# Strict: `...` on a single line, no nested backticks.
CODE_SPAN_RE = re.compile(r"`([^`\n]+)`")

# Disallow punctuation inside backticks for `.md` tokens: `foo.md,`
TRAILING_PUNCT_RE = re.compile(r"[,\.;:]+$")

# Inventory context = list items or tables
LIST_ITEM_RE = re.compile(r"^\s*(?:[-*+]\s+|\d+\.\s+)")


@dataclass(frozen=True)
class TokenHit:
    token: str
    line_no: int
    col_no: int
    line_text: str
    in_inventory_context: bool


@dataclass
class LintMessage:
    level: str  # "ERROR" | "WARNING"
    message: str
    line_no: Optional[int] = None

    def format(self) -> str:
        if self.line_no is None:
            return f"{self.level}: {self.message}"
        return f"{self.level}: L{self.line_no}: {self.message}"


def _read_utf8(path: Path) -> str:
    try:
        return path.read_text(encoding="utf-8")
    except UnicodeDecodeError as e:
        raise ValueError(f"Manifest is not valid UTF-8: {e}") from e


def _is_inventory_context_line(line: str) -> bool:
    """
    Inventory contexts are intentionally simple and mechanical:
    - Markdown list items: - ...  * ...  + ...  1. ...
    - Markdown table rows: any line containing '|'
    """
    s = line.rstrip("\n")
    if LIST_ITEM_RE.match(s):
        return True
    if "|" in s:
        return True
    return False


def extract_tokens(text: str) -> List[TokenHit]:
    hits: List[TokenHit] = []
    for idx, line in enumerate(text.splitlines(), start=1):
        inv = _is_inventory_context_line(line)
        for m in CODE_SPAN_RE.finditer(line):
            token = m.group(1)
            hits.append(
                TokenHit(
                    token=token,
                    line_no=idx,
                    col_no=m.start(1) + 1,
                    line_text=line,
                    in_inventory_context=inv,
                )
            )
    return hits


def is_md_token(token: str) -> bool:
    return token.endswith(".md")


def repo_root_from_manifest(manifest_path: Path) -> Path:
    """
    Convention shared with tooling:
    - If manifest lives at IER/IER-manifest.md => repo root is parent of IER/
    - Else repo root is manifest parent
    """
    if manifest_path.parent.name == "IER":
        return manifest_path.parent.parent
    return manifest_path.parent


def resolve_token(repo_root: Path, token: str) -> Path:
    """
    Resolution contract:
    - Bare IER corpus chapter tokens like `IER-foo.md` resolve to IER/IER-foo.md
    - Any token containing '/' resolves repo-relative exactly as written
    """
    if "/" not in token and token.startswith("IER-") and token.endswith(".md"):
        return repo_root / "IER" / token
    return repo_root / token


def is_ier_corpus_token(token: str) -> bool:
    """
    IER corpus chapter tokens are ONLY:
      - bare:        IER-foo.md
      - explicit:    IER/IER-foo.md
    Excludes IER-manifest.md itself.
    """
    if token == "IER-manifest.md" or token.endswith("/IER-manifest.md"):
        return False

    if token.startswith("IER/"):
        base = token.split("/")[-1]
        return base.startswith("IER-") and base.endswith(".md") and base != "IER-manifest.md"

    if "/" not in token:
        return token.startswith("IER-") and token.endswith(".md") and token != "IER-manifest.md"

    return False


def validate_token_integrity(hits: Iterable[TokenHit]) -> Tuple[List[LintMessage], List[TokenHit]]:
    """
    For `.md` tokens, disallow trailing punctuation *inside backticks*.
    This catches common human formatting mistakes like `IER-foo.md,`
    """
    msgs: List[LintMessage] = []
    kept: List[TokenHit] = []
    for h in hits:
        t = h.token.strip()
        if not t:
            continue

        if is_md_token(t) and TRAILING_PUNCT_RE.search(t):
            msgs.append(
                LintMessage(
                    "ERROR",
                    f"Invalid reference token (trailing punctuation inside backticks): `{h.token}`",
                    h.line_no,
                )
            )
            continue

        kept.append(
            TokenHit(
                token=t,
                line_no=h.line_no,
                col_no=h.col_no,
                line_text=h.line_text,
                in_inventory_context=h.in_inventory_context,
            )
        )

    return msgs, kept


def lint_manifest(
    manifest_path: Path,
    *,
    strict_path: bool = True,
    warn_backticked_md_outside_inventory: bool = False,
) -> Tuple[int, List[LintMessage]]:
    """
    Returns: (exit_code, messages)
      - exit_code: 0 for success, 2 for fatal errors
    """
    msgs: List[LintMessage] = []
    fatal = False

    # A1: canonical path check
    if strict_path:
        expected = (Path("IER") / "IER-manifest.md").as_posix()
        try:
            rel = manifest_path.resolve().relative_to(Path.cwd().resolve()).as_posix()
        except Exception:
            rel = manifest_path.as_posix()
        if rel != expected:
            msgs.append(LintMessage("ERROR", f"Manifest path must be `{expected}` (got `{rel}`)", None))
            fatal = True

    # A2: UTF-8 check
    try:
        text = _read_utf8(manifest_path)
    except Exception as e:
        msgs.append(LintMessage("ERROR", str(e), None))
        return 2, msgs

    repo_root = repo_root_from_manifest(manifest_path)

    # Extract tokens and validate token integrity
    hits = extract_tokens(text)
    integ_msgs, hits = validate_token_integrity(hits)
    msgs.extend(integ_msgs)
    if any(m.level == "ERROR" for m in integ_msgs):
        fatal = True

    # Optional: warn for backticked `.md` outside inventory context
    if warn_backticked_md_outside_inventory:
        for h in hits:
            if is_md_token(h.token) and not h.in_inventory_context:
                msgs.append(
                    LintMessage(
                        "WARNING",
                        f"Backticked `.md` token outside inventory context is ignored by linter: `{h.token}`",
                        h.line_no,
                    )
                )

    # Only count `.md` tokens in inventory contexts
    inv_md_hits = [h for h in hits if h.in_inventory_context and is_md_token(h.token)]

    # E1/E2: ordering by first appearance; duplicates ignored (warn)
    seen: Set[str] = set()
    chapter_refs_ordered: List[Tuple[Path, TokenHit]] = []

    for h in inv_md_hits:
        # Only IER corpus chapter tokens are considered "chapter-like"
        if not is_ier_corpus_token(h.token):
            continue

        resolved = resolve_token(repo_root, h.token)

        # Chapter-like by contract already; keep defensive guard:
        base = resolved.name
        if not (base.startswith("IER-") and base.endswith(".md") and base != "IER-manifest.md"):
            continue

        key = os.path.normpath(str(resolved))
        if key in seen:
            msgs.append(
                LintMessage(
                    "WARNING",
                    f"Duplicate chapter reference ignored (first occurrence wins): `{h.token}` -> {resolved}",
                    h.line_no,
                )
            )
            continue

        seen.add(key)
        chapter_refs_ordered.append((resolved, h))

    # I: minimal success requires at least one chapter-like reference
    if not chapter_refs_ordered:
        msgs.append(
            LintMessage(
                "ERROR",
                "No IER corpus chapters found in inventory contexts. "
                "Need at least one list/table entry containing `IER-*.md` or `IER/IER-*.md`.",
                None,
            )
        )
        fatal = True

    # F1/F2: existence checks for chapter-like references
    for resolved, h in chapter_refs_ordered:
        if not resolved.exists():
            msgs.append(LintMessage("ERROR", f"Manifest lists missing file: {resolved}", h.line_no))
            fatal = True

    # H3: mixed path styles warning — only for IER corpus entries
    ier_tokens = [h.token for _, h in chapter_refs_ordered]
    used_bare = any(("/" not in t) for t in ier_tokens)
    used_explicit_ier = any(t.startswith("IER/") for t in ier_tokens)
    if used_bare and used_explicit_ier:
        msgs.append(
            LintMessage(
                "WARNING",
                "Mixed IER corpus reference styles detected (some `IER-foo.md`, some `IER/IER-foo.md`). "
                "Consider standardizing on bare `IER-*.md`.",
                None,
            )
        )

    return (2 if fatal else 0), msgs


def main(argv: Optional[List[str]] = None) -> int:
    ap = argparse.ArgumentParser(description="Lint IER-manifest.md invariants.")
    ap.add_argument(
        "--manifest",
        default="IER/IER-manifest.md",
        help="Path to manifest file (default: IER/IER-manifest.md)",
    )
    ap.add_argument(
        "--no-strict-path",
        action="store_true",
        help="Do not enforce canonical path (useful for testing).",
    )
    ap.add_argument(
        "--warnings-as-errors",
        action="store_true",
        help="Treat warnings as fatal (non-zero exit).",
    )
    ap.add_argument(
        "--warn-md-outside-inventory",
        action="store_true",
        help="Warn on backticked `.md` tokens outside inventory contexts (ignored by default).",
    )
    args = ap.parse_args(argv)

    manifest_path = Path(args.manifest)

    if not manifest_path.exists():
        print(f"ERROR: manifest not found: {manifest_path}", file=sys.stderr)
        return 2

    code, msgs = lint_manifest(
        manifest_path,
        strict_path=not args.no_strict_path,
        warn_backticked_md_outside_inventory=args.warn_md_outside_inventory,
    )

    errors = [m for m in msgs if m.level == "ERROR"]
    warns = [m for m in msgs if m.level == "WARNING"]

    for m in errors:
        print(m.format(), file=sys.stderr)
    for m in warns:
        print(m.format(), file=sys.stderr)

    if args.warnings_as_errors and warns and code == 0:
        return 2

    return code


if __name__ == "__main__":
    raise SystemExit(main())
