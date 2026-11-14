#!/usr/bin/env python3
"""Exercise the real executable's fail-fast contract without a terminal."""
import pathlib
import subprocess
import sys
import tempfile


def main():
    if len(sys.argv) not in (2, 3) or (len(sys.argv) == 3 and sys.argv[2] not in ("all", "startup-errors")):
        print("Usage: startup_errors.py BINARY [all|startup-errors]", file=sys.stderr)
        return 2
    binary = str(pathlib.Path(sys.argv[1]).resolve())
    with tempfile.TemporaryDirectory(prefix="gol-errors-") as directory:
        root = pathlib.Path(directory)
        malformed = root / "invalid.cells"
        malformed.write_text("O.X\n")
        empty = root / "empty.cells"
        empty.write_text("! empty fixture\n...\n")
        wide = root / "wide.cells"
        wide.write_text("OOOO\n")
        missing = root / "missing.cells"
        cases = [
            (["--width", "0"], ["--width", "decimal integer"]),
            (["--height", "-1"], ["--height", "decimal integer"]),
            (["--delay", "1x"], ["--delay", "decimal integer"]),
            (["--seed", "18446744073709551616"], ["--seed"]),
            (["--width"], ["--width requires a value"]),
            (["--unknown"], ["unknown option", "--help"]),
            (["--pattern", "unknown"], ["unknown pattern", "glider"]),
            (["--pattern", "block", "--x", "0"], ["supplied together"]),
            (["--pattern", "block", "--load", str(missing)], ["mutually exclusive"]),
            (["--bounded", "--toroidal"], ["mutually exclusive"]),
            (["--width", "2147483647"], ["exceeds visible grid"]),
            (["--load", str(missing)], ["I/O error", "Cannot open", str(missing)]),
            (["--load", str(root)], ["I/O error", "Cannot read", str(root)]),
            (["--load", str(malformed)], ["Invalid .cells format", "line 1, column 3", str(malformed)]),
            (["--load", str(empty)], ["Empty pattern", "no live cells", str(empty)]),
            (["--load", str(wide), "--width", "3", "--height", "3"],
             ["outside the world", "World is 3x3", "pattern is 4x1", str(wide)]),
            (["--load", str(wide), "--width", "8", "--height", "6", "--x", "7", "--y", "0"],
             ["outside the world", "World is 8x6", str(wide)]),
            (["--pattern", "glider", "--width", "2", "--height", "2"],
             ["outside the world", "World is 2x2", "pattern is 3x3"]),
        ]
        for arguments, fragments in cases:
            result = subprocess.run([binary, *arguments], capture_output=True, timeout=5)
            message = result.stderr.decode("utf-8", errors="replace")
            if (result.returncode != 1 or result.stdout or not message.startswith("error: ")
                    or b"\x1b" in result.stdout + result.stderr or message.count("\n") != 1
                    or any(fragment not in message for fragment in fragments)):
                print(f"FAIL startup-errors: {arguments!r}; status={result.returncode}; "
                      f"stdout={result.stdout!r}; stderr={result.stderr!r}", file=sys.stderr)
                return 1
        # Informational options work without a TTY and short-circuit startup:
        # the missing file would fail if world/file initialization were reached.
        for option in ("--help", "--version"):
            result = subprocess.run([binary, "--load", str(missing), option],
                                    capture_output=True, timeout=5)
            expected = (b"IER Game of Life 0.3.0", b"Copyright (c) 2026 Michael Lehotay",
                        b"CC BY-NC 4.0", b"https://creativecommons.org/licenses/by-nc/4.0/",
                        b"https://github.com/mlehotay/ier")
            if (result.returncode or result.stderr or b"\x1b" in result.stdout
                    or any(fragment not in result.stdout for fragment in expected)
                    or (option == "--help" and b"--version" not in result.stdout)):
                print(f"FAIL startup-errors: {option}; stdout={result.stdout!r}; "
                      f"stderr={result.stderr!r}", file=sys.stderr)
                return 1
    print(f"PASS startup-errors ({len(cases)} failures, help and version; no ANSI output)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
