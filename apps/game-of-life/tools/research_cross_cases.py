#!/usr/bin/env python3
"""Reconstruct declared cross-cases through the existing recorder and C engine."""

import argparse
import hashlib
import json
import os
from pathlib import Path
import sys

from research_run import RunError, WorldEngine, run, sha256
from research_measure import Trace, measure, ordered
from research_production import export


GLIDER = (
    ((1,0),(2,1),(0,2),(1,2),(2,2)),
    ((0,1),(2,1),(1,2),(2,2),(1,3)),
    ((2,1),(0,2),(2,2),(1,3),(2,3)),
    ((1,1),(2,2),(3,2),(1,3),(2,3)),
)
TOAD = (((1,1),(2,1),(3,1),(0,2),(1,2),(2,2)),
        ((2,0),(0,1),(3,1),(0,2),(3,2),(1,3)))
GUN = (
    '........................O...........',
    '......................O.O...........',
    '............OO......OO............OO',
    '...........O...O....OO............OO',
    'OO........O.....O...OO..............',
    'OO........O...O.OO....O.O...........',
    '..........O.....O.......O...........',
    '...........O...O....................',
    '............OO......................',
)
# Pilot-selected contact fixture; the source result documents the selection
# and the independent local count argument, not a confirmatory prediction.
ANNIHILATION = (
    ((6,5),(7,6),(5,7),(6,7),(7,7),(8,9),(9,9),(8,10),(9,10)),
    ((5,6),(7,6),(6,7),(7,7),(6,8),(7,8),(8,8),(8,9),(9,9),(8,10),(9,10)),
    ((7,6),(5,7),(6,8),(9,8),(8,10),(9,10)),
    ((6,7),(8,9),(9,9)),
    (),
)


def shifted(points, dx=5, dy=5):
    return frozenset((x+dx,y+dy) for x,y in points)


def glider(tick, dx=5, dy=5, phase=0):
    cycle, phase = divmod(tick + phase, 4)
    return shifted(GLIDER[phase], dx+cycle, dy+cycle)


def block(x,y):
    return frozenset(((x,y),(x+1,y),(x,y+1),(x+1,y+1)))


def gun():
    return frozenset((x+3,y+3) for y,row in enumerate(GUN) for x,cell in enumerate(row) if cell=='O')


def scenarios(proposal_hash=None):
    result = {}
    def add(name, live, steps, width=32, height=32, actions=None):
        data = {'schema':'gol-run/1','run_id':'cross-'+name+'/1',
                'protocol_id':'q3-q5-bounded-reconstruction/1','proposal_revision':'0.2',
                'purpose':'reconstruction','width':width,'height':height,
                'topology':'bounded','rule':'B3/S23','initial_live':ordered(live),
                'actions':actions if actions is not None else [{'kind':'step','count':steps}],
                'max_cells':width*height,'max_records':steps+4}
        if proposal_hash is not None:
            data['proposal_sha256']=proposal_hash
        result[name]=data
    left,near,far=block(2,2),block(6,2),block(8,2)
    for name,live in (('left',left),('right-near',near),('right-far',far),
                      ('blocks-near',left|near),('blocks-far',left|far)):
        add(name,live,32,14,10)
    base=glider(0); near=base|{(5,5)}; far=base|{(15,15)}
    for name,live in (('glider',base),('glider-near',near),('glider-far',far),
                      ('remainder',base|block(24,20)),('remainder-near',near|block(24,20))):
        add(name,live,8)
    add('toad',shifted(TOAD[0]),2)
    add('toad-outside',shifted(TOAD[0])|{(5,5)},2)
    add('toad-b',shifted(TOAD[1]),4)
    add('toad-b-minus',shifted(TOAD[1])-{(7,5)},4)
    add('collision-block',block(8,9),4)
    add('collision-extinction',ANNIHILATION[0],4)
    add('mixed-block',block(9,7),1)
    add('mixed-contact',base|block(9,7),1)
    add('gun',gun(),64,96,80)
    add('gun-remove-output',gun(),64,96,80,
        [{'kind':'step','count':30},{'kind':'install','live':ordered(gun())},{'kind':'step','count':34}])
    # Literal phase of the detached output at gun tick 30, specified without
    # copying a calculated snapshot. It is normalized standard phase 3.
    add('detached',glider(0,25,11,3),34,96,80)
    return result


SELECTIONS = {
    'glider-near': ([[6,5],[6,8],[7,8]],'glider'),
    'toad-outside': ([[5,6],[6,5]],'toad'),
    'toad-b-minus': ([[6,6],[7,6],[8,6]],'toad-b'),
    'collision-extinction': ([[7,8],[8,8],[6,7]],None),
    'mixed-contact': ([[8,6],[8,7],[8,8]],None),
    'gun-remove-output': ([[26,12],[27,13],[28,13]],None),
}


def write_json(path, value):
    with path.open('x',encoding='utf-8') as handle:
        text=json.dumps(value,indent=2,allow_nan=False)+'\n'
        if handle.write(text)!=len(text):
            raise RunError('short cross-case input write')


def load_jsonl(path):
    return [json.loads(line) for line in path.read_text().splitlines()]


def require(condition, label):
    if not condition:
        raise RunError('cross-case check failed: '+label)


def verify(directory, library):
    """Check complete literal fixtures, replay, interval sets and comparisons."""
    directory=Path(directory)
    boards={}; snapshots={}
    protocol_hash=sha256(directory/'protocol.json')
    for name,data in scenarios().items():
        trace=Trace(directory/(name+'.jsonl'))
        try:
            expected_manifest={**data,'protocol_sha256':protocol_hash}
            if 'proposal_sha256' in trace.manifest:
                expected_manifest['proposal_sha256']=trace.manifest['proposal_sha256']
            require(trace.manifest==expected_manifest,name+' declared scenario manifest')
            require((directory/(name+'.json')).read_text()==trace.header['manifest_text'],name+' exact saved replay input')
            observations=list(trace.snapshots())
        finally:
            trace.close()
        snapshots[name]=[snapshot for snapshot,_ in observations]
        boards[name]=[live for _,live in observations]
        # Replay through the same C engine independently of the recorder and
        # analysis wrappers. Literal controls below provide separate oracles.
        engine=WorldEngine(library,data['width'],data['height'],data['topology'])
        try:
            engine.install(data['initial_live'])
            for snapshot,live in observations:
                if snapshot['type']=='step': engine.step()
                elif snapshot['type']=='install':
                    engine.install(ordered(gun()))
                expected=[''.join('O' if (x,y) in live else '.' for x in range(data['width']))
                          for y in range(data['height'])]
                require(engine.rows()==expected,name+' native replay at event '+str(snapshot['event_index']))
        finally:
            engine.close()
        measures=load_jsonl(directory/(name+'-measures.jsonl'))
        require(len(measures)==len(observations)+2,name+' complete observations')
        require(measures[-1]['status']=='complete' and measures[-1]['trace_sha256']==[sha256(directory/(name+'.jsonl'))],name+' measured input hash')
        # Independent segment-wide set derivation from all exported boards.
        for segment in {item['segment'] for item in snapshots[name]}:
            indices=[i for i,item in enumerate(snapshots[name]) if item['segment']==segment]
            sets=[boards[name][i] for i in indices]
            union=set().union(*sets); intersection=set(sets[0]).intersection(*sets)
            interval=measures[indices[-1]+1]['interval']
            require(interval['visited']==ordered(union) and interval['continuously_live']==ordered(intersection),name+' interval sets')
    for name,(sites,pair) in SELECTIONS.items():
        path=directory/(name+'-context.json')
        records=load_jsonl(directory/(name+'-production.jsonl'))
        require(records[0]['context_text']==path.read_text() and records[0]['context_sha256']==sha256(path),name+' production context hash')
        expected_hashes=[sha256(directory/(name+'.jsonl'))]
        if pair: expected_hashes.append(sha256(directory/(pair+'.jsonl')))
        steps=sum(item['type']=='step' for item in snapshots[name])
        require(records[-1]['type']=='completion' and records[-1]['status']=='complete'
                and records[-1]['trace_sha256']==expected_hashes
                and records[-1]['records']==len(records)
                and records[-1]['updates']==steps*len(sites)
                and records[-1]['events']==len(boards[name]),name+' production completion')
    for name in ('left','right-near','right-far','blocks-near','blocks-far','collision-block','mixed-block'):
        require(all(live==boards[name][0] for live in boards[name]),name+' literal stable board')
    for separation in ('near','far'):
        require(all(joint==left|right for joint,left,right in zip(boards['blocks-'+separation],boards['left'],boards['right-'+separation])),separation+' component union')
    require(all(live==glider(tick) for tick,live in enumerate(boards['glider'])),'every literal glider phase')
    require(boards['glider-far'][1:]==boards['glider'][1:],'far perturbation disappears without changing glider')
    require((6,5) in boards['glider-near'][1] and all(not live for live in boards['glider-near'][6:]),'near survival and extinction at six')
    for name,control in (('remainder','glider'),('remainder-near','glider-near')):
        require(all(live==local|block(24,20) for live,local in zip(boards[name],boards[control])),name+' independent remainder')
    require(all(live==shifted(TOAD[tick%2]) for tick,live in enumerate(boards['toad'])),'literal toad A phases')
    require(all(live==shifted(TOAD[(tick+1)%2]) for tick,live in enumerate(boards['toad-b'])),'literal toad B phases')
    horizontal=frozenset(((5,7),(6,7),(7,7))); vertical=frozenset(((6,6),(6,7),(6,8)))
    require(boards['toad-b-minus'][1:]==[horizontal,vertical,horizontal,vertical],'toad removal becomes literal blinker')
    require(len(boards['toad-outside'][1])==6 and boards['toad-outside'][1]!=boards['toad'][1]
            and boards['toad-outside'][2]!=boards['toad'][0],'equal-population outside control')
    require(boards['collision-extinction']==[frozenset(phase) for phase in ANNIHILATION],'complete four-step contact fixture')
    require(boards['mixed-contact'][1]==glider(1)|block(9,7)|{(8,6),(8,8)},'complete first mixed contact')
    mixed=[item for item in load_jsonl(directory/'mixed-contact-production.jsonl') if item['type']=='update']
    require([item['neighbor_count'] for item in mixed]==[3,4,3] and [item['output'] for item in mixed]==[1,0,1],'mixed births and inhibited OFF center')
    original=boards['gun']; removed=boards['gun-remove-output']; detached=boards['detached']
    require(original[0]==gun(),'literal initial gun')
    for tick,outputs in ((30,[(23,9,['O..','.OO','OO.'])]),
                         (60,[(23,9,['O..','.OO','OO.']),(30,17,['O.O','.OO','.O.'])])):
        expected=set(gun())
        for ox,oy,shape in outputs:
            expected.update((x+3+ox,y+3+oy) for y,row in enumerate(shape) for x,cell in enumerate(row) if cell=='O')
        require(original[tick]==expected,'literal complete gun checkpoint '+str(tick))
    require(all(live==glider(tick,25,11,3) for tick,live in enumerate(detached)),'every literal detached glider phase')
    require(removed[:31]==original[:31] and removed[31]==gun(),'same gun prefix and declared removal')
    for horizon in range(35):
        require(original[30+horizon]==removed[31+horizon]|detached[horizon]
                and not (removed[31+horizon]&detached[horizon]),'gun/output complete-board comparison '+str(horizon))
    # Grain comparison: endpoint equalities omit intermediate ON/OFF changes.
    return {'schema':'gol-cross-case-results/1','status':'complete',
            'runs':len(boards),'checks':[
                'all complete C-engine replays and segment interval sets',
                'literal separated blocks at both separations for 32 steps',
                'glider near/far interventions and eight-step independent remainder',
                'toad role removal and outside equal-population control',
                'four-step complete extinction fixture and mixed-contact joint births',
                'complete gun checkpoints 30/60 and 35 gun/output union comparisons'],
            'grain':{'glider_cycle_changed_sum':sum(item['stats']['changed'] for item in snapshots['glider'][1:5]),
                     'glider_endpoint_difference':len(glider(0)^glider(4)),
                     'glider_comoving_endpoint_difference':len(glider(0)^shifted(glider(4),-1,-1)),
                     'toad_cycle_changed_sum':sum(item['stats']['changed'] for item in snapshots['toad'][1:3]),
                     'toad_endpoint_difference':len(boards['toad'][0]^boards['toad'][2])},
            'limits':['reconstruction with pilot-selected collisions','declared bounded horizons and selections',
                      'no actual-cause or constitutive-membership verdict','no physical qualification or full token continuity']}


def reconstruct(directory, library, proposal_hash=None):
    directory=Path(directory)
    directory.mkdir(parents=True,exist_ok=False)
    definitions=scenarios(proposal_hash)
    protocol={'schema':'gol-cross-case-protocol/1','coordinator_sha256':sha256(Path(__file__)),
              'runs':definitions,'production_selections':SELECTIONS,
              'purpose':'reconstruction; collision placements selected through pilot runs'}
    write_json(directory/'protocol.json',protocol)
    protocol_hash=sha256(directory/'protocol.json')
    for name,definition in definitions.items():
        definition['protocol_sha256']=protocol_hash
        path=directory/(name+'.json')
        write_json(path,definition)
        run(path,directory/(name+'.jsonl'),library)
        measure(directory/(name+'.jsonl'),directory/(name+'-measures.jsonl'),128)
    for name,(sites,pair) in SELECTIONS.items():
        context={'schema':'gol-production-context/1','context_id':name+'-selected-observations/1',
                 'trace_run_id':definitions[name]['run_id'],'proposal_revision':'0.2',
                 'selection_reason':'Declared cross-case observation sites, not inferred members',
                 'targets':sites,'max_updates':1000}
        path=directory/(name+'-context.json');write_json(path,context)
        export(directory/(name+'.jsonl'),path,directory/(name+'-production.jsonl'),
               directory/(pair+'.jsonl') if pair else None)
    report=verify(directory,library)
    report['protocol_sha256']=protocol_hash
    report['artifact_sha256']={path.name:sha256(path) for path in sorted(directory.iterdir()) if path.is_file()}
    partial=directory/'results.json.partial';write_json(partial,report)
    with partial.open('rb') as handle: os.fsync(handle.fileno())
    os.link(partial,directory/'results.json');partial.unlink()
    return report


def main(argv=None):
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('output_directory',type=Path)
    parser.add_argument('--engine',type=Path,required=True)
    parser.add_argument('--proposal-sha256')
    parser.add_argument('--check-only',action='store_true')
    args=parser.parse_args(argv)
    try:
        report=(verify(args.output_directory.resolve(),args.engine.resolve()) if args.check_only
                else reconstruct(args.output_directory.resolve(),args.engine.resolve(),args.proposal_sha256))
    except (OSError,ValueError,KeyError,TypeError,MemoryError,OverflowError) as error:
        print(f'cross-case reconstruction failed: {error}',file=sys.stderr)
        return 1
    print(f"Verified {report['runs']} cross-case runs; no membership or qualification verdict.")
    return 0


if __name__=='__main__':
    raise SystemExit(main())
