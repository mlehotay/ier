#!/usr/bin/env python3
"""Bounded mechanism audits; all trajectories come from the native C engine."""
import argparse
import json
from pathlib import Path
import sys
from research_run import APP_ROOT, RunError, WorldEngine, run, sha256
from research_measure import Trace, ordered
from research_production import export, neighborhood
from research_cross_cases import TOAD, block, glider, shifted

PROTOCOL = APP_ROOT / 'research/protocols/mechanism-phase4-v1.json'
SOURCES = ('src/world.c', 'src/world.h', 'tools/research_run.py',
           'tools/research_measure.py', 'tools/research_production.py',
           'tools/research_cross_cases.py', 'tools/research_mechanism.py')


def require(value, label):
    if not value:
        raise RunError('mechanism audit failed: ' + label)


def load(path):
    return json.loads(path.read_text())


def write(path, value):
    with path.open('x', encoding='utf-8') as handle:
        json.dump(value, handle, indent=2, allow_nan=False)
        handle.write('\n')


def definitions():
    """Predeclared finite preparations; never search outputs for preferred labels."""
    result = {}
    def add(name, live, ticks, w=24, h=24, topology='bounded', actions=None):
        result[name] = dict(schema='gol-run/1', run_id=name+'/1',
            protocol_id='mechanism-phase4-native-audits/1', proposal_revision='0.15',
            purpose='confirmatory', width=w, height=h, topology=topology,
            rule='B3/S23', initial_live=ordered(live),
            actions=actions or [dict(kind='step', count=ticks)],
            max_cells=w*h, max_records=2000)
    def batch(name, inputs, w, h, topology):
        actions = []
        for i, live in enumerate(inputs):
            if i:
                actions.append(dict(kind='install', live=ordered(live)))
            actions.append(dict(kind='step', count=1))
        add(name, inputs[0], 1, w, h, topology, actions)
    local = [{(j % 3, j // 3) for j in range(9) if mask & (1 << j)} for mask in range(512)]
    batch('local512', local, 3, 3, 'toroidal')
    outer = [(x, y) for y in range(4) for x in range(4) if x == 3 or y == 3]
    inputs = [base | {p for j, p in enumerate(outer) if mask & (1 << j)}
        for base in (set(), {(1,1),(0,0),(1,0)}, {(0,0),(1,0),(2,0)}) for mask in range(128)]
    batch('privacy384', inputs, 4, 4, 'bounded')
    add('breach-control', block(3,3), 2, 10, 10)
    add('breach-treated', block(3,3) | {(1,3)}, 2, 10, 10)
    add('moving-glider', glider(0,3,3), 4, 16, 16)
    a, b = block(3,3), block(12,3)
    for name, live in [('block-left',a),('block-right',b),('block-union',a|b),
        ('repair-left',a-{(3,3)}),('repair-right',b-{(12,3)}),('repair-union',(a|b)-{(3,3),(12,3)})]:
        add(name, live, 8)
    for topology in ('bounded','toroidal'):
        for far in (False,True):
            rx, ry = (14,6) if far else (10,2)
            left, right = shifted(TOAD[0],6,6), shifted(TOAD[0],rx,ry)
            prefix = f'toad-{topology}-'+('far' if far else 'near')
            add(prefix+'-joint', left|right, 2, topology=topology)
            for direction in ('lr','rl'):
                source, receiver = (left,right) if direction=='lr' else (right,left)
                edit = (10,6) if direction=='lr' else (rx-1,ry+3)
                for name, live in [('source',source),('receiver',receiver),
                    ('edited-source',source|{edit}),('edited-joint',left|right|{edit})]:
                    add(prefix+'-'+direction+'-'+name, live, 2, topology=topology)
        for label, outer in [('u0',set()),('u1',{(4,6)}),('u3',{(4,5),(4,6),(4,7)})]:
            add(f'support-{topology}-{label}-absent', outer, 1, topology=topology)
            add(f'support-{topology}-{label}-present', block(6,6)|outer, 1, topology=topology)
    add('checkpoint-native', shifted(TOAD[0]), 2)
    add('checkpoint-installed', shifted(TOAD[0]), 2, actions=[dict(kind='step',count=1),
        dict(kind='install',live=ordered(shifted(TOAD[1]))),dict(kind='step',count=1)])
    return result


def snapshots(path):
    trace = Trace(path)
    try:
        return list(trace.snapshots())
    finally:
        trace.handle.close()


def target_oracle(live, target):
    """Independent one-cut truth predicate on the 3x3 torus, never a stepper."""
    x, y = target
    n = sum(((x+dx)%3,(y+dy)%3) in live for dy in (-1,0,1) for dx in (-1,0,1) if (dx,dy)!=(0,0))
    return int(n==3 or (target in live and n==2))


def evaluate(directory):
    boards = {name:snapshots(directory/(name+'.jsonl')) for name in definitions()}
    report = {}
    steps = [live for s,live in boards['local512'] if s['type']=='step']
    require(len(steps)==512,'complete local family')
    universe = frozenset((x,y) for y in range(3) for x in range(3))
    for mask, actual in enumerate(steps):
        initial = frozenset((j%3,j//3) for j in range(9) if mask & (1<<j))
        expected = universe if len(initial)==3 else initial if len(initial)==4 else frozenset()
        require(actual==expected,f'closed-form torus oracle {mask}')
    base = (1<<4)|(1<<0)|(1<<1)
    compatibility = [int((1,1) in steps[m]) for m in (base,base|(1<<2),base|(1<<3),base|(1<<2)|(1<<3))]
    require(compatibility==[1,1,1,0],'single versus joint edits')
    report['P3-T2'] = dict(local_inputs=512, compatibility_outputs=compatibility,
        scope='common-root intervention compatibility only; no conserved resource or endogenous redistribution proved')
    privacy = [live for s,live in boards['privacy384'] if s['type']=='step']
    require(len(privacy)==384,'privacy family size')
    require([[int((1,1) in x) for x in privacy[i:i+128]] for i in (0,128,256)]==[[0]*128,[1]*128,[1]*128], 'fixed input screening')
    c = block(3,3)
    control = [live for _,live in boards['breach-control']]
    treated = [live for _,live in boards['breach-treated']]
    require(control==[c,c,c],'stable literal core')
    require(treated[1]==c|{(2,3),(2,4)},'literal first-step interface breach')
    require((3,3) not in treated[2],'predicted second-step core death')
    moving = [live for _,live in boards['moving-glider']]
    require(moving==[glider(t,3,3) for t in range(5)],'literal glider phases')
    interfaces = []
    for before, after in zip(moving,moving[1:]):
        needed = {(x+dx,y+dy) for x,y in after for dy in (-1,0,1) for dx in (-1,0,1)}
        interfaces.append(len(needed-before))
    report['P3-T1'] = dict(exterior_preparations=384, fixed_outputs=[0,1,1],
        breach_first_core_equal=True, breach_second_core_equal=False,
        moving_interface_sizes=interfaces,scope='privacy and changing inputs; no organizational boundary inferred')
    a,b=block(3,3),block(12,3)
    for t in range(9):
        require(boards['block-left'][t][1]==a and boards['block-right'][t][1]==b
            and boards['block-union'][t][1]==a|b,'far blocks literal union')
    toads=[]
    for topology in ('bounded','toroidal'):
        for far in (False,True):
            rx,ry=(14,6) if far else (10,2)
            prefix=f'toad-{topology}-'+('far' if far else 'near')
            nominal=boards[prefix+'-joint']
            for direction in ('lr','rl'):
                p=prefix+'-'+direction
                source,receiver=boards[p+'-source'],boards[p+'-receiver']
                edited_source,edited=boards[p+'-edited-source'],boards[p+'-edited-joint']
                target=((rx+(2 if far else 1),ry if far else ry+3) if direction=='lr' else (8,6))
                diffs=[]
                for t in range(3):
                    so=(6,6) if direction=='lr' else (rx,ry)
                    ro=(rx,ry) if direction=='lr' else (6,6)
                    require(source[t][1]==shifted(TOAD[t%2],*so),'literal source toad')
                    require(receiver[t][1]==shifted(TOAD[t%2],*ro),'literal receiver toad')
                    require(nominal[t][1]==source[t][1]|receiver[t][1],'nominal toad superposition')
                    diffs.append(len(edited[t][1]^(edited_source[t][1]|receiver[t][1])))
                require(target in nominal[1][1] and (target in edited[1][1])==far,'receiver target witness')
                require(diffs==([0,0,0] if far else [0,2,4]),'complete toad difference counts')
                toads.append(dict(topology=topology,separation='far' if far else 'near',direction=direction,differences=diffs))
    report['P3-T3']=dict(far_block_ticks=9,toad_comparisons=toads,
        scope='nominal generators and reciprocal intervention effects; membership not established')
    cuts=(frozenset({(1,1)}),frozenset(p for p in universe if sum(p)%2==0))
    matches=0
    for mask in range(512):
        initial=frozenset((j%3,j//3) for j in range(9) if mask & (1<<j))
        for inside in cuts:
            combined=frozenset(p for part in (inside,universe-inside) for p in part if target_oracle(initial,p))
            require(combined==steps[mask],'local transition/service representation')
            matches+=1
    report['P3-T4']=dict(transition_service_comparisons=matches,transition_service_matches=matches,
        organizational_status='NOT_ASSESSED',
        reason='Transition/service representation is universal at this typing; a complete independently justified native renewal obligation was not measured.',
        attempted_repairs={'fixed_services':'excludes responsive reciprocal rivals by stipulation',
            'composite_aspect':'concatenating readouts does not establish an indivisible operative aspect',
            'native_producer':'provenance is necessary but does not yet define constitutive scope'})
    c=block(6,6)
    expected={'u0':(frozenset(),c),'u1':(frozenset(),c|{(5,6),(5,7)}),
        'u3':(frozenset({(3,6),(4,6),(5,6)}),c|{(3,6),(4,6),(5,5)})}
    effects=[]
    for topology in ('bounded','toroidal'):
        for label,(absent,present) in expected.items():
            require(boards[f'support-{topology}-{label}-absent'][1][1]==absent,'literal absent support '+label)
            require(boards[f'support-{topology}-{label}-present'][1][1]==present,'literal present support '+label)
            effects.append([topology,label,int((5,6) in absent),int((5,6) in present)])
    for name,full in [('repair-left',a),('repair-right',b),('repair-union',a|b)]:
        require(all(live==full for _,live in boards[name][1:]),'literal regeneration '+name)
    report['P3-T5']=dict(support_effects=effects,independent_aggregate_repairs=True,
        scope='specified model support/loss/recovery only; no complete material renewal or self-maintenance')
    native,installed=boards['checkpoint-native'],boards['checkpoint-installed']
    require([live for _,live in native]==[shifted(TOAD[0]),shifted(TOAD[1]),shifted(TOAD[0])],'literal checkpoint boards')
    require([s['type'] for s,_ in installed]==['initial','step','install','step'],'install events')
    require(installed[1][1]==installed[2][1] and installed[-1][1]==native[-1][1],'equal checkpoint and successor')
    require(installed[1][0]['segment']!=installed[2][0]['segment'],'install resets segment')
    report['P3-T6']=dict(equal_checkpoint=True,equal_final_board=True,
        native_events=[s['type'] for s,_ in native],installed_events=[s['type'] for s,_ in installed],
        scope='event provenance only; no full physical stage-continuity verdict')
    report['P3-T7']=dict(status='INTERPRETATION_AUDIT',qualification='NOT_ASSESSED',
        scope='No UEF/tension quantity measured; no experience or non-experience verdict licensed')
    return report


def production_context():
    return dict(schema='gol-production-context/1',context_id='mechanism-local-center/1',
        trace_run_id='local512/1',proposal_revision='0.15',selection_reason='all 512 ON/OFF neighborhoods',
        targets=[[1,1]],max_updates=512)


def reconstruct(directory,library):
    directory=Path(directory).resolve()
    directory.mkdir(parents=False,exist_ok=False)
    protocol=load(PROTOCOL)
    write(directory/'protocol.json',protocol)
    ph=sha256(directory/'protocol.json')
    for name,spec in definitions().items():
        spec.update(protocol_sha256=ph,proposal_sha256=protocol['proposal_sha256'])
        write(directory/(name+'.json'),spec)
        run(directory/(name+'.json'),directory/(name+'.jsonl'),library)
    context=production_context()
    write(directory/'local-context.json',context)
    export(directory/'local512.jsonl',directory/'local-context.json',directory/'local-production.jsonl')
    result=dict(schema='gol-mechanism-audit/1',source_sha256={p:sha256(APP_ROOT/p) for p in SOURCES},
        proposal_source=protocol['proposal_source'],proposal_sha256=protocol['proposal_sha256'],
        protocol_sha256=ph,findings=evaluate(directory))
    result['artifact_sha256']={p.name:sha256(p) for p in sorted(directory.iterdir()) if p.is_file()}
    write(directory/'results.json',result)
    return result


def verify(directory,library):
    directory=Path(directory).resolve();result=load(directory/'results.json')
    require(result['schema']=='gol-mechanism-audit/1','report schema')
    require(load(directory/'protocol.json')==load(PROTOCOL),'protocol identity')
    require(result['source_sha256']=={p:sha256(APP_ROOT/p) for p in SOURCES},'source revision mismatch')
    files={name+suffix for name in definitions() for suffix in ('.json','.jsonl')}
    files|={'protocol.json','local-context.json','local-production.jsonl'}
    require(set(result['artifact_sha256'])==files,'artifact inventory')
    require({p.name for p in directory.iterdir()}==files|{'results.json'},'unexpected/missing artifact')
    for name,digest in result['artifact_sha256'].items():
        require(sha256(directory/name)==digest,'artifact hash '+name)
    protocol=load(PROTOCOL)
    require(result['proposal_source']==protocol['proposal_source'] and result['proposal_sha256']==protocol['proposal_sha256']
        and result['protocol_sha256']==sha256(directory/'protocol.json'),'proposal/protocol source contract')
    for name,spec in definitions().items():
        spec.update(protocol_sha256=result['protocol_sha256'],proposal_sha256=result['proposal_sha256'])
        require(load(directory/(name+'.json'))==spec,'fixture definition '+name)
        engine=WorldEngine(library,spec['width'],spec['height'],spec['topology'])
        try:
            for snapshot,live in snapshots(directory/(name+'.jsonl')):
                if snapshot['type']=='step': engine.step()
                else: engine.install(live)
                require(engine.rows()==snapshot['rows'],'native replay '+name)
        finally:
            engine.close()
    require(evaluate(directory)==result['findings'],'derived findings mismatch')
    production=[json.loads(line) for line in (directory/'local-production.jsonl').read_text().splitlines()]
    header=production[0]
    require(load(directory/'local-context.json')==production_context(), 'declared production context')
    require(header['context']==load(directory/'local-context.json') and
        header['context_text']==(directory/'local-context.json').read_text() and
        header['context_sha256']==sha256(directory/'local-context.json'), 'production context provenance')
    require(production[-1]['trace_sha256']==[sha256(directory/'local512.jsonl')], 'production trace provenance')
    updates=[r for r in production if r['type']=='update']
    require(len(updates)==512 and production[-1]['status']=='complete','production completeness')
    for mask,row in enumerate(updates):
        require([x['value'] for x in row['inputs']]==[(mask>>j)&1 for j in range(9)],'exact ON/OFF tuples')
        initial={(j%3,j//3) for j in range(9) if mask & (1<<j)}
        require(row['output']==target_oracle(initial,(1,1)),'production truth oracle')
        actual = initial if len(initial)==4 else {(x,y) for y in range(3) for x in range(3)} if len(initial)==3 else set()
        require(row['target']==[1,1] and all(row[k]==v for k,v in neighborhood(initial,actual,(1,1),
            {'width':3,'height':3,'topology':'toroidal'}).items()), 'full production tuple/target')
        for key,kind,event,step,tick in [('source_event','initial' if mask==0 else 'install',2*mask,mask,0),
                                       ('result_event','step',2*mask+1,mask+1,1)]:
            require(row[key]==dict(type=kind,event_index=event,step_index=step,segment=mask,tick=tick),
                    'production occurrence counters')
    return result


def main(argv=None):
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('output_directory',type=Path)
    parser.add_argument('--engine',required=True,type=Path)
    parser.add_argument('--check-only',action='store_true')
    args=parser.parse_args(argv)
    try:
        result=(verify if args.check_only else reconstruct)(args.output_directory,args.engine.resolve())
        print(f'Verified {len(definitions())} native traces; 512 local inputs; 384 exterior preparations.')
        print('P3-T4:',result['findings']['P3-T4']['organizational_status'])
        return 0
    except (RunError,OSError,ValueError,KeyError,StopIteration) as error:
        print(f'Error: {error}',file=sys.stderr);return 1


if __name__=='__main__':
    raise SystemExit(main())
