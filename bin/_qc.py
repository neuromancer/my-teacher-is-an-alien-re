#!/usr/bin/env python3
"""Quick compare using /tmp/navout to avoid file deletion."""
import sys, os
sys.path.insert(0, 'bin')
from compileAndCompare import read_assembly, parse_mnemonics, side_by_side
import Levenshtein

OUT = '/tmp/navout'

def compile():
    os.makedirs(OUT, exist_ok=True)
    CC = "env INCLUDE='compilers\\msvc420\\include;compilers\\msvc420\\mfc\\include' ./wibo compilers/msvc420/bin/CL.EXE /nologo /c /O2 /GX /I msvc420\\\\include /I 3rdparty\\\\miles\\\\include /I 3rdparty\\\\smack\\\\include"
    r1 = os.system(f"{CC} src/mCNavNode.cpp /Fo{OUT}/mCNavNode.obj /Fa{OUT}/mCNavNode.asm > {OUT}/mCNavNode.stdout 2>&1")
    r2 = os.system(f"{CC} src/mCNavigator.cpp /Fo{OUT}/mCNavigator.obj /Fa{OUT}/mCNavigator.asm > {OUT}/mCNavigator.stdout 2>&1")
    if r1 != 0:
        print("mCNavNode.cpp compilation failed!")
        with open(f"{OUT}/mCNavNode.stdout") as f: print(f.read())
    if r2 != 0:
        print("mCNavigator.cpp compilation failed!")
        with open(f"{OUT}/mCNavigator.stdout") as f: print(f.read())
    return r1 == 0 and r2 == 0

def compare(fn, dis, asm):
    result = read_assembly(fn, asm)
    if result is None: return None, 'Not found', None
    p, s = result
    with open(dis, 'rb') as f:
        t = f.read().decode('utf-8', errors='ignore').lower()
        t = chr(10).join(t.splitlines()[3:])
    pm = parse_mnemonics(p); tm = parse_mnemonics(t)
    d = Levenshtein.distance(pm, tm); ml = max(len(pm), len(tm))
    sim = (1-d/ml)*100 if ml > 0 else 100
    return sim, side_by_side(p, t), s

fns = [
    ('mCNavNode::Update', 'code/FUN_413280.disassembled.txt', f'{OUT}/mCNavNode.asm'),
    ('mCNavNode::LBLParse', 'code/FUN_413380.disassembled.txt', f'{OUT}/mCNavNode.asm'),
    ('mCNavNode::AddSpriteList', 'code/FUN_412F40.disassembled.txt', f'{OUT}/mCNavNode.asm'),
    ('mCNavNode::Activate', 'code/FUN_4130F0.disassembled.txt', f'{OUT}/mCNavNode.asm'),
    ('mCNavNode::GetNextNode', 'code/FUN_4131D0.disassembled.txt', f'{OUT}/mCNavNode.asm'),
    ('mCNavigator::~mCNavigator', 'code/FUN_4136F0.disassembled.txt', f'{OUT}/mCNavigator.asm'),
    ('mCNavigator::Update', 'code/FUN_413BC0.disassembled.txt', f'{OUT}/mCNavigator.asm'),
    ('mCNavigator::OnProcessEnd', 'code/FUN_413840.disassembled.txt', f'{OUT}/mCNavigator.asm'),
    ('mCNavigator::LBLParse', 'code/FUN_4138C0.disassembled.txt', f'{OUT}/mCNavigator.asm'),
]

if not compile():
    sys.exit(1)

target = sys.argv[1] if len(sys.argv) > 1 else None
for fn, dis, asm in fns:
    if target and fn != target:
        continue
    sim, txt, seh = compare(fn, dis, asm)
    if sim is None:
        print(f'{fn}: NOT FOUND')
    else:
        print(f'{fn}: {sim:.2f}%')
        if target:
            print(txt)
            if seh and seh.strip(): print(f'SEH:\n{seh}')
