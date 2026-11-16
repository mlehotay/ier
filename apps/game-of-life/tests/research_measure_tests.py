#!/usr/bin/env python3
"""Qualify observations against literal boards and malformed-record controls."""

import contextlib
import copy
import io
import json
from pathlib import Path
import subprocess
import sys
import tempfile
from unittest import mock

APP = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(APP / 'tools'))
import research_measure as measure
import research_run as runner
from research_run_tests import GLIDER, TOAD, FailingOutput, manifest, shifted

LIBRARY = None


def snapshot(segment, tick):
    return {'segment': segment, 'tick': tick, 'event_index': tick, 'step_index': tick}


def phases():
    for points, union, intersection, lag, vector in (
        (GLIDER, 11, 0, 4, [1, 1]), (TOAD, 10, 2, 2, [0, 0]),
    ):
        observations = measure.Observations(32, 32, 'bounded', 8)
        for tick, phase in enumerate(points):
            result = observations.observe(snapshot(0, tick), frozenset(map(tuple, shifted(phase))))
            if tick:
                counts = (2, 2, 3, 4, 3/5) if points is GLIDER else (4, 4, 2, 8, 1/3)
                assert tuple(result['turnover'].values()) == counts
            else:
                assert result['turnover'] is None
                assert result['exact_recurrence']['status'] == 'insufficient_history'
        assert len(result['interval']['visited']) == union
        assert len(result['interval']['continuously_live']) == intersection
        assert result['translated_recurrence']['lag'] == lag
        assert result['translated_recurrence']['translation'] == vector
        assert result['translated_recurrence']['velocity'] == [v/lag for v in vector]
        assert result['extent'] == ([6, 6, 8, 8] if points is GLIDER else [5, 6, 8, 7])
        assert result['exact_recurrence']['status'] == ('no_match' if points is GLIDER else 'match')
    block = frozenset(((5,5), (6,5), (5,6), (6,6)))
    for points in (block, frozenset()):
        observations = measure.Observations(32, 32, 'bounded', 2)
        first = observations.observe(snapshot(0, 0), points)
        assert first['extent'] == ([5,5,6,6] if points else None)
        for tick in range(1, 5):
            result = observations.observe(snapshot(0, tick), points)
            assert result['exact_recurrence']['lag'] == 1
            assert result['turnover']['retained_live_fraction'] == (1 if points else None)
        assert result['exact_recurrence']['from_tick'] == 3
        assert result['exact_recurrence']['truncated']
        assert result['translated_recurrence']['status'] == ('match' if points else 'empty_live_set')
        assert result['interval']['to_tick'] == 4
        assert len(result['interval']['visited']) == len(points)
        reset = observations.observe(snapshot(1, 0), points)
        assert reset['turnover'] is None and not reset['exact_recurrence']['truncated']
        assert reset['exact_recurrence']['status'] == 'insufficient_history'


def limits():
    # A history too short for the two-step oscillator cannot infer a period.
    observations = measure.Observations(32, 32, 'bounded', 2)
    for tick, phase in enumerate(TOAD):
        result = observations.observe(snapshot(0, tick), frozenset(map(tuple, shifted(phase))))
    assert result['exact_recurrence']['status'] == 'no_match'
    assert result['exact_recurrence']['truncated']
    assert len(result['interval']['visited']) == 10  # Interval sets survive eviction.
    # A literal translating edge set matches by values but fails margin scope.
    observations = measure.Observations(8, 8, 'bounded', 4)
    observations.observe(snapshot(0, 0), frozenset(((0,1), (1,1))))
    result = observations.observe(snapshot(0, 1), frozenset(((1,1), (2,1))))
    assert result['translated_recurrence']['status'] == 'insufficient_dead_margin'
    assert result['translated_recurrence']['unqualified_match']['translation'] == [1,0]
    for points in (frozenset(), frozenset(((0,1), (7,1)))):
        observations = measure.Observations(8, 8, 'toroidal', 4)
        observations.observe(snapshot(0, 0), points)
        result = observations.observe(snapshot(0, 1), points)
        assert result['exact_recurrence']['lag'] == 1
        assert result['translated_recurrence']['status'] == 'unsupported_topology'
    # Count and extent agreement cannot substitute for full-set equality.
    observations = measure.Observations(32, 32, 'bounded', 4)
    observations.observe(snapshot(0, 0), frozenset(((5,5), (7,7), (5,7))))
    result = observations.observe(snapshot(0, 1), frozenset(((5,5), (7,7), (7,5))))
    assert result['translated_recurrence']['status'] == 'no_match'


def record(root, name, data):
    path = root / (name + '.json')
    path.write_text(json.dumps(data))
    output = root / (name + '.jsonl')
    runner.run(path, output, LIBRARY)
    return output


def load(path):
    return [json.loads(line) for line in path.read_text().splitlines()]


def paired_and_cli():
    with tempfile.TemporaryDirectory(prefix='gol-measures-') as temporary:
        root = Path(temporary)
        # Existing independent-block near/far controls; removing the right
        # block leaves exactly its four sites different for all 32 steps.
        left = [[2,2],[3,2],[2,3],[3,3]]
        for right in (6, 8):
            extra = [[right,2],[right+1,2],[right,3],[right+1,3]]
            data = manifest(copy.deepcopy(left), 32)
            control = record(root, f'left-{right}', data)
            data['initial_live'] += extra
            joint = record(root, f'joint-{right}', data)
            output = root / f'paired-{right}.jsonl'
            result = subprocess.run([sys.executable, str(APP / 'tools/research_measure.py'),
                                     str(joint), str(output), '--pair', str(control), '--history-limit', '4'],
                                    capture_output=True, text=True)
            assert result.returncode == 0, result.stderr
            records = load(output)
            assert len(records) == 35
            for item in records[1:-1]:
                assert item['paired_difference'] == {'count': 4, 'sites': extra}
                assert item['population'] == 8
                assert len(item['interval']['visited']) == len(item['interval']['continuously_live']) == 8
            assert records[-1]['observations'] == 33
            assert records[-1]['trace_sha256'] == [runner.sha256(joint), runner.sha256(control)]
            assert records[0]['analyzer_sha256'] == runner.sha256(APP / 'tools/research_measure.py')
            again = root / f'again-{right}.jsonl'
            measure.measure(joint, again, 4, control)
            assert again.read_bytes() == output.read_bytes()
        # Equal-board install resets the analysis even when the future agrees.
        data = manifest(count=1)
        data['actions'] += [{'kind':'install', 'live':shifted(GLIDER[1])}, {'kind':'step', 'count':1}]
        trace = record(root, 'install', data)
        output = root / 'install-measured.jsonl'
        measure.measure(trace, output, 4)
        records = load(output)
        assert records[3]['turnover'] is None
        assert records[3]['interval']['from_tick'] == records[3]['interval']['to_tick'] == 0
        assert records[3]['exact_recurrence']['status'] == 'insufficient_history'
        assert records[4]['turnover']['births'] == 2
        # Initial-only traces have no fabricated transition or repeat.
        data = manifest([]); data['actions'] = []
        trace = record(root, 'initial-only', data)
        output = root / 'initial-only-measured.jsonl'
        measure.measure(trace, output, 2)
        assert len(load(output)) == 3 and load(output)[1]['turnover'] is None


def invalid():
    with tempfile.TemporaryDirectory(prefix='gol-invalid-measures-') as temporary:
        root = Path(temporary)
        trace = record(root, 'source', manifest())
        pending = root / 'trace.partial'
        pending.write_bytes(trace.read_bytes())
        with contextlib.redirect_stderr(io.StringIO()):
            assert measure.main([str(pending), str(root/'partial-input-output')]) == 1
        assert not (root/'partial-input-output.partial').exists()
        base = load(trace)
        variants = []
        for index, key, value in (
            (2, 'tick', 2), (2, 'tick', True), (2, 'population', 99),
            (2, 'rows', ['.']), (2, 'transition_valid', False), (2, 'state_changed', False),
            (2, 'stats', {'births':2}), (1, 'rows', ['.'*32]*32),
            (0, 'manifest_sha256', '0'*64), (-1, 'records', 0), (-1, 'events', True),
        ):
            changed = copy.deepcopy(base)
            changed[index][key] = value
            variants.append(changed)
        variants.extend((base[:-1], base + [base[-1]], base[:2] + base[3:]))
        for i, records in enumerate(variants):
            path = root / f'invalid-{i}.jsonl'
            path.write_text(''.join(json.dumps(row)+'\n' for row in records))
            output = root / f'rejected-{i}.jsonl'
            with contextlib.redirect_stderr(io.StringIO()):
                assert measure.main([str(path), str(output)]) == 1
            assert not output.exists()
        for i, raw in enumerate(('{}\n', '{broken', '[]\n',
                                 json.dumps(base[0])[:-1]+',"type":"header"}\n')):
            path = root / f'malformed-{i}'
            path.write_text(raw)
            with contextlib.redirect_stderr(io.StringIO()):
                assert measure.main([str(path), str(root/f'malformed-out-{i}')]) == 1
        # Paired context must match beyond population or nominal generation.
        for i, (key, value) in enumerate((('topology','toroidal'), ('protocol_id','other'),
                                         ('actions',[{'kind':'step','count':3}]))):
            data = manifest(); data[key] = value
            pair = record(root, f'mismatch-{i}', data)
            with contextlib.redirect_stderr(io.StringIO()):
                assert measure.main([str(trace), str(root/f'pair-{i}'), '--pair', str(pair)]) == 1
            assert not (root/f'pair-{i}.partial').exists()
        for limit in ('0', '1', 'true', '100000000'):
            with contextlib.redirect_stderr(io.StringIO()):
                try:
                    assert measure.main([str(trace), str(root/'limit'), '--history-limit', limit]) == 1
                except SystemExit as error:
                    assert error.code == 2
            assert not (root/'limit.partial').exists()
        for name in ('existing', 'dangling', 'partial'):
            output = root / name
            if name == 'existing':
                output.write_text('preserve')
            elif name == 'dangling':
                output.symlink_to(root/'absent')
            else:
                (root/'partial.partial').write_text('preserve')
            with contextlib.redirect_stderr(io.StringIO()):
                assert measure.main([str(trace), str(output)]) == 1
            if name == 'existing':
                assert output.read_text() == 'preserve'
            elif name == 'dangling':
                assert output.is_symlink()
            else:
                assert (root/'partial.partial').read_text() == 'preserve'
        # I/O failure cannot turn incomplete analysis into a completed result.
        for name in ('fsync', 'link'):
            with mock.patch.object(measure.os, name, side_effect=OSError('injected failure')):
                with contextlib.redirect_stderr(io.StringIO()):
                    assert measure.main([str(trace), str(root/name)]) == 1
            assert not (root/name).exists() and (root/(name+'.partial')).exists()
        original_open = Path.open
        for failure in ('write', 'short-write', 'flush', 'close'):
            def open_file(path, *args, **kwargs):
                handle = original_open(path, *args, **kwargs)
                return FailingOutput(handle, failure) if path.name.endswith('.partial') else handle
            output = root / failure
            with mock.patch.object(Path, 'open', open_file), contextlib.redirect_stderr(io.StringIO()):
                assert measure.main([str(trace), str(output)]) == 1
            assert not output.exists() and output.with_name(output.name+'.partial').exists()
        # A concurrent publisher wins without being overwritten.
        output = root / 'race'
        def racing_publish(source, target):
            Path(target).write_text('other analysis')
            raise FileExistsError('concurrent publication')
        with mock.patch.object(measure.os, 'link', racing_publish), contextlib.redirect_stderr(io.StringIO()):
            assert measure.main([str(trace), str(output)]) == 1
        assert output.read_text() == 'other analysis'


def main():
    global LIBRARY
    cases = {'phases':phases, 'limits':limits, 'paired':paired_and_cli, 'invalid':invalid}
    if len(sys.argv) != 3 or (sys.argv[2] != 'all' and sys.argv[2] not in cases):
        print(f"Usage: {sys.argv[0]} ENGINE [all|{'|'.join(cases)}]", file=sys.stderr)
        return 2
    LIBRARY = Path(sys.argv[1]).resolve()
    selected = cases if sys.argv[2] == 'all' else {sys.argv[2]: cases[sys.argv[2]]}
    for name, test in selected.items():
        test()
        print(f'PASS measures-{name}')
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
