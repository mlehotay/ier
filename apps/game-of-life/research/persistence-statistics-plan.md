# Draft Plan: Persistence Statistics for the Life Machine

Status: draft proposal; implementation not started\
Authority: non-canonical\
Date: 2026-11-13

## Objective and placement

Extend the simulator so an investigator can follow persistence, turnover,
recurrence and responses to interventions in the
[machine description](game-of-life-machine.md). The
[glider](worked-examples/glider.md) and [toad](worked-examples/toad.md) provide
the first exact acceptance examples.

This durable design proposal lives in `research/`. Executable
implementation tasks, once selected, belong in the project's existing
`_work/plans/` and task dashboard. This draft changes no task state and does
not close or expand the current phase-completion plan. Finish that baseline's
control and exit audits before implementing this extension.

## Existing implementation

`world_step` computes live cells, births, deaths, survivors and changed states
in `StepStats`. `StatsSnapshot` and the pane expose all except survivors,
alongside generation, density, world dimensions, topology and run controls.
`world_recompute_stats` recounts population and zeros transition fields after
non-step mutations. The existing tests verify stats lifecycle, toad phases
and glider translation.

World size currently follows terminal geometry. Resize can discard live
cells. Pattern/file placement is additive at runtime, the loader normalizes
live bounds, and startup runs immediately. These behaviors must be explicit
in research records. The implementation has no trajectory export, pattern
identity tracker, per-cell editing command or interval-history metrics.

## Measurement contract

Let L(t) be the complete live-site set after generation t in a fixed world.
Use a declared uninterrupted observation segment [a,b]. Any selected region
is an explicit mask or window, with its rule and coordinates recorded; it is
not automatically an owned operation.

| Measurement | Definition and boundary |
| --- | --- |
| Survivors | Size of the intersection of previous and current live sets; expose the existing native count |
| Births / deaths | Current live sites absent from the previous live set, and previous live sites absent from the current live set |
| Changed | Births + deaths; a state-change count, not number of moving cells |
| Retained live fraction | Survivors divided by previous population; unavailable when previous population is zero |
| Live extent | Tight live bounding box; absent for an empty board |
| Visited sites | Size of the union of L(t) over [a,b] |
| Continuously live sites | Size of the intersection of L(t) over [a,b], including both endpoints |
| Live-run age | Consecutive live generations at a site, with left-censoring flagged when the segment starts live |
| Exact recurrence | Smallest observed positive lag with equal complete boards within the retained history |
| Translated recurrence | Smallest observed lag with equality of complete live sets after one recorded translation |
| Displacement and velocity | The matching translation vector and vector divided by lag; available only with a qualified recurrence match |
| Paired-run difference | Symmetric difference of two complete live sets at the same generation under matched context |

At a single transition verify population identities:
`new population = survivors + births` and `old population = survivors + deaths`.
Do not label visited or live
sites “constitutive members,” or causal contributors “owned cells.” Keep
membership and ownership as separately investigated relations.

On bounded worlds, translated recurrence needs a dead margin large enough for
the declared validation horizon. A match alone does not establish indefinite
motion before a wall. Initially report seam-crossing toroidal translation as
unsupported; later support must specify modular displacement and ambiguous
matches. A raw centroid can jump at a torus seam, so omit centroid velocity.
Exact full-board recurrence remains meaningful on either finite topology.

Empty boards can have exact recurrence but no meaningful live-shape translation.
Report missing history, an ambiguous translation or a non-match within the
search window explicitly; absence of a detected repeat is not aperiodicity.
Hashes may shortlist comparisons but full equality must confirm them.

Whole-board metrics come first. Region-specific tracking is later work:
two gliders, collisions and births outside a selected window defeat naive
one-pattern assumptions. Footprint connectivity is not an identity algorithm.

## Production and event records

Give each run and segment a stable identifier. Preserve rule, topology,
dimensions, exact initial board, fixture/placement, code revision, generation,
event sequence and observation horizon. Save the initial board even for random
startup: a seed alone is not portable across C library PRNG implementations.

Record events for computed steps, clear, randomize, successful pattern/file
placement, topology changes and real world resize. A future cell-edit command
must record coordinates and before/after state. A rejected operation records
failure without changing state or opening a new segment. Pause/resume and
display changes are metadata, with unchanged boards distinguished from steps.

Any successful world intervention starts a fresh analysis segment, preserving
the old one in the event log. Keep the app's generation semantics unchanged;
store an event ordinal because multiple interventions can occur at one
generation. Interval history, recurrence and live-run ages must not silently
span a mutation. A redraw matching a previous grid is a new event, not inferred
successor production. Provenance reports model operations, without claiming
physical irreversible inheritance or subject identity.

## Proposed implementation sequence

1. **Reconcile documentation.** Update orientation, design addendum and related
   support notes against current Unity, Persistence, Ownership, Processes and
   Continuity. Keep valid singleton-support mathematics while removing the
   withdrawn non-factorization exclusion. Inventory affected consumers; make
   no canonical changes.
2. **Expose native survivors.** Add the existing count to snapshots and a compact
   optional pane line. Give non-step values an explicit validity convention;
   no tick occurred at initialization. Preserve layout bounds and small-terminal
   behavior.
3. **Add observation history.** Implement a separate bounded-memory analysis
   module with interval union/intersection, extent and live-run ages. Keep it
   outside rule evaluation. Bound configurable history and report truncation.
4. **Add recurrence.** Compare complete boards, then translated live sets with
   declared topology/edge limits. Report match lag, displacement, validation
   interval and search limits. Do not silently promote a pattern label into
   an explanation-completeness result.
5. **Add reproducible recording and intervention comparisons.** Provide a
   non-interactive fixed-size runner using the same C world engine, explicit
   initial boards, machine-readable snapshots and event records. Define its
   command/output contract before implementation. Add a precise paused cell-edit
   operation and checkpoint install only if needed for the selected protocol;
   log each distinctly. Support paired-run board differences without changing
   the simulator rule.
6. **Qualify and present.** Add only selected compact stats to the pane, with
   detailed histories in export. Verify exact examples and lifecycle semantics,
   then run the aggregate test gate and workflow validation. Keep research
   execution and interpretation in its dossier.

Draft task boundaries above are proposals, not allocated task IDs. This plan
does not commission a dependency graph, all-cuts search, scalar unity score,
entropy proxy or ownership classifier. Those require a separate justified
question and measurement contract.

## Acceptance evidence

| Case | Required result |
| --- | --- |
| Glider, generations 0–4 | Population 5; births/deaths 2/2 and survivors 3 per tick; visited 11; continuously live intersection 0; translated lag 4 with (+1,+1) |
| Toad, generations 0–2 | Population 6; births/deaths 4/4 and survivors 2 per tick; visited 10; continuously live 2; exact lag 2 |
| Block | Population 4; zero changes; exact lag 1; zero displacement |
| Empty to isolated birth-free extinction | Correct empty denominators and absent extent; no invented translated motion |
| Toad outside perturbation | Equal next population but unequal complete board; ordinary phase return fails |
| Glider near perturbation | Complete extinction at generation 6 |
| Matching computed and installed checkpoints | Equal later boards and recurrence metrics; different recorded production events |
| Resize / topology / load / randomize | Recomputed snapshot and explicit segment boundary; no mixed-context recurrence |
| Toroidal seam and finite wall | Explicit unsupported/limited translation status; no incorrect unbounded-motion certificate |

Test complete boards as well as counters. Keep literal expected fixtures
independent of the production algorithm; use existing Make targets and extend
the aggregate gate for implemented features. A recording failure must be
visible and must not leave a purported complete research trace. Analysis
allocation failure must preserve simulator state and report missing metrics.

## Research handoff

The [research dossier proposal](../../research/investigations/game-of-life/README.md)
owns bounded questions, protocol versions, evidence derivation and limits.
The app owns one simulator and its tests. Before a research run, freeze the
selected metric definitions, dimensions, interventions and comparison horizon.
This draft specifies no completed research result.
