#!/usr/bin/env python3
"""Check declared interface and native-storage contracts on the 3x3 torus.

From the app root first run: make build/research/world.so
Then run this script from any directory. This is a bounded model comparison,
not a constitutive-membership or host-level physical qualification test.
"""

import ctypes
import hashlib
import json
from collections import Counter
from pathlib import Path
import sys

APP = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(APP / 'tools'))
from research_run import WorldEngine

OUTER = tuple((x, y) for y in range(3) for x in range(3) if (x, y) != (1, 1))


def quotient(mask):
    count = mask.bit_count()
    return 'T' if count == 2 else 'R' if count == 3 else 'D'


def partition(table, observation, actions=(0, 1)):
    """Coarsest future-response equivalence for all words in the action alphabet."""
    labels = (0,) * 256
    while True:
        signatures = {}
        refined = []
        for state in range(256):
            signature = (observation(state), tuple(labels[table[state, action][1]] for action in actions))
            if signature not in signatures:
                signatures[signature] = len(signatures)
            refined.append(signatures[signature])
        refined = tuple(refined)
        if refined == labels:
            return labels
        labels = refined


def trace(table, mask, word):
    outputs = []
    for center in word:
        output, mask = table[mask, center]
        outputs.append(output)
    return tuple(outputs)


def storage_contract(library):
    """Compare current-array action with construction in the other allocation."""
    engine = WorldEngine(library, 3, 3, 'toroidal')
    patterns = (bytes(9), bytes([1] * 9), bytes([0xa5] * 9),
                bytes((29*j + 17) % 256 for j in range(9)))
    checked = stable = 0
    centers = {}
    try:
        for mask in range(512):
            initial = bytes((mask >> j) & 1 for j in range(9))
            total = sum(initial)
            expected = bytes([1] * 9) if total == 3 else initial if total == 4 else bytes(9)
            for scratch in patterns:
                engine.install([(j % 3, j // 3) for j, bit in enumerate(initial) if bit])
                old_input = ctypes.addressof(engine.world.cells.contents)
                old_output = ctypes.addressof(engine.world.next.contents)
                assert old_input != old_output
                for j, value in enumerate(scratch):
                    engine.world.next[j] = value
                assert engine.board() == initial
                engine.step()
                assert ctypes.addressof(engine.world.cells.contents) == old_output
                assert ctypes.addressof(engine.world.next.contents) == old_input
                assert ctypes.string_at(old_input, 9) == initial
                assert engine.board() == expected
                assert all(bit in (0, 1) for bit in engine.board())
                births = sum(not a and b for a, b in zip(initial, expected))
                deaths = sum(a and not b for a, b in zip(initial, expected))
                survivors = sum(a and b for a, b in zip(initial, expected))
                assert (engine.stats.live_cells, engine.stats.births, engine.stats.deaths,
                        engine.stats.survivors, engine.stats.changed) == (
                            sum(expected), births, deaths, survivors, births + deaths)
                if mask in (7, 3):
                    centers.setdefault(mask, set()).add(engine.board()[4])
                stable += initial == expected
                checked += 1
    finally:
        engine.close()
    assert checked == 2048 and stable == 508  # Empty plus 126 four-ON boards, four policies each.
    assert centers == {7: {1}, 3: {0}}
    return {'input_boards': 512, 'scratch_policies': 4, 'native_calls_checked': checked,
            'conserved_input_and_swapped_roles': checked, 'stable_board_active_calls': stable,
            'input_effect_center_outputs': [1, 0],
            'scope': 'C-level byte values and buffer roles; no host maintenance or closure claim'}


def intervention_contract(library, step_table):
    """Require a quotient to respect named exterior-site assignments too."""
    actions = (('step', 0), ('step', 1)) + tuple(
        ('set', j, value) for j in range(8) for value in (0, 1))
    extended = {(mask, ('step', c)): result for (mask, c), result in step_table.items()}
    engine = WorldEngine(library, 3, 3, 'toroidal')
    edits = probes = 0
    try:
        for mask in range(256):
            original = [xy for bit, xy in enumerate(OUTER) if mask & (1 << bit)]
            for action in actions[2:]:
                _, j, value = action
                engine.install(original)
                x, y = OUTER[j]
                engine.lib.world_set(ctypes.byref(engine.world), x, y, value)
                board = engine.board()
                actual = sum(board[y*3+x] << bit for bit, (x, y) in enumerate(OUTER))
                expected = (mask & ~(1 << j)) | (value << j)
                assert actual == expected and board[4] == 0
                extended[mask, action] = None, actual
                edits += 1
            for held in range(8):
                engine.install(original)
                two_on = tuple(j for j in range(8) if j != held)[:2]
                for j, (x, y) in enumerate(OUTER):
                    if j != held:
                        engine.lib.world_set(ctypes.byref(engine.world), x, y, int(j in two_on))
                engine.step()
                bit = (mask >> held) & 1
                assert engine.board() == bytes([bit] * 9)
                probes += 1
    finally:
        engine.close()
    labels = partition(extended, lambda s: tuple(extended[s, a][0] for a in actions), actions)
    assert len(set(labels)) == 256
    assert edits == 4096 and probes == 2048 and len(extended) == 4608
    assert quotient(3) == quotient(5) == 'T'
    edited = [extended[mask, ('set', 1, 0)][1] for mask in (3, 5)]
    assert edited == [1, 5] and [quotient(mask) for mask in edited] == ['D', 'T']
    outputs = [step_table[mask, 1][0] for mask in edited]
    assert outputs == [0, 1]
    return {'action_count': len(actions), 'native_assignments_checked': edits,
            'step_transitions_reused': len(step_table), 'response_classes': len(set(labels)),
            'native_held_bit_probes': probes, 'single_edit_center_outputs': outputs,
            'scope': 'model-level site edits; no regime-preservation or qualification claim'}


def main():
    library = APP / 'build/research/world.so'
    engine = WorldEngine(library, 3, 3, 'toroidal')
    table = {}
    try:
        for mask in range(256):
            count = mask.bit_count()
            for center in (0, 1):
                live = [xy for bit, xy in enumerate(OUTER) if mask & (1 << bit)]
                if center:
                    live.append((1, 1))
                engine.install(live)
                engine.step()
                board = engine.board()
                next_mask = sum(board[y*3+x] << bit for bit, (x, y) in enumerate(OUTER))
                output = board[4]
                # Independent closed-form oracle for this exact topology.
                total = count + center
                expected_mask = 255 if total == 3 else mask if total == 4 else 0
                expected_output = int(count == 3 or (center == 1 and count == 2))
                assert (output, next_mask) == (expected_output, expected_mask), (mask, center, board)
                # Output plus exterior mask covers all nine successor sites.
                table[mask, center] = output, next_mask
    finally:
        engine.close()

    center_classes = partition(table, lambda s: tuple(table[s, c][0] for c in (0, 1)))
    count_classes = partition(table, lambda s: (s.bit_count(),))
    exterior_classes = partition(table, lambda s: (s,))
    assert sorted(Counter(center_classes).values()) == [28, 56, 172]
    assert len(set(count_classes)) == 9
    assert len(set(exterior_classes)) == 256
    for a in range(256):
        for b in range(256):
            assert (center_classes[a] == center_classes[b]) == (quotient(a) == quotient(b))
            assert (count_classes[a] == count_classes[b]) == (a.bit_count() == b.bit_count())

    # Check the proposed quotient against every native output and next class.
    machine = {'D': ((0, 'D'), (0, 'D')),
               'T': ((0, 'D'), (1, 'D')),
               'R': ((1, 'D'), (1, 'R'))}
    for (mask, center), (output, next_mask) in table.items():
        assert machine[quotient(mask)][center] == (output, quotient(next_mask))
    # Distinct immediate output signatures prove the three-state lower bound.
    assert len({tuple(pair[0] for pair in row) for row in machine.values()}) == 3
    first = trace(table, 7, (0, 0))
    second = trace(table, 7, (1, 0))
    assert (first, second) == ((1, 0), (1, 1))

    storage = storage_contract(library)
    interventions = intervention_contract(library, table)

    fingerprints = {}
    for path in (APP / 'src/world.c', APP / 'src/world.h',
                 APP / 'tools/research_run.py', library, Path(__file__).resolve()):
        fingerprints[str(path.relative_to(APP))] = hashlib.sha256(path.read_bytes()).hexdigest()
    print(json.dumps({
        'context': '3x3 torus; arbitrary center input before each step; all exterior preparations',
        'native_transitions_checked': len(table),
        'native_storage_contract': storage,
        'native_intervention_contract': interventions,
        'center_response_states': len(set(center_classes)),
        'center_class_sizes': {q: sum(quotient(s) == q for s in range(256)) for q in machine},
        'count_observation_states': len(set(count_classes)),
        'exterior_observation_states': len(set(exterior_classes)),
        'input_words': ['00', '10'], 'output_words': [''.join(map(str, first)), ''.join(map(str, second))],
        'all_history_basis': '512 native output/next-class equalities; induction over input words',
        'sha256': fingerprints,
    }, indent=2, sort_keys=True))


if __name__ == '__main__':
    main()
