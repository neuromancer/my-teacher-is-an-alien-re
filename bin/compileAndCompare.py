#!/usr/bin/env python3

import re
import Levenshtein
import os
import sys

from os import system
from time import sleep
from argparse import ArgumentParser

def read_assembly(function_name, file_path):
    # Read the file with the assembly code
    with open(file_path, 'r') as file:
        assembly = file.read()

    if f";	COMDAT ?{function_name}@" in assembly:
        print("Symbols detected in assembly code:")
        # If the function is a COMDAT, we need to handle it differently
        symbols = assembly.split(f";	COMDAT ?{function_name}@")[2]
        symbols = symbols.split(f"_TEXT	SEGMENT")[1]
        symbols = symbols.split(f"?{function_name}@")[0]
        print(symbols)

    # Parse assembly code
    if f"; {function_name}, COMDAT" in assembly:
        assembly = assembly.split(f"; {function_name}, COMDAT")[1]
        assembly = assembly.split(f"; {function_name}")[0]
    elif f"_{function_name} PROC NEAR" in assembly:
        assembly = assembly.split(f"_{function_name} PROC NEAR")[1]
        assembly = assembly.split(f"_{function_name} ENDP")[0]
    else:
        return None

    # Discard the lines that start with ";"
    assembly = "\n".join([line for line in assembly.split("\n") if not line.strip().startswith(";")])

    # Discard empty lines
    assembly = "\n".join([line for line in assembly.split("\n") if line.strip() != ""])

    # Remove the header
    if "PROC NEAR" in assembly:
        assembly = assembly.split("PROC NEAR")[1]
        assembly = assembly.split("ENDP")[0]

    return assembly

def parse_mnemonics(assembly_code):
    normalization_map = {
        'je': 'jz',
        'jne': 'jnz',
        'jb': 'jc',
        'jae': 'jnc',
        'jg': 'jnle',
        'jge': 'jnl',
        'jl': 'jnge',
        'jle': 'jng',
    }
    mnemonics = []
    for line in assembly_code.split('\n'):
        line = line.strip()
        if line and not line.endswith(':') and not line.startswith('?'):
            parts = line.split(None, 1)
            mnemonic = parts[0]
            if mnemonic in normalization_map:
                mnemonic = normalization_map[mnemonic]
            mnemonics.append(mnemonic)
    return mnemonics

def side_by_side(str1, str2, tab_size=4):
    str1 = str1.replace('\t', ' ' * tab_size)
    str2 = str2.replace('\t', ' ' * tab_size)
    lines1 = str1.split('\n')
    lines2 = str2.split('\n')
    max_length = max(len(line) for line in lines1) if lines1 else 0
    max_length = min(max_length, 48)
    max_lines = max(len(lines1), len(lines2))
    result = []
    for i in range(max_lines):
        line1 = lines1[i] if i < len(lines1) else ""
        line2 = lines2[i] if i < len(lines2) else ""
        if (len(line1) > max_length - 2):
            line1 = line1[:max_length - 2] + ".."
        if (len(line2) > max_length - 2):
            line2 = line2[:max_length - 2] + ".."
        padded_line1 = line1.ljust(max_length)
        combined = f"{padded_line1} | {line2}"
        result.append(combined)
    return '\n'.join(result)

def get_similarity(function_name, disassembled_code_path, clean_build=True):
    if clean_build:
        if system("make clean all") != 0:
            print("Make failed")
            exit(1)

    asm_file_path = None
    produced_code = None
    for filename in os.listdir("out"):
        if filename.endswith(".asm"):
            filepath = os.path.join("out", filename)
            with open(filepath, 'r') as f:
                content = f.read()
                if function_name in content:
                    asm_file_path = filepath
                    produced_code = read_assembly(function_name, filepath)
                    if produced_code is not None:
                        break

    if asm_file_path is None:
        return None, "Function not found in any .asm file."

    with open(disassembled_code_path, 'rb') as file:
        target_code = file.read()
        target_code = target_code.decode('utf-8', errors='ignore').lower()
        target_code = "\n".join(target_code.splitlines()[3:])

    produced_mnemonics = parse_mnemonics(produced_code)
    target_mnemonics = parse_mnemonics(target_code)

    distance = Levenshtein.distance(produced_mnemonics, target_mnemonics)
    max_len = max(len(produced_mnemonics), len(target_mnemonics))
    if max_len == 0:
        similarity = 100.0
    else:
        similarity = (1 - distance / max_len) * 100

    return similarity, side_by_side(produced_code, target_code)

def main():
    parser = ArgumentParser(description="Recover high-level code from assembly code")
    parser.add_argument("function_name", help="Function name to compare")
    parser.add_argument("disassembled_code", help="Path to the disassembled code from Ghidra")
    args = parser.parse_args()

    similarity, comparison_text = get_similarity(args.function_name, args.disassembled_code)

    print (f"Comparison for function '{args.function_name}':")
    if similarity is None:
        print(comparison_text)
        sys.exit(1)

    print(comparison_text)
    print(f"\nSimilarity: {similarity:.2f}%")

if __name__ == "__main__":
    main()
