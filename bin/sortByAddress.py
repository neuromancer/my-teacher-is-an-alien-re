#!/usr/bin/env python3
"""
Script to check function order in C++ source files.

Usage:
    python3 sortByAddress.py [directory]

Shows which files have out-of-order functions and what the correct order should be.
Does NOT modify files - manual editing is required to maintain code integrity.
"""

import os
import re
import sys

def check_cpp_file(filepath):
    """Check a single C++ file for out-of-order functions."""
    with open(filepath, 'r') as f:
        content = f.read()

    # Find all function start markers
    pattern = r'/\* Function start: (0x[0-9a-fA-F]+) \*/'
    matches = list(re.finditer(pattern, content))

    if len(matches) < 2:
        return None  # Nothing to check

    functions = []
    for match in matches:
        start_pos = match.start()
        addr = match.group(1)
        addr_int = int(addr, 16)

        # Find the line number
        line_num = content[:start_pos].count('\n') + 1

        functions.append((addr_int, addr, start_pos, line_num))

    # Check if in correct order
    is_sorted = all(functions[i][0] < functions[i+1][0] for i in range(len(functions)-1))

    if is_sorted:
        return None  # Already sorted

    # Build report
    sorted_funcs = sorted(functions, key=lambda x: x[0])

    return {
        'filepath': filepath,
        'current': [(f[1], f[3]) for f in functions],  # (addr, line_num)
        'correct': [(f[1], f[3]) for f in sorted_funcs],
        'num_functions': len(functions)
    }

def check_directory(directory):
    """Check all C++ files in a directory."""
    issues = []

    for filename in sorted(os.listdir(directory)):
        if filename.endswith(".cpp"):
            filepath = os.path.join(directory, filename)
            result = check_cpp_file(filepath)
            if result:
                issues.append(result)

    return issues

def main():
    # Get directory (default to 'src')
    directory = 'src'
    for arg in sys.argv[1:]:
        if not arg.startswith('--'):
            directory = arg
            break

    print(f"Checking function order in {directory}...")
    print()

    issues = check_directory(directory)

    if not issues:
        print("All files have correctly ordered functions!")
        return 0

    print(f"Found {len(issues)} files with out-of-order functions:\n")

    for issue in issues:
        print(f"=== {issue['filepath']} ({issue['num_functions']} functions) ===")
        print("Current order (address : line)")
        for addr, line in issue['current']:
            print(f"  {addr} : L{line}")
        print("Correct order:")
        for addr, line in issue['correct']:
            print(f"  {addr}")
        print()

    print("To fix: Manually reorder functions in each file by address.")
    print("Functions with lower addresses should appear first in the file.")
    return 1

if __name__ == "__main__":
    sys.exit(main())
