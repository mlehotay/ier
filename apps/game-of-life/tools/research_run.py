#!/usr/bin/env python3
"""Record a fixed-world Life run through the existing compiled C engine."""

import argparse
import ctypes
import hashlib
import json
import os
from pathlib import Path
import re
import subprocess
import sys


APP_ROOT = Path(__file__).resolve().parents[1]
CELL_LIMIT = 16_777_216
REQUIRED = {
    "schema", "run_id", "protocol_id", "proposal_revision", "purpose",
    "width", "height", "topology", "rule", "initial_live", "actions",
    "max_cells", "max_records",
}
HASH_FIELDS = {"protocol_sha256", "proposal_sha256"}


class RunError(ValueError):
    pass


def unique_object(pairs):
    result = {}
    for key, value in pairs:
        if key in result:
            raise RunError(f"duplicate JSON key: {key!r}")
        result[key] = value
    return result


def integer(value, label, minimum=1):
    if type(value) is not int or value < minimum:
        raise RunError(f"{label} must be an integer >= {minimum}")
    return value


def live_cells(value, width, height, label):
    if not isinstance(value, list):
        raise RunError(f"{label} must be a coordinate list")
    seen = set()
    for pair in value:
        if not isinstance(pair, list) or len(pair) != 2:
            raise RunError(f"{label} coordinates must be [x,y] pairs")
        x, y = pair
        integer(x, label + " x", 0)
        integer(y, label + " y", 0)
        if x >= width or y >= height:
            raise RunError(f"{label} coordinate outside the world: {pair}")
        if (x, y) in seen:
            raise RunError(f"{label} contains duplicate coordinates")
        seen.add((x, y))


def validate_manifest(manifest):
    if not isinstance(manifest, dict):
        raise RunError("manifest must be a JSON object")
    if REQUIRED - manifest.keys() or manifest.keys() - REQUIRED - HASH_FIELDS:
        raise RunError("missing or unknown manifest fields")
    if manifest["schema"] != "gol-run/1" or manifest["rule"] != "B3/S23":
        raise RunError("unsupported schema or rule")
    for key in ("run_id", "protocol_id", "proposal_revision"):
        if not isinstance(manifest[key], str) or not manifest[key].strip():
            raise RunError(f"{key} must be a nonempty string")
    if manifest["purpose"] not in ("pilot", "reconstruction", "confirmatory"):
        raise RunError("invalid run purpose")
    for key in HASH_FIELDS:
        if key in manifest and (not isinstance(manifest[key], str) or not re.fullmatch("[0-9a-f]{64}", manifest[key])):
            raise RunError(f"{key} must be a lowercase SHA-256")
        if manifest["purpose"] == "confirmatory" and key not in manifest:
            raise RunError(f"confirmatory input requires {key}")
    width = integer(manifest["width"], "width")
    height = integer(manifest["height"], "height")
    cells = width * height
    if cells > min(CELL_LIMIT, integer(manifest["max_cells"], "max_cells")):
        raise RunError("world exceeds cell limit")
    if manifest["topology"] not in ("bounded", "toroidal"):
        raise RunError("unsupported topology")
    live_cells(manifest["initial_live"], width, height, "initial_live")
    actions = manifest["actions"]
    if not isinstance(actions, list):
        raise RunError("actions must be a list")
    records = 3
    for action in actions:
        if not isinstance(action, dict):
            raise RunError("each action must be an object")
        if action.get("kind") == "step" and set(action) == {"kind", "count"}:
            records += integer(action["count"], "step count")
        elif action.get("kind") == "install" and set(action) == {"kind", "live"}:
            live_cells(action["live"], width, height, "install live")
            records += 1
        else:
            raise RunError("unknown action or action fields")
    if records > integer(manifest["max_records"], "max_records"):
        raise RunError("run exceeds record limit")
    return records


# Private binding of src/world.h. Keep this layout synchronized with that
# header; all transitions and native counts come from src/world.c.
class World(ctypes.Structure):
    _fields_ = [("w", ctypes.c_int), ("h", ctypes.c_int),
                ("cells", ctypes.POINTER(ctypes.c_uint8)),
                ("next", ctypes.POINTER(ctypes.c_uint8)), ("toroidal", ctypes.c_int)]


class StepStats(ctypes.Structure):
    _fields_ = [(key, ctypes.c_ulong) for key in
                ("live_cells", "births", "deaths", "survivors", "changed")]


class WorldEngine:
    def __init__(self, library, width, height, topology):
        self.lib = ctypes.CDLL(str(library))
        self.lib.world_init.argtypes = [ctypes.POINTER(World), ctypes.c_int, ctypes.c_int, ctypes.c_int]
        self.lib.world_init.restype = ctypes.c_int
        self.lib.world_destroy.argtypes = [ctypes.POINTER(World)]
        self.lib.world_clear.argtypes = [ctypes.POINTER(World)]
        self.lib.world_set.argtypes = [ctypes.POINTER(World), ctypes.c_int, ctypes.c_int, ctypes.c_int]
        for name in ("world_step", "world_recompute_stats"):
            function = getattr(self.lib, name)
            function.argtypes = [ctypes.POINTER(World), ctypes.POINTER(StepStats)]
        for name in ("world_destroy", "world_clear", "world_set", "world_step", "world_recompute_stats"):
            getattr(self.lib, name).restype = None
        self.world = World()
        self.stats = StepStats()
        if self.lib.world_init(ctypes.byref(self.world), width, height, topology == "toroidal") != 0:
            raise RunError("C engine could not allocate the world")

    def install(self, live):
        self.lib.world_clear(ctypes.byref(self.world))
        for x, y in live:
            self.lib.world_set(ctypes.byref(self.world), x, y, 1)
        self.lib.world_recompute_stats(ctypes.byref(self.world), ctypes.byref(self.stats))

    def step(self):
        self.lib.world_step(ctypes.byref(self.world), ctypes.byref(self.stats))

    def board(self):
        return ctypes.string_at(self.world.cells, self.world.w * self.world.h)

    def rows(self):
        board = self.board()
        return ["".join("O" if cell else "." for cell in board[y * self.world.w:(y + 1) * self.world.w])
                for y in range(self.world.h)]

    def close(self):
        self.lib.world_destroy(ctypes.byref(self.world))


def sha256(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def provenance(library):
    source_paths = ("src/world.c", "src/world.h", "tools/research_run.py")
    sources = {name: sha256(APP_ROOT / name) for name in source_paths}
    try:
        revision = subprocess.run(["git", "-C", str(APP_ROOT), "rev-parse", "HEAD"],
                                  capture_output=True, text=True, check=False)
        revision_id = revision.stdout.strip() if revision.returncode == 0 else None
    except OSError:
        revision_id = None
    return {"source_revision": revision_id,
            "source_sha256": sources,
            "source_fingerprint": hashlib.sha256(json.dumps(sources, sort_keys=True).encode()).hexdigest(),
            "library_sha256": sha256(library)}


def run(manifest_path, output_path, library):
    manifest_path = Path(manifest_path).resolve()
    text = manifest_path.read_text(encoding="utf-8")
    manifest = json.loads(text, object_pairs_hook=unique_object,
                          parse_constant=lambda value: (_ for _ in ()).throw(RunError(f"invalid JSON constant: {value}")))
    expected_records = validate_manifest(manifest)
    output_path = Path(output_path)
    if not output_path.is_absolute():
        output_path = manifest_path.parent / output_path
    output_path = output_path.absolute()
    partial = output_path.with_name(output_path.name + ".partial")
    if os.path.lexists(output_path) or os.path.lexists(partial):
        raise RunError("output or partial output already exists")
    if not output_path.parent.is_dir():
        raise RunError("output directory does not exist")
    library = Path(library).resolve()
    code = provenance(library)
    engine = WorldEngine(library, manifest["width"], manifest["height"], manifest["topology"])
    try:
        engine.install(manifest["initial_live"])
        event = step = segment = tick = records = 0
        with partial.open("x", encoding="utf-8") as handle:
            def write(record):
                nonlocal records
                line = json.dumps(record, ensure_ascii=True, allow_nan=False, separators=(",", ":")) + "\n"
                if handle.write(line) != len(line):
                    raise RunError("short recording write")
                records += 1

            def snapshot(kind, transition_valid=False, **extra):
                return {"type": kind, "event_index": event, "step_index": step,
                        "segment": segment, "tick": tick, "rows": engine.rows(),
                        "population": engine.stats.live_cells, "transition_valid": transition_valid,
                        "stats": {name: getattr(engine.stats, name) for name, _ in StepStats._fields_} if transition_valid else None,
                        **extra}

            write({"type": "header", "schema": "gol-trace/1", "manifest": manifest,
                   "manifest_text": text, "manifest_sha256": hashlib.sha256(text.encode()).hexdigest(),
                   "metrics": {"native-step-counts": "1"}, "relations": {}, **code})
            write(snapshot("initial"))
            for action in manifest["actions"]:
                if action["kind"] == "step":
                    for _ in range(action["count"]):
                        engine.step()
                        event += 1
                        step += 1
                        tick += 1
                        write(snapshot("step", True, state_changed=bool(engine.stats.changed)))
                else:
                    before = engine.board()
                    engine.install(action["live"])
                    event += 1
                    segment += 1
                    tick = 0
                    write(snapshot("install", state_changed=before != engine.board()))
            write({"type": "completion", "status": "complete", "events": event, "steps": step,
                   "segments": segment + 1, "records": records + 1})
            if records != expected_records:
                raise RunError("record count disagrees with validated manifest")
            handle.flush()
            os.fsync(handle.fileno())
        # Same-directory hard-link publication is atomic and refuses to replace
        # an output created by another run after the initial existence check.
        os.link(partial, output_path)
        try:
            partial.unlink()
        except OSError as error:
            print(f"warning: completed output published; partial cleanup failed: {error}", file=sys.stderr)
    finally:
        engine.close()
    return output_path


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("manifest")
    parser.add_argument("output")
    parser.add_argument("--engine", type=Path, default=APP_ROOT / "build/research/world.so")
    args = parser.parse_args(argv)
    try:
        run(args.manifest, args.output, args.engine)
    except (OSError, ValueError, MemoryError, OverflowError) as error:
        print(f"research run failed: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
