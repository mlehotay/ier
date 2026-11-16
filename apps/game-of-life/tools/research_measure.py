#!/usr/bin/env python3
"""Measure complete fixed-world traces without evaluating a second Life rule."""

import argparse
from collections import deque
import hashlib
import json
import os
from pathlib import Path
import sys

from research_run import RunError, integer, unique_object, validate_manifest


def decode(text):
    def invalid(value):
        raise RunError(f"invalid JSON constant: {value}")
    return json.loads(text, object_pairs_hook=unique_object, parse_constant=invalid)


def live_set(rows, width, height):
    if (not isinstance(rows, list) or len(rows) != height
            or any(not isinstance(row, str) or len(row) != width or set(row) - {'.', 'O'} for row in rows)):
        raise RunError("invalid complete board rows")
    return frozenset((x, y) for y, row in enumerate(rows) for x, cell in enumerate(row) if cell == 'O')


def ordered(points):
    return [list(point) for point in sorted(points, key=lambda point: (point[1], point[0]))]


def extent(points):
    if not points:
        return None
    xs, ys = zip(*points)
    return [min(xs), min(ys), max(xs), max(ys)]


class Observations:
    """Bounded recurrence history, segment-wide interval sets, no rule engine."""

    def __init__(self, width, height, topology, history_limit):
        self.width, self.height, self.topology = width, height, topology
        self.history = deque(maxlen=integer(history_limit, "history limit", 2))
        self.segment = None

    def observe(self, snapshot, live):
        if snapshot['segment'] != self.segment:
            self.segment = snapshot['segment']
            self.history.clear()
            self.visited, self.continuous = set(live), set(live)
        else:
            self.visited.update(live)
            self.continuous.intersection_update(live)
        tick = snapshot['tick']
        previous = self.history[-1][1] if self.history else None
        turnover = None
        if previous is not None:
            survivors = len(previous & live)
            births, deaths = len(live - previous), len(previous - live)
            turnover = {'births': births, 'deaths': deaths, 'survivors': survivors,
                        'changed': births + deaths,
                        'retained_live_fraction': survivors / len(previous) if previous else None}
        self.history.append((tick, live))
        candidates = list(self.history)[:-1]
        search = {'from_tick': self.history[0][0], 'to_tick': tick,
                  'history_limit': self.history.maxlen, 'truncated': self.history[0][0] > 0}
        exact = {'status': 'insufficient_history' if not candidates else 'no_match', **search}
        translated = dict(exact)
        if self.topology == 'toroidal':
            translated['status'] = 'unsupported_topology'
        elif not live:
            translated['status'] = 'empty_live_set'
        for old_tick, old in reversed(candidates):
            lag = tick - old_tick
            if exact['status'] != 'match' and old == live:
                exact.update(status='match', lag=lag, match_from_tick=old_tick)
            if translated['status'] not in ('no_match', 'insufficient_history') or not old:
                continue
            # A finite nonempty set has at most one ordinary translation onto
            # another finite set: minima fix the vector. Always confirm the set.
            before, after = extent(old), extent(live)
            dx, dy = after[0] - before[0], after[1] - before[1]
            if frozenset((x + dx, y + dy) for x, y in old) != live:
                continue
            # Conservative dead-margin qualification for the whole comparison
            # interval: at least lag dead cells separate all live sites from
            # each wall. A failed check reports an observed equality separately.
            window = [points for time, points in self.history if time >= old_tick]
            margin = min((min(x, y, self.width-1-x, self.height-1-y)
                          for points in window for x, y in points), default=0)
            if margin < lag:
                # Continue looking for a qualified match at a larger lag; retain
                # the smallest unqualified equality if none qualifies.
                translated.setdefault('unqualified_match', {
                    'lag': lag, 'translation': [dx, dy], 'minimum_dead_margin': margin})
                continue
            translated.update(status='match', lag=lag, match_from_tick=old_tick,
                              translation=[dx, dy], velocity=[dx/lag, dy/lag],
                              minimum_dead_margin=margin)
        if translated['status'] == 'no_match' and 'unqualified_match' in translated:
            translated['status'] = 'insufficient_dead_margin'
        return {'type': 'observation',
                **{key: snapshot[key] for key in ('event_index', 'step_index', 'segment', 'tick')},
                'population': len(live), 'extent': extent(live), 'turnover': turnover,
                'interval': {'from_tick': 0, 'to_tick': tick,
                             'visited': ordered(self.visited), 'continuously_live': ordered(self.continuous)},
                'exact_recurrence': exact, 'translated_recurrence': translated}


class Trace:
    """Validate event, manifest and count integrity; this does not prove B3/S23."""

    def __init__(self, path):
        self.path = Path(path)
        if self.path.name.endswith('.partial'):
            raise RunError('partial trace is not a published recording')
        self.handle = self.path.open(encoding='utf-8', newline='')
        self.digest = hashlib.sha256()
        self.count = 0
        try:
            self.header = self.read()
            if self.header.get('type') != 'header' or self.header.get('schema') != 'gol-trace/1':
                raise RunError('unsupported trace header')
            self.manifest = self.header['manifest']
            self.expected = validate_manifest(self.manifest)
            text = self.header['manifest_text']
            decoded = decode(text)
            validate_manifest(decoded)
            if (decoded != self.manifest
                    or hashlib.sha256(text.encode()).hexdigest() != self.header['manifest_sha256']):
                raise RunError('manifest provenance mismatch')
        except BaseException:
            self.handle.close()
            raise

    def read(self):
        line = self.handle.readline()
        if not line:
            raise RunError('incomplete trace')
        self.digest.update(line.encode('utf-8'))
        self.count += 1
        record = decode(line)
        if not isinstance(record, dict):
            raise RunError('trace record must be an object')
        return record

    def snapshots(self):
        manifest = self.manifest
        event = step = segment = tick = 0
        previous = None

        def snapshot(kind, installed=None):
            nonlocal previous
            record = self.read()
            if record.get('type') != kind:
                raise RunError('trace event sequence disagrees with manifest')
            for key, expected in (('event_index', event), ('step_index', step), ('segment', segment), ('tick', tick)):
                if integer(record.get(key), key, 0) != expected:
                    raise RunError('trace event counters disagree with manifest')
            live = live_set(record.get('rows'), manifest['width'], manifest['height'])
            if integer(record.get('population'), 'population', 0) != len(live):
                raise RunError('trace population disagrees with complete board')
            if installed is not None and live != frozenset(map(tuple, installed)):
                raise RunError('installed board disagrees with manifest')
            if record.get('transition_valid') is not (kind == 'step'):
                raise RunError('invalid transition validity')
            if kind == 'step':
                stats = {'live_cells': len(live), 'births': len(live - previous),
                         'deaths': len(previous - live), 'survivors': len(live & previous),
                         'changed': len(live ^ previous)}
                native = record.get('stats')
                if (not isinstance(native, dict) or set(native) != set(stats)
                        or any(type(native[k]) is not int or native[k] != v for k, v in stats.items())):
                    raise RunError('native counts disagree with complete boards')
            elif record.get('stats') is not None:
                raise RunError('non-step counts must be unavailable')
            if previous is not None and record.get('state_changed') is not (live != previous):
                raise RunError('invalid state change flag')
            previous = live
            return record, live

        yield snapshot('initial', manifest['initial_live'])
        for action in manifest['actions']:
            if action['kind'] == 'install':
                event += 1
                segment += 1
                tick = 0
                yield snapshot('install', action['live'])
            else:
                for _ in range(action['count']):
                    event += 1
                    step += 1
                    tick += 1
                    yield snapshot('step')
        completion = self.read()
        expected = {'type': 'completion', 'status': 'complete', 'events': event,
                    'steps': step, 'segments': segment + 1, 'records': self.expected}
        if (completion != expected or any(type(completion.get(k)) is not int
                                         for k in ('events', 'steps', 'segments', 'records'))
                or self.count != self.expected or self.handle.read()):
            raise RunError('invalid completion or trailing trace data')

    def close(self):
        self.handle.close()


def matched_context(left, right):
    for key in ('width', 'height', 'topology', 'rule', 'protocol_id', 'proposal_revision', 'purpose'):
        if left.manifest[key] != right.manifest[key]:
            raise RunError(f'paired context mismatch: {key}')
    for key in ('protocol_sha256', 'proposal_sha256'):
        if left.manifest.get(key) != right.manifest.get(key):
            raise RunError(f'paired context mismatch: {key}')
    for key in ('source_fingerprint', 'library_sha256'):
        if left.header.get(key) != right.header.get(key) or not left.header.get(key):
            raise RunError(f'paired engine context mismatch: {key}')
    schedule = lambda manifest: [(a['kind'], a.get('count')) for a in manifest['actions']]
    if schedule(left.manifest) != schedule(right.manifest):
        raise RunError('paired event schedules differ')


def measure(trace_path, output_path, history_limit=64, pair_path=None):
    integer(history_limit, 'history limit', 2)
    # The maximum retained cell slots is explicit; interval sets cost at most
    # one complete world each. No partial trace or silent history interpolation.
    traces = []
    try:
        traces.append(Trace(trace_path))
        trace = traces[0]
        if history_limit * trace.manifest['width'] * trace.manifest['height'] > 16_777_216:
            raise RunError('retained history exceeds 16777216 cell slots')
        if pair_path is not None:
            traces.append(Trace(pair_path))
            matched_context(trace, traces[1])
        output = Path(output_path).absolute()
        partial = output.with_name(output.name + '.partial')
        if os.path.lexists(output) or os.path.lexists(partial):
            raise RunError('output or partial output already exists')
        observations = Observations(trace.manifest['width'], trace.manifest['height'],
                                    trace.manifest['topology'], history_limit)
        with partial.open('x', encoding='utf-8') as handle:
            def write(record):
                line = json.dumps(record, allow_nan=False, separators=(',', ':')) + '\n'
                if handle.write(line) != len(line):
                    raise RunError('short measurement write')
            write({'type': 'header', 'schema': 'gol-measures/1', 'history_limit': history_limit,
                   'analyzer_sha256': hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
                   'validator_sha256': hashlib.sha256(Path(__file__).with_name('research_run.py').read_bytes()).hexdigest(),
                   'run_id': trace.manifest['run_id'],
                   'paired_run_id': traces[1].manifest['run_id'] if len(traces) == 2 else None,
                   'relations': {}})
            generators = [item.snapshots() for item in traces]
            count = 0
            for snapshot, live in generators[0]:
                result = observations.observe(snapshot, live)
                if len(generators) == 2:
                    _, other = next(generators[1])
                    difference = live ^ other
                    result['paired_difference'] = {'count': len(difference), 'sites': ordered(difference)}
                write(result)
                count += 1
            if len(generators) == 2:
                if next(generators[1], None) is not None:
                    raise RunError('paired trace has extra snapshots')
            write({'type': 'completion', 'status': 'complete', 'observations': count,
                   'trace_sha256': [item.digest.hexdigest() for item in traces]})
            handle.flush()
            os.fsync(handle.fileno())
        os.link(partial, output)
        try:
            partial.unlink()
        except OSError as error:
            print(f'warning: completed output published; partial cleanup failed: {error}', file=sys.stderr)
        return output
    finally:
        for trace in traces:
            trace.close()


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('trace')
    parser.add_argument('output')
    parser.add_argument('--history-limit', type=int, default=64)
    parser.add_argument('--pair')
    args = parser.parse_args(argv)
    try:
        measure(args.trace, args.output, args.history_limit, args.pair)
    except (OSError, ValueError, KeyError, TypeError, StopIteration, MemoryError, OverflowError) as error:
        print(f'research measurement failed: {error}', file=sys.stderr)
        return 1
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
