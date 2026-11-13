# The Game of Life Machine

Status: exploratory machine description\
Authority: non-canonical\
Date: 2026-11-13

There is a glider moving across the screen. What makes that happen?

The Game of Life Machine starts with an organization that persists through
change, then traces how its activity produces further activity. Shape, size,
periodicity and locomotion are differentiated qualities of that succession.
The [glider](worked-examples/glider.md) and [toad](worked-examples/toad.md)
provide small, inspectable worked examples.

## Apparatus

The implementation displays a finite grid and computes synchronous Conway
updates, **B3/S23**. A dead site becomes live with three live neighbors; a live
site survives with two or three. Every next state is computed from the same
previous grid. Eight adjacent sites form the neighborhood.

At the model grain, each fixed site is an ongoing cell process. Live and dead
are its states; a dead site can participate in subsequent updates and become
live again. The abstract processes are realized by the computer, whose physical
operation is a separate grain of investigation. The glider moves while the
grid sites stay fixed.

The controls let the investigator initialize, pause, step, run and load
patterns. They are experimental controls, without the Christmas tree panel's
assigned cognition role. The current application supplies bounded and toroidal
worlds, built-in patterns and `.cells` loading. It has no interactive arbitrary
cell-toggle command, trajectory recorder, automatic pattern tracker or
ownership analyzer. Use prepared starting fixtures for the interventions below.

## Start with persistence

Follow the actual sequence before classifying it. A glider returns to its
initial shape after four updates, translated one site diagonally. A toad
returns after two updates at the same position. Each configuration changes
the neighborhood conditions that determine the next.

The [persistence-first account](../../../IER/IER-persistence.md) treats that
observable continuation as a candidate for investigation. It invites questions
about production, maintenance, participant turnover and boundaries. Recurrence
is a measured regularity of the sequence; naming a regime does not replace the
account of how the sequence occurs.

## One organization, differentiated qualities

Shape describes arrangement; size counts live sites; periodicity describes
recurrence; locomotion describes displacement through that recurrence. They
can be distinguished while arising in one succession of updates.

The proposed comparison with experiential unity starts here: sights, smells,
pains and thoughts can be differentiated within an unfolding experience.
Redness can be differentiated within a sight. The Life example makes the
organization side inspectable. Whether a physical candidate qualifies for the
IER identity proposal remains a separate question; a moving pattern alone
does not supply that argument.

## Membership, boundaries and bilateral participation

Distinguish the current live footprint, the sites visited over an interval,
and the sites where an intervention can affect continuation. They need not
coincide. A dead site may be about to enter the footprint; a site that never
enters it may still alter the next phase when perturbed.

The chat's bilateral ownership prompt is **“I made this”**, together with
**“what I made helps make my next phase.”** Trace both directions for a cell's
participation: which prior configuration produced its live role, and what that
role contributes next. Its role in a pattern can end while the site process
continues on the grid.

This is a useful experimental prompt. Canonical
[ownership](../../../IER/IER-ownership.md) asks where the relevant continuation
burden is borne. Full [constitutive membership](../../../IER/IER-processes.md#the-full-membership-relation)
also requires contribution, global bearing, joint resolution, diachronic uptake
and common typing. Bilateral causal traces and removal effects provide material
for those questions, without establishing the full relation.

## Production history and interruption

Two runs can have identical grids from a checkpoint onward while having
different histories. In one, the previous phase produced the checkpoint by
`world_step`; in another, the operator cleared and installed the checkpoint.
The update rule gives matching future boards in both cases.

History is therefore additional experimental evidence. Record actual update
and intervention events; a lawful pair of snapshots alone cannot identify
how the successor was installed. This comparison concerns abstract production
provenance. Canonical [continuity](../../../IER/IER-continuity.md) also requires
owned closure, effective typed inheritance, mediated change and two-sided
uniqueness. Neither stopping the display nor pausing the program by itself
establishes cessation of a physical UEF.

## Determinism and admissible futures

For a complete grid and fixed rule/topology, there is exactly one next grid.
Changing the initial cells compares different lawful trajectories. A family
of perturbations must be declared explicitly; it is a different object from
the outgoing successor set of one fixed grid.

Current [Unity](../../../IER/IER-unity.md#structural-basis) withdraws
successor-support non-factorization as a necessary unity condition. Singleton
support alone supplies neither unity nor disunity. Older
[orientation](gol-orientation.md) and support-analysis notes retain historical
claims based on that earlier requirement; their exclusionary unity conclusion
does not control this persistence-first description. Their broader revision
is included in the [draft statistics plan](persistence-statistics-plan.md).

Extinction or a collision outcome is an ordinary model result. Calling it
IER collapse requires the distinct foreclosure account under
[Collapse](../../../IER/IER-collapse.md).

## Current use and proposed development

The worked examples give coordinates, expected transitions and executable
starting fixtures. Existing tests verify basic glider and toad behavior. The
draft statistics plan proposes turnover, recurrence, spatial extent,
intervention comparison and event provenance. No new analysis code is supplied
by these documents.

The [research proposal](../../research/investigations/game-of-life/README.md)
keeps questions, protocols and eventual evidence together while this app owns
the simulator and fixtures. The
[Christmas Tree Machine](../../../examples/collections/christmas-tree/christmas-tree-machine.md)
provides a complementary physical proposal: ornaments reshape the arrangement
of an ongoing display; Life exposes successive update production directly.

Provenance: the two discussions and three generated descriptions retained in
`_work/gol-tree/`, read against the current canonical sources linked above.
