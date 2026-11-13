# Worked Example: The Toad

Status: reconstructed worked example\
Authority: non-canonical

A toad alternates between two shapes in the same place. What makes it keep
doing that? Use synchronous B3/S23 on an otherwise empty grid, away from edges.
Coordinates increase rightward and downward.

## Two phases

| Row | Phase A, generation 0 | Phase B, generation 1 |
| --- | --- | --- |
| 0 | — | (2,0) |
| 1 | (1,1), (2,1), (3,1) | (0,1), (3,1) |
| 2 | (0,2), (1,2), (2,2) | (0,2), (3,2) |
| 3 | — | (1,3) |

One more update restores A at the same coordinates: period two and zero
displacement. In each direction, two live sites survive, four die and four
are born. Population is six and changed states are eight per update.

From A to B, each of the four inner sites has four live neighbors and dies.
The two ends, (3,1) and (0,2), have two neighbors and survive. Four sites are
born, including (2,0), supported by A's upper row.

From B to A, the end sites survive again. B's four newly live sites each have
one live neighbor and die. Each of A's four inner sites now has three live
neighbors and is born again. The changing neighborhood conditions produce
the return.

## Persistence through partial turnover

The union of the two footprints contains ten sites. Two remain continuously
live; the other eight alternate in and out of the live footprint. The toad
persists through this turnover while staying in place. The
[glider](glider.md) shows both turnover and translation, with no site
continuously live over its four-update cycle.

Population, alternating shape, period and zero locomotion are qualities of
the same succession. A fixed spatial envelope is compatible with changing
participation. Neither the six-site instantaneous footprint nor the ten-site
cycle footprint settles constitutive membership.

## Trace bilateral participation

The upper row of A produces the birth at (2,0). In B that live state helps
produce all three sites of A's upper row on the following update.

Pause at B and remove (2,0). The upper-row births each lose one of their three
neighbors and fail. The next configuration is just (0,2), (1,2), (2,2).
Another update produces a vertical three-cell row, and a blinker then persists.

The ordinary cycle gives a trace for “I made this” in both directions. The
removal comparison shows a specific contribution to continuation. The
[machine description](../game-of-life-machine.md) explains the additional
ownership and all-five membership questions that this does not decide.

## An outside intervention

Reset to A and add (0,0), outside both A's current footprint and its ten-site
cycle footprint. The expected birth at (0,1) is suppressed: its neighbor count
becomes four. A new birth at (1,0) replaces it in the population count. The
next grid still contains six live sites but fails to return to A afterward.

Equal population therefore does not imply equal organization or continuation.
Record the complete grid alongside counts when checking an intervention.

## Reproduce in the current application

Run separate trials from `apps/game-of-life`:

```bash
./gol --bounded --load research/worked-examples/fixtures/toad.cells --x 5 --y 6 --delay 1000
./gol --bounded --load research/worked-examples/fixtures/toad-b-minus-cell.cells --x 5 --y 6 --delay 1000
./gol --bounded --load research/worked-examples/fixtures/toad-outside-perturbation.cells --x 5 --y 5 --delay 1000
```

The diagram origin is (5,5) in world coordinates for all three commands.
The first two fixtures have an empty leading row, which the `.cells` loader
normalizes away. Their placement at (5,6) compensates for that normalization;
the outside-perturbation fixture already has a live site in row 0.
The second fixture starts at B with (2,0) removed. It does not enact an edit
after an actual A→B transition, so it cannot provide that edit's provenance.

Pause with space and advance with `n`; startup runs immediately. Restart if
generation 0 has already passed. Avoid terminal resize. The built-in `toad`
starts from the tight two-row bounding box, one row above this local A when
given the same placement origin. Use the fixtures for these coordinate tables.

`make test-unit TEST_CASE=toad-period` checks exact phases and step counts in
the implementation. The proposed
[statistics plan](../persistence-statistics-plan.md) adds interval turnover
and provenance measurements; they are not current UI features.

Source: `Game-of-Life-Toad.md` and the later bilateral-participation discussion
in `2-gol-chat.md` from the original `_work/gol-tree` handoff.
