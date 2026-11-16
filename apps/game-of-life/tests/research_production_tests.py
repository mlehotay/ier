#!/usr/bin/env python3
"""Qualify exact tuples, joint absences, event provenance and export failures."""

from collections import Counter
import contextlib
import copy
import io
import json
from pathlib import Path
import subprocess
import sys
import tempfile
from unittest import mock

APP = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(APP / 'tools'))
import research_production as production
import research_run as runner
from research_run_tests import GLIDER, FailingOutput, manifest, shifted

LIBRARY = None


def exhaustive():
    # All 512 literal bit assignments, observed through the C engine. The
    # oracle checks positional encoding plus independently derived category
    # totals, without a second Python implementation of B3/S23.
    for topology in ('bounded', 'toroidal'):
        engine = runner.WorldEngine(LIBRARY, 5, 5, topology)
        categories = Counter()
        try:
            for mask in range(512):
                literal = [(1 + i % 3, 1 + i // 3) for i in range(9) if mask & (1 << i)]
                engine.install(literal)
                engine.step()
                after = {(x,y) for y, row in enumerate(engine.rows()) for x, cell in enumerate(row) if cell == 'O'}
                record = production.neighborhood(set(literal), after, (2,2),
                                                 {'width':5, 'height':5, 'topology':topology})
                assert [item['site'] for item in record['inputs']] == [[1+i%3,1+i//3] for i in range(9)]
                assert [item['offset'] for item in record['inputs']] == [[i%3-1,i//3-1] for i in range(9)]
                assert [item['value'] for item in record['inputs']] == [int(bool(mask & (1<<i))) for i in range(9)]
                assert record['neighbor_count'] == mask.bit_count() - int(bool(mask & 16))
                assert record['output'] == int((2,2) in after)
                categories[record['process']] += 1
            assert categories == {'production':56, 'ON':84, 'destruction':172, 'OFF':199, 'null':1}
        finally:
            engine.close()


def geometry():
    for topology, count in (('bounded',0), ('toroidal',8)):
        engine = runner.WorldEngine(LIBRARY, 1, 1, topology)
        try:
            engine.install([[0,0]])
            engine.step()
            assert engine.rows() == ['.']
            record = production.neighborhood({(0,0)}, set(), (0,0), {'width':1,'height':1,'topology':topology})
            assert record['neighbor_count'] == count
            assert record['process'] == 'destruction'
            for i, item in enumerate(record['inputs']):
                assert item['site'] == ([0,0] if i == 4 or topology == 'toroidal' else None)
                assert item['value'] == int(i == 4 or topology == 'toroidal')
        finally:
            engine.close()
    # Corner orientation and wrapping are checked against literal coordinates.
    record = production.neighborhood({(2,2),(2,0),(0,2)}, set(), (0,0),
                                     {'width':3,'height':3,'topology':'toroidal'})
    assert [item['site'] for item in record['inputs']] == [[2,2],[0,2],[1,2],[2,0],[0,0],[1,0],[2,1],[0,1],[1,1]]
    assert [item['value'] for item in record['inputs']] == [1,1,0,1,0,0,0,0,0]
    assert record['neighbor_count'] == 3


def record(root, name, data):
    path = root / (name + '.json')
    path.write_text(json.dumps(data))
    trace = root / (name + '.jsonl')
    runner.run(path, trace, LIBRARY)
    return trace


def context(data, targets=None):
    return {'schema':'gol-production-context/1', 'context_id':'test-context',
            'trace_run_id':data['run_id'], 'proposal_revision':data['proposal_revision'],
            'selection_reason':'Literal qualification control, no membership inference',
            'targets':'all' if targets is None else targets, 'max_updates':4096}


def export(root, name, trace, value, pair=None):
    path = root / (name + '-context.json')
    path.write_text(json.dumps(value))
    output = root / (name + '-production.jsonl')
    production.export(trace, path, output, pair)
    return output, [json.loads(line) for line in output.read_text().splitlines()]


def interventions():
    with tempfile.TemporaryDirectory(prefix='gol-production-') as temporary:
        root = Path(temporary)
        initial = [[3,3],[2,3],[4,3]]  # ON center with exactly two ON neighbors.
        base = manifest(copy.deepcopy(initial), 1)
        trace = record(root, 'base', base)
        for name, added, output_differs in (('one',[[3,2]],False), ('two',[[3,2],[3,4]],True)):
            data = manifest(initial + added, 1)
            changed = record(root, name, data)
            path, rows = export(root, name, changed, context(data, [[3,3]]), trace)
            update = rows[3]
            assert update['type'] == 'update' and update['target'] == [3,3]
            assert update['neighbor_count'] == 2 + len(added)
            assert update['paired']['neighbor_count'] == 2
            assert update['paired']['output'] == 1
            assert update['output'] == int(not output_differs)
            assert update['output_differs'] == output_differs
            assert update['input_difference_offsets'] == ([[0,-1]] if len(added)==1 else [[0,-1],[0,1]])
            assert rows[1]['paired_board_difference'] == added
            assert rows[-1]['trace_sha256'] == [runner.sha256(changed),runner.sha256(trace)]
            assert rows[0]['source_sha256']['research_production.py'] == runner.sha256(APP/'tools/research_production.py')
            # Fresh-process export must be byte-for-byte identical.
            again = root / (name + '-again.jsonl')
            result = subprocess.run([sys.executable, str(APP/'tools/research_production.py'),
                                     str(changed), str(root/(name+'-context.json')), str(again), '--pair', str(trace)],
                                    capture_output=True, text=True)
            assert result.returncode == 0, result.stderr
            assert again.read_bytes() == path.read_bytes()
        # A successful equal-board install has no production record. The next
        # actual step explicitly takes that installation as its source event.
        data = manifest(count=1)
        data['actions'] += [{'kind':'install','live':shifted(GLIDER[1])},{'kind':'step','count':1}]
        trace = record(root, 'installed', data)
        _, rows = export(root, 'installed', trace, context(data, [[6,6]]))
        events = [row for row in rows if row['type']=='event']
        updates = [row for row in rows if row['type']=='update']
        assert len(events) == 4 and len(updates) == 2
        assert events[2]['occurrence']['type'] == 'install' and not events[2]['state_changed']
        assert updates[1]['source_event'] == {'type':'install','event_index':2,'step_index':1,'segment':1,'tick':0}
        assert updates[1]['result_event']['tick'] == 1
        # All-target order, count and initial-only conventions.
        data = manifest([],1); data.update(width=2,height=2)
        trace = record(root, 'all', data)
        _, rows = export(root, 'all', trace, context(data))
        assert [row['target'] for row in rows if row['type']=='update'] == [[0,0],[1,0],[0,1],[1,1]]
        assert rows[-1]['updates']==4 and rows[-1]['records']==len(rows)
        data['actions']=[]
        trace = record(root,'initial-only',data)
        _, rows = export(root,'initial-only',trace,context(data))
        assert len(rows)==3 and rows[-1]['updates']==0


def failures():
    with tempfile.TemporaryDirectory(prefix='gol-production-failures-') as temporary:
        root = Path(temporary)
        data = manifest(count=1)
        trace = record(root,'source',data)
        base = context(data)
        invalid = []
        for key,value in (('targets',[]),('targets','other'),('targets',[[1,1],[1,1]]),
                          ('targets',[[True,1]]),('targets',[[32,0]]),('max_updates',True),
                          ('max_updates',1023),('schema','other'),('trace_run_id','other'),
                          ('proposal_revision','other'),('selection_reason','')):
            item=copy.deepcopy(base);item[key]=value;invalid.append(item)
        item=copy.deepcopy(base);del item['targets'];invalid.append(item)
        item=copy.deepcopy(base);item['extra']=0;invalid.append(item)
        for i,value in enumerate(invalid):
            path=root/f'context-{i}.json';path.write_text(json.dumps(value))
            output=root/f'invalid-{i}'
            with contextlib.redirect_stderr(io.StringIO()):
                assert production.main([str(trace),str(path),str(output)])==1
            assert not output.exists() and not output.with_name(output.name+'.partial').exists()
        limited = manifest([], 1); limited.update(width=1001,height=1000,max_cells=1001000)
        item=context(limited);item['max_updates']=1001001
        try:
            production.validate_context(item, limited)
            raise AssertionError('hard limit accepted')
        except runner.RunError:
            pass
        path=root/'valid-context.json';path.write_text(json.dumps(base))
        mismatched=manifest(count=2)
        pair=record(root,'mismatched',mismatched)
        with contextlib.redirect_stderr(io.StringIO()):
            assert production.main([str(trace),str(path),str(root/'mismatched-output'),'--pair',str(pair)])==1
        assert not (root/'mismatched-output.partial').exists()
        # Late failure after snapshots have streamed must leave only partial.
        broken=root/'broken.jsonl';broken.write_text(''.join(trace.read_text().splitlines(keepends=True)[:-1]))
        output=root/'broken-output'
        with contextlib.redirect_stderr(io.StringIO()):
            assert production.main([str(broken),str(path),str(output)])==1
        assert not output.exists() and output.with_name(output.name+'.partial').exists()
        for label in ('existing','partial','dangling'):
            output=root/label
            if label=='existing': output.write_text('preserve')
            elif label=='partial': output.with_name(output.name+'.partial').write_text('preserve')
            else: output.symlink_to(root/'absent')
            with contextlib.redirect_stderr(io.StringIO()):
                assert production.main([str(trace),str(path),str(output)])==1
            if label=='existing': assert output.read_text()=='preserve'
            elif label=='dangling': assert output.is_symlink()
            else: assert output.with_name(output.name+'.partial').read_text()=='preserve'
        original_open = Path.open
        for failure in ('write','short-write','flush','close','fsync','link'):
            def open_file(file, *args, **kwargs):
                handle = original_open(file, *args, **kwargs)
                return FailingOutput(handle,failure) if file.name.endswith('.partial') else handle
            with contextlib.ExitStack() as stack:
                stack.enter_context(mock.patch.object(Path,'open',open_file))
                if failure in ('fsync','link'):
                    stack.enter_context(mock.patch.object(production.os,failure,side_effect=OSError('injected failure')))
                stack.enter_context(contextlib.redirect_stderr(io.StringIO()))
                assert production.main([str(trace),str(path),str(root/failure)])==1
            assert not (root/failure).exists() and (root/(failure+'.partial')).exists()
        output=root/'race'
        def racing_publish(source,target):
            Path(target).write_text('other export')
            raise FileExistsError('concurrent publication')
        with mock.patch.object(production.os,'link',racing_publish), contextlib.redirect_stderr(io.StringIO()):
            assert production.main([str(trace),str(path),str(output)])==1
        assert output.read_text()=='other export'
        with mock.patch.object(production,'neighborhood',side_effect=MemoryError('injected allocation')):
            with contextlib.redirect_stderr(io.StringIO()):
                assert production.main([str(trace),str(path),str(root/'allocation')])==1
        assert not (root/'allocation').exists() and (root/'allocation.partial').exists()


def main():
    global LIBRARY
    cases={'exhaustive':exhaustive,'geometry':geometry,'interventions':interventions,'failures':failures}
    if len(sys.argv)!=3 or (sys.argv[2]!='all' and sys.argv[2] not in cases):
        print(f"Usage: {sys.argv[0]} ENGINE [all|{'|'.join(cases)}]",file=sys.stderr)
        return 2
    LIBRARY=Path(sys.argv[1]).resolve()
    selected=cases if sys.argv[2]=='all' else {sys.argv[2]:cases[sys.argv[2]]}
    for name,test in selected.items():
        test()
        print(f'PASS production-{name}')
    return 0


if __name__=='__main__':
    raise SystemExit(main())
