#!/usr/bin/env python3
"""Quick compare: compiles only the two nav files and checks similarities."""
import sys, os, re
sys.path.insert(0, 'bin')
from compileAndCompare import read_assembly, parse_mnemonics, side_by_side
import Levenshtein

# Compile just the two files
COMPILE_CMD = "env INCLUDE='compilers\\msvc420\\include;compilers\\msvc420\\mfc\\include' ./wibo compilers/msvc420/bin/CL.EXE /nologo /c /O2 /GX /I msvc420\\\\include /I 3rdparty\\\\miles\\\\include /I 3rdparty\\\\smack\\\\include"
os.system(f"{COMPILE_CMD} src/mCNavNode.cpp /Foout/mCNavNode.obj /Faout/mCNavNode.asm > out/mCNavNode.stdout 2>&1")
os.system(f"{COMPILE_CMD} src/mCNavigator.cpp /Foout/mCNavigator.obj /Faout/mCNavigator.asm > out/mCNavigator.stdout 2>&1")

def compare_fn(fn, dis, asm_file):
    with open(asm_file, 'r') as f:
        content = f.read()
    if fn not in content:
        return None, f"Function '{fn}' not found in {asm_file}", None
    result = read_assembly(fn, asm_file)
    if result is None:
        return None, f"Could not extract assembly for '{fn}'", None
    produced_code, seh_code = result

    with open(dis, 'rb') as f:
        target_code = f.read().decode('utf-8', errors='ignore').lower()
        target_code = "\n".join(target_code.splitlines()[3:])

    produced_mnemonics = parse_mnemonics(produced_code)
    target_mnemonics = parse_mnemonics(target_code)
    distance = Levenshtein.distance(produced_mnemonics, target_mnemonics)
    max_len = max(len(produced_mnemonics), len(target_mnemonics))
    similarity = (1 - distance / max_len) * 100 if max_len > 0 else 100.0
    return similarity, side_by_side(produced_code, target_code), seh_code

functions = [
    ('mCNavNode::Update', 'code/FUN_413280.disassembled.txt', 'out/mCNavNode.asm'),
    ('mCNavNode::LBLParse', 'code/FUN_413380.disassembled.txt', 'out/mCNavNode.asm'),
    ('mCNavNode::AddSpriteList', 'code/FUN_412F40.disassembled.txt', 'out/mCNavNode.asm'),
    ('mCNavNode::Activate', 'code/FUN_4130F0.disassembled.txt', 'out/mCNavNode.asm'),
    ('mCNavNode::GetNextNode', 'code/FUN_4131D0.disassembled.txt', 'out/mCNavNode.asm'),
    ('mCNavigator::~mCNavigator', 'code/FUN_4136F0.disassembled.txt', 'out/mCNavigator.asm'),
    ('mCNavigator::Update', 'code/FUN_413BC0.disassembled.txt', 'out/mCNavigator.asm'),
    ('mCNavigator::OnProcessEnd', 'code/FUN_413840.disassembled.txt', 'out/mCNavigator.asm'),
    ('mCNavigator::LBLParse', 'code/FUN_4138C0.disassembled.txt', 'out/mCNavigator.asm'),
]

target = sys.argv[1] if len(sys.argv) > 1 else None

for fn, dis, asm in functions:
    if target and fn != target:
        continue
    similarity, comparison_text, seh_code = compare_fn(fn, dis, asm)
    if similarity is None:
        print(f'{fn}: NOT FOUND - {comparison_text}')
    else:
        print(f'{fn}: {similarity:.2f}%')
        if target:
            print(comparison_text)
            if seh_code and seh_code.strip():
                print(f'\nDetected SEH code:')
                print(seh_code)
