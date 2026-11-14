# Game of Life (Terminal, ANSI, Pane-Based)

An experimental implementation of Conway’s Game of Life with a structured terminal UI.

Simulator version: **0.3.0** (experimental; independent of the IER theory version).
Public IER repository: <https://github.com/mlehotay/ier>.
Use `./gol --version` or runtime commands `:version`, `:license` and `:repo`
to view this information. See the [development TODO](docs/todo.md) for remaining work.
See the [changelog](CHANGELOG.md) for simulator milestones and current changes.

See the [origin and name of the Game of Life](docs/origin.md) for a short history.
The [resource guide](docs/gol-resources.md) covers cellular automata, rule
variants and philosophical comparisons; the
[static display guide](docs/gol-pattern-display.md) covers pattern figures
for Markdown and publications without JavaScript or live simulation.

This project serves two roles:

* a clean, well-structured terminal application
* a proposed substrate for structural analysis using the IER framework


## Status

Status: experimental\
Authority: non-canonical

The current executable is a simulator baseline, not the completed GoL phase or an implemented IER analysis layer. Runtime commands and random, built-in or file startup initialization are implemented. Non-interactive suites verify block stability, oscillator periods and glider translation; gun emission is verified; the phase-exit audit remains open. See the [design](docs/gol-design.md#implementation-status), [deferred analysis](research/gol-design-addendum.md), and canonical GoL account (`IER/IER-game-of-life.md` in the development repository).

A standalone [`.cells` parser](docs/cells-format.md) supports startup and runtime
`load`. A standalone [RLE parser](docs/rle-format.md) is also available; format
selection in startup/runtime loading remains separate, so `load` still uses
`.cells`. Shared
[placement](docs/patterns.md#shared-placement) is implemented with centered and
explicit-coordinate modes and atomic bounds checks.
A deterministic [built-in registry](docs/patterns.md) supplies block, blinker,
toad, beacon, glider, LWSS and Gosper data. Pattern/file/placement options are
[parsed into startup configuration](docs/cli.md) and applied to a fresh board.


## Features

- Classic Conway’s Game of Life (B3/S23)
- Deterministic evolution
- Toroidal or bounded grid
- Interactive controls
- Pane-based UI:
  - top: command/status line
  - left: simulation grid
  - right: stats pane
- Pane rendering with absolute cursor positioning and clipped output; the bottom-right cell stays unused to prevent scroll
- Resize-aware layout
- Simple command system (`:`)


## Build

```bash
make -C apps/game-of-life
```

Run from the repository root (or use `make` and `./gol` inside this directory):

```bash
apps/game-of-life/gol --help
apps/game-of-life/gol --seed 42 --delay 100 --bounded
```

## Tests

Run `make test` inside this directory for the non-interactive world, UI, integration,
parser, registry, CLI, command and executable startup-error suites, or
`make test-unit TEST_CASE=isolated-cell` to select a case. Make, a C11 compiler
and Python 3 (standard library only for executable startup checks) are required. See [test usage and coverage](tests/README.md).

The stats pane appears at 60 columns and 14 rows or larger. Below either
threshold it collapses first; below 20 columns or 5 rows, a compact fallback
keeps command input available. A one-row terminal shows the active command
instead of the fallback message; a 1x1 screen has no safe text cell.

Stats show generation, running/paused state, delay, world dimensions, topology,
live cells, density and births/deaths/changed. Density is the live fraction of
the current world, shown to four decimals. Births/deaths/changed summarize the
last tick; clear, randomize, topology changes and world resize reset them while
refreshing the live count. Pause and stats visibility changes retain them.


## Controls

### Normal mode

| Key     | Action             |
| ------- | ------------------ |
| `q`     | Quit               |
| `space` | Pause / resume     |
| `n`     | Step (when paused) |
| `r`     | Randomize          |
| `c`     | Clear              |
| `+`     | Faster             |
| `-`     | Slower             |
| `s`     | Toggle stats pane  |
| `:`     | Enter command mode |


### Command mode

Enter with:

```
:
```

Examples:

```
help
stats on
stats off
torus on
torus off
bounded
seed 17
delay 250
clear
randomize
pattern glider
pattern block 0 0
load "patterns/my oscillator.cells" center
```

Controls:

* `Enter` -> execute
* `Esc` -> cancel

Command replies use `OK:` or `Error:`. Help is ordinary feedback from `:help` or `:h`; there is no help display mode. The next ordinary key clears feedback and is consumed; `:` enters a new command immediately. `pattern` and `load` add live cells, centered by default or at explicit `X Y`; use `clear` first for a fresh board. `seed` changes the next randomization's seed. See [runtime command rules](docs/commands.md). Random startup accepts `--width`, `--height`, `--seed`, `--delay`, `--bounded`, `--toroidal`, and `--help`; dimensions must fit the initial visible grid and delay must be positive. Use `--pattern` or `--load` to initialize a fresh board, centered by default or at `--x N --y N`; explicit coordinates override `--center`. Only random startup uses the seed, and invalid files or placement fail before terminal mode changes. See [CLI rules and status](docs/cli.md).


## UI Layout

```
+------------------------------------------------------+
| Command / status                                     |
+--------------------------------------+---------------+
|                                      |               |
|              Grid                    |    Stats      |
|                                      |               |
+--------------------------------------+---------------+
```


## Project Structure

```
src/
  main.c        -> entry point
  app.*         -> main loop + coordination
  world.*       -> simulation logic (authoritative state)
  term.*        -> terminal control (raw mode, ANSI)
  ui.*          -> layout + rendering (viewport)
  stats.*       -> derived metrics
  command.*     -> command system
  cli.*         -> startup option parsing

docs/          -> simulator usage, formats and design
research/      -> machine descriptions, worked examples and analysis proposals
```


## Design Principles

### Separation of Concerns

The system is explicitly divided into:

* world -> simulation state and evolution
* ui -> rendering and layout
* stats -> derived quantities
* analysis (IER) -> future descriptive extension, not present executable code


### World vs Viewport

> The terminal display is a projection, not the system.

* stepping uses the full allocated world
* the UI renders the world once at the pane's top-left and blanks unused space; toroidal wrapping applies only to simulation
* the current resize policy changes world dimensions to the grid pane, preserves the top-left overlap and dead-fills added cells; cells outside a shrunken world are lost
* a future analysis must record resize and topology changes as interventions, or use a separately specified fixed-world policy; world/view independence is a deferred design goal

An unchanged-size resize signal leaves explicit startup dimensions intact.
Real terminal size changes adopt pane dimensions (1x1 in the tiny fallback).
Failed size reads or allocations are retried; stepping waits for verified
geometry, and failed allocations retain the existing board until retry succeeds.


### Minimalism

* no external UI libraries
* direct ANSI control
* explicit, inspectable behavior


### Determinism

* evolution is fully determined by the current configuration
* random startup and `r` use the configured seed; the C library PRNG gives repeatability within a build/runtime, not a portable cross-platform board format


## Current IER Research Direction

GoL is the primary v10.12.1 testbed for proposed real organizational membership,
boundary change and successor production. Glider and toad examples investigate
changing participating roles; full experiential qualification remains separate.
The Shannon-information/informational-tension distinction asks why these
information updates lack intrinsic tension on the hypothesis. It introduces
neither an entropy threshold nor an inference that all computation is
non-experiential. The [research material](research/README.md) states the model
questions and limits; the simulator does not implement a UEF qualification test.

## IER Integration

This project is part of the IER (Informational Experiential Realism) work.

Its research studies how organization produces its continuation through
changing participating roles, with controlled comparisons of turnover,
recurrence, boundary change, and intervention history.

### Current Formal Status

For one complete board the deterministic update has one successor, whose
same-fibre projections factorize. This does not exclude unity: the canonical
account withdrew non-factorization as a necessary unity condition. General
constitutive membership and full UEF qualification remain open questions.
The simulator computes ordinary step statistics; research recording,
interval measurements and production traces remain proposed.

The existing IER mathematical register supplies the needed state, transition,
set and relation vocabulary. No separate GoL math-register extension is needed.

### Relevant Documents

- [Research overview](research/README.md)
- Canonical GoL account: `IER/IER-game-of-life.md` in the development repository
- [Life machine](research/game-of-life-machine.md) and worked
  [glider](research/worked-examples/glider.md) / [toad](research/worked-examples/toad.md) examples
- [Persistence statistics plan](research/persistence-statistics-plan.md)
- [Reading list](research/GoL-Reading-List.md) and [follow-up questions](research/GoL-Follow-up-Reading-List.md)
- [Later UI and instrumentation design](research/gol-design-addendum.md)
- [Explanation-completeness proposal](research/IER-GoL-explanation-completeness-criterion.md)


## Intended Use

This is not a game engine or pattern explorer.

It is intended to become a controlled experimental system for:

* structural analysis
* reproducible experiments
* testing formal results


## Future Work

The [statistics plan](research/persistence-statistics-plan.md) specifies
fixed-world recording, turnover and interval measurements, recurrence,
matched interventions, and production provenance. The
[development TODO](docs/todo.md#keep-research-separate) summarizes the software
allocation and its implementation boundaries.


## Notes

* rendering avoids full-screen clears to prevent flicker
* requires ANSI-compatible terminal
* designed for Unix-like environments


## License

Copyright © 2026 Michael Lehotay. The IER Game of Life simulator source and
documentation are licensed under **Creative Commons Attribution-NonCommercial
4.0 International (CC BY-NC 4.0)**, matching the public IER repository.
See [LICENSE](LICENSE) for the full terms, or
<https://creativecommons.org/licenses/by-nc/4.0/>.

Release metadata is maintained in `src/version.h`. Bump the simulator version
there and update the development and public READMEs when preparing a release;
the experimental `0.x` version does not imply phase completion.
