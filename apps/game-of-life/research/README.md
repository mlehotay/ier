# Game of Life Research Material

## Status

Status: draft\
Authority: non-canonical

This material is exploratory. GoL is the primary v10.12.1 testbed for proposed
real organizational membership, boundary change and successor production.
Intrinsic informational tension remains the further qualification question;
Shannon state distinctions alone establish neither tension nor its absence.
The [canonical membership discussion](../../../IER/IER-processes.md#general-organizational-membership)
states the proposal and its limits.

This directory is the single home for GoL research: readings, descriptions,
worked examples, analysis proposals, investigation protocols and eventual
results. Simulator usage, formats and implementation documentation remain in [docs/](../docs/).

The [GoL reading list](reading-list.md) collects literature on glider
organization, causal contribution, continuity, autonomy, scale and boundary
criteria, with proposed comparisons for the IER relational work.
The [follow-up queue](reading-list-followup.md) records unresolved questions
and targeted further reading after the nineteen first-pass source notes.
The development review is `_work/unity/review/gol-literature-assessment.md`;
its private source notes retain explicit coverage and reproduction limits.

The v10.12.1 source review, existing-case assessment, accepted guide and
bounded owner reconciliation are complete. App plans 0001/0002 and the
`gol-0028`–`gol-0032` delivery are closed at their software/evidence scopes.
Unfinished proposal and physical cases remain `todo-240`/`todo-237` in the
ready development plan `_work/plans/v10.12-gol-relations-resumption.md`.
New research awaits explicit resumption. Protocols, saved results and their
limits remain in [investigations/](investigations/); none establishes a complete
physical membership criterion or a qualified experiential instance.


Start with the [Life machine](game-of-life-machine.md), then the worked
[glider](worked-examples/glider.md), [toad](worked-examples/toad.md) and
[loaf](worked-examples/loaf.md) examples. The loaf supplies persistence without
live-site turnover as a contrast to the other two.
Their starting fixtures are in [worked-examples/fixtures/](worked-examples/fixtures/).
The [statistics plan](persistence-statistics-plan.md) specifies implemented recording/whole-board measurements and proposed role instrumentation.

The development repository's canonical GoL account, `IER/IER-game-of-life.md`,
explains the organizational relations and their limits. The ordinary
state/update, set, relation and projection vocabulary in `IER/IER-math.md` and
`IER/IER-math-addons.md` is sufficient; this application needs no separate IER
math-register extension. These articles are not selected for the public export.

For one complete board, the fixed update gives one successor. Its same-fibre
support factorizes, but non-factorization is no longer a necessary unity
condition. Filtering projected successors or collecting outputs from different
starting boards studies other mathematical objects; neither supplies the
missing constitutive boundary or membership rule. The superseded
predicate/support programme and its roadmap have been removed. Git history
preserves those notes and their original review scope.

Two separate proposals remain useful:

- [Later UI and instrumentation design](gol-design-addendum.md), including
  world/viewport separation and optional overlays.
- [Explanation completeness](explanation-completeness-criterion.md),
  a prospective predictive stopping criterion, not a UEF qualification test.

## Research method and ownership

In the development repository, the root plan at
`_work/plans/v10.12.0-unity-fix-canon-propagation.md` supplies the five-phase
completion sequence; that private workflow file is outside the public export.
This research directory supplies phase 3 protocols
and evidence; the app plan owns reproduction software. The simulator baseline
audit precedes instrumentation implementation; source work and reproduction
specification can proceed now.

GoL-specific questions, protocols, evidence and conclusions live here. The
simulator, fixtures and software tests stay in this app; implementation work
continues through its existing plan and task dashboard.

Shared methods and calibrations remain in `apps/research/`. In particular,
`apps/research/controlled-investigation-sop.md` supplies question readiness,
apparatus qualification, evidence and failure routing. Following that method
does not require storing GoL records beside it or copying the SOP into this app.
Those shared development resources are not part of the public export.

## Starting investigation

The development-only record at
`apps/game-of-life/research/investigations/local-process-reconstruction.md`
documents Beer (2015) local and selected-network checks through the existing
engine. Its unit cases are in the [test surface](../tests/README.md).
This bounded source work is separate from the Q1–Q5 protocols below and from
organizational or experiential verdicts; the record is outside the public export.

This is exploratory/reconstruction work, with no selected confirmatory study
or accepted investigation result. Source annotations and task allocation do
not select or complete confirmatory studies. Freeze each actual protocol and
proposal/implementation version before interpreting its run evidence.

The [first draft protocol](investigations/question-and-protocol.md) asks how a
glider and a toad preserve their measured recurrence while continuously live sites turn
over. This is a small reconstruction question with exact board comparisons.
It can qualify basic measurements before stronger claims are attempted.

The fixed-world recorder now supplies complete boards and step/install
provenance through `make research-run`; its contract is in the
[statistics document](persistence-statistics-plan.md#fixed-world-reproduction-interface--draft-1).
The development-only Q1 execution record and raw traces are at
`apps/game-of-life/research/investigations/q1-recorder-reconstruction.md`
and its `runs/2026-11-14-q1-recorder/` companion. The stated glider/toad
boards and native counts match. `make research-measure` now exports interval
sets, turnover and recurrence from those traces, with matching Q1 results;
paired-run differences are separately qualified controls. Those model results do not settle membership
or qualification. Exact-neighborhood records now export all labeled ON/OFF
conditions at declared target sites through `make research-production`. The
development-only Q2 record at
`apps/game-of-life/research/investigations/q2-production-installation.md`
retains actual computed/installed checkpoint histories: all aligned future
boards agree, with different recorded source-event kinds. Q3–Q5 remain open;
no actual-cause or membership classifier is implemented.

The development record at
`apps/game-of-life/research/investigations/q3-q5-cross-case-reconstruction.md`
retains 21 later pilot/model reconstructions, with exact inputs and a hash
index. The author then requested reassessing the discriminating cases before
section 3 execution. Those pilots remain at their model scope; they are not
the accepted physical-adequacy suite. Further case selection must identify
the proposal claim, contrasting predictions and physical interpretation.

## Proposed sequence of investigations

| Question | Comparison and useful evidence | Limit or next burden |
| --- | --- | --- |
| Q1: Persistence through live-site turnover | Glider versus toad; exact phases, recurrence, interval union/intersection | Live footprint is an operational selection, not certified constitutive membership |
| Q2: Production versus installation | Ordinary step to a checkpoint versus clearing and installing the identical checkpoint; event history and matching later boards | Abstract event provenance does not establish physical typed inheritance or full continuity |
| Q3: Bilateral participation and boundary | Trace which neighborhoods produce a cell's live role and what it contributes next; matched one-cell interventions inside/outside footprints | Causal contribution and dependence alone do not decide ownership or all-five membership |
| Q4: Collision and successor attribution | Glider and a chosen still life, compared with each alone; full boards and production traces | Similar outgoing shapes and mixed ancestry do not decide token identity |
| Q5: Joint organization under interventions | Independently justified candidate, cut, grain and perturbation family; compare actual update dependence and substitution behavior | Perturbation ensembles are not a fixed state's successor fibre; factorization is not a unity verdict |

Q1 and Q2 have bounded reconstruction records in the development repository,
as described above. Q3–Q5 remain a question backlog, not completed studies.
Split a question further when its decision rules diverge.
No aggregate “percentage of the theory mapped” should be assigned.

## Collision question retained from the discussion

During contact, a birth could have two neighbors from the incoming glider and
one from the still life. That is a conditional example of joint causal ancestry,
not a recorded collision result. Ask both who helped produce the live state
and which subsequent organization its role helps produce.

Three contrasts deserve separate fixtures: the glider is absorbed while a
remnant persists; a continuing moving pattern is altered; or an outgoing glider
appears after mixed interaction. Exact outcomes depend on still-life choice,
orientation, phase, distance and topology. Choose and verify these before
making claims about an actual collision. A familiar output establishes a
pattern type; persistence of an effect does not by itself establish continuity
of the incoming glider. Two predecessors contributing to one output do not
establish merged subject identity.

## Evidence and directory growth

Start with the first protocol and the
[machine description](game-of-life-machine.md),
[glider](worked-examples/glider.md) and
[toad](worked-examples/toad.md).
For each selected question, add a bounded protocol and a result/derivation
document under `investigations/`. Add a `runs/` subdirectory there only when a
run exists, preserving exact initial boards, implementation revision, manifest, events and output. Give
each run an identifier and distinguish reconstruction from confirmatory work.
Do not create empty evidence folders or placeholder result certificates.

A result must distinguish what was predicted, what was measured, how the
decision rule was applied and what remains unresolved. Amendments preserve
the earlier protocol and evidence and identify which controls need rerunning.
Another investigator should be able to reproduce the same complete boards
and recalculate the reported quantities.

## Relation to current canon

Using the shared method does not claim selection under the wider relations
research programme or reopen its completed work.

Current [Unity](../../../IER/IER-unity.md) and
[Persistence](../../../IER/IER-persistence.md) place physical organization
and persistence first. Singleton successor support is not an exclusionary
unity test. [Ownership](../../../IER/IER-ownership.md),
[membership](../../../IER/IER-processes.md#the-full-membership-relation)
and [Continuity](../../../IER/IER-continuity.md) retain separate physical
burdens. The proposed Life measurements are evidence about a declared model,
without experiential attribution or an identity-proposal test.

The Christmas tree proposal remains an
examples collection (`examples/collections/christmas-tree/README.md` in the development repository).
It could motivate a later bounded comparison, but its physical construction
is separate from this project's first scope.

## Origin of this proposal

This research directory preserves the directions developed in the two chats
originally handed off in `_work/gol-tree`. The useful content now resides in
the example collection, app machine description, worked examples, fixtures,
statistics proposal and investigation records. Conversation wording is not an
accepted definition or result. Unavailable attachment notices and expired chat-library
links are not research inputs; current repository sources are linked directly.
