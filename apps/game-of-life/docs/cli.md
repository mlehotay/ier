# Startup CLI

`cli_parse_args` accepts the following separate flag/value arguments:

```text
--width N --height N --delay MS --seed N
--bounded --toroidal --help
--pattern NAME --load FILE --x N --y N --center
```

Startup supports random, built-in and `.cells` sources. With no selector, the
world is randomized with the current seed. `--pattern` and `--load` initialize
a fresh board through the same [placement helper](patterns.md#shared-placement)
used by runtime commands. They do not randomize or use the seed. Generation
starts at zero and live cells/density are valid before the first tick.

Initialization reads terminal geometry, creates a world with the requested or
pane-derived dimensions and topology, applies exactly one source, then refreshes
statistics and finalizes layout flags on a common success path. The app uses
the same defaults as CLI parsing. If startup terminal-size lookup fails, the
established 80x24 fallback is used; valid tiny geometry uses the 1x1 fallback
board. An explicit width or height overrides only that dimension. Dimensions
must fit the initial grid pane. Failed loading or
placement prints a startup error and frees the world before raw mode or the
alternate screen is entered; it never substitutes a random or empty board.

```bash
./gol --pattern glider --bounded
./gol --pattern block --width 8 --height 6 --x 0 --y 0
./gol --load "patterns/my oscillator.cells" --center
```

Runtime `pattern` and `load` commands are available independently of startup
initialization; see [command rules](commands.md). Numeric validation is shared
between startup and runtime commands.

## Selection and Placement Rules

- `--pattern` accepts one exact lowercase name from the [registry](patterns.md).
- `--load` records a nonempty path of up to 255 bytes. Parsing arguments does not open the file or check its format/existence.
- `--pattern` and `--load` are mutually exclusive, in either argument order.
- Placement defaults to centered. `--x` and `--y` must both be supplied and accept decimal integers from 0 through `INT_MAX`.
- Explicit coordinates override `--center` regardless of flag order.
- Coordinates and `--center` require a pattern or file selector; they do not apply to random startup.
- Repeated occurrences of the same value-bearing option use the last value. Selecting both source types still fails.
- Shared placement (`gol-0009`) validates bounds against a particular world; the CLI parser only records coordinates.

Existing width/height/delay options accept decimal integers from 1 through
`INT_MAX`; seeds accept 0 through `UINT_MAX`. Empty strings, signs, whitespace,
trailing text and overflow are rejected. `--bounded` and `--toroidal` remain
mutually exclusive. Use `./` for a path that starts with `--`, so it cannot be
mistaken for another option.

## Diagnostics

Malformed/missing values report the option and required decimal range. Unknown
options point to `--help`; unknown patterns list accepted names. Parse failures
leave config unchanged. Startup size/placement failures report the world and
pattern dimensions. File failures use categorized shared detail with available
line/column and path; failures exit 1 before raw mode or alternate-screen output.
Help exits 0. Terminal-bound diagnostics replace control/non-ASCII bytes with `?`.

## API and Verification

Initialize `StartupConfig` through `cli_startup_defaults`. Parsed placement
lives in `StartupConfig.placement`; the app copies it into its placement state.
Successful parsing returns 0 and commits the completed config. Errors return
-1 and help returns 1, both leaving the caller's config unchanged. Errors go
to the supplied stream (stderr when null); help goes to stdout.

`make test-cli` covers source selection, numeric and string limits, placement
precedence, invalid combinations and transactional error handling. It is part
of `make test`. The app integration cases `startup-patterns`, `startup-files`
and `startup-failures` exercise CLI-to-world initialization, exact cell states,
topology, centering/coordinate precedence, fresh stats and cleanup after errors.
See [test commands](../tests/README.md).
