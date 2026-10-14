# Built-in Pattern Registry

`gol-0008` supplies seven immutable patterns in `src/pattern.c`, and `gol-0009`
supplies shared world placement. Runtime `pattern` and `load` are implemented
under `gol-0006`; see [command rules](commands.md). Startup application is
implemented under `gol-0010` and `gol-0012`; [startup selection](cli.md)
initializes a fresh board with a built-in or parsed pattern instead of random.

| Name | Width x height | Live cells | Selected phase |
| --- | --- | --- | --- |
| `block` | 2x2 | 4 | Filled square |
| `blinker` | 3x1 | 3 | Horizontal row |
| `toad` | 4x2 | 6 | Upper row shifted right |
| `beacon` | 4x4 | 8 | Two diagonally touching filled blocks |
| `glider` | 3x3 | 5 | `.O.` / `..O` / `OOO` |
| `lwss` | 5x4 | 9 | `.O..O` / `O....` / `O...O` / `OOOO.` |
| `gosper` | 36x9 | 36 | Standard horizontal Gosper glider gun |

Coordinates use `(dx,dy)` offsets from the top-left of the tight bounding box,
with positive x to the right and positive y down. Cell lists are row-major,
contain no duplicates, and retain the fixed phase/orientation above. Registry
lookups allocate nothing, introduce no randomness and do not modify worlds.

## API

Include `pattern.h`. `pattern_lookup(name)` accepts exact lowercase names and
returns a borrowed `const Pattern *`, or `NULL` for null/unknown names.
`pattern_count()` returns the registry size; `pattern_at(index)` enumerates in
the table's order and returns `NULL` outside the registry.

`pattern_dimensions(pattern, &width, &height)` returns 0 for a valid view;
either output may be null. Missing/empty patterns or nonpositive dimensions
return -1 without changing outputs. Built-ins and their cell arrays have static
lifetime: callers must not modify or free them.

`CellsOffset` aliases `PatternCell`, so an owned `.cells` result can supply a
borrowed `Pattern` view without copying or casting its offsets. That view lasts
only until the owning `CellsPattern` is replaced or destroyed. It does not
transfer ownership to the registry.

## Shared Placement

`pattern_place(world, pattern, x, y)` adds the pattern's live cells at an explicit
top-left origin. `pattern_place_centered` uses
`((world_width - pattern_width) / 2, (world_height - pattern_height) / 2)`;
an odd extra row/column is left on the bottom/right.
`pattern_apply(world, pattern, placement)` selects between them: null placement
or absent coordinates defaults to centered; `has_xy` overrides the centering
flag, including when the explicit position is invalid.

Both built-ins and borrowed views of `.cells` output use this same path.
Placement adds live cells without clearing existing cells or changing the
scratch buffer, dimensions or topology. Callers that want to replace the world
must clear it first and recompute statistics after successful placement. No
app generation, run state or statistics are reset by these World-level helpers.

The entire declared bounding box must fit, even in toroidal mode. Negative or
overflow-prone positions, oversized boxes and invalid offsets are rejected
before any writes; placement never clips or wraps. Worlds must be initialized
and have dimensions whose area fits the simulator's integer indexing.
Results distinguish success, invalid arguments/world, invalid pattern data and
out-of-bounds placement; `pattern_place_message(result)` supplies a concise
message. Failure leaves both world buffers unchanged.

## Reference Phases and Tests

The small patterns use the conventional shapes documented by LifeWiki:
[block](https://conwaylife.com/wiki/Block),
[blinker](https://conwaylife.com/wiki/Blinker),
[toad](https://conwaylife.com/wiki/Toad),
[beacon](https://conwaylife.com/wiki/Beacon), and
[glider](https://conwaylife.com/wiki/Glider).
The LWSS coordinates match the RLE phase `bo2bo$o$o3bo$4o!` on the
[LWSS page](https://conwaylife.com/wiki/LWSS). The gun matches the example
on the [plaintext format page](https://conwaylife.com/wiki/Plaintext).
These mathematical cell arrangements are stored as offsets, not copied article
text. Reference ASCII rows are version-controlled in `tests/pattern_tests.c`;
test execution performs no downloads.

`make test-patterns` checks registry lookup/enumeration, exact dimensions and
counts, unique normalized in-bounds coordinates, stable identity/order and
agreement with reference rows parsed through the `.cells` API. It is included
in `make test`. Placement cases additionally check exact world output for all
seven patterns, centering/explicit precedence, boundary fits, additive behavior,
parsed-pattern views and atomic rejection in both topologies.
`make test-unit` now verifies block stability, exact blinker/toad/beacon phases
and glider translation from the registry, with full-board and tick-statistics
checks in both topologies. A separate glider case crosses the torus seams.
Gun emission is verified by `gol-0024`; see [test coverage](../tests/README.md).

## Gun Emission Checkpoints

The registered phase is the 36-cell, period-30
[Gosper glider gun](https://conwaylife.com/wiki/Gosper%27s_glider_gun).
`gosper-emission` uses fixed, complete checkpoints at generations 30/60/90/120:
the original 36x9 gun phase recurs and the total populations are 41/46/51/56.
Coordinates below are relative to the gun's original top-left:

| Glider age at a checkpoint | Top-left | Reference rows |
| --- | --- | --- |
| 0 | 23,9 | `O..` / `.OO` / `OO.` |
| 30 | 30,17 | `O.O` / `.OO` / `.O.` |
| 60 | 38,24 | `O..` / `.OO` / `OO.` |
| 90 | 45,32 | `O.O` / `.OO` / `.O.` |

Each checkpoint contains one new glider at age zero and all previously emitted
gliders at the listed older phases. Four ticks later each one keeps its phase
and moves (+1,+1); a surrounding dead border confirms detached motion. Expected
boards are literal fixtures, cross-checked with an independent sparse B3/S23
calculation during development and not derived from world_step at test runtime.
The real registry and placement path seed the world.

The case runs through generation 124 in 96x80 worlds at origins (3,3) and (8,8)
under bounded and toroidal topology. These finite runs have enough margin to
avoid clipping, wrapping or returning gliders interacting with the gun. This
verifies repeated emission over the stated interval; it is not an assertion of
unlimited growth on a finite board. No external fixtures or downloads are needed.
