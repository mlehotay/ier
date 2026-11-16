#!/usr/bin/env python3
"""Qualify fixed-world recording, independent phase boards and I/O failures."""

import contextlib
import copy
import importlib.util
import io
import json
from pathlib import Path
import subprocess
import sys
import tempfile
from unittest import mock


APP = Path(__file__).resolve().parents[1]
spec = importlib.util.spec_from_file_location("gol_research_run", APP / "tools/research_run.py")
runner = importlib.util.module_from_spec(spec)
spec.loader.exec_module(runner)
LIBRARY = None

# Literal mathematical fixtures from the worked examples, not computed by a
# Python implementation of Life. Coordinates are before the declared offset.
GLIDER = [
    [(1,0),(2,1),(0,2),(1,2),(2,2)],
    [(0,1),(2,1),(1,2),(2,2),(1,3)],
    [(2,1),(0,2),(2,2),(1,3),(2,3)],
    [(1,1),(2,2),(3,2),(1,3),(2,3)],
    [(2,1),(3,2),(1,3),(2,3),(3,3)],
]
TOAD = [
    [(1,1),(2,1),(3,1),(0,2),(1,2),(2,2)],
    [(2,0),(0,1),(3,1),(0,2),(3,2),(1,3)],
    [(1,1),(2,1),(3,1),(0,2),(1,2),(2,2)],
]


def shifted(points):
    return [[x+5, y+5] for x, y in points]


def rows(points):
    live = {tuple(pair) for pair in points}
    return ["".join("O" if (x,y) in live else "." for x in range(32)) for y in range(32)]


def manifest(initial=None, count=4):
    return {"schema":"gol-run/1", "run_id":"fixture", "protocol_id":"Q1-reconstruction/1",
            "proposal_revision":"0.2", "purpose":"reconstruction", "width":32, "height":32,
            "topology":"bounded", "rule":"B3/S23", "initial_live":shifted(GLIDER[0]) if initial is None else initial,
            "actions":[{"kind":"step", "count":count}], "max_cells":1024, "max_records":100}


def invoke(directory, value, output="trace.jsonl", raw=None):
    directory.mkdir(parents=True, exist_ok=True)
    input_path = directory / "manifest.json"
    input_path.write_text(json.dumps(value) if raw is None else raw)
    result = subprocess.run([sys.executable, str(APP / "tools/research_run.py"), str(input_path),
                             output, "--engine", str(LIBRARY)], cwd=APP, capture_output=True, text=True)
    return result, directory / output


def load(path):
    return [json.loads(line) for line in path.read_text().splitlines()]


def replay():
    with tempfile.TemporaryDirectory(prefix="gol-research-") as temporary:
        root = Path(temporary)
        for name, phases, counts in (("glider", GLIDER, (2,2,3,4)), ("toad", TOAD, (4,4,2,8))):
            for topology in ("bounded", "toroidal"):
                data = manifest(shifted(phases[0]), len(phases)-1)
                data["topology"] = topology
                result, output = invoke(root / (name + " " + topology), data)
                assert result.returncode == 0, result.stderr
                trace = load(output)
                snapshots = trace[1:-1]
                assert len(trace) == len(phases)+2
                for tick, (snapshot, phase) in enumerate(zip(snapshots, phases)):
                    assert snapshot["rows"] == rows(shifted(phase))
                    assert (snapshot["event_index"], snapshot["step_index"], snapshot["tick"], snapshot["segment"]) == (tick,tick,tick,0)
                    assert snapshot["population"] == len(phase)
                    assert snapshot["transition_valid"] == (tick != 0)
                    if tick:
                        assert tuple(snapshot["stats"][key] for key in ("births","deaths","survivors","changed")) == counts
                    else:
                        assert snapshot["stats"] is None
                assert trace[-1] == {"type":"completion", "status":"complete", "events":len(phases)-1,
                                     "steps":len(phases)-1, "segments":1, "records":len(trace)}
                assert not output.with_name(output.name+".partial").exists()
                # A fresh process using the same exact inputs reproduces boards
                # and native observations; provenance identifies actual code.
                again, second = invoke(output.parent, data, "second.jsonl")
                assert again.returncode == 0, again.stderr
                assert load(second) == trace
        empty = manifest([], 1)
        result, output = invoke(root / "empty", empty)
        assert result.returncode == 0, result.stderr
        assert all(row == "."*32 for row in load(output)[2]["rows"])
        assert load(output)[2]["stats"] == dict.fromkeys(("live_cells","births","deaths","survivors","changed"),0)
        block = manifest([[5,5],[6,5],[5,6],[6,6]], 1)
        result, output = invoke(root / "block", block)
        assert result.returncode == 0, result.stderr
        assert load(output)[1]["rows"] == load(output)[2]["rows"]
        assert load(output)[2]["transition_valid"] and not load(output)[2]["state_changed"]
        # Asymmetric transitions distinguish births from deaths in the native
        # ABI binding; balanced oscillator counts alone would not catch a swap.
        for name, initial, expected, statistics in (
            ("extinction", [[5,5]], [], {"live_cells":0,"births":0,"deaths":1,"survivors":0,"changed":1}),
            ("corner-birth", [[5,5],[6,5],[5,6]], [[5,5],[6,5],[5,6],[6,6]],
             {"live_cells":4,"births":1,"deaths":0,"survivors":3,"changed":1}),
        ):
            result, output = invoke(root / name, manifest(initial, 1))
            assert result.returncode == 0, result.stderr
            assert load(output)[2]["rows"] == rows(expected)
            assert load(output)[2]["stats"] == statistics


def inputs():
    base = manifest()
    variants = []
    for key, value in (("width",True),("height",0),("max_cells",1023),("max_records",6),
                       ("rule","B36/S23"),("topology","other"),("schema","other"),
                       ("initial_live",[[32,0]]),("initial_live",[[1,1],[1,1]]),
                       ("initial_live",[[1.0,1]]),("actions",[{"kind":"step","count":0}]),
                       ("actions",[{"kind":"step","count":True}]),
                       ("actions",[{"kind":"step","count":1,"extra":0}]),
                       ("actions",[{"kind":"step","count":1},{"kind":"install","live":[[-1,0]]}]),
                       ("actions",[{"kind":"edit","live":[]}]),("purpose","confirmatory")):
        changed = copy.deepcopy(base)
        changed[key] = value
        variants.append(changed)
    extra = copy.deepcopy(base); extra["unknown"] = 1; variants.append(extra)
    missing = copy.deepcopy(base); del missing["protocol_id"]; variants.append(missing)
    with tempfile.TemporaryDirectory(prefix="gol-inputs-") as temporary:
        root = Path(temporary)
        for i, value in enumerate(variants):
            result, output = invoke(root / str(i), value)
            assert result.returncode == 1 and "research run failed:" in result.stderr
            assert not output.exists() and not output.with_name(output.name+".partial").exists()
        raw = json.dumps(base)[:-1]+',"width":32}'
        result, output = invoke(root / "duplicates", base, raw=raw)
        assert result.returncode == 1 and "duplicate JSON key" in result.stderr
        assert not output.with_name(output.name+".partial").exists()
        for raw in ("{broken", json.dumps(base).replace('"max_cells": 1024', '"max_cells": NaN')):
            result, output = invoke(root / "malformed", base, raw=raw)
            assert result.returncode == 1 and not output.exists()
        zero = manifest([], 1); zero["actions"] = []; zero["max_records"] = 3
        result, output = invoke(root / "initial-only", zero)
        assert result.returncode == 0 and len(load(output)) == 3
        assert load(output)[-1]["steps"] == 0


def provenance_and_install():
    with tempfile.TemporaryDirectory(prefix="gol-provenance-") as temporary:
        root = Path(temporary)
        data = manifest(count=1)
        data["actions"] += [{"kind":"install", "live":shifted(GLIDER[1])}, {"kind":"step", "count":1}]
        result, output = invoke(root / "installed", data)
        assert result.returncode == 0, result.stderr
        trace = load(output)
        assert trace[2]["rows"] == trace[3]["rows"]
        assert trace[3]["type"] == "install" and not trace[3]["state_changed"]
        assert not trace[3]["transition_valid"] and trace[3]["stats"] is None
        assert (trace[3]["step_index"],trace[3]["segment"],trace[3]["tick"]) == (1,1,0)
        assert (trace[4]["step_index"],trace[4]["segment"],trace[4]["tick"]) == (2,1,1)
        assert trace[4]["rows"] == rows(shifted(GLIDER[2]))
        control, control_output = invoke(root / "computed", manifest(count=2))
        assert control.returncode == 0
        assert load(control_output)[3]["rows"] == trace[4]["rows"]
        header = trace[0]
        assert header["manifest"] == data and json.loads(header["manifest_text"]) == data
        assert header["library_sha256"] == runner.sha256(LIBRARY)
        for name, digest in header["source_sha256"].items():
            assert digest == runner.sha256(APP / name)
        assert header["relations"] == {} and header["metrics"] == {"native-step-counts":"1"}
        before = output.read_bytes()
        repeat, _ = invoke(output.parent, data)
        assert repeat.returncode == 1 and output.read_bytes() == before
        pending = root / "partial"; pending.mkdir()
        (pending / "trace.jsonl.partial").write_text("old partial")
        result, output = invoke(pending, data)
        assert result.returncode == 1 and not output.exists()
        assert output.with_name(output.name+".partial").read_text() == "old partial"
        dangling = root / "dangling"; dangling.mkdir()
        (dangling / "trace.jsonl").symlink_to(dangling / "missing")
        result, output = invoke(dangling, data)
        assert result.returncode == 1 and output.is_symlink()
        assert not output.with_name(output.name+".partial").exists()
        with mock.patch.object(runner.subprocess, "run", side_effect=FileNotFoundError("no git")):
            assert runner.provenance(LIBRARY)["source_revision"] is None


class FailingOutput:
    def __init__(self, handle, failure):
        self.handle, self.failure, self.writes = handle, failure, 0

    def __enter__(self):
        return self

    def __getattr__(self, name):
        return getattr(self.handle, name)

    def write(self, text):
        self.writes += 1
        if self.failure == "write" and self.writes == 3:
            raise OSError("injected recording write failure")
        if self.failure == "short-write" and self.writes == 3:
            return 0
        return self.handle.write(text)

    def flush(self):
        if self.failure == "flush":
            raise OSError("injected flush failure")
        return self.handle.flush()

    def __exit__(self, *_):
        self.handle.close()
        if self.failure == "close":
            raise OSError("injected close failure")


def io_failures():
    original_open = Path.open
    original_close = runner.WorldEngine.close
    with tempfile.TemporaryDirectory(prefix="gol-io-") as temporary:
        root = Path(temporary)
        for failure in ("write", "short-write", "flush", "close", "fsync", "publish"):
            directory = root / failure; directory.mkdir()
            input_path = directory / "manifest.json"; input_path.write_text(json.dumps(manifest()))
            output = directory / "trace.jsonl"
            def open_file(path, *args, **kwargs):
                handle = original_open(path, *args, **kwargs)
                return FailingOutput(handle, failure) if path.name.endswith(".partial") else handle
            def racing_publish(source, target):
                Path(target).write_text("other run")
                raise FileExistsError("injected concurrent publication")
            stderr = io.StringIO()
            with contextlib.ExitStack() as stack:
                stack.enter_context(mock.patch.object(Path, "open", open_file))
                closed = stack.enter_context(mock.patch.object(runner.WorldEngine, "close", autospec=True, side_effect=original_close))
                if failure == "fsync":
                    stack.enter_context(mock.patch.object(runner.os, "fsync", side_effect=OSError("injected fsync")))
                if failure == "publish":
                    stack.enter_context(mock.patch.object(runner.os, "link", side_effect=racing_publish))
                stack.enter_context(contextlib.redirect_stderr(stderr))
                result = runner.main([str(input_path), str(output), "--engine", str(LIBRARY)])
                assert result == 1 and "research run failed:" in stderr.getvalue()
                assert closed.call_count == 1 and not closed.call_args.args[0].world.cells
            assert output.with_name(output.name+".partial").exists()
            if failure == "publish":
                assert output.read_text() == "other run"
            else:
                assert not output.exists()
        directory = root / "allocation"; directory.mkdir()
        path = directory / "manifest.json"; path.write_text(json.dumps(manifest()))
        with mock.patch.object(runner, "WorldEngine", side_effect=MemoryError("injected allocation")), contextlib.redirect_stderr(io.StringIO()):
            assert runner.main([str(path), str(directory/"out"), "--engine", str(LIBRARY)]) == 1
        assert not (directory/"out").exists() and not (directory/"out.partial").exists()
        failed_library = mock.Mock()
        failed_library.world_init.return_value = -1
        with mock.patch.object(runner.ctypes, "CDLL", return_value=failed_library), contextlib.redirect_stderr(io.StringIO()):
            assert runner.main([str(path), str(directory/"native-out"), "--engine", str(LIBRARY)]) == 1
        assert not (directory/"native-out.partial").exists()


def main():
    global LIBRARY
    cases = {"replay":replay, "inputs":inputs, "provenance":provenance_and_install, "io-failures":io_failures}
    if len(sys.argv) != 3 or (sys.argv[2] != "all" and sys.argv[2] not in cases):
        print(f"Usage: {sys.argv[0]} ENGINE [all|{'|'.join(cases)}]", file=sys.stderr)
        return 2
    LIBRARY = Path(sys.argv[1]).resolve()
    selected = cases if sys.argv[2] == "all" else {sys.argv[2]:cases[sys.argv[2]]}
    for name, test in selected.items():
        try:
            test()
        except (AssertionError, OSError, ValueError, subprocess.SubprocessError) as error:
            print(f"FAIL research-{name}: {error}", file=sys.stderr)
            return 1
        print(f"PASS research-{name}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
