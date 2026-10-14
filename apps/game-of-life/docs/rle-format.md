# RLE Parser

`src/rle.c` provides standalone `rle_parse` and `rle_load` alongside the `.cells`
parser. Both return the same owned `CellsPattern`, `CellsResult` and `CellsError`
types. Release either format's output with `cells_pattern_destroy`. A borrowed
`Pattern` view can use the existing [shared placement API](patterns.md#shared-placement).

Startup `--load` and runtime `load` still select `.cells`. Automatic format
selection is separate integration work; this task adds the parser API and tests.

## Accepted Format

The parser implements the two-state Life subset described in
[Golly's RLE format documentation](https://golly.sourceforge.io/Help/formats.html).
It validates declared dimensions strictly, whereas Golly may ignore dimensions
when loading a file.

```text
#N Glider
x = 3, y = 3, rule = B3/S23
bo$2bo$3o!
```

- One header is required, in order: `x = WIDTH, y = HEIGHT`, optionally followed by `, rule = RULE`. The header must end with a line ending before body data.
- Width/height and run counts are positive decimal integers through `INT_MAX`. Header spacing is allowed around tokens; numbers and rule tokens cannot contain whitespace.
- Omitted rule means Conway Life. Supported declarations are `B3/S23`, `S23/B3`, `23/3`, their survival-digit permutations and letter-case variants. Other rules, topology suffixes and extra fields are rejected.
- `b` advances through dead cells, `o` adds live cells and `$` advances rows and resets the column. A preceding count repeats that action; otherwise the count is one. Tags are lowercase.
- `!` terminates the pattern and cannot have a count. Missing termination, unknown tags and non-comment trailing data are rejected.
- Spaces, tabs and physical line endings may separate body tokens; splitting a count from its tag is rejected. Only `$` advances the pattern row.
- Blank lines and comments beginning with `#` in column one are accepted before the header and between body tokens, including after termination. Comments are opaque metadata; position/generation fields do not affect normalized output. Inline or indented comments are rejected.
- LF, CRLF and CR line endings are supported, including an unterminated final body line. NUL and non-ASCII bytes are rejected, including in comments.
- Cell runs must fit the declared width and height. Row advances may reach exactly the declared height for a final `$!`, but no further row advance or cell token is allowed there.
- Empty/all-dead patterns fail. Declared dead margins are trimmed: output dimensions are the tight live-cell box, with offsets normalized to its top-left.

## Ownership and Errors

Initialize output to `{0}`. Success frees any previous output and installs the
new pattern. Failure frees partial input and preserves all previous output.
Stream parsing reads through the terminator/trailing comments to EOF and leaves
the stream open. File loading closes the file before committing output; read,
open or close errors leave previous output unchanged.

Status codes are shared with `.cells`: `CELLS_OK`, `CELLS_INVALID_ARGUMENT`,
`CELLS_IO_ERROR`, `CELLS_INVALID_FORMAT`, `CELLS_EMPTY`, `CELLS_TOO_LARGE` and
`CELLS_NO_MEMORY`. Malformed headers, incompatible rules and bound violations
are format errors; integer/storage overflow is too large. Error details include
a message and available one-based input line/column; API/open/close errors have
zero locations. A successful call clears the optional error object.

The reader streams comments and headers without fixed line buffers. Encoded
dead runs and skipped rows need no expanded storage; only live offsets are
allocated. Live-cell expansion remains subject to available memory and the
addressable allocation size.

## Verification

`make test-rle` runs selectable valid, invalid, allocation and file cases and is
included in `make test`. Fixtures are original literal strings; checks cover
exact glider offsets, long comments, large sparse bounds, normalization,
Conway rule forms, row counts, line endings, shared placement, output reuse,
error locations and atomic failure. Test-only allocation/read/close hooks verify
cleanup and I/O propagation. See [test documentation](../tests/README.md).
