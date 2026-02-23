#!/usr/bin/env python3
"""Compare compiled ASM with disassembled without running make."""
import sys
import os
import subprocess

# Find project root from this script's location
PROJECT_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(PROJECT_ROOT, 'bin'))
from compileAndCompare import read_assembly, parse_mnemonics, side_by_side
import Levenshtein

function_name = sys.argv[1]
disasm_file = sys.argv[2]
asm_file = sys.argv[3] if len(sys.argv) > 3 else "out/SC_Sound.asm"

# Read files using subprocess since Python file I/O may have sandbox issues
try:
    asm_content = subprocess.check_output(['cat', asm_file], text=True)
except:
    print(f"Cannot read ASM file: {asm_file}")
    sys.exit(1)

try:
    disasm_content = subprocess.check_output(['cat', disasm_file], text=True, errors='replace')
except:
    print(f"Cannot read disasm file: {disasm_file}")
    sys.exit(1)

# Write to temp files so read_assembly can open them
import tempfile
with tempfile.NamedTemporaryFile(mode='w', suffix='.asm', delete=False) as f:
    f.write(asm_content)
    tmp_asm = f.name

with tempfile.NamedTemporaryFile(mode='w', suffix='.txt', delete=False) as f:
    f.write(disasm_content)
    tmp_disasm = f.name

try:
    result = read_assembly(function_name, tmp_asm)
    if result is None:
        print(f"Function '{function_name}' not found")
        sys.exit(1)

    produced_code, seh_code = result
    target_code = '\n'.join(disasm_content.lower().splitlines()[3:])

    produced_mnemonics = parse_mnemonics(produced_code)
    target_mnemonics = parse_mnemonics(target_code)

    distance = Levenshtein.distance(produced_mnemonics, target_mnemonics)
    max_len = max(len(produced_mnemonics), len(target_mnemonics))
    similarity = (1 - distance / max_len) * 100

    print(side_by_side(produced_code, target_code))
    print(f"\nSimilarity: {similarity:.2f}%")
    if seh_code and seh_code.strip():
        print(f"\nSEH code:\n{seh_code}")
finally:
    os.unlink(tmp_asm)
    os.unlink(tmp_disasm)
