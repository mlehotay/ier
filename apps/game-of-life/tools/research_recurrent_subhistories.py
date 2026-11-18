#!/usr/bin/env python3
"""Enumerate declared recurrent subhistories through the native Life engine.

First run make build/research/world.so from apps/game-of-life. The report tests
Candidate E's bounded model predictions, not constitutive or UEF membership.
"""

import hashlib
import json
from itertools import combinations
from collections import Counter
from pathlib import Path
import sys

APP = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(APP / 'tools'))
from research_run import WorldEngine
from research_cross_cases import GLIDER, TOAD

SIZE = 24


def shifted(points, dx, dy):
    return frozenset((x + dx, y + dy) for x, y in points)


def fixtures():
    block = (frozenset(((0, 0), (1, 0), (0, 1), (1, 1))),)
    blinker = (frozenset(((0, 1), (1, 1), (2, 1))),
               frozenset(((1, 0), (1, 1), (1, 2))))
    result = []
    for name, phases, delta in (('block', block, (0, 0)), ('blinker', blinker, (0, 0)),
                                ('toad', TOAD, (0, 0)), ('glider', GLIDER, (1, 1))):
        literal = tuple(shifted(phase, 6, 6) for phase in phases)
        literal += (shifted(literal[0], *delta),)
        result.append((name, literal, delta, (literal[0],)))
    for name, phases, right in (
            ('blocks-near', block, (10, 6)), ('blocks-far', block, (12, 6)),
            ('toads-near', TOAD, (10, 2)), ('toads-far', TOAD, (14, 6))):
        left_phases = tuple(shifted(phase, 6, 6) for phase in phases)
        right_phases = tuple(shifted(phase, *right) for phase in phases)
        literal = tuple(a | b for a, b in zip(left_phases, right_phases))
        literal += (literal[0],)
        result.append((name, literal, (0, 0), (left_phases[0], right_phases[0])))
    return result


def inspect(engine):
    board = engine.board()
    assert all(bit in (0, 1) for bit in board)
    return frozenset((j % SIZE, j // SIZE) for j, bit in enumerate(board) if bit), board


def audit(library, topology, fixture):
    name, reference, delta, predicted_parts = fixture
    period = len(reference) - 1
    points = tuple(sorted(reference[0]))
    engine = WorldEngine(library, SIZE, SIZE, topology)
    accepted = {}
    rejected = Counter()
    transcript = hashlib.sha256()
    try:
        engine.install(reference[0])
        for tick, expected in enumerate(reference):
            if tick:
                engine.step()
            assert inspect(engine)[0] == expected, (topology, name, tick, 'literal oracle')
        for mask in range(1 << len(points)):
            initial = frozenset(point for j, point in enumerate(points) if mask & (1 << j))
            engine.install(initial)
            history = []
            transcript.update(mask.to_bytes(2, 'little'))
            for tick in range(period + 1):
                if tick:
                    engine.step()
                sites, board = inspect(engine)
                history.append(sites)
                transcript.update(board)
            assert history[0] == initial
            if not mask:
                assert all(not phase for phase in history)
                rejected['empty-control'] += 1
            elif any(not y <= x for y, x in zip(history, reference)):
                rejected['outside-reference'] += 1
            elif history[-1] != shifted(initial, *delta):
                rejected['recurrence-failed'] += 1
            else:
                accepted[mask] = tuple(history)
    finally:
        engine.close()
    occurrences = {mask: frozenset((t, x, y) for t, phase in enumerate(history[:-1])
                                    for x, y in phase)
                   for mask, history in accepted.items()}
    minimal = {mask for mask, events in occurrences.items()
               if not any(other < events for other in occurrences.values())}
    phase_minimal = {mask for mask, history in accepted.items()
                     if not any(other_mask != mask and all(a <= b for a, b in zip(other, history))
                                for other_mask, other in accepted.items())}
    assert minimal == phase_minimal
    assert len(accepted) + sum(rejected.values()) == 1 << len(points)
    actual_parts = {accepted[mask][0] for mask in minimal}
    coverage = all(frozenset().union(*(accepted[mask][t] for mask in minimal)) == reference[t]
                   for t in range(period + 1))
    return {
        'fixture': name, 'topology': topology, 'period': period, 'translation': delta,
        'initial_subsets_checked': 1 << len(points), 'rejections': dict(sorted(rejected.items())),
        'admissible_histories': [
            {'mask': mask, 'initial_sites': sorted(history[0]),
             'on_occurrences': len(occurrences[mask]),
             'phase_sites': [sorted(phase) for phase in history]}
            for mask, history in sorted(accepted.items())],
        'elementary_masks': sorted(minimal), 'minima_cover_reference': coverage,
        'predicted_minima_match': actual_parts == set(predicted_parts),
        'all_native_boards_sha256': transcript.hexdigest(),
    }


def support_profile(library, topology, current, targets):
    """All minimal actual OFF certificates with current ON literals fixed."""
    current, targets = frozenset(current), frozenset(targets)
    def neighborhood(site):
        x, y = site
        result = set()
        for dy in (-1, 0, 1):
            for dx in (-1, 0, 1):
                if not (dx or dy):
                    continue
                u, v = x + dx, y + dy
                if topology == 'toroidal':
                    result.add((u % SIZE, v % SIZE))
                elif 0 <= u < SIZE and 0 <= v < SIZE:
                    result.add((u, v))
        return frozenset(result)
    clauses = []
    mandatory = frozenset()
    domain = frozenset().union(*(neighborhood(t) | {t} for t in targets)) - current
    for target in sorted(targets):
        neighbors = neighborhood(target)
        count = len(neighbors & current)
        off = neighbors - current
        assert count == 3 or (count == 2 and target in current)
        assert len(neighbors) == 8  # These fixtures never touch a finite boundary.
        clauses.append((target, count, off))
        if count == 3:
            mandatory |= off
    family = {mandatory}
    for target, count, off in clauses:
        if count == 2:
            candidates = {fixed | frozenset(choice) for fixed in family
                          for choice in combinations(sorted(off), 5)}
            family = {fixed for fixed in candidates if not any(other < fixed for other in candidates)}
    engine = WorldEngine(library, SIZE, SIZE, topology)
    cache = {}
    transcript = hashlib.sha256()
    def run(added):
        added = frozenset(added)
        assert added <= domain
        if added not in cache:
            engine.install(current | added)
            transcript.update(engine.board())
            engine.step()
            board = engine.board()
            transcript.update(board)
            cache[added] = board
        return cache[added]
    certificates = []
    try:
        for fixed in sorted(family, key=lambda sites: tuple(sorted(sites))):
            free = tuple(sorted(domain - fixed))
            for assignment in range(1 << len(free)):
                added = {site for bit, site in enumerate(free) if assignment & (1 << bit)}
                board = run(added)
                assert all(board[y*SIZE+x] for x, y in targets)
            witnesses = []
            for literal in sorted(fixed):
                critical = next(((target, {literal}) for target, count, off in clauses
                                 if count == 3 and literal in off), None)
                if critical is None:
                    critical = next(((target, {literal} | (off - fixed))
                                     for target, count, off in clauses
                                     if count == 2 and literal in off and len(off - fixed) == 1), None)
                assert critical is not None
                target, added = critical
                assert (added & fixed) == {literal}
                board = run(added)
                assert board[target[1]*SIZE+target[0]] == 0
                if len(added) == 2:
                    for singleton in added:
                        alone = run({singleton})
                        assert all(alone[y*SIZE+x] for x, y in targets)
                witnesses.append({'removed_off': literal, 'added_on': sorted(added),
                                  'lost_target': target})
            certificates.append({'off_sites': sorted(fixed), 'off_count': len(fixed),
                                 'free_local_inputs': len(free), 'completion_checks': 1 << len(free),
                                 'necessity_witnesses': witnesses})
    finally:
        engine.close()
    if len(current) == 4 and current == targets:
        xs, ys = {x for x, y in current}, {y for x, y in current}
        if len(xs) == len(ys) == 2 and max(xs)-min(xs) == max(ys)-min(ys) == 1:
            ring = {(x, y) for x in range(min(xs)-1, max(xs)+2)
                    for y in range(min(ys)-1, max(ys)+2)} - current
            assert family == {frozenset(ring)} and len(ring) == 12
    return {'certificates': certificates, 'native_steps': len(cache),
            'native_boards_sha256': transcript.hexdigest()}


def add_support_profiles(library, report, fixture):
    name, reference, _, _ = fixture
    histories = {row['mask']: tuple(frozenset(tuple(site) for site in phase)
                                   for phase in row['phase_sites'])
                 for row in report['admissible_histories']}
    profiles = []
    for phase in range(report['period']):
        parts = [support_profile(library, report['topology'], histories[mask][phase],
                                 histories[mask][phase+1]) for mask in report['elementary_masks']]
        joint = parts[0] if len(parts) == 1 else support_profile(
            library, report['topology'], reference[phase], reference[phase+1])
        overlaps = []
        if len(parts) == 2:
            for left in parts[0]['certificates']:
                for right in parts[1]['certificates']:
                    a = set(map(tuple, left['off_sites']))
                    b = set(map(tuple, right['off_sites']))
                    overlaps.append(len(a & b))
        if name.startswith('blocks-'):
            assert len(joint['certificates']) == 1
            assert joint['certificates'][0]['off_count'] == 24
        profiles.append({'phase': phase, 'parts': parts, 'joint': joint,
                         'factor_off_overlap_range': [min(overlaps), max(overlaps)] if overlaps else None})
    report['support_profiles'] = profiles


def support_renewal_witness(library):
    core = frozenset(((6, 6), (7, 6), (6, 7), (7, 7)))
    ring = frozenset((x, y) for x in range(5, 9) for y in range(5, 9)) - core
    edit = (4, 6)
    births = frozenset(((5, 6), (5, 7)))
    for topology in ('bounded', 'toroidal'):
        engine = WorldEngine(library, SIZE, SIZE, topology)
        try:
            engine.install(core | {edit})
            assert not (ring & (core | {edit}))
            engine.step()
            assert inspect(engine)[0] == core | births
            assert engine.stats.births == 2 and engine.stats.deaths == 1
        finally:
            engine.close()
    return {'current_on': sorted(core), 'current_off_certificate': sorted(ring),
            'outside_edit': edit, 'next_on': sorted(core | births),
            'scope': 'core ON guarantee holds while its enabling OFF ring is not preserved'}


def support_context_contrasts(library):
    core = frozenset(((6, 6), (7, 6), (6, 7), (7, 7)))
    ring = frozenset((x, y) for x in range(5, 9) for y in range(5, 9)) - core
    target = (5, 6)
    one = frozenset(((4, 6),))
    three = frozenset(((4, 5), (4, 6), (4, 7)))
    cases = (
        ('quiet-alone', frozenset(), frozenset(), (0, 0, 0)),
        ('quiet-body', core, core, (0, 0, 4)),
        ('one-alone', one, frozenset(), (0, 1, 0)),
        ('one-body', core | one, core | {(5, 6), (5, 7)}, (2, 1, 4)),
        ('three-alone', three, {(3, 6), (4, 6), (5, 6)}, (2, 2, 1)),
        ('three-body', core | three, core | {(3, 6), (4, 6), (5, 5)}, (2, 2, 5)),
    )
    reports = []
    for topology in ('bounded', 'toroidal'):
        engine = WorldEngine(library, SIZE, SIZE, topology)
        results = {}
        try:
            for name, initial, expected, counts in cases:
                assert not (set(initial) & ring)
                engine.install(initial)
                engine.step()
                actual, _ = inspect(engine)
                assert actual == expected, (topology, name, actual, expected)
                stats = engine.stats
                assert (stats.births, stats.deaths, stats.survivors) == counts
                assert stats.live_cells == len(expected) and stats.changed == counts[0] + counts[1]
                results[name] = {'next_on': sorted(actual), 'target_on': target in actual,
                                 'births_deaths_survivors': counts}
        finally:
            engine.close()
        assert not results['quiet-alone']['target_on'] and not results['quiet-body']['target_on']
        assert not results['one-alone']['target_on'] and results['one-body']['target_on']
        assert results['three-alone']['target_on'] and not results['three-body']['target_on']
        assert (5, 5) in set(map(tuple, results['three-body']['next_on'])) & ring
        reports.append({'topology': topology, 'cases': results})
    assert reports[0]['cases'] == reports[1]['cases']
    return {'target': target, 'reports': reports,
            'scope': 'local support-condition effects; no whole-maintenance or membership verdict'}


def main():
    library = APP / 'build/research/world.so'
    reports = [audit(library, topology, fixture)
               for topology in ('bounded', 'toroidal') for fixture in fixtures()]
    assert sum(row['initial_subsets_checked'] for row in reports) == 17648
    for a, b in zip(reports[:8], reports[8:]):
        assert a['fixture'] == b['fixture']
        assert a['admissible_histories'] == b['admissible_histories']
        assert a['all_native_boards_sha256'] == b['all_native_boards_sha256']
    for index, report in enumerate(reports):
        add_support_profiles(library, report, fixtures()[index % 8])
    for a, b in zip(reports[:8], reports[8:]):
        assert a['support_profiles'] == b['support_profiles']
    paths = (Path(__file__).resolve(), APP / 'src/world.c', APP / 'src/world.h',
             APP / 'tools/research_run.py', APP / 'tools/research_cross_cases.py', library)
    print(json.dumps({'proposal': '0.14; recurrent generators with context-relative support effects',
                      'reports': reports, 'support_renewal_witness': support_renewal_witness(library),
                      'support_context_contrasts': support_context_contrasts(library),
                      'sha256': {
                          str(path.relative_to(APP)): hashlib.sha256(path.read_bytes()).hexdigest()
                          for path in paths}}, indent=2, sort_keys=True))
    if not all(row['predicted_minima_match'] and row['minima_cover_reference'] for row in reports):
        raise SystemExit('Candidate E fixture prediction failed; inspect the complete report.')


if __name__ == '__main__':
    main()
