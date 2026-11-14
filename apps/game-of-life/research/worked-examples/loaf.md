# Worked Example: The Loaf

Status: reconstructed worked example\
Authority: non-canonical

A loaf keeps the same seven-site shape after every update. What makes that
possible? Use synchronous B3/S23 on an otherwise empty grid, away from edges.
Coordinates increase rightward and downward.

## One unchanged phase

```text
.OO.
O..O
.O.O
..O.
```

`O` is live and `.` is dead. The complete live set is:

| Row | Live sites at generations 0 and 1 |
| --- | --- |
| 0 | (1,0), (2,0) |
| 1 | (0,1), (3,1) |
| 2 | (1,2), (3,2) |
| 3 | (2,3) |

Every live site has exactly two live neighbors, so all seven survive. No dead
site has exactly three live neighbors, so none is born. The next complete grid
equals the previous grid. Repeating that update proves period one and zero
displacement under these fixed conditions: the loaf is a still life.

Each update has seven survivors, zero births, zero deaths and zero changed
states. Over any nonempty uninterrupted observation interval, the visited and
continuously live sets both contain the same seven sites.

## Persistence without live-site turnover

The [toad](toad.md) changes shape without translating. The [glider](glider.md)
changes its footprint while translating. The loaf supplies a third comparison:
its live footprint stays unchanged and none of its live roles turns over.

The unchanged board does not mean that an executing simulator has stopped
updating. Each step still evaluates the rule against the preceding board.
Conversely, a paused display or a stored image could show the same shape.
Snapshots alone cannot distinguish those production histories. A fixed point
of the model, ongoing computation and physically owned maintenance are
different claims.

## Trace a continuing role and its contribution

At (1,0), the two live neighbors are (2,0) and (0,1). They supply the count
that permits its survival. In the next phase, (1,0) in turn contributes to
both of those sites' survival counts. The role continues without a new birth.

Reset to generation 0 and remove (1,0), holding all other states fixed.
On the next update:

- (2,0) and (0,1) each have only one live neighbor and die.
- (1,1), previously dead with four live neighbors, now has three and is born.
- (3,1), (1,2), (3,2) and (2,3) survive.

The resulting complete live set is (1,1), (3,1), (1,2), (3,2), (2,3):
five sites, with four survivors, one birth, two deaths and three changed
states. The ordinary loaf is not restored by this first update.

This comparison shows a specific contribution to continuation. It does not
make every causal contributor a constitutive member or establish full UEF
ownership. The [machine description](../game-of-life-machine.md#membership-boundaries-and-bilateral-participation)
keeps those additional questions separate.

## An outside intervention

Reset to the intact loaf and add (0,0), outside its seven-site live footprint.
The added site has two live neighbors and survives. It also gives the dead
site (1,-1) three neighbors, producing a birth above the original bounding box.
All seven original sites survive this update. The next board therefore has
nine live sites: eight survivors, one birth and no deaths.

The original footprint and its 4x4 bounding box are different selections.
Neither encloses every site that can affect continuation under interventions.
An unchanged isolated footprint does not establish independence from its
surroundings or settle the constitutive boundary.

## Reproduce in the current application

Run separate trials from `apps/game-of-life`:

```bash
./gol --bounded --load research/worked-examples/fixtures/loaf.cells --x 5 --y 5 --delay 1000
./gol --bounded --load research/worked-examples/fixtures/loaf-minus-cell.cells --x 5 --y 5 --delay 1000
./gol --bounded --load research/worked-examples/fixtures/loaf-outside-perturbation.cells --x 5 --y 5 --delay 1000
```

All three fixtures have tight bounds beginning at local x=0 and y=0, so add
(5,5) to the coordinates above. The outside trial's birth at (1,-1) appears
at world coordinate (6,4); leave that surrounding margin empty.

Pause with space and advance with `n`. Startup runs immediately; restart if
generation 0 has already passed. Avoid terminal resize, which changes the
world. The removal and addition fixtures start separate comparison runs;
they do not record edits during an actual loaf trajectory. Interval-set and
production-history recording remain proposed in the
[statistics plan](../persistence-statistics-plan.md).
