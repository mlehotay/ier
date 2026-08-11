# Game of Life (Terminal, ANSI, Pane-Based)

A minimal, high-performance implementation of Conway - s Game of Life with a structured terminal UI.

This project serves two roles:

* a clean, well-structured terminal application
* a platform for structural analysis using the IER framework


## Status

Status: experimental
Authority: non-canonical
See the repository status-zones governance document.


## Features

- Classic Conway - s Game of Life (B3/S23)
- Deterministic evolution
- Toroidal or bounded grid
- Interactive controls
- Pane-based UI:
  - top: command/status line
  - left: simulation grid
  - right: stats pane
- Non-flickering rendering (no full-screen clears)
- Resize-aware layout
- Simple command system (`:`)


## Build

```bash
make
````

Run:

```bash
./gol
```


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
```

Controls:

* `Enter` -> execute
* `Esc` -> cancel

Command output is transient and cleared on next input.


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
* analysis (IER) -> structural interpretation


### World vs Viewport

> The terminal display is a projection, not the system.

* the simulation operates on the full grid
* the UI renders a viewport of that state
* resizing or layout changes do not affect dynamics or analysis


### Minimalism

* no external UI libraries
* direct ANSI control
* explicit, inspectable behavior


### Determinism

* evolution is fully determined by the current configuration
* no randomness except explicit initialization


## IER Integration

This project is part of the IER (Informational Experiential Realism) work.

It provides a concrete system for studying:

* counterfactual dependence (D(s))
* diachronic relations (C_t)
* region extraction
* structural closure
* projection of admissible futures


### Key Result

> Closure predicates on projected continuations do not, in general, factor across subregions - even under deterministic dynamics.


### Relevant Documents

```
docs/IER-game-of-life
docs/IER-life-nondecomposability
docs/IER-math-register-expansion
docs/IER-GoL-getting-started
docs/IER-GoL-future-directions
```


## Intended Use

This is not a game engine or pattern explorer.

It is a controlled experimental system for:

* structural analysis
* reproducible experiments
* testing formal results


## Future Work

Planned directions include:

* explicit witness construction for non-decomposability
* automated computation of (D(s)) and (C_t)
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
