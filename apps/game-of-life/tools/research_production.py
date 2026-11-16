#!/usr/bin/env python3
"""Export exact ON/OFF neighborhoods and declared paired-run observations."""

import argparse
import hashlib
import os
from pathlib import Path
import sys
import json

from research_measure import Trace, decode, matched_context, ordered
from research_run import RunError, integer, live_cells


OFFSETS = tuple((dx, dy) for dy in (-1, 0, 1) for dx in (-1, 0, 1))
FIELDS = {'schema', 'context_id', 'trace_run_id', 'proposal_revision',
          'selection_reason', 'targets', 'max_updates'}


def validate_context(context, manifest):
    if not isinstance(context, dict) or set(context) != FIELDS:
        raise RunError('missing or unknown production context fields')
    if context['schema'] != 'gol-production-context/1':
        raise RunError('unsupported production context schema')
    for key in ('context_id', 'trace_run_id', 'proposal_revision', 'selection_reason'):
        if not isinstance(context[key], str) or not context[key].strip():
            raise RunError(f'{key} must be a nonempty string')
    if context['trace_run_id'] != manifest['run_id'] or context['proposal_revision'] != manifest['proposal_revision']:
        raise RunError('production context does not match primary trace')
    targets = context['targets']
    if targets == 'all':
        count = manifest['width'] * manifest['height']
    else:
        live_cells(targets, manifest['width'], manifest['height'], 'targets')
        if not targets:
            raise RunError('targets must not be empty')
        count = len(targets)
    steps = sum(action['count'] for action in manifest['actions'] if action['kind'] == 'step')
    updates = count * steps
    if updates > min(1_000_000, integer(context['max_updates'], 'max_updates')):
        raise RunError('production export exceeds update limit')
    return updates


def targets(context, manifest):
    if context['targets'] == 'all':
        for y in range(manifest['height']):
            for x in range(manifest['width']):
                yield x, y
    else:
        yield from context['targets']


def occurrence(snapshot):
    return {key: snapshot[key] for key in ('type', 'event_index', 'step_index', 'segment', 'tick')}


def neighborhood(live, after, target, manifest):
    x, y = target
    width, height = manifest['width'], manifest['height']
    inputs = []
    for dx, dy in OFFSETS:
        px, py = x + dx, y + dy
        if manifest['topology'] == 'toroidal':
            px, py = px % width, py % height
        exterior = not (0 <= px < width and 0 <= py < height)
        inputs.append({'offset': [dx, dy], 'site': None if exterior else [px, py],
                       'value': 0 if exterior else int((px, py) in live)})
    center, output = inputs[4]['value'], int((x, y) in after)
    if output:
        process = 'ON' if center else 'production'
    elif center:
        process = 'destruction'
    else:
        process = 'OFF' if any(item['value'] for item in inputs) else 'null'
    return {'inputs': inputs, 'neighbor_count': sum(item['value'] for i, item in enumerate(inputs) if i != 4),
            'output': output, 'process': process}


def export(trace_path, context_path, output_path, pair_path=None):
    traces = []
    try:
        traces.append(Trace(trace_path))
        trace = traces[0]
        context_text = Path(context_path).read_text(encoding='utf-8')
        context = decode(context_text)
        expected_updates = validate_context(context, trace.manifest)
        if pair_path is not None:
            traces.append(Trace(pair_path))
            matched_context(trace, traces[1])
        output = Path(output_path).absolute()
        partial = output.with_name(output.name + '.partial')
        if os.path.lexists(output) or os.path.lexists(partial):
            raise RunError('output or partial output already exists')
        with partial.open('x', encoding='utf-8') as handle:
            records = updates = events = 0

            def write(record):
                nonlocal records
                line = json.dumps(record, allow_nan=False, separators=(',', ':')) + '\n'
                if handle.write(line) != len(line):
                    raise RunError('short production write')
                records += 1

            sources = {name: hashlib.sha256(Path(__file__).with_name(name).read_bytes()).hexdigest()
                       for name in ('research_production.py', 'research_measure.py', 'research_run.py')}
            write({'type': 'header', 'schema': 'gol-production/1',
                   'relations': {'exact-neighborhood': '1'}, 'source_sha256': sources,
                   'context': context, 'context_text': context_text,
                   'context_sha256': hashlib.sha256(context_text.encode()).hexdigest(),
                   'manifests': [item.manifest for item in traces]})
            generators = [item.snapshots() for item in traces]
            previous = previous_other = previous_snapshot = None
            for snapshot, live in generators[0]:
                event = {'type': 'event', 'occurrence': occurrence(snapshot),
                         'population': snapshot['population'],
                         'state_changed': snapshot.get('state_changed')}
                other = None
                if len(generators) == 2:
                    _, other = next(generators[1])
                    event['paired_board_difference'] = ordered(live ^ other)
                write(event)
                events += 1
                if snapshot['type'] == 'step':
                    for x, y in targets(context, trace.manifest):
                        result = {'type': 'update', 'target': [x, y],
                                  'source_event': occurrence(previous_snapshot),
                                  'result_event': occurrence(snapshot),
                                  **neighborhood(previous, live, (x, y), trace.manifest)}
                        if other is not None:
                            comparison = neighborhood(previous_other, other, (x, y), trace.manifest)
                            result['paired'] = comparison
                            result['input_difference_offsets'] = [a['offset'] for a, b in zip(result['inputs'], comparison['inputs'])
                                                                  if a['value'] != b['value']]
                            result['output_differs'] = result['output'] != comparison['output']
                        write(result)
                        updates += 1
                previous, previous_other, previous_snapshot = live, other, snapshot
            if len(generators) == 2 and next(generators[1], None) is not None:
                raise RunError('paired trace has extra snapshots')
            if updates != expected_updates:
                raise RunError('update count disagrees with declared context')
            write({'type': 'completion', 'status': 'complete', 'updates': updates, 'events': events,
                   'records': records + 1, 'trace_sha256': [item.digest.hexdigest() for item in traces]})
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
    parser.add_argument('context')
    parser.add_argument('output')
    parser.add_argument('--pair')
    args = parser.parse_args(argv)
    try:
        export(args.trace, args.context, args.output, args.pair)
    except (OSError, ValueError, KeyError, TypeError, StopIteration, MemoryError, OverflowError) as error:
        print(f'research production failed: {error}', file=sys.stderr)
        return 1
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
