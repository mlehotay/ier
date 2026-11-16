# Runtime Commands

Enter command mode with `:`, type a command and press Enter. Backspace edits;
Esc cancels. Commands are case-sensitive. Leading/trailing whitespace and tabs
between arguments are accepted. Extra arguments and malformed numbers fail.

| Command | Behavior |
| --- | --- |
| `help` or `h` | List command syntax as transient feedback; use `[` and `]` to page long replies. |
| `help keys` | List normal controls, reply navigation and dismissal. |
| `help COMMAND` | Show the selected command's syntax, for example `help load`. |
| `version` | Show the simulator name and version, independent of IER theory versions. |
| `license` | Show CC BY-NC 4.0 and its full-terms URL. |
| `repo` | Show the public IER repository URL: <https://github.com/mlehotay/ier>. |
| `stats on` / `stats off` | Show/hide stats without changing the world or last tick counts. |
| `torus on` / `torus off` | Enable/disable neighbor wrapping and refresh the stats snapshot. |
| `bounded` | Alias for bounded topology (`torus off`). |
| `seed N` | Set the seed for subsequent `randomize` commands and the `r` key; leave the current world unchanged. |
| `delay MS` | Set a positive millisecond delay, shared with the normal-mode speed keys. |
| `pattern NAME [center\|X Y]` | Add a built-in pattern using shared placement. |
| `load FILE [center\|X Y]` | Parse a `.cells` file and add its normalized live cells using shared placement. |
| `clear` | Clear both world buffers and reset generation and step counts. |
| `randomize` | Randomize with the current seed, reset generation and refresh live-cell statistics. |

Seed accepts decimal digits from 0 to `UINT_MAX`; delay accepts 1 to `INT_MAX`.
`version`, `license` and `repo` accept no arguments and preserve the world,
generation, pause state, settings and statistics. `:repo` gives the URL its
own reply so it is readable in an ordinary 80-column terminal. Feedback is
readable with `[` and `]` on narrower terminals; `./gol --version` prints all metadata
without entering terminal mode. URLs are plain text for copying or opening
with a terminal's link support; the app does not launch a browser.

Coordinates accept 0 to `INT_MAX`. Signs, fractions, trailing characters and
overflow are rejected with the same numeric rules as startup options.

Pattern names are the exact lowercase [registry names](patterns.md). Placement
defaults to centered independently of startup coordinates. `center` explicitly
selects that default; `X Y` supplies a complete coordinate pair. Placement is
additive and preserves generation, pause state and topology. Use `clear` first
to replace a board. Success refreshes live cells/density and zeros tick counts.
Invalid names, bounds, coordinates or file content leave the board, scratch
buffer, generation, settings and statistics unchanged. No clipping or wrapping
is applied to placement, including in toroidal mode.

Quote a complete file path with single or double quotes if it contains spaces.
Quoted tokens do not support escapes or shell expansion; relative paths resolve
against the process's current directory. For example:

```text
clear
pattern glider
pattern block 0 0
load "patterns/my oscillator.cells" center
seed 17
randomize
delay 250
bounded
```

Every execution leaves command mode and requests redraw. Replies begin with
`OK:` or `Error:`. Known commands with invalid arguments show their syntax;
unknown commands/names provide guidance. World mutations report live counts,
placement reports the origin, and out-of-bounds errors include dimensions. Load
errors use shared categories and retain available parser line/column and file
context, matching startup loading. Dynamic feedback is
sanitized to printable ASCII and clipped to pane width. Input exceeding 255
characters is marked in the editor and rejected as a whole on Enter; Esc cancels.

Help is an ordinary command reply from `:help` or `:h`, with the same lifetime
and rendering as other feedback. It does not create another display mode or
change layout/simulation state. There is no normal-mode h shortcut. While a
reply is present, `[` and `]` move backward/forward by the current visible
text width; an available `[ ]` prefix advertises navigation. Very narrow panes
omit the prefix to leave room for text. One-row terminals show the reply in
their only row, preserving the reserved bottom-right cell. Resizing keeps the
current reply position; a new reply or command resets it.

`Esc` and unbound keys dismiss a reply. Normal control keys clear it and also
execute on the same press, including `q` and space; `:` enters a new command
immediately. In the command editor, brackets and control letters are text,
Backspace deletes, Enter executes and Esc cancels. The normal hint starts
with `:help`; `:help keys` and `:help COMMAND` provide focused guidance.
Normal `n` steps only while paused with usable geometry and no pending resize.
Speed keys change delay by 10ms, clamped to the positive range 1–`INT_MAX`.
Input stays responsive even at a long delay; receiving a key or redrawing a
reply does not trigger an extra automatic step. Pause/delay changes establish
a fresh automatic-step deadline.
Pattern/file [startup initialization](cli.md) uses the same placement rules on
a fresh board, while runtime placement adds to the current board.
