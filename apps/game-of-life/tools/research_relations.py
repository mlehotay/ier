#!/usr/bin/env python3
"""Verify the public relation evidence and rerun its native finite comparisons."""

import argparse
import copy
import gzip
import json
from pathlib import Path
import subprocess
import sys

from research_run import APP_ROOT, RunError
from research_aspect_feedback import self_test, verify as verify_feedback
from research_mechanism import verify as verify_mechanism


RUNS = APP_ROOT / 'research/investigations/runs'
REPORTS = (
    ('research_responsive_interface.py', '2026-11-17-response-aspects.json'),
    ('research_recurrent_subhistories.py', '2026-11-17-recurrent-subhistories.json.gz'),
)


def require(condition, message):
    if not condition:
        raise RunError('relation verification failed: ' + message)


def compare_report(saved, actual):
    """Compiler-specific library bytes are provenance, not a portable oracle."""
    saved, actual = copy.deepcopy(saved), copy.deepcopy(actual)
    for report in (saved, actual):
        fingerprints = report['sha256']
        binary = fingerprints.pop('build/research/world.so')
        require(isinstance(binary, str) and len(binary) == 64
                and all(c in '0123456789abcdef' for c in binary), 'library fingerprint')
    require(saved == actual, 'finite findings or source fingerprints differ')


def check_reports():
    for script, name in REPORTS:
        result = subprocess.run(
            [sys.executable, str(APP_ROOT / 'tools' / script)],
            check=True, capture_output=True, text=True,
        )
        actual = json.loads(result.stdout)
        path = RUNS / name
        payload = gzip.decompress(path.read_bytes()) if path.suffix == '.gz' else path.read_bytes()
        saved = json.loads(payload)
        compare_report(saved, actual)
        print('PASS native finite report:', name)
        yield saved, actual


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--self-test', action='store_true')
    args = parser.parse_args()
    library = APP_ROOT / 'build/research/world.so'
    try:
        pairs = list(check_reports())
        if args.self_test:
            self_test(library)
            saved, actual = pairs[0]
            # A different compiler can produce different library bytes.
            other_build = copy.deepcopy(saved)
            other_build['sha256']['build/research/world.so'] = '0' * 64
            compare_report(other_build, actual)
            for corrupt in ('findings', 'source'):
                bad = copy.deepcopy(saved)
                if corrupt == 'findings':
                    bad['center_response_states'] = 4
                else:
                    bad['sha256']['src/world.c'] = '0' * 64
                try:
                    compare_report(bad, actual)
                except RunError:
                    pass
                else:
                    raise RunError('forged ' + corrupt + ' accepted')
            print('PASS source/result corruption rejection and compiler portability')
        else:
            verify_mechanism(RUNS / '2026-11-17-mechanism-phase4', library)
            verify_feedback(RUNS / '2026-11-17-response-aspect-feedback', library)
            print('PASS both complete saved trajectory bundles: hashes, fixtures, native replay and findings')
        return 0
    except (RunError, OSError, ValueError, KeyError, subprocess.CalledProcessError) as error:
        print('Error:', error, file=sys.stderr)
        return 1


if __name__ == '__main__':
    raise SystemExit(main())
