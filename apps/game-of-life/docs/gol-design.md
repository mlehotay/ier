# gol-design

## Design Spec: Game of Life Terminal Application

## Purpose

This document defines the design of the Game of Life application itself.

Its scope is limited to building a complete, stable, and maintainable terminal implementation of Conway - s Game of Life with:

- ANSI terminal rendering
- a structured pane-based UI
- interactive controls
- pattern support
- file loading
- simulation-native statistics

This document does not specify future IER analysis features.

Those belong in separate future-work documents.


## Scope

This design covers the GoL application as a standalone program.

It includes:

- simulation state and stepping
- terminal layout and redraw behavior
- command and input handling
- built-in pattern support
- external pattern loading
- resize handling
- ordinary Game of Life statistics
- code organization

It excludes:

- theory-specific overlays
- future-cone visualization
- coherence or nondecomposability analysis
- proof-oriented instrumentation
- region-level analytical tooling
- theory-driven metrics beyond ordinary simulator needs


## Core design stance

The application is deliberately based on:

- raw terminal mode
- ANSI escape sequences
- explicit cursor positioning
- fixed pane layout
- simple direct rendering
- minimal dependencies

This is a structured terminal simulator, not a terminal widget framework.


## Primary goals

The primary goals are:

- implement Conway - s Game of Life correctly
- make terminal behavior stable and predictable
- provide an interactive, usable interface
- support deterministic and file-based pattern initialization
- keep the codebase maintainable
- separate simulation logic from UI and terminal control


## Non-goals

This project is not intended to provide:

- a general terminal UI toolkit
- overlapping windows or dynamic pane management
- mouse interaction
- a retained-mode widget system
- analysis-layer visualization in the GoL phase


## Feature definition of the GoL phase

The GoL phase is considered complete when the application provides:

- stable ANSI terminal rendering
- correct resize handling
- no redraw-induced terminal scrolling
- pane-based UI with command and stats support
- interactive runtime controls
- deterministic built-in patterns
- pattern placement controls
- file-based pattern loading
- topology control
- simulation-native statistics
- maintainable modular source structure


## Architecture overview

The application is organized into a small set of focused modules:

```text
src/
  main.c
  app.c
  app.h
  world.c
  world.h
  term.c
  term.h
  ui.c
  ui.h
  stats.c
  stats.h
  command.c
  command.h
````

### Responsibility split

#### `world.*`

Owns:

* world dimensions
* allocation and cleanup
* clear
* randomization
* stepping
* neighbor counting
* topology behavior
* resizing policy if world size changes

#### `term.*`

Owns:

* raw mode enable/disable
* terminal restoration
* terminal size queries
* signal-related terminal events
* cursor movement helpers
* low-level terminal output helpers
* input polling

#### `ui.*`

Owns:

* layout computation
* pane geometry
* frame redraw
* command pane rendering
* grid pane rendering
* stats pane rendering
* separators and borders

#### `stats.*`

Owns:

* collection of derived simulation metrics
* step summaries
* formatting-oriented statistical snapshots

#### `command.*`

Owns:

* command buffer state
* command parsing
* command execution dispatch
* transient messages and errors

#### `app.*`

Owns:

* top-level runtime state
* subsystem coordination
* main-loop update behavior
* resize integration
* redraw policy
* input routing

#### `main.c`

Owns:

* process entry
* CLI parsing
* app initialization
* startup and shutdown flow


## Simulation model

The simulation is standard Conway - s Game of Life.

### Rule set

* birth: `B3`
* survival: `S23`

### Topology modes

The application supports:

* bounded mode
* toroidal mode

Topology is part of world behavior, not UI behavior.


## Terminal UI model

The application uses a fixed three-pane layout.

```text
+--------------------------------------------------------------+
| Command / status                                             |
+------------------------------------------+-------------------+
|                                          |                   |
|                                          |                   |
|               Grid pane                  |    Stats pane     |
|                                          |                   |
|                                          |                   |
+------------------------------------------+-------------------+
```

This layout is part of the current GoL phase, not deferred work.

### Pane roles

#### Command pane

Used for:

* status summary
* command input
* transient messages
* mode hints

#### Grid pane

Used for:

* rendering the Game of Life board
* showing the visible simulation state clearly and stably

#### Stats pane

Used for:

* simulation-native metadata
* live counts and basic derived values
* runtime mode display


## Geometry model

Screen geometry is centralized.

```c
typedef struct {
    int x;
    int y;
    int w;
    int h;
} Rect;
```

```c
typedef struct {
    int screen_w;
    int screen_h;

    Rect cmd;
    Rect grid;
    Rect stats;

    int stats_visible;
    int stats_width;
    int cmd_height;
    int draw_borders;
} Layout;
```

A single layout computation path determines pane dimensions from raw terminal size.

This keeps terminal size detection separate from drawable-area policy.


## Terminal sizing and redraw policy

### Raw terminal size

Terminal helpers return raw terminal dimensions only.

They do not embed layout policy.

### Layout policy

Layout computation determines:

* command pane height
* stats pane visibility and width
* grid pane size
* fallback behavior in small terminals

### Scroll avoidance

Rendering must not rely on line-stream behavior.

The canonical rendering rule is:

> draw using explicit cursor positioning only.

In particular:

* do not depend on a trailing newline after the final visible row
* do not allow redraw to push content past the bottom of the screen
* redraw each pane in place

This is required to avoid terminal scroll behavior.


## Resize behavior

On terminal resize:

* terminal dimensions are re-read
* layout is recomputed
* the screen is fully redrawn
* the application remains visually stable

The implementation may either:

* resize the world with overlap preservation, or
* keep world dimensions stable and adapt only the view

For the GoL phase, either policy is acceptable as long as it is:

* explicit
* consistent
* predictable

If the world is resized, the overlapping region must be preserved and newly exposed cells must begin dead.


## Application state model

A single top-level application object coordinates runtime state.

Representative structure:

```c
typedef struct {
    World world;
    Layout layout;

    unsigned long generation;
    unsigned int seed;
    int delay_ms;
    int paused;
    int running;

    int needs_redraw;
    int resized;

    int show_stats;
    int show_help;

    StepStats last_step;
    CommandLine cmd;
} App;
```

The exact shape may evolve, but the architectural rule is fixed:

> subsystem state should be coordinated centrally rather than scattered across the loop.


## Rendering architecture

Rendering is frame-oriented and pane-specific.

Representative structure:

```c
void ui_draw_frame(const App *app);
void ui_draw_command_pane(const App *app, const Rect *r);
void ui_draw_grid_pane(const App *app, const Rect *r);
void ui_draw_stats_pane(const App *app, const Rect *r);
void ui_draw_borders(const Layout *L);
```

### Rendering rules

* redraw using absolute cursor moves
* treat each pane as clipped to its rectangle
* keep border logic minimal
* prefer correctness and stability over clever terminal tricks

The UI is simple by design.


## Command interface

The command system is part of the GoL application proper.

It is used for runtime control, not for future analysis features.

### Command-line state

Representative structure:

```c
#define CMD_BUF_CAP 256

typedef struct {
    int active;
    char buf[CMD_BUF_CAP];
    int len;
    int cursor;
    char message[CMD_BUF_CAP];
} CommandLine;
```

### Command-mode behavior

At minimum:

* `:` enters command mode
* text input edits the command buffer
* `Enter` executes
* `Esc` cancels
* `Backspace` deletes

### GoL-phase command targets

The command system should support ordinary simulator operations such as:

* help
* stats on/off
* torus on/off
* bounded mode
* seed changes
* delay changes
* pattern selection
* file loading
* clear
* randomize


## Input model

Input is routed centrally.

Representative entry point:

```c
void app_handle_key(App *app, int ch);
```

### Routing policy

* if command mode is active, input goes to the command editor
* otherwise, input is interpreted as normal-mode control

### Normal-mode controls

Expected runtime controls include:

* `q` quit
* `space` pause/resume
* `n` single-step
* `r` randomize
* `c` clear
* `+` faster
* `-` slower
* `:` enter command mode
* `s` toggle stats visibility
* `h` toggle help or hints

This is sufficient for the GoL phase.


## Built-in pattern support

Deterministic built-in patterns are required for the GoL phase.

### Initial built-in set

Recommended baseline set:

* block
* blinker
* toad
* beacon
* glider
* LWSS
* gosper glider gun

### Interface

The application should support startup options such as:

```bash
./gol --pattern glider
./gol --pattern blinker
./gol --pattern gosper
```

If no explicit position is provided, placement should default to centered.


## Pattern placement controls

The application should support deterministic placement of built-in patterns.

### Interface

```bash
./gol --pattern glider --x 10 --y 5
./gol --pattern gosper --center
```

### Policy

* explicit coordinates override centering
* centering is the default
* out-of-bounds placement must follow one documented rule

Preferred first-pass rule:

> fail clearly on invalid placement rather than silently clipping.


## File-based pattern loading

External pattern loading is part of the GoL phase.

### Purpose

This makes the application a reusable Life tool rather than only a built-in demo program.

### Recommended support order

1. internal development format if useful
2. `.cells`
3. RLE later

### Interface

```bash
./gol --load patterns/glider.cells
```

The loading subsystem belongs to ordinary simulator functionality.


## Statistics model

The GoL phase includes only simulator-native statistics.

These are ordinary descriptive metrics derived from simulation state.

### Step statistics

Representative structure:

```c
typedef struct {
    unsigned long live_cells;
    unsigned long births;
    unsigned long deaths;
    unsigned long survivors;
    unsigned long changed;
} StepStats;
```

Representative stepping interface:

```c
void world_step(World *w, StepStats *out);
```

### Snapshot statistics

Representative stats-pane structure:

```c
typedef struct {
    unsigned long generation;
    unsigned long live_cells;
    double density;
    unsigned long births;
    unsigned long deaths;
    unsigned long changed;

    int paused;
    int delay_ms;
    int toroidal;
} StatsSnapshot;
```

Collected through a dedicated function such as:

```c
void stats_collect(const App *app, StatsSnapshot *out);
```

### Initial stats pane content

The stats pane should display values such as:

* generation
* paused/running
* delay
* world dimensions
* topology
* live cells
* density
* births
* deaths
* changed cells

These are sufficient for the GoL phase.


## Main loop design

The loop should keep subsystem boundaries explicit.

Representative structure:

```c
while (app.running) {
    app_handle_signals(&app);
    app_handle_resize(&app);
    app_poll_input(&app);
    app_update(&app);

    if (app.needs_redraw) {
        ui_draw_frame(&app);
    }

    app_sleep(&app);
}
```

### Responsibilities

#### `app_handle_signals`

* consume signal-related events
* mark resize pending if needed

#### `app_handle_resize`

* re-read raw terminal size
* recompute layout
* apply world or view resize policy
* request redraw

#### `app_poll_input`

* read nonblocking input
* dispatch by current mode

#### `app_update`

* advance simulation if running
* record step stats
* request redraw when state changes

#### `ui_draw_frame`

* redraw panes and separators
* leave cursor in a predictable state


## Failure and fallback behavior

The program must fail clearly and display sane messages.

### Small terminals

If the terminal is too small:

* command pane has highest priority
* stats pane may collapse first
* grid pane receives remaining space
* if usable rendering is impossible, show a compact error/status message instead of drawing garbage

### Runtime errors

The command pane should surface errors such as:

* invalid commands
* load failures
* invalid pattern names
* out-of-bounds placement
* terminal-too-small state


## CLI surface

The application should behave like a small but proper command-line tool.

### Target option style

```bash
./gol --width 80 --height 24 --seed 42 --delay 100 --pattern glider
```

### Likely option set

* `--width` N
* `--height` N
* `--delay` MS
* `--seed` N
* `--pattern` NAME
* `--load` FILE
* `--bounded`
* `--toroidal`

The CLI belongs to the GoL application layer, not a later extension layer.


## Validation and expected behaviors

The GoL phase should include lightweight validation against known Life behavior.

### Useful checks

* block remains stable
* blinker has period 2
* toad has period 2
* beacon has period 2
* glider translates diagonally
* gosper emits gliders

These checks do not require a heavy testing framework at first, but they should exist in some reproducible form.


## Completion boundary

The GoL phase ends when the application is:

* stable in the terminal
* correct as a Life simulator
* comfortable to operate
* deterministic when requested
* capable of loading external patterns
* maintainable as a small codebase

At that point, the application is a complete GoL layer.

Future theory-oriented work should be specified separately rather than folded back into this document.


## Summary

This project is a modular ANSI terminal implementation of Conway - s Game of Life with:

* pane-based UI
* interactive controls
* command support
* built-in patterns
* file loading
* topology control
* simulator-native statistics

Its design priorities are:

* correctness
* terminal stability
* maintainability
* explicit subsystem boundaries
* completion of the GoL layer before analysis-oriented extension
