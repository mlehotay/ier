# Draft Plan: Persistence Statistics for the Life Machine

Status: recording, selected measurements and exact-neighborhood records implemented; stronger roles remain proposed\
Authority: non-canonical\
Date: 2026-11-13; interface draft updated 2026-11-14

## Objective and placement

Extend the simulator so an investigator can follow persistence, turnover,
recurrence and responses to interventions in the
[machine description](game-of-life-machine.md). The
[glider](worked-examples/glider.md) and [toad](worked-examples/toad.md) provide
the first exact acceptance examples.

This durable design proposal lives in `research/`. Executable
implementation tasks now belong to app plan `0002-gol-unity-relations-reproduction`,
`gol-0028`–`gol-0032`, in the existing `_work/plans/` and task dashboard.
Interface v1, the fixed-world recorder and the selected measurements below are implemented. The root unity plan
owns source assessment and relation definitions; the app research directory owns protocols
and findings. The user closed the simulator baseline at `58258b3d`; broader
analysis is still a separate workstream. The
[reading list](reading-list.md) informs definitions and their limits;
bibliography annotations are not implemented measurements.

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
in research records. The interactive app has no pattern identity tracker,
per-cell editing command or interval-history metrics. The separate fixed-world
recorder now exports exact trajectories and step/install events without using
terminal geometry.

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

## Whole-board measurement interface v1

`gol-0030` implements turnover, extent, interval sets, exact/translated
recurrence and optional paired-run differences in `tools/research_measure.py`.
Live-run ages, regions, interactive display and production/role relations are
outside this implemented subset. Analysis reads the recorder's complete boards;
it does not implement another Life rule or modify the recording engine.

```bash
make research-measure TRACE=trace.jsonl OUTPUT=measures.jsonl HISTORY_LIMIT=64
make research-measure TRACE=treated.jsonl PAIR=control.jsonl OUTPUT=paired.jsonl
```

Paths use the invoking app directory (unlike the recorder's manifest-relative
output). `HISTORY_LIMIT` includes the current snapshot, defaults to 64 and must
be at least 2. Dimensions times history limit may not exceed 16,777,216 retained
cell slots. Sparse Python sets have additional overhead. Analysis streams the
input and output; interval sets retain at most the complete world, while only
recurrence history is evicted. Install events reset all these sets/history,
even when the installed board equals the previous one. No interval spans an
install; tick 0 includes that segment's initial/installed board.

The `gol-measures/1` JSONL output has a header, one observation per snapshot
and a completion record with SHA-256 of the exact input trace bytes. Header
hashes identify the analyzer and imported manifest validator. Each observation
retains event/step/segment/tick, population, extent `[min_x,min_y,max_x,max_y]`
with inclusive endpoints (null if empty), turnover (null at segment tick 0),
the segment interval's full visited/continuously-live coordinate lists, and
recurrence records. Coordinates are ordered by y then x. Retained fraction
is null after a previously empty board; unchanged computed steps still have
valid zero turnover. No live-run age or membership output is implied.

Recurrence searches the retained same-segment boards in increasing lag order,
using complete equality. Records disclose search ticks, limit and truncation.
Statuses distinguish `insufficient_history`, `no_match`, `match`,
`unsupported_topology`, `empty_live_set` and `insufficient_dead_margin`.
An interval set remains complete from tick 0 even when recurrence history is
truncated; a no-match result says nothing about unobserved lags.

Exact recurrence works for both topologies, including empty boards. Ordinary
translation is supported only on bounded boards with nonempty live sets.
Minima fix the only possible ordinary translation of two finite nonempty
sets; complete translated equality then confirms it, so there are no ambiguous
ordinary matches in this selected domain. Toroidal modular matches, including
their ambiguity, remain unsupported. Translation `[dx,dy]` and velocity
`[dx/lag,dy/lag]` refer to the matched complete live sets, without pattern
identity claims.

A qualified translated match requires at least `lag` dead cells between every
live site and each wall throughout the retained comparison interval. This is
a conservative finite-horizon guard, not a guarantee of indefinite motion.
The record reports lag, starting tick and minimum margin. Observed translated
equality that fails the guard appears as `unqualified_match`; it cannot supply
velocity. Edge behavior still belongs to the finite declared world.

Paired analysis requires equal dimensions, rule, topology, protocol ID/hash,
proposal revision/hash, purpose, engine fingerprint/binary and step/install
schedule. Exact initial/installed boards and run IDs may differ. It compares
complete boards at matching event/segment/local-tick and step indices and
exports the symmetric-difference sites/count. The trace manifests retain the
interventions; context matching does not itself justify a causal inference or
assert a scientific protocol was accepted. Nonmatching schedules are rejected
rather than aligned by an ambiguous nominal generation.

Before successful publication, the analyzer checks manifest/text/hash,
initial/install boards, row dimensions/alphabet, event order/counters,
population, native turnover counts against adjacent board sets, state-change
flags and terminal completeness. This is trace integrity, not independent
validation of the Life rule or source claims. Qualification uses independent
literal phases and the same C engine as separate controls.

Outputs follow exclusive `.partial` creation, checked writes, flush/fsync/close
and atomic no-overwrite publication, preserving existing files and concurrent
outputs. A later invalid record or I/O failure leaves no newly published result;
any retained partial is explicitly incomplete. The original trace is read-only.

## Production and event records

### Exact-neighborhood export v1

`gol-0031` selects proposal revision 0.2's exact production record P(e) and
bounded run comparison. The app-local relation is `exact-neighborhood/1`.
It does not compute sufficient sets or actual causes. Those are separate
relations, with distinct intervention/contingency policies. The export records
all nine labeled conditions, including OFF inputs; it does not assume each
condition is necessary or select members from the resulting links.

Use `make research-production TRACE=trace.jsonl CONTEXT=context.json
OUTPUT=production.jsonl`, optionally with `PAIR=control.jsonl`. All paths use
the invoking app directory. Freeze the context before interpreting results:

```json
{
  "schema": "gol-production-context/1",
  "context_id": "declared-local-observations",
  "trace_run_id": "block-example",
  "proposal_revision": "0.2",
  "selection_reason": "Complete board control; no inferred membership",
  "targets": "all",
  "max_updates": 64
}
```

These seven fields are required and unknown fields are rejected. Targets are
either `all` or an explicit nonempty list of unique in-bounds integer `[x,y]`
pairs. Their order is preserved; `all` uses row-major board order. Selection
is an investigator-declared observation scope, not a discovered boundary.
`trace_run_id` and proposal revision must match the primary trace. The positive
integer `max_updates` caps target count times computed steps, with a hard
ceiling of 1,000,000 update records per export. Initial-only traces are valid.
Paired context requirements are the same as the whole-board measurement
interface; no arbitrary alignment or unstated counterfactual ensemble is used.

The `gol-production/1` JSONL output contains a header, an event record for
every initial/step/install snapshot, one update record per selected target
for each computed step, and completion. Header retains the exact context text
and hash, original run manifests and exporter/validator source hashes;
completion identifies exact input trace hashes and record counts. Event
records retain counters, population and state-change information. With a pair,
they also retain the full board difference, including actual initial/install
differences; every unlisted site agrees at that event. The traces retain exact
initial boards and interventions. These finite comparisons make no assertion
that arbitrary changes are physically admissible at the implementation level.

An update names its target and resulting event/segment/tick. `source_event`
names the preceding occurrence, with its actual kind and counters. Its nine
inputs are ordered NW, N, NE, W, center, E, SW, S, SE; each retains offset,
resolved site and binary value. Bounded exterior conditions have site null,
their original offset and fixed value zero. Toroidal coordinates wrap, retaining
distinct offset slots even when several refer to one site on a small torus;
neighbor counts follow the engine's eight directional slots. The center is
excluded from that count. No independent degrees of freedom are implied by
aliased slots. The output is copied from the recorded resulting complete board.

Process names are descriptive before/after classes: `production` (0→1),
`ON` (1→1), `destruction` (1→0), `OFF` (0→0 with nonempty input tuple) and
`null` (all nine inputs OFF and output OFF). For each pair the update retains
the other run's exact tuple/output, differing input offsets and whether output
differs. Following an occurrence into the next tuple is possible through its
site and event IDs; this is a recorded use, not a proof of necessary causal
contribution or owned continuation. No backwards link crosses an install.

The exporter uses validated traces and never evaluates a second Life rule.
It publishes only after both traces complete and uses checked exclusive partial
writes and atomic no-overwrite publication. Partial trace inputs are rejected.
Qualification checks all 512 literal interior neighborhoods against the native
engine and independent combinatorial category totals, joint inhibitory
controls, bounded/toroidal edges, aliases, paired histories and I/O failures.
Actual dossier runs and their scientific interpretations remain separately
owned; passing apparatus tests does not execute Q2–Q5.

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

1. **Fix the measurement scope.** Use the
   canonical GoL account (`IER/IER-game-of-life.md` in the development repository) and current Unity,
   Persistence, Ownership, Processes and Continuity owners. Keep the valid
   singleton-support calculation without treating it as a unity exclusion.
   Define the selected observations independently of a desired membership
   verdict; the old predicate/support programme is not a prerequisite.
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

The sequence above is a design resource; app tasks `gol-0028`–`gol-0032` in
plan 0002 own selected reproduction work and acceptance evidence. Survivor-pane
presentation remains optional UI work outside the required headless path. This plan
does not commission a dependency graph, all-cuts search, scalar unity score,
entropy proxy or ownership classifier. Those require a separate justified
question and measurement contract.

## Fixed-world reproduction interface — draft 1

Interface version 1 is frozen for `gol-0028`, coordinated with root proposal
revision 0.2. The heading retains its draft-1 anchor for existing links.
This is an app-local research format, not an IER schema. The baseline is
user-approved at `58258b3d`; `gol-0029` owns implementation of this contract.

### Inputs and execution

The Make entry point is `make research-run INPUT=manifest.json
OUTPUT=trace.jsonl`. It builds `build/research/world.so` from the existing
`src/world.c` and runs `tools/research_run.py` with Python's standard ctypes
binding to `World`/`world_step`. No terminal geometry or second rule engine
is used. Relative output paths use the manifest's directory. Both the exact
submitted manifest text and its resolved inputs are retained in the trace.

Use a JSON manifest with `schema` equal to `gol-run/1` and these fields:

| Field | Contract |
| --- | --- |
| `run_id`, `protocol_id`, `proposal_revision`, `purpose` | Required identifiers; purpose is `pilot`, `reconstruction` or `confirmatory`. A string alone does not establish protocol acceptance. |
| `width`, `height`, `topology`, `rule` | Positive dimensions; topology `bounded` or `toroidal`; rule exactly `B3/S23`. Dimensions stay fixed during this run. |
| `initial_live` | Exact unique integer `[x,y]` pairs in bounds; every unlisted site is OFF. Empty list is valid. No centering, clipping or loader normalization. |
| `actions` | Ordered `step` actions with positive integer `count`, or `install` actions with an exact complete `live` set in the same coordinate convention. |
| `max_cells`, `max_records` | Explicit positive resource limits. Validate dimensions, multiplication, action counts and expected records before execution; reject excess rather than truncate silently. |

All listed fields are required. Unknown fields are rejected. Optional
`protocol_sha256` and `proposal_sha256` are lowercase 64-digit hex values;
confirmatory manifests require both, while the runner does not certify their
scientific acceptance. Limit worlds to at most 16,777,216 cells as well as
the manifest's smaller cap. The exact required record count is three plus
the sum of step counts and number of installs; an empty action list is valid.
Header metric version `native-step-counts/1` covers only the existing C counts;
unimplemented relation modules must not acquire output labels.

Reject unknown schema/rule/action names, duplicate JSON keys or live coordinates,
noninteger/out-of-range numbers, invalid topology and malformed inputs. Random
seeds are not initial states. A fixed initial board or fully specified install
is required; interactive clear/randomize/resize/topology/edit operations are
outside draft 1. Q3 can first compare separately declared initial boards.

Unfold a counted step into individually recorded synchronous transitions.
Maintain `event_index` across all events and `step_index` across computed
steps. Begin segment 0 with local `tick=0`; increment tick after each step.
An install opens the next segment at tick 0 without incrementing step_index.
These are research counters; they do not redefine the interactive app's
generation-reset behavior.

### Complete snapshots, events and errors

Write JSON Lines with header schema `gol-trace/1`: one header, an initial snapshot, one event/snapshot per
unfolded action and one final completion record. Each snapshot includes every
row of the declared board as an exact-width `.`/`O` string; ordering is top
to bottom, x increases rightward and y downward. Display crops are excluded.

| Record | Required content |
| --- | --- |
| Header | Schema, resolved manifest, rule/topology/dimensions, protocol/proposal/metric identifiers, exact source/binary identifiers and dirty-source fingerprint if applicable. A commit name alone cannot identify modified code. |
| Initial | Complete rows, population, indices and segment 0; transition statistics marked unavailable. |
| Step | Before/after indices, complete resulting rows, native births/deaths/survivors/changed and population; transition statistics valid even for unchanged boards. |
| Install | Complete supplied rows, indices, new segment and whether values changed; no step statistics or implied F-produced successor. An explicitly accepted equal-board install remains an install event, not a computed step or evidence of a different later trajectory. |
| Completion | Total event/step/segment counts and a successful terminal status, written only after all expected records. |

Prevalidate inputs before creating a result file. Stream to a distinct partial
path; publish the requested output only after successful writes and close.
Use nonzero exit for input, allocation, engine or output failure and preserve
a visibly incomplete partial trace with its diagnostic where possible. Never
overwrite an existing completed output implicitly. Consumers require successful
exit, a completion record and expected counts; a partial file is not a result.
Flush/close errors must prevent publication. Bound memory independently of H;
later analysis failure must leave complete board evidence available and mark
affected metrics unavailable rather than fabricate zero values.

### Fixtures and interpretation handoff

For the existing Q1 protocol use 32×32 bounded boards, diagram origin (5,5),
the exact glider generations 0–4 and toad generations 0–2 in the worked-example
tables, with no later interventions. Add (5,5) to every table coordinate.
Do not load the normalized toad file at that same offset: its leading OFF row
is absent after parsing. The manifest's exact coordinates avoid that ambiguity.
Use separate block and empty-board controls. Every listed phase must match a
complete-board oracle; population/overlap checks alone are insufficient.

Q2 uses a computed checkpoint and an independently specified identical install
followed by matched steps. The expected result is equal complete boards and
different event/segment provenance, not different deterministic futures. Q3's
near/far perturbations and the two-block controls retain their own exact
inputs/horizons. Q4 collision orientation, placement and each input-alone
control still need selection and verification; do not label an unverified
collision fixture confirmatory. Q5 must declare its own masks, interventions
and grain rather than inherit a domain from graph connectivity.

Draft 1 records native board evolution and provenance. Later measurement and
role modules identify their own versions and contexts. Minimal Boolean
sufficiency in proposal 0.2 is distinct from actual causes and membership.
The earlier unit reconstructions remain separate from this export contract.
Runner qualification checks the stated Q1 board sequences, replay and Q2-style
equal-board installation; full study interpretation stays with the dossier.
Interface review, remaining fixture scope and
failure-test oracles were resolved at this interface scope as follows. Later
Q4/Q5 fixture selection and relation-specific qualification stay with their
actual reproduction tasks, not a claim that those studies have been executed.

| Failure/control | Required oracle for implementation |
| --- | --- |
| Malformed JSON, duplicate keys/coordinates, unknown fields/rule/action, Boolean or noninteger counts, out-of-bounds cells | Nonzero exit before any output/partial file is created. |
| Resource limits and action expansion | Reject before execution if cell or record limits are exceeded; preserve exact event counts for empty, counted-step and install histories. |
| Existing final/partial output | Nonzero exit without replacing either file; publication must also reject a racing final-path creation. |
| Engine allocation/load error | Nonzero exit, no published result, and release any allocated world. |
| Write/flush/close/publication error | Nonzero exit, diagnostic and no newly published result; any partial trace remains visibly incomplete evidence. |
| Q1 literal fixtures | Complete 32×32 boards equal the independently stated glider 0–4 and toad 0–2 tables, including offset (5,5), plus exact native counts. |
| Equal-board install | Preserve equal future boards but record an install and a new segment, with unavailable transition statistics and no extra computed step. |

Review outcome: exact format/lifecycle/error semantics, independent initial
phase fixtures and the staged Q1–Q5 scope are specified. `gol-0028` is complete;
actual runner output and failure qualification belong to `gol-0029` onward.

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

The [research overview](README.md#proposed-sequence-of-investigations) owns
bounded questions, protocol versions, evidence derivation and limits, with
protocols and eventual results in [investigations/](investigations/).
The app owns one simulator and its tests. Before a research run, freeze the
selected metric definitions, dimensions, interventions and comparison horizon.
This draft specifies no completed research result.
