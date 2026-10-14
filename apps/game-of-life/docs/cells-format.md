# `.cells` Parser

`src/cells.c` implements standalone file ingestion for `gol-0011`.
Runtime `load` uses the parser and shared placement under `gol-0006`; see
[command rules](commands.md). Startup loading is implemented under `gol-0012`:
the [CLI parser](cli.md) records `--load`, and app initialization loads the file
into a fresh board with centered default or explicit coordinates. Load/placement
errors free temporary parser data and the world before terminal mode changes.
Borrowed views of parser output use the [shared placement helper](patterns.md#shared-placement).

The standalone [RLE parser](rle-format.md) uses the same output, ownership and
error types. Current startup/runtime `load` still selects `.cells`.

Startup/runtime load errors use `cells_format_error` for common printable
categories, available line/column and path context. Placement errors additionally
report world/pattern dimensions and source. Runtime failure preserves the board;
startup failure frees the world before terminal mode changes.

## Accepted Input

The baseline notation follows the [LifeWiki plaintext format](https://conwaylife.com/wiki/Plaintext):
`O` marks a live cell, `.` marks a dead cell, and lines beginning with `!`
are comments or metadata. The parser's exact policy is:

- Comments are optional and may occur between data rows; they consume no grid row.
- Empty lines count as dead rows. Rows may have different lengths; omitted trailing cells are dead.
- LF, CRLF and CR line endings and a final row without a newline are accepted.
- Whitespace, lowercase `o`, `*`, inline comments, NUL bytes and other data characters are rejected. This is not an RLE parser.
- Empty, comment-only and all-dead inputs return `CELLS_EMPTY`.

The result contains live-cell offsets in row-major order. Width and height
describe the tight live-cell bounding box: dead margins are discarded and
coordinates are translated so the box starts at `(0,0)`. For example,
`..O...` followed by `....O.` produces a 3x2 pattern with offsets `(0,0)` and
`(2,1)`.

## API and Ownership

Include `cells.h` and initialize a `CellsPattern` to `{0}`.
`cells_parse(FILE *, ...)` reads from the current stream position without closing
it; `cells_load(path, ...)` opens and closes a binary-mode file. Paths are
interpreted by content, without requiring a filename extension.

On success, either function frees the output's previously owned cells and
replaces the pattern. On failure, the existing output remains unchanged.
Call `cells_pattern_destroy()` when finished; it resets the fields and may be
called again safely. The parser never mutates a world or chooses placement.

Results distinguish invalid arguments, I/O errors, malformed input, empty
patterns, coordinate/allocation-size overflow, and allocation failure.
Optional `CellsError` includes a message and 1-based input line/column where
applicable (zero otherwise); successful calls clear it.

Rows are streamed without a fixed line-length buffer. Source cell coordinates
must fit nonnegative `int` values below `INT_MAX`, and growing live-cell arrays
are checked for allocation-size overflow. Allocation and file I/O failures
free partial results; file-close failure also leaves the output unchanged.

Run `make test-cells` for parser regression checks; see
[test coverage](../tests/README.md).
