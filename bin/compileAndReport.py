#!/usr/bin/env python3

import os
import re
from compileAndCompare import get_similarity

def get_function_name(line):
    # Skip lines that are clearly not function definitions
    stripped = line.strip()
    if stripped.startswith('//') or stripped.startswith('/*') or stripped.startswith('*'):
        return None
    if stripped.startswith('if') or stripped.startswith('for') or stripped.startswith('while'):
        return None
    if stripped.startswith('{') or stripped.startswith('}') or stripped == '':
        return None

    # Regular expression to find function names, including destructors (like ~Parser)
    match = re.search(r'\b([a-zA-Z0-9_:]+::[~a-zA-Z0-9_]+)\s*\(', line)
    if match:
        return match.group(1)

    # Handle extern "C" functions with calling conventions
    # Match: extern "C" returntype __cdecl functionname(
    match = re.search(r'extern\s+"C"\s+[\w\s\*]+\s+(?:__cdecl|__fastcall|__stdcall)?\s*\*?([a-zA-Z_][a-zA-Z0-9_]*)\s*\(', line)
    if match:
        return match.group(1)

    # Fallback for non-class functions with various return types
    # Match: returntype functionname( - handles pointers like FILE*, void*, etc.
    match = re.search(r'^\s*(?:[\w\s\*]+)\s+\*?([a-zA-Z_][a-zA-Z0-9_]*)\s*\(', line)
    if match:
        func_name = match.group(1)
        # Filter out keywords that might be incorrectly matched
        if func_name not in ('if', 'for', 'while', 'switch', 'return', 'else', 'extern'):
            return func_name

    return None

def run_comparison(function_name, address):
    disassembled_file = f"code/FUN_{address}.disassembled.txt"
    if not os.path.exists(disassembled_file):
        return "N/A"

    try:
        similarity, _, _ = get_similarity(function_name, disassembled_file, clean_build=False)
        if similarity is not None:
            return f"{similarity:.2f}%"
        else:
            return "Error"
    except Exception as e:
        return f"Error: {e}"

def main():
    import sys

    full_mode = '--full' in sys.argv
    if full_mode:
        src_dir = "src-full"
        code_dir = "code-full"
        out_dir = "out-full"
        clean_target = "clean-full"
        build_target = "full"
        map_skip = "src-full/map"
    else:
        src_dir = "src"
        code_dir = "code"
        out_dir = "out"
        clean_target = "clean"
        build_target = "all"
        map_skip = "src/map"

    report = []

    # Clean and build once
    os.system(f"make {clean_target}")
    os.system(f"make {build_target} > /dev/null 2>&1")

    for root, _, files in os.walk(src_dir):
        if map_skip in root:
            continue

        for file in files:
            if file.endswith(".cpp"):
                filepath = os.path.join(root, file)
                with open(filepath, "r") as f:
                    lines = f.readlines()

                for i, line in enumerate(lines):
                    match = re.search(r"/\* Function start: 0x([0-9a-fA-F]+) \*/", line)
                    if match:
                        address = match.group(1).upper()
                        # Look for function definition in the next lines
                        for j in range(i + 1, len(lines)):
                            function_name = get_function_name(lines[j])
                            if function_name:
                                similarity = run_comparison_in(function_name, address, code_dir, out_dir)
                                report.append((filepath, function_name, f"0x{address}", similarity))
                                break

    print("--- Similarity Report ---")
    for filepath, function_name, address, similarity in report:
        print(f"File: {filepath}, Function: {function_name}, Address: {address}, Similarity: {similarity}")
    print("-----------------------------------------")


def run_comparison_in(function_name, address, code_dir, out_dir="out"):
    disassembled_file = f"{code_dir}/FUN_{address}.disassembled.txt"
    if not os.path.exists(disassembled_file):
        return "N/A"

    try:
        similarity, _, _ = get_similarity(function_name, disassembled_file, clean_build=False, out_dir=out_dir)
        if similarity is not None:
            return f"{similarity:.2f}%"
        else:
            return "Error"
    except Exception as e:
        return f"Error: {e}"


if __name__ == "__main__":
    main()
