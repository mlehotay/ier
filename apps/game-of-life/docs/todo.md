# IER Game of Life Development TODO

Status: development roadmap summary\
Authority: non-canonical\
Last reconciled: 2026-11-14 (simulator 0.3.0)

This is the readable simulator backlog. In the development repository, task
status is owned by `apps/game-of-life/_work/tasks.csv`, expanded definitions by
`_work/tasks.md`, and phase scope by `_work/plans/0001-gol-phase-completion.md`.
Those private workflow files are not part of the public projection. This guide
summarizes them without replacing their execution state.

## Finish the simulator baseline

Do these in dependency order:

- [ ] **Improve the controls, UI and command/help discovery (`gol-0015`, ready).**
  Define clear normal-mode bindings, command entry, help discovery and feedback
  dismissal. Make the available actions understandable within the pane layout.
  The previous persistent help toggle was removed; help currently behaves as
  transient command feedback. Decide how users can read command guidance that
  exceeds the visible width, including on small terminals. Update usage docs
  and regression checks with the agreed behavior.
- [ ] **Audit the resulting controls (`gol-0016`, blocked on `gol-0015`).**
  Check pause/resume, single-step while paused, clear, randomize, speed,
  statistics visibility, command editing/cancel/execute and redraw behavior.
  Confirm which actions dismiss feedback and whether they also execute, and
  verify that informational commands preserve simulation state. Recheck narrow
  and tiny terminals after UI changes.
- [ ] **Run the phase-exit review (`gol-0025`, todo).**
  Check the final behavior against the [design](gol-design.md): startup options,
  commands, built-in patterns, external `.cells` loading, statistics, terminal
  stability and reproducible tests. Most prerequisites are already done; the
  control audit remains open. Passing tests alone does not close the plan.

Small-terminal fallback, clipped pane drawing, resize hardening, command/load
diagnostics and startup ordering are already implemented. Revisit them where
the control redesign changes their behavior, rather than treating them as
missing features. The current world deliberately tracks the terminal grid:
shrinking discards cells outside the retained overlap.

## Follow-on simulator work outside the baseline exit criteria

- [ ] **Connect RLE loading to startup and runtime commands.** The standalone
  parser (`gol-0026`) is done; `--load` and `:load` still use `.cells`.
  Specify format selection and diagnostics, preserve atomic placement/failure
  behavior, and add end-to-end tests. See [RLE policy](rle-format.md).
- [ ] **Consider independent world and viewport behavior.** A fixed-world mode,
  off-screen storage and panning are deferred design work. Specify them before
  changing the current resize contract, especially for reproducible experiments.
- [ ] **Add an offline snapshot/figure export path if needed.** The
  [static display guide](gol-pattern-display.md) proposes fixture-based,
  generation-labeled SVG/PNG output through an owning Make target. There is
  currently no headless exporter. Preserve rule, topology, placement, crop and
  source provenance.

These are follow-on proposals, not extra dependencies silently added to the
current phase. Add bounded implementation tasks when taking them up.

## Release and public distribution

Version 0.3.0, `--version`, runtime `:version` / `:license` / `:repo`, the
CC BY-NC 4.0 license and the public IER URL are provided. For each release:

- [ ] Update `src/version.h` and the development/public READMEs together.
- [ ] Run `make test` and `make check-work` from the app directory; run the
  owning repository projection and link checks when preparing the public copy.
- [ ] Check that the projected app includes its headers, license, tests and
  linked docs, and builds independently of private workflow files.
- [ ] Publish a release only after the applicable review; keep simulator
  versions independent of the IER theory version.

## Keep research separate

The literature-led unity reproduction work is allocated in development plan
`0002-gol-unity-relations-reproduction`, tasks `gol-0028`–`gol-0032`:

- [ ] Specify fixed-world inputs, events/snapshots, relation versions and independent expected fixtures (`gol-0028`, ready).
- [ ] Add a headless runner through the existing C engine and reproducible provenance (`gol-0029`, after specification and baseline phase exit).
- [ ] Add reviewed persistence and matched-run measurements (`gol-0030`).
- [ ] Add reviewed ON/OFF production and contribution traces and checkpoint provenance comparisons (`gol-0031`).
- [ ] Reproduce separation, interaction, gun/output and grain comparisons and hand evidence to the dossier (`gol-0032`).

Use the [nineteen-paper list](../research/GoL-Reading-List.md) to inform definitions
and controls. Root unity tasks own the physical proposal, case assessment and
canonical reconciliation. Software output supports a declared relation; it is
not a membership or experiential-unity verdict.


IER instrumentation, dependence/region/support analysis and persistence
measurements remain proposals under [research/](../research/README.md).
They follow completion of the simulator baseline and are not missing ordinary
Life controls. The simulator does not currently implement a UEF qualification
test. See the [statistics plan](../research/persistence-statistics-plan.md)
for the proposed research measurements and their limits.
