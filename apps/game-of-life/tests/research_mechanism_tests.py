#!/usr/bin/env python3
"""Independent-oracle replay and integrity tests for bounded mechanism audits."""
import contextlib
import io
import json
from pathlib import Path
import sys
import tempfile

APP=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(APP/'tools'))
import research_mechanism as audit
from research_run import RunError, sha256


def raises(call):
    try:
        call()
    except (RunError,OSError,ValueError,KeyError):
        return
    raise AssertionError('expected rejection')


def save(path,value):
    path.write_text(json.dumps(value,indent=2)+'\n')


def replay(library):
    with tempfile.TemporaryDirectory(prefix='gol-mechanism-replay-') as temp:
        root=Path(temp)
        first=audit.reconstruct(root/'first',library)
        second=audit.reconstruct(root/'second',library)
        assert first==second
        before={p.name:p.read_bytes() for p in (root/'first').iterdir()}
        checked=audit.verify(root/'first',library)
        assert checked==first
        assert before=={p.name:p.read_bytes() for p in (root/'first').iterdir()}
        f=checked['findings']
        assert f['P3-T1']['exterior_preparations']==384
        assert f['P3-T2']['compatibility_outputs']==[1,1,1,0]
        assert f['P3-T4']['transition_service_matches']==1024
        assert f['P3-T4']['organizational_status']=='NOT_ASSESSED'
        assert f['P3-T6']['installed_events']==['initial','step','install','step']
        assert f['P3-T7']['qualification']=='NOT_ASSESSED'


def corrupt(library):
    with tempfile.TemporaryDirectory(prefix='gol-mechanism-corrupt-') as temp:
        root=Path(temp)/'case';audit.reconstruct(root,library)
        original={p.name:p.read_bytes() for p in root.iterdir()}
        def reset():
            for name,data in original.items():(root/name).write_bytes(data)
        # Forged derived observation cannot pass by keeping raw trace hashes.
        result=audit.load(root/'results.json')
        result['findings']['P3-T2']['compatibility_outputs']=[1,1,1,1]
        save(root/'results.json',result)
        raises(lambda:audit.verify(root,library));reset()
        # Changed fixture cannot be laundered by rehashing its artifact row.
        spec=audit.load(root/'breach-control.json');spec['initial_live']=[]
        save(root/'breach-control.json',spec)
        result=audit.load(root/'results.json')
        result['artifact_sha256']['breach-control.json']=sha256(root/'breach-control.json')
        save(root/'results.json',result)
        raises(lambda:audit.verify(root,library));reset()
        # Preserve all exported truth values while corrupting the target location.
        path=root/'local-production.jsonl'
        rows=[json.loads(line) for line in path.read_text().splitlines()]
        next(r for r in rows if r['type']=='update')['target']=[0,0]
        path.write_text(''.join(json.dumps(r)+'\n' for r in rows))
        result=audit.load(root/'results.json')
        result['artifact_sha256'][path.name]=sha256(path)
        save(root/'results.json',result)
        raises(lambda:audit.verify(root,library));reset()
        # Incomplete data, unknown files and existing output paths are refused.
        (root/'privacy384.jsonl').unlink()
        raises(lambda:audit.verify(root,library));reset()
        (root/'unexpected.json').write_text('{}')
        raises(lambda:audit.verify(root,library));(root/'unexpected.json').unlink()
        raises(lambda:audit.reconstruct(root,library))
        assert original=={p.name:p.read_bytes() for p in root.iterdir()}
        with contextlib.redirect_stderr(io.StringIO()):
            assert audit.main([str(root/'missing'),'--engine',str(root/'absent.so')])==1
        assert not (root/'missing/results.json').exists()


def main():
    cases={'replay':replay,'corrupt':corrupt}
    if len(sys.argv)!=3 or (sys.argv[2]!='all' and sys.argv[2] not in cases):
        print(f'Usage: {sys.argv[0]} ENGINE [all|replay|corrupt]',file=sys.stderr);return 2
    library=Path(sys.argv[1]).resolve()
    selected=cases if sys.argv[2]=='all' else {sys.argv[2]:cases[sys.argv[2]]}
    for name,test in selected.items():
        test(library);print('PASS mechanism-'+name)
    return 0


if __name__=='__main__':
    raise SystemExit(main())
