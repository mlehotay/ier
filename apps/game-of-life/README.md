# Game of Life (Terminal, ANSI, Pane-Based)

An experimental implementation of Conway’s Game of Life with a structured terminal UI.

This project serves two roles:

* a clean, well-structured terminal application
* a proposed substrate for structural analysis using the IER framework


## Status

Status: experimental\
Authority: non-canonical\

The current executable is a simulator baseline, not the completed GoL phase or an implemented IER analysis layer. Runtime commands and random, built-in or file startup initialization are implemented. Non-interactive suites verify block stability, oscillator periods and glider translation; gun emission is verified; the phase-exit audit remains open. See the [design](docs/gol-design.md#implementation-status), [deferred analysis](docs/gol-design-addendum.md), and [orientation](docs/gol-orientation.md).

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

docs/
  gol-design
  gol-design-addendum
  IER-*      -> structural analysis layer
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


## IER Integration

This project is part of the IER (Informational Experiential Realism) work.

Its documentation proposes studying:

* counterfactual dependence $D(s)$
* diachronic relations $C_t$
* region extraction
* structural closure
* projection of admissible futures


### Current Formal Status

The singleton-projection calculation and conditional filtered-set argument are established algebraically. The proposed GoL predicate case and regional perturbation support still need explicit computed witnesses and fully specified history/neighborhood policies. The simulator computes ordinary step statistics, not dependence graphs, closure tests or regional support. None of these objects independently establishes physical UEF qualification.


### Relevant Documents

- [Orientation and canonical boundary](docs/gol-orientation.md)
- [Predicate construction](docs/IER-GoL-predicate-nondecomposability.md) and [conditional argument](docs/IER-life-nondecomposability.md)
- [Regional support proposal](docs/IER-GoL-support-admissibility.md)
- [Local mathematical notation](docs/IER-math-register-expansion.md)
- [Analysis workflow](docs/IER-GoL-getting-started.md)
- [Future directions](docs/IER-GoL-future-directions.md)
- [Constraint cartography proposal](docs/IER-GoL-constraint-cartography.md)
- [Explanation-completeness proposal](docs/IER-GoL-explanation-completeness-criterion.md)


## Intended Use

This is not a game engine or pattern explorer.

It is intended to become a controlled experimental system for:

* structural analysis
* reproducible experiments
* testing formal results


## Future Work

Planned directions include:

* explicit witness construction for non-decomposability
* automated computation of $D(s)$ and $C_t$
* region and closure diagnostics
* perturbation and sensitivity analysis
* pattern taxonomy based on structural properties
* alternative continuation semantics (recruitment)
* scaling and cross-rule validation


## Notes

* rendering avoids full-screen clears to prevent flicker
* requires ANSI-compatible terminal
* designed for Unix-like environments


## License

TBD
