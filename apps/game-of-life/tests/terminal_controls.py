#!/usr/bin/env python3
"""Exercise real terminal input, long-delay responsiveness and restoration."""

import fcntl
import os
from pathlib import Path
import select
import struct
import subprocess
import sys
import termios
import time


def read_until(master, expected, timeout=3):
    deadline = time.monotonic() + timeout
    output = bytearray()
    while time.monotonic() < deadline:
        ready, _, _ = select.select([master], [], [], max(0, deadline - time.monotonic()))
        if not ready:
            break
        data = os.read(master, 65536)
        if not data:
            break
        output.extend(data)
        if expected in output:
            return bytes(output)
        if len(output) > 1024 * 1024:
            raise AssertionError("unexpected unbounded terminal output")
    raise AssertionError(f"terminal did not produce {expected!r}; tail={bytes(output[-200:])!r}")


def responsive_quit(executable):
    for width, height in ((80, 24), (20, 5), (6, 1)):
        master, slave = os.openpty()
        process = None
        saved = termios.tcgetattr(slave)
        try:
            fcntl.ioctl(slave, termios.TIOCSWINSZ, struct.pack("HHHH", height, width, 0, 0))
            process = subprocess.Popen(
                [str(executable), "--seed", "42", "--delay", "2147483647"],
                stdin=slave, stdout=slave, stderr=slave, start_new_session=True,
            )
            tiny = width < 20 or height < 5
            read_until(master, b"Term" if tiny else b"[RUNN")
            os.write(master, b":help\r")
            output = read_until(master, b"OK:")
            if not tiny:
                assert b"gen=1 " in output, "input wakeup caused an extra automatic step"
            os.write(master, b" ")
            read_until(master, b"Term" if tiny else b"[PAUS")
            os.write(master, b":help keys\r")
            output = read_until(master, b"OK:")
            if not tiny:
                assert b"gen=1 " in output, "paused input changed generation"
            # One q must dismiss the reply AND quit, without waiting for the
            # simulation's INT_MAX-millisecond deadline or taking another tick.
            os.write(master, b"q")
            assert process.wait(timeout=3) == 0, "nonzero terminal exit"
            assert termios.tcgetattr(slave) == saved, "terminal attributes were not restored"
            print(f"PASS responsive-quit ({width}x{height}, long delay, feedback and raw-mode restoration)")
        finally:
            if process is not None and process.poll() is None:
                process.kill()
                process.wait()
            termios.tcsetattr(slave, termios.TCSANOW, saved)
            os.close(slave)
            os.close(master)


def main():
    if len(sys.argv) not in (2, 3) or (len(sys.argv) == 3 and sys.argv[2] not in ("all", "responsive-quit")):
        print(f"Usage: {sys.argv[0]} EXECUTABLE [all|responsive-quit]", file=sys.stderr)
        return 2
    try:
        responsive_quit(Path(sys.argv[1]).resolve())
    except (AssertionError, OSError, subprocess.TimeoutExpired) as error:
        print(f"FAIL responsive-quit: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
