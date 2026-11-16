#!/usr/bin/env python3
"""Qualify cross-case pipeline replay and rejected incomplete/corrupt evidence."""

import contextlib
import io
import json
from pathlib import Path
import subprocess
import sys
import tempfile

APP=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(APP/'tools'))
import research_cross_cases as cross

LIBRARY=None


def replay():
    with tempfile.TemporaryDirectory(prefix='gol-cross-tests-') as temporary:
        root=Path(temporary)
        first=cross.reconstruct(root/'first',LIBRARY)
        result=subprocess.run([sys.executable,str(APP/'tools/research_cross_cases.py'),
                               str(root/'second'),'--engine',str(LIBRARY)],capture_output=True,text=True)
        assert result.returncode==0,result.stderr
        second=json.loads((root/'second/results.json').read_text())
        assert first==second and first['runs']==21
        assert first['grain']=={'glider_cycle_changed_sum':16,'glider_endpoint_difference':8,
                               'glider_comoving_endpoint_difference':0,'toad_cycle_changed_sum':16,'toad_endpoint_difference':0}
        for name,digest in first['artifact_sha256'].items():
            assert cross.sha256(root/'first'/name)==digest
            assert (root/'first'/name).read_bytes()==(root/'second'/name).read_bytes()
        # The read-only checker verifies existing raw boards and measurements.
        before=(root/'first/results.json').read_bytes()
        assert cross.main([str(root/'first'),'--engine',str(LIBRARY),'--check-only'])==0
        assert (root/'first/results.json').read_bytes()==before


def rejected():
    with tempfile.TemporaryDirectory(prefix='gol-cross-rejected-') as temporary:
        root=Path(temporary)
        cross.reconstruct(root/'cases',LIBRARY)
        before=(root/'cases/results.json').read_bytes()
        with contextlib.redirect_stderr(io.StringIO()):
            assert cross.main([str(root/'cases'),'--engine',str(LIBRARY)])==1
        assert (root/'cases/results.json').read_bytes()==before
        # A corrupt derived set must fail even though all raw traces are valid.
        path=root/'cases/glider-measures.jsonl'
        rows=cross.load_jsonl(path);rows[-2]['interval']['visited']=[]
        path.write_text(''.join(json.dumps(row)+'\n' for row in rows))
        with contextlib.redirect_stderr(io.StringIO()):
            assert cross.main([str(root/'cases'),'--engine',str(LIBRARY),'--check-only'])==1
        # Engine failure cannot yield a successful aggregate record.
        with contextlib.redirect_stderr(io.StringIO()):
            assert cross.main([str(root/'failed'),'--engine',str(root/'missing.so')])==1
        assert not (root/'failed/results.json').exists()


def main():
    global LIBRARY
    cases={'replay':replay,'rejected':rejected}
    if len(sys.argv)!=3 or (sys.argv[2]!='all' and sys.argv[2] not in cases):
        print(f"Usage: {sys.argv[0]} ENGINE [all|{'|'.join(cases)}]",file=sys.stderr)
        return 2
    LIBRARY=Path(sys.argv[1]).resolve()
    selected=cases if sys.argv[2]=='all' else {sys.argv[2]:cases[sys.argv[2]]}
    for name,test in selected.items():
        test()
        print(f'PASS cross-cases-{name}')
    return 0


if __name__=='__main__':
    raise SystemExit(main())
