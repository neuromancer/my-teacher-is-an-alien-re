#!/usr/bin/env python3

import os
import re
from compileAndCompare import get_similarity

def get_function_name(line):
    # Regular expression to find function names, including destructors (like ~Parser)
    match = re.search(r'\b([a-zA-Z0-9_:]+::[~a-zA-Z0-9_]+)\s*\(', line)
    if match:
        return match.group(1)

    # Fallback for non-class functions with various return types
    # Match: returntype functionname( - handles pointers like FILE*, void*, etc.
    match = re.search(r'^\s*(?:[\w\s\*]+)\s+\*?([a-zA-Z_][a-zA-Z0-9_]*)\s*\(', line)
    if match:
        return match.group(1)

    return None

def run_comparison(function_name, address):
    disassembled_file = f"code/FUN_{address}.disassembled.txt"
    if not os.path.exists(disassembled_file):
        return "N/A"

    try:
        similarity, _ = get_similarity(function_name, disassembled_file, clean_build=False)
        if similarity is not None:
            return f"{similarity:.2f}%"
        else:
            return "Error"
    except Exception as e:
        return f"Error: {e}"

def main():
    report = []
    src_dir = "src"

    # Clean and build once
    os.system("make all > /dev/null 2>&1")

    for root, _, files in os.walk(src_dir):
        if "src/map" in root:
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
                                similarity = run_comparison(function_name, address)
                                report.append((filepath, function_name, f"0x{address}", similarity))
                                break

    print("--- Similarity Report ---")
    for filepath, function_name, address, similarity in report:
        print(f"File: {filepath}, Function: {function_name}, Address: {address}, Similarity: {similarity}")
    print("-----------------------------------------")

if __name__ == "__main__":
    main()
