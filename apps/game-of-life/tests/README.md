# Simulator Tests

## Status

Status: experimental
Authority: non-canonical

Run from `apps/game-of-life` with Make, a C11 compiler (`cc` by default), and
Python 3 for the executable startup-error and POSIX terminal suites:

```bash
make test
make test-unit
make test-unit TEST_CASE=isolated-cell
make test-unit TEST_CASE=local-process-partition
make test-unit TEST_CASE=source-process-networks
make test-unit TEST_CASE=independent-block-boundaries
make test-unit TEST_CASE=minimal-sufficient-boundaries
make test-unit TEST_CASE=minimal-sufficient-components
make test-unit TEST_CASE=periodic-minimal-components
make test-unit TEST_CASE=whole-board-minimal-components
make test-unit TEST_CASE=native-non-null-components
make test-unit TEST_CASE=paired-toad-components
make test-unit TEST_CASE=paired-toad-interventions
make test-unit TEST_CASE=block-support-certificates
make test-unit TEST_CASE=block-regeneration-products
make test-unit TEST_CASE=boundary-replay
make test-unit TEST_CASE=toad-period
make test-unit TEST_CASE=glider-translation
make test-unit TEST_CASE=gosper-emission
make test-ui TEST_CASE=render-bounds
make test-ui TEST_CASE=single-world
make test-ui TEST_CASE=stats-pane
make test-integration TEST_CASE=resize-failures
make test-integration TEST_CASE=stats-lifecycle
make test-cells TEST_CASE=cells-valid
make test-patterns TEST_CASE=pattern-registry
make test-cli TEST_CASE=cli-placement
make test-cli TEST_CASE=cli-numbers
make test-startup TEST_CASE=startup-errors
make test-terminal TEST_CASE=responsive-quit
make test-research TEST_CASE=replay
make test-commands TEST_CASE=command-files
make test-commands TEST_CASE=command-feedback
make test-integration TEST_CASE=command-help
make test-integration TEST_CASE=reply-navigation
make test-integration TEST_CASE=normal-controls
make test-rle TEST_CASE=rle-valid
make test-integration TEST_CASE=command-routing
make test-integration TEST_CASE=startup-order
make test-integration TEST_CASE=startup-patterns
make test-integration TEST_CASE=startup-files
```

`make test` adopts the aggregate command contract in
`fley-org/process/fley-testing.md`. It includes the world unit suite, UI
rendering suite, app-resize integration suite, `.cells` parser suite and built-in
registry, CLI, runtime command, RLE, executable startup-error, real-terminal
and fixed-world research-recording suites.
`make test-unit` runs the world suite, or one exact case
selected through `TEST_CASE`. The default `all` selects every registered case;
empty and unknown selections fail. `make test-ui` runs the rendering suite;
its case selectors are `render-bounds`, `single-world`, `stats-pane` or `all`. The aggregate always runs the
UI, resize, parser, registry and CLI suites after the selected world cases. `make test-integration`
selects all resize cases or an exact case name. `make test-cells` selects all
parser cases or an exact case name. `make test-patterns` runs all registry cases
or an exact case name. `make test-cli` selects all CLI cases or an exact case
name. `make test-commands` selects all runtime command cases or an exact case
name. `make test-rle` selects all RLE cases or an exact case name. The aggregate
includes both suites. `make test-startup` builds the real executable and selects
`startup-errors` or `all`. `make test-terminal` selects `responsive-quit` or
`all`, using Python's standard POSIX PTY interface and the real executable.
It checks normal/narrow/one-row terminals with an INT_MAX delay: help and pause
remain responsive, input wakeups do not add automatic ticks, one q exits from
feedback, and terminal attributes are restored. The suite does not open a GUI
or alter the user's terminal; it owns its pseudoterminals and child processes.
Both executable suites are in the aggregate gate.

`make test-research` builds the existing C engine as a shared library and
qualifies the Python/ctypes recording wrapper. Select `replay`, `inputs`,
`provenance`, `io-failures` or `all`. Literal 32×32 glider/toad phases and
native counts are checked at every step under both topologies; empty/block
controls and fresh-process replay are included. Input tests enforce exact
coordinates, strict JSON and declared resources. Equal-board install tests
preserve event/segment distinctions. Injected write/short-write/flush/close,
fsync, publication-race and allocation/API failures verify visible failure,
no replacement of completed output and native-world cleanup. No Python Life
rule is used. Source hashes identify modified code even without a Git checkout.
Temporary inputs/outputs are isolated and removed after tests; the research
library is `build/research/world.so`, removed by `make clean`.

`make test-measures` qualifies whole-board analysis without a second Life rule.
Select `phases`, `limits`, `paired`, `invalid` or `all`. Literal glider/toad
boards give independent interval, turnover, extent and recurrence expectations.
Block/empty, truncated history with complete interval sets, dead-margin failure,
toroidal translation exclusion and equal-board install resets cover boundary
semantics. Existing near/far block controls run through the C recorder for 32
steps; paired block removal differs at exactly four complete-board sites.
Fresh-process and repeated analysis outputs agree. Malformed/incomplete/partial
traces, mismatched contexts, resource limits, existing files and injected
write/short-write/flush/close/fsync/publication failures must not publish a new
completed output. This suite belongs to the aggregate `make test` gate.

`make test-production` qualifies `exact-neighborhood/1`. Select `exhaustive`,
`geometry`, `interventions`, `failures` or `all`. All 512 interior literal
input tuples are observed through the C engine under both topologies, with
independent positional checks and source category totals 56/84/172/199/1.
Corner wrapping, bounded exterior OFF slots and one-cell torus aliases retain
the engine's directional-neighbor semantics. Paired controls show why one
added neighbor can preserve a two-neighbor survivor while two added neighbors
kill it. Equal-board installs create no update record; subsequent source events
identify the install. Tests also cover target selection/order, initial-only
traces, fresh-process replay, context/resource errors, unequal pair schedules,
incomplete traces, allocation/output failures and no-overwrite publication
including a concurrent writer. The aggregate gate includes this suite;
no Python Life rule or actual-cause/membership criterion is added.

`make test-cross-cases` selects `replay`, `rejected` or `all`. It reconstructs
the declared 21-run matrix through the same C engine and existing recording,
measurement and production modules, checks literal controls and complete native
replays, and compares fresh-process artifact bytes. The read-only checker
recalculates interval sets and verifies saved inputs, production contexts and
completion metadata. Existing directories, corrupt derived sets and engine
failure are rejected. These are model/apparatus checks; they do not select or
accept the root plan's physical-adequacy cases. The suite is in `make test`.

The native runner links `src/world.c` and `src/pattern.c` directly and never initializes the app,
terminal, UI, or event loop. Tests need no TTY, network, external services,
downloaded dependencies, or runtime fixtures. Each case owns and destroys its
worlds. Outputs are console diagnostics and the ignored executable
`build/tests/world-tests`; the rendering runner is `build/tests/ui-tests`.
The resize runner and its allocation-injection object are
`build/tests/resize-tests` and `build/tests/resize-world.o`. `make clean`
removes all test executables and the injection object.

`local-process-partition` exhausts all 512 labeled 3×3 configurations at an
interior site under both topologies. It checks each central output against
an explicit B3/S23 truth table and verifies the binomial input multiplicities
and Beer (2015) Figure 3's five class totals. This is a local-rule source
reconstruction, not a process-network or membership analysis. The development
repository retains its bounded evidence at
`apps/game-of-life/research/investigations/local-process-reconstruction.md`;
that record is outside the public export.

The parser runner/object are `build/tests/cells-tests` and
`build/tests/cells-parser.o`, also removed by `make clean`. Only the test object
redirects `realloc` and `fclose` to failure-injection hooks; product builds use
the standard library. Parser fixtures are original literal strings in
`cells_tests.c`, streamed through temporary files. File-loading tests create
a unique `/tmp/gol-cells-*` directory and remove it after success or failure.
No network, pattern downloads or pre-existing runtime files are needed.

| Parser case | Coverage |
| --- | --- |
| `cells-valid` | Exact offsets and tight bounds, comments, blank rows, irregular row lengths, LF/CRLF/CR, missing final newline, 5001-column rows/comments and reusable output ownership. |
| `cells-invalid` | Invalid symbols, whitespace, inline comments, NUL/high bytes, exact line/column errors, empty/all-dead inputs, invalid arguments and preservation of existing output. |
| `cells-allocation` | Failure of initial allocation and growth, partial-result cleanup and successful 128-cell growth. |
| `cells-files` | Successful path loading, missing files, mid-read I/O failure, close failure and malformed file preservation. |

`build/tests/pattern-tests` links the registry and normal `.cells` parser.
`pattern-registry` verifies names/order, stable identity, dimensions/populations,
unique normalized coordinates and invalid API inputs. `pattern-fixtures`
compares all seven offset lists with reference ASCII phases through the parser
and checks a borrowed view of owned parser data. References and selected phases
are documented in [patterns](../docs/patterns.md); fixture rows are stored in
`pattern_tests.c` and require no downloads. Temporary streams are closed after
each parse, and `make clean` removes the executable. These are data/registry
checks; evolution and translation run in the world suite, and gun emission runs in the world suite as gol-0024.

The same pattern suite also runs `pattern-placement` (all seven built-ins under
both topologies, centered/default/explicit positions, boundary fits and additive
behavior), `pattern-rejection` (negative/overflow coordinates, oversized boxes,
bad late offsets and invalid worlds with no buffer mutation), and
`pattern-parsed-placement` (owned `.cells` output through the shared helper).
These link the real world module and check caller-recomputed live counts.

`build/tests/cli-tests` links the CLI parser and registry without terminal setup.
`cli-selection` covers defaults, all names, file paths, repeated options and
bounded string storage; `cli-placement` covers ordering, centering precedence,
zero coordinates and integer maxima; `cli-invalid` verifies errors and that
failed parses leave the previous config unchanged. Diagnostic capture uses
temporary streams. No files are loaded by argument parsing. Product help and
startup errors are also smoke-checked during development. `make clean` removes
the executable. `cli-numbers` checks every numeric option's minimum/maximum,
signs, whitespace, fractions, junk, overflow, missing/null values and transactional
rejection, plus malformed API vectors and printable error output.
`cli-information` verifies that help/version return an informational exit
without committing partially parsed startup settings.

The rendering suite links `ui.c`, `stats.c` and `world.c` against terminal
capture functions. Across 504 frames it checks layout thresholds, bounds of
every cursor move and text span, preservation of the bottom-right cell,
command/feedback visibility, collapsed/hidden stats and wide-row chunking. It covers
zero/negative dimensions, 1x1 screens and widths through 4097 columns. It
needs no TTY; actual terminal ANSI behavior is separately exercised by PTY
smoke checks during development.

`single-world` checks that a 4x3 board appears exactly once in a larger pane
in bounded and toroidal modes, with stats visible and collapsed. Unused pane
space must erase stale live-cell glyphs while world dimensions, cell state and
toroidal lookups remain unchanged.

`stats-pane` checks exact text and padding for every required field at 60x14
and 80x24, both topologies and running/paused modes. It verifies four-decimal
density, maximum unsigned-long counters, erasure after shorter values and
blanking when stats are hidden. The minimum pane still reserves the terminal's
bottom-right cell and displays the complete topology line.

The integration suite links the real CLI parser, app, command dispatcher, layout, renderer,
stats and world against a controlled terminal-size reader/output capture. Only
the test world object redirects `calloc` to a fault-injection hook; product
builds use the normal allocator. Cases need no TTY or external dependencies:

| Case | Expected behavior |
| --- | --- |
| `startup-order` | Parsed random configuration, topology, fresh flags/command/scratch state, stats before first tick, null/default parity, individual dimension overrides, failed-size-read fallback and tiny startup geometry. |
| `startup-patterns` | All seven built-ins under both topologies: centered defaults, pane-derived dimensions, explicit coordinates overriding center, exact-size fits, no random cells, zero scratch buffer and fresh generation/stats before the first tick. Seeds do not affect pattern output. |
| `startup-files` | CLI loading of paths with spaces, normalization of dead margins, centered and explicit boundary fits in both topologies, initial stats, malformed/empty/missing/oversized files and invalid placement cleanup. |
| `startup-failures` | Unknown pattern/direct invalid mode, negative/extreme coordinates, too-small worlds/tiny terminals and failures of both world allocations produce errors and leave no allocated world. |
| `command-help` | Command h feedback, normal hint, editing/cancel, no alternate display mode and simulation/layout/stat preservation. |
| `reply-navigation` | Read all characters of a maximum-length reply at normal, narrow, tiny and one-row sizes; bounded paging, resize-position retention, reset/dismissal and editor precedence without world/stat changes. |
| `normal-controls` | All normal bindings with/without feedback under both topologies; same-press actions, literal blinker step, seeded randomization, clear, speed limits, editor precedence and step suppression for running/tiny/pending-resize states. |
| `command-routing` | Commands typed through the app key handler execute on Enter, redraw and preserve pause state; Backspace/Esc and immediate re-entry work. A maximum delay stays safe with normal-mode speed keys. |
| `stats-lifecycle` | Snapshots match independently counted cells before the first tick, after automatic/manual steps, pause, delay changes, stats toggles, topology commands, clear, seeded randomize and resize. A blinker plus an isolated cell gives known births/deaths/changed counts. Non-step world mutations reset those counts; pause and display toggles retain them. |
| `resize-transitions` | Shrink/grow, stats-threshold crossings, tiny/zero screens and recovery preserve top-left overlap, dead-fill added space, reset step counts after world mutations, redraw every safe cell and retain topology, generation, pause and command state. Covers evolved boards in both topologies and running/paused modes. |
| `resize-noop` | An unchanged-size signal leaves explicit startup dimensions and step counts intact; a real size change adopts pane dimensions. |
| `resize-failures` | Failed size reads retry without stepping; failure of either world allocation preserves both buffers and suspends automatic/manual stepping. Retry succeeds without another signal, or returns to the retained geometry without allocation. |

Startup file fixtures use a unique `/tmp/gol-startup-*` directory and are removed
on success/failure. Error checks capture stderr through temporary streams; no
terminal raw mode or external dependencies are required.

Tiny-terminal fallback retains a 1x1 world; cells discarded by a shrink never
return on growth. Stats-pane text is checked against the recomputed live count.

The command runner (`build/tests/command-tests`) links the real command,
registry, parser and world modules without a terminal. `make clean` removes it.
File fixtures use a unique `/tmp/gol-commands-*` directory and are removed on
success or failure; no downloaded fixtures or external dependencies are needed.

| Command case | Coverage |
| --- | --- |
| `command-controls` | All required control commands and help alias, whitespace, seed/delay limits, seed-only updates, deterministic randomization and clear/reset semantics. |
| `command-information` | Version/license/public URL replies fit 80 columns, preserve world/scratch/settings/statistics, reject extra arguments and remain discoverable in help. |
| `command-patterns` | All seven names under both topologies, centered/default/explicit positions, exact additive cell results, stats refresh and scratch/generation/pause preservation. |
| `command-files` | Quoted paths with spaces, centered/explicit/additive placement, malformed/empty/oversized/missing files and error locations. |
| `command-feedback` | Specific success/error wording, command syntax guidance, pattern bounds context, printable feedback, help replies and rejection of overflowed editor input. |
| `command-invalid` | Unknown commands, extra/missing arguments, numeric overflow/signs, incomplete coordinates and malformed quoting; rejection preserves both buffers and all app state except command feedback/redraw. |

| Case | Input and expected result |
| --- | --- |
| `empty-world` | A dead 5x5 world stays dead for four steps in bounded and toroidal modes; all step counts remain zero. |
| `isolated-cell` | One central live cell on a bounded 5x5 world dies in one step, producing one death/change and no births/survivors; the next step is empty. |
| `block-stable` | Four-cell block stays unchanged for eight steps, including exact live/survivor counts and zero births/deaths/changed. |
| `blinker-period` | Horizontal/vertical reference phases alternate over eight steps: three live cells, two births/deaths, one survivor, four changed cells per tick. |
| `toad-period` | Six-cell reference phases alternate over eight steps: four births/deaths, two survivors and eight changes per tick. |
| `beacon-period` | Eight/six-cell reference phases alternate over eight steps, with two deaths then two births, six survivors and two changes. |
| `gosper-emission` | Through 124 generations: exact recurring gun core and 1/2/3/4 detached gliders at 30/60/90/120, populations 41/46/51/56, plus glider translations four ticks later. Two origins, both topologies, tick count consistency and stable metadata. |
| `glider-translation` | Sixteen steps, exact (+1,+1) displacement every four steps, five live cells and exact step counts throughout. Covers bounded/toroidal interior and a toroidal case crossing both seams. |
| `seeded-repeatability` | Two toroidal 9x7 worlds initialized with seed 42 match initially and through eight steps, including statistics. This checks repeatability within the current C runtime, not a portable board fixture. |
| `local-process-partition` | All 512 labeled neighborhoods under each topology: every central output, binomial input multiplicities and Beer Figure 3's class totals; no process-network or membership inference. |
| `source-process-networks` | Literal full-board block/blinker/glider phases and their selected phase/position process construction: 16/84, 30/162 and 88/566 nodes/links under both topologies; no arbitrary-boundary discovery or printed-edge isomorphism claim. |
| `independent-block-boundaries` | Two separately stable blocks versus their complete-board union over 32 ticks; adjacent OFF rings give raw cross-links while farther masks do not. Adverse control for graph-based membership under both topologies. |
| `minimal-sufficient-boundaries` | Exhaustive engine-backed truth table and fixed-input mask search: each facing two-neighbor OFF site has a unique seven-OFF sufficient set retaining the reciprocal cross-ring condition. Logical sufficiency only. |

Cases are registered in `world_tests.c`. Failed checks identify the function,
source line and expression, then print a command to reproduce the case.
Checks remain active with `NDEBUG`. The runner exits 0 for success, 1 for failed
checks and 2 for invalid arguments or an empty selection. Make propagates both
runner and compiler failures as nonzero status.

Use `make -B test` after changing compiler/linker flags to force a rebuild.
The registry-based still-life/oscillator cases (`gol-0022`) use independent
literal ASCII phases at two interior origins under both topologies, checking
every cell across the entire board, all StepStats fields and stable metadata.
They do not derive expected phases from the update algorithm or registry offsets.
Glider translation (`gol-0023`) checks four successive period translations,
including seam crossings on a torus. No external fixtures or downloads are used.
`gosper-emission` (`gol-0024`) checks four exact emission checkpoints and each
glider's four-step translation, in 96x80 worlds at two origins under both
topologies. Every checkpoint scans the complete board and checks the growing
population, while each tick verifies count consistency and stable metadata.
The fixed fixture's provenance and finite-board limits are documented in
[gun checkpoint notes](../docs/patterns.md#gun-emission-checkpoints).

The RLE runner (`build/tests/rle-tests`) links the RLE parser, `.cells` ownership
helper, registry and world. Only its parser object redirects allocation,
read/error and close calls to failure hooks. Fixtures are original strings and
file checks use a unique `/tmp/gol-rle-*` directory, removed on success/failure.
`make clean` removes the runner and `build/tests/rle-parser.o`.

| RLE case | Coverage |
| --- | --- |
| `rle-valid` | Exact glider offsets, rule forms, LF/CRLF/CR, physical line wrapping, row counts, normalization, large sparse bounds, long comments, reusable ownership and shared placement. |
| `rle-invalid` | Header/rule/tag/count/termination/bounds errors, NUL/high bytes, empty input, integer overflow, exact locations, invalid arguments and previous-output preservation. |
| `rle-allocation` | Failure of initial/growth allocations after partial live expansion, cleanup and successful 128-cell growth. |
| `rle-files` | Loading, missing files, injected mid-read/close failures and malformed input, without replacing previous output. |

The API and strict accepted subset are documented in [RLE format](../docs/rle-format.md).
Startup/runtime format-selection integration remains separate work.

The executable startup-error suite uses Python's standard library to run the
real binary with piped output, checking 18 invalid CLI/load/placement invocations
and help/version. Informational options print version, copyright, license and
public URL without a TTY, and skip opening a nonexistent startup file. Every
failure must exit 1 with one diagnostic line, no stdout and no
ANSI escapes. Cases cover missing/malformed/conflicting flags, pane size limits,
missing/read/invalid/empty files and invalid placement. Files live in a temporary
`/tmp/gol-errors-*` directory and are removed afterward. No TTY or third-party
Python packages are needed. Native suite targets still require only Make and C11.

`boundary-replay` follows the frozen comparison in the root
case record at `_work/unity/review/discrimination-cases.md`, section
“Boundary replay comparison — 15 November 2026” in the containing repository.
It enumerates 256 neighbor masks with a fixed singleton cut, checking nominal
exterior replay and searching center-flip perturbations through four ticks in
both topologies. All transitions use the native engine. Exterior overwrites
are explicit interventions; agreement or divergence is not a membership verdict.

`minimal-sufficient-components` extends the selected two-block control to both
any-minimum and every-minimum input graphs. Exhaustive local sufficiency is
checked against symbolic minima for each local class; complete pairwise strong
component partitions are checked against independent role/geometry predictions
for near/far fixtures and both topologies. It keeps the ON-core result separate
from shared OFF conditions. It supplies no arbitrary-pattern boundary detector.

`periodic-minimal-components` is a root-owned exploratory follow-up after plan
0002 closure. It checks complete literal phases, local truth-table outputs,
selected source counts and independent graph traversals for block, blinker,
toad and glider under both minimal-set edge policies and topologies. It reports
component size/ON-product censuses. The fixed masks and recurrence quotient
are supplied, not discovered; the result is not constitutive membership.

`whole-board-minimal-components` repeats the periodic audit on the complete
12×12 toroidal phase-board, with the glider return translation reduced modulo
12. It checks literal boards, full-input counts and independent reachability,
and reports how productive components depend on the analysis mask. Post-audit
symbolic checks verify the null-background and four-ON-neighbor minima. This
is a root-owned boundary test; closed plan 0002 remains closed.

`native-non-null-components` scans every coordinate of the native toroidal
boards, admitting update centers with a nonzero nine-input tuple. An independent
literal radius-one predicate is checked at every site/phase, as are the earlier
component censuses. This computes an explicit local process domain without a
shape recognizer; it does not establish unique organizational grouping or the
irrelevance of excluded background conditions.

`paired-toad-components` scans ninety relative toad placements per topology,
retaining only complete-board two-step union histories verified against literal
isolated phases. It audits both graph policies, ON-product origins and independent
traversals, with a predeclared far control and a recorded near mixed-component
witness. The result tests nominal separation rather than independence under all
perturbations or physical membership. This is root-owned formulation work.

`paired-toad-interventions` tests predicted cross-pattern inhibitory OFF
conditions against edited isolated sources, nominal union histories and a far
control. Literal phases and 3/4-neighbor target predictions supply independent
oracles. Far disjoint input patches verify a sufficient condition for two-step
composition. This distinguishes nominal superposition from a specified finite
intervention family, without installing a membership or unity criterion.

`block-support-certificates` checks two declared domains on one block board:
a four-ON-site core with twelve required exterior OFFs, and a sixteen-site
extension with sixteen exterior OFFs and four free outer corners. Literal
next-state expectations, all sixteen corner assignments, distant fill checks
and a necessity witness for each support literal validate the one-step
certificates. Locality supplies the remaining-completion argument. These
certificates account for joint support; they do not select a unique boundary.

`block-regeneration-products` enumerates sixteen initial corner masks and
256 mask pairs for separated tiles under both topologies. Complete-board
predictions and native production/destruction counts distinguish one-corner
repair, undamaged maintenance and decay. The two-block restoration family
tests the limit of regeneration as a unique-boundary criterion. Equal repaired
boards do not by themselves establish retained history or full token identity.
