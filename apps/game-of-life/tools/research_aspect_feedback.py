#!/usr/bin/env python3
"""Response-aspect feedback cases; trajectories use the existing native recorder.

Run from the app directory. First build the existing engine:
    make build/research/world.so
Then create a fresh dossier or verify it read-only:
    python3 tools/research_aspect_feedback.py OUTPUT
    python3 tools/research_aspect_feedback.py OUTPUT --check-only
This is not an organizational certificate or another Life engine.
"""
import argparse
import json
from pathlib import Path
import sys
import tempfile

APP=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(APP/'tools'))
from research_run import run,sha256,WorldEngine,RunError
from research_measure import Trace,ordered

PROTOCOL=APP/'research/protocols/response-aspect-feedback-017-v1.json'
OUTER=tuple((x,y) for y in range(3) for x in range(3) if (x,y)!=(1,1))
MASKS=tuple(m for m in range(256) if m.bit_count()==3)
FULL=frozenset((x,y) for y in range(3) for x in range(3))
SOURCES=(Path(__file__).resolve(),APP/'tools/research_run.py',APP/'tools/research_measure.py',
         APP/'src/world.c',APP/'src/world.h')


def require(condition,label):
    if not condition:raise RunError('aspect-feedback check failed: '+label)


def write(path,value):
    with path.open('x',encoding='utf-8') as f:
        json.dump(value,f,indent=2,allow_nan=False);f.write('\n')


def load(path):return json.loads(path.read_text())


def points(mask,center):
    return frozenset(p for j,p in enumerate(OUTER) if mask & (1<<j)) | ({(1,1)} if center else set())


def aspect(live):
    count=len(live-{(1,1)})
    return ['T' if count==2 else 'R' if count==3 else 'D',int((1,1) in live)]


def definitions(protocol_hash,proposal_hash):
    result={}
    def spec(name,initial,actions):
        result[name]=dict(schema='gol-run/1',run_id=name+'/1',
            protocol_id='response-aspect-feedback-017/1',proposal_revision='0.17',
            purpose='confirmatory',protocol_sha256=protocol_hash,proposal_sha256=proposal_hash,
            width=3,height=3,topology='toroidal',rule='B3/S23',initial_live=ordered(initial),
            actions=actions,max_cells=9,max_records=2000)
    for branch in ('stable','center_removed','externally_restored'):
        actions=[]
        for i,mask in enumerate(MASKS):
            if i:actions.append(dict(kind='install',live=ordered(points(mask,branch=='stable'))))
            if branch=='externally_restored':
                actions.extend([dict(kind='step',count=1),
                    dict(kind='install',live=ordered(points(mask,1))),dict(kind='step',count=3)])
            else:actions.append(dict(kind='step',count=4))
        spec(branch,points(MASKS[0],branch=='stable'),actions)
    for mask in (3,5):
        spec('edit-'+str(mask),points(mask,1),[dict(kind='install',live=ordered(points(mask & ~2,1))),
                                              dict(kind='step',count=1)])
    return result


def snapshots(path):
    trace=Trace(path)
    try:return list(trace.snapshots())
    finally:trace.close()


def evaluate(directory):
    values={};sequences={};endpoints={}
    for branch in ('stable','center_removed','externally_restored'):
        rows=snapshots(directory/(branch+'.jsonl'))
        length=6 if branch=='externally_restored' else 5
        require(len(rows)==56*length,'complete branch '+branch)
        sequences[branch]=[];endpoints[branch]=[]
        for i,mask in enumerate(MASKS):
            group=rows[i*length:(i+1)*length]
            start='initial' if i==0 else 'install'
            if branch=='stable':
                expected=[points(mask,1)]*5;types=[start]+['step']*4
            elif branch=='center_removed':
                expected=[points(mask,0),FULL,frozenset(),frozenset(),frozenset()]
                types=[start]+['step']*4
            else:
                expected=[points(mask,0),FULL]+[points(mask,1)]*4
                types=[start,'step','install']+['step']*3
            require([live for _,live in group]==expected,'literal boards '+branch+' '+str(mask))
            require([s['type'] for s,_ in group]==types,'event provenance '+branch)
            path=[aspect(live) for _,live in group]
            sequences[branch].append(path);endpoints[branch].append(group[-1][1])
            if branch=='externally_restored':
                require(group[1][0]['segment']!=group[2][0]['segment'],'restoration segment reset')
        require(len({tuple(map(tuple,p)) for p in sequences[branch]})==1,'uniform predicted aspect path')
        values[branch]=dict(preparations=56,aspect_center_sequence=sequences[branch][0])
    require(endpoints['stable']==endpoints['externally_restored'],'matched restored endpoints')
    require(all(not p for p in endpoints['center_removed']),'extinction under center removal')
    witness=[]
    for mask in (3,5):
        rows=snapshots(directory/('edit-'+str(mask)+'.jsonl'))
        require([r['type'] for r,_ in rows]==['initial','install','step'],'edit event sequence')
        expected=[points(mask,1),points(mask & ~2,1),frozenset() if mask==3 else FULL]
        require([live for _,live in rows]==expected,'edit literal boards')
        witness.append(dict(mask=mask,aspect_before=aspect(rows[0][1])[0],
            aspect_after_edit=aspect(rows[1][1])[0],next_center=aspect(rows[2][1])[1]))
    require([x['next_center'] for x in witness]==[0,1],'same-class edit witness')
    return dict(branches=values,congruence_witness=witness,
        matched_restored_endpoints=56,equal_first_center_output_despite_different_aspect=56,
        organizational_status='NOT_ASSESSED',material_renewal='NOT_ESTABLISHED',
        scope='native response-aspect upkeep, loss, external restoration and intervention compatibility only')


def reconstruct(directory,library):
    directory=Path(directory).resolve();directory.mkdir(exist_ok=False)
    protocol=load(PROTOCOL);write(directory/'protocol.json',protocol)
    ph=sha256(directory/'protocol.json')
    for name,spec in definitions(ph,protocol['proposal_sha256']).items():
        write(directory/(name+'.json'),spec)
        run(directory/(name+'.json'),directory/(name+'.jsonl'),library)
    report=dict(schema='gol-aspect-feedback-results/1',proposal_source=protocol['proposal_source'],
        proposal_sha256=protocol['proposal_sha256'],protocol_sha256=ph,
        source_sha256={str(p.relative_to(APP)):sha256(p) for p in SOURCES},
        findings=evaluate(directory))
    report['artifact_sha256']={p.name:sha256(p) for p in sorted(directory.iterdir()) if p.is_file()}
    write(directory/'results.json',report)
    return report


def verify(directory,library):
    directory=Path(directory).resolve();report=load(directory/'results.json');protocol=load(PROTOCOL)
    require(report['schema']=='gol-aspect-feedback-results/1','report schema')
    require(load(directory/'protocol.json')==protocol,'frozen protocol')
    require(report['source_sha256']=={str(p.relative_to(APP)):sha256(p) for p in SOURCES},'checker/native source hashes')
    require(report['protocol_sha256']==sha256(directory/'protocol.json') and
            report['proposal_source']==protocol['proposal_source'] and
            report['proposal_sha256']==protocol['proposal_sha256'],'protocol/proposal source')
    specs=definitions(report['protocol_sha256'],report['proposal_sha256'])
    names={n+suffix for n in specs for suffix in ('.json','.jsonl')}|{'protocol.json'}
    require(set(report['artifact_sha256'])==names,'complete evidence inventory')
    require({p.name for p in directory.iterdir()}==names|{'results.json'},'unexpected or missing evidence')
    for name,digest in report['artifact_sha256'].items():require(sha256(directory/name)==digest,'artifact '+name)
    for name,spec in specs.items():
        require(load(directory/(name+'.json'))==spec,'fixture '+name)
        engine=WorldEngine(library,3,3,'toroidal')
        try:
            for snapshot,live in snapshots(directory/(name+'.jsonl')):
                if snapshot['type']=='step':engine.step()
                else:engine.install(live)
                require(engine.rows()==snapshot['rows'],'native replay '+name)
        finally:engine.close()
    require(evaluate(directory)==report['findings'],'independent oracles/findings')
    return report


def self_test(library):
    with tempfile.TemporaryDirectory(prefix='gol-aspect-feedback-') as temp:
        root=Path(temp);first=reconstruct(root/'a',library);second=reconstruct(root/'b',library)
        require(first==second,'independent fresh reproduction')
        before={p.name:p.read_bytes() for p in (root/'a').iterdir()}
        verify(root/'a',library)
        require(before=={p.name:p.read_bytes() for p in (root/'a').iterdir()},'read-only verification')
        try:reconstruct(root/'a',library)
        except FileExistsError:pass
        else:raise RunError('existing evidence overwritten')
        path=root/'a/results.json';bad=load(path);bad['findings']['matched_restored_endpoints']=0
        path.write_text(json.dumps(bad))
        try:verify(root/'a',library)
        except RunError:pass
        else:raise RunError('forged derived findings accepted')
    print('PASS literal native oracles, repeatability, read-only replay, no overwrite and forged-result rejection')


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('directory',type=Path,nargs='?')
    parser.add_argument('--check-only',action='store_true')
    parser.add_argument('--self-test',action='store_true')
    args=parser.parse_args();library=APP/'build/research/world.so'
    try:
        if args.self_test:self_test(library);return 0
        if args.directory is None:parser.error('directory required without --self-test')
        report=(verify if args.check_only else reconstruct)(args.directory,library)
        print(json.dumps(report['findings'],indent=2));return 0
    except (RunError,OSError,ValueError,KeyError) as error:
        print('Error:',error,file=sys.stderr);return 1


if __name__=='__main__':raise SystemExit(main())
