# Worked Example: The Glider

Status: reconstructed worked example\
Authority: non-canonical

There is a glider moving across the screen. Follow what makes that happen.
Use B3/S23 on an otherwise empty grid. Coordinates increase rightward and
downward. Keep the pattern and its future path away from finite-board edges.

## Follow four updates

| Generation | Live sites |
| --- | --- |
| 0 | (1,0), (2,1), (0,2), (1,2), (2,2) |
| 1 | (0,1), (2,1), (1,2), (2,2), (1,3) |
| 2 | (2,1), (0,2), (2,2), (1,3), (2,3) |
| 3 | (1,1), (2,2), (3,2), (1,3), (2,3) |
| 4 | (2,1), (3,2), (1,3), (2,3), (3,3) |

Every update has three survivors, two deaths and two births: five live sites,
four changed states. Generation 4 is generation 0 translated by (+1,+1).
The period modulo translation is four; velocity is (1/4,1/4) sites per
generation. Shape, population, recurrence and motion describe differentiated
qualities of this one succession.

Only (2,1) belongs to both endpoint live sets, and it is dead at generation 3.
The intersection of all five live sets is empty: no site stays continuously
live throughout the cycle. Fixed sites are not transported; live participation
turns over as the pattern moves. Water parcels passing through a waterfall
offer a comparison for turnover, while the glider also changes location.

## Trace how a role enters and contributes

At generation 0 the three bottom-row sites give (1,3) exactly three live
neighbors, producing its birth. The site (1,0), with one live neighbor, dies.
At generation 1, (1,3) contributes to the next birth at (2,3). Switch (1,3)
off before that update and (2,3) has only two live neighbors, so stays dead.

This traces the bilateral prompt: the preceding organization made this live
role, and the role helps make its next phase. It motivates the ownership
question without turning causal ancestry into a completed ownership criterion.
See the [machine's membership distinctions](../game-of-life-machine.md#membership-boundaries-and-bilateral-participation).

## Probe the boundary

Reset to generation 0 for each intervention.

| Added live site | First effect | Subsequent result |
| --- | --- | --- |
| (10,10) | Isolated added cell dies | The original glider follows its ordinary trajectory |
| (0,0) | (1,0) now has two neighbors and survives | The altered configuration is empty at generation 6 |

The five-site current footprint is one boundary description. The site (1,3)
lies outside it and enters on the next update. The perturbation at (0,0)
affects continuation from outside the original footprint. A causal influence
boundary therefore needs a phase, intervention family and horizon. Empty
neighbors matter to the rule too; a boundary analysis cannot simply discard
dead sites.

## Reproduce in the current application

Run from `apps/game-of-life`:

```bash
./gol --bounded --load research/worked-examples/fixtures/glider.cells --x 5 --y 5 --delay 1000
./gol --bounded --load research/worked-examples/fixtures/glider-near-perturbation.cells --x 5 --y 5 --delay 1000
./gol --bounded --load research/worked-examples/fixtures/glider-far-perturbation.cells --x 5 --y 5 --delay 1000
```

Each launch is a separate trial. Fixture coordinates are the table coordinates;
add (5,5) for world coordinates. The far fixture needs an 11x11 placement box,
so use a terminal whose grid accommodates that box at the offset. Press space
to pause, then `n` to step. Startup begins running, so restart if you miss
generation 0. Keep terminal geometry fixed: resize changes the actual world.

The existing `make test-unit TEST_CASE=glider-translation` checks recurrence,
translation and per-tick counts using the implementation. Arbitrary cell edits
and provenance recording remain proposed features. The intervention results
above can be checked from their separate starting fixtures.

## Next comparisons

Erase-and-redraw can preserve every subsequent grid while changing how the
checkpoint was produced. A glider–still-life collision can produce cells with
neighbors from both incoming patterns. Neither outgoing shape nor contribution
counts alone decides ownership or token continuation. The
[research proposal](../../../research/investigations/game-of-life/README.md)
separates those follow-up questions from this reconstructed example.

Source: `2-gol-chat.md` and `Game-of-Life-Machine.md` from the original
`_work/gol-tree` handoff, integrated with the current
[machine description](../game-of-life-machine.md).
