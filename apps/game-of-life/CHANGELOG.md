# IER Game of Life Changelog

Status: development history\
Authority: non-canonical

Simulator versions are independent of IER theory versions. The 0.1.0 and
0.2.0 entries retrospectively name development stages; they do not assert
historical release tags or exact release dates. Version 0.4 describes the
current working version and is not yet a published release. The 0.3.0 entry
retains the preceding development snapshot.

## 0.4 — Unreleased

### Added

- Bounded mechanism audit recording and read-only verification through the
  existing native engine, with independent oracles and replay/corruption tests.
  The results explicitly leave organizational scope and UEF qualification
  unassessed; no new Life rule or organizational classifier is implemented.

### Changed

- Updated the simulator version to `0.4`, using major/minor numbering without
  a patch component, as requested by the author.

## 0.3.0 — Earlier Development Snapshot (Unreleased)

Simulator baseline approaching completion; IER instrumentation planned.

### Added

- Explicit simulator version in `src/version.h`, exposed by `--version` and
  `--help` before terminal initialization.
- Runtime `version`, `license` and `repo` commands for viewing application
  metadata and the public IER URL without changing simulation state.
- Explicit CC BY-NC 4.0 license for simulator source and documentation,
  matching the public IER repository.
- A [development TODO](docs/todo.md) covering remaining controls/UI work,
  audits and follow-on simulator proposals.
- [Resource and rule guide](docs/gol-resources.md), expanded
  [historical background](docs/origin.md), and a
  [static pattern display guide](docs/gol-pattern-display.md).
- Regression checks for informational CLI exits and state-preserving runtime
  metadata commands; public export inputs for the new files.

### Changed

- Separated general simulator documentation in `docs/` from IER investigations
  in `research/`, with worked glider/toad examples and reproducible fixtures.
- Clarified the research direction: GoL implements inspectable organizational
  relations, including successor production, turnover and boundary change.
  Their proposed membership formalization and further experiential
  qualification remain research questions. See the
  [research orientation](research/README.md) and
  [Life machine description](research/game-of-life-machine.md).

### Remaining work

- Improve controls, command/help discovery and UI feedback; complete the
  dependent control audit and simulator phase-exit review.
- Connect the completed standalone RLE parser to startup/runtime file loading.
  Current `--load` and `load` still select `.cells`.
- Implement proposed IER statistics, persistence/pattern metrics and analysis
  instrumentation. Existing live-cell, density and per-step counts are
  simulator statistics; they do not implement those research measurements.

These open items are described in the [TODO](docs/todo.md). This version does
not declare the simulator phase complete or claim a UEF qualification test.

## 0.2.0 — Expanded Simulator (Retrospective Milestone)

Branch development brought the application close to a full terminal simulator.

- Structured command, grid and statistics panes with clipped ANSI rendering,
  small-terminal fallback and hardened resize behavior.
- Startup configuration and runtime commands for topology, random seeds,
  timing, statistics visibility and world initialization.
- Deterministic built-in patterns: block, blinker, toad, beacon, glider,
  lightweight spaceship and Gosper glider gun.
- `.cells` parsing and startup/runtime loading, shared centered or explicit
  placement, bounds validation and atomic failure behavior.
- Standalone RLE parsing with strict Conway-rule validation.
- Live population, density, births, deaths and changed-cell statistics.
- Non-interactive world, UI, resize, parser, placement, CLI, command and
  startup-error checks, including oscillator periods, glider translation
  and Gosper gun emission.

Controls/help redesign and final audits remained open. IER analysis and
research-specific pattern metrics were not implemented at this stage.

## 0.1.0 — Minimal Simulation (Retrospective Milestone)

- Initial minimal working Conway Game of Life simulation.
- Deterministic synchronous B3/S23 evolution with a terminal display.

This stage established the simulation foundation for the later application
and IER investigations.
