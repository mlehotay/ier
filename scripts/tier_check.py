import re
from pathlib import Path

TIER_NUM = {"T1": 1, "T2": 2, "T3": 3, "T4": 4}

IER_FILE_RE = re.compile(r"`(IER-[a-z0-9\-]+\.md)`", re.IGNORECASE)

ANCHOR_START_RE = re.compile(r"(All .* authority resides exclusively in|If any statement .* conflicts)", re.IGNORECASE)

def parse_manifest(manifest_text: str):
    # expects lines like: 4. **[T2] `IER-math.md`** — ...
    tiers = {}
    for m in re.finditer(r"\[T([1-4])\].*?`(IER-[^`]+\.md)`", manifest_text):
        tiers[m.group(2)] = f"T{m.group(1)}"
    return tiers

def extract_refs(text: str):
    return set(m.group(1) for m in IER_FILE_RE.finditer(text))

def extract_anchor_refs(text: str):
    lines = text.splitlines()
    refs = set()
    in_anchor = False
    for line in lines:
        if ANCHOR_START_RE.search(line):
            in_anchor = True
        if in_anchor:
            refs |= extract_refs(line)
            # heuristic: stop anchor after a blank line following a bullet block
            if line.strip() == "":
                in_anchor = False
    return refs

def check(repo_dir: Path):
    manifest_path = repo_dir / "IER-manifest.md"
    manifest = manifest_path.read_text(encoding="utf-8")
    tiers = parse_manifest(manifest)

    errors, warnings = [], []

    for file in tiers:
        path = repo_dir / file
        if not path.exists():
            errors.append(f"Manifest lists missing file: {file}")
            continue
        text = path.read_text(encoding="utf-8")
        tier_a = TIER_NUM[tiers[file]]

        hard = extract_anchor_refs(text) - {file}
        soft = extract_refs(text) - {file}

        for dep in sorted(hard):
            if dep in tiers:
                if TIER_NUM[tiers[dep]] > tier_a:
                    errors.append(f"Tier inversion (HARD): {file} ({tiers[file]}) anchors to {dep} ({tiers[dep]})")
            else:
                warnings.append(f"{file} anchors to unmanifested file: {dep}")

        for dep in sorted(soft - hard):
            if dep in tiers:
                if TIER_NUM[tiers[dep]] > tier_a:
                    warnings.append(f"Downstream reference (SOFT): {file} ({tiers[file]}) mentions {dep} ({tiers[dep]})")

    return errors, warnings
