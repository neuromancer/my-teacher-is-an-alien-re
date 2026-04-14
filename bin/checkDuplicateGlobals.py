#!/usr/bin/env python3
"""
Check for duplicate or similarly-named global variables in the codebase.

Scans src/*.h and src/*.cpp for global declarations with address suffixes
(e.g., g_Something_0046aa30) and reports:
1. Same address with different variable names
2. Same variable name declared with different types
3. Extern declarations without matching definitions (and vice versa)
"""

import re
import os
import sys
from collections import defaultdict

# Pattern: identifier ending with _XXXXXXXX (hex address suffix, 8 hex digits)
ADDR_SUFFIX_RE = re.compile(r'\b(\w+_([\da-fA-F]{8}))\b')

# Pattern for extern declarations
EXTERN_RE = re.compile(r'extern\s+(?:"C"\s+)?(.+?)\s+(\w+_([\da-fA-F]{8}))\s*[\[;]')

# Pattern for global definitions (non-extern)
GLOBAL_DEF_RE = re.compile(r'^(?!.*\bextern\b)(.+?)\s+(\w+_([\da-fA-F]{8}))\s*[\[;=]')


def scan_file(filepath):
    """Scan a file for global variable declarations with address suffixes."""
    externs = []  # (type, name, address, line_no)
    definitions = []  # (type, name, address, line_no)

    try:
        with open(filepath, 'r', errors='replace') as f:
            lines = f.readlines()
    except IOError:
        return externs, definitions

    in_function = 0  # brace depth tracker for skipping function bodies

    for line_no, line in enumerate(lines, 1):
        stripped = line.strip()

        # Skip comments
        if stripped.startswith('//'):
            continue

        # Track brace depth (rough heuristic for function bodies)
        in_function += stripped.count('{') - stripped.count('}')

        # Only look at file-scope declarations
        if in_function > 0:
            continue

        # Check for extern declarations
        m = EXTERN_RE.search(stripped)
        if m:
            type_str = m.group(1).strip()
            name = m.group(2)
            addr = m.group(3).lower()
            externs.append((type_str, name, addr, line_no))
            continue

        # Check for global definitions
        m = GLOBAL_DEF_RE.search(stripped)
        if m:
            type_str = m.group(1).strip()
            name = m.group(2)
            addr = m.group(3).lower()
            # Skip if inside a function parameter or local var
            if 'static' in type_str or type_str.startswith(('int', 'char', 'void',
                'unsigned', 'short', 'long', 'float', 'double', 'BOOL',
                'BYTE', 'WORD', 'DWORD', 'HDC', 'HPALETTE', 'HWND')):
                definitions.append((type_str, name, addr, line_no))
            elif '*' in type_str or type_str[0].isupper():
                definitions.append((type_str, name, addr, line_no))

    return externs, definitions


def strip_name_prefix(name):
    """Remove common prefixes like g_ DAT_ etc. and the address suffix."""
    # Remove address suffix
    name_no_addr = re.sub(r'_[\da-fA-F]{8}$', '', name)
    # Remove common prefixes
    for prefix in ['g_', 'DAT_', 'PTR_', 'FUN_']:
        if name_no_addr.startswith(prefix):
            name_no_addr = name_no_addr[len(prefix):]
    return name_no_addr.lower()


def main():
    src_dir = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), 'src')

    if not os.path.isdir(src_dir):
        print(f"Error: src directory not found at {src_dir}", file=sys.stderr)
        sys.exit(1)

    # Collect all declarations
    by_address = defaultdict(list)  # addr -> [(type, name, file, line_no, is_extern)]
    by_name = defaultdict(list)     # name -> [(type, file, line_no, is_extern)]

    for filename in sorted(os.listdir(src_dir)):
        if not (filename.endswith('.h') or filename.endswith('.cpp')):
            continue

        filepath = os.path.join(src_dir, filename)
        externs, definitions = scan_file(filepath)

        for type_str, name, addr, line_no in externs:
            by_address[addr].append((type_str, name, filename, line_no, True))
            by_name[name].append((type_str, filename, line_no, True))

        for type_str, name, addr, line_no in definitions:
            by_address[addr].append((type_str, name, filename, line_no, False))
            by_name[name].append((type_str, filename, line_no, False))

    issues = []

    # Check 1: Same address with different variable names
    print("=" * 70)
    print("DUPLICATE ADDRESSES (same address, different names)")
    print("=" * 70)
    found_dupes = False
    for addr in sorted(by_address.keys()):
        entries = by_address[addr]
        names = set(e[1] for e in entries)
        if len(names) > 1:
            found_dupes = True
            print(f"\n  Address 0x{addr}:")
            for name in sorted(names):
                matching = [e for e in entries if e[1] == name]
                for type_str, _, filename, line_no, is_extern in matching:
                    kind = "extern" if is_extern else "definition"
                    print(f"    {name:50s} ({type_str}) [{filename}:{line_no}] ({kind})")
    if not found_dupes:
        print("  None found.")

    # Check 2: Same name with different types
    print()
    print("=" * 70)
    print("TYPE MISMATCHES (same name, different types)")
    print("=" * 70)
    found_mismatches = False
    for name in sorted(by_name.keys()):
        entries = by_name[name]
        types = set()
        for type_str, _, _, _ in entries:
            # Normalize type for comparison
            t = type_str.replace('  ', ' ').strip()
            types.add(t)
        if len(types) > 1:
            found_mismatches = True
            print(f"\n  {name}:")
            for type_str, filename, line_no, is_extern in entries:
                kind = "extern" if is_extern else "definition"
                print(f"    {type_str:40s} [{filename}:{line_no}] ({kind})")
    if not found_mismatches:
        print("  None found.")

    # Check 3: Definitions without externs in globals.h (just count)
    globals_h_externs = set()
    globals_h_path = os.path.join(src_dir, 'globals.h')
    if os.path.exists(globals_h_path):
        externs_gh, _ = scan_file(globals_h_path)
        globals_h_externs = set(e[1] for e in externs_gh)

    defined_names = set()
    for entries in by_name.values():
        for _, filename, _, is_extern in entries:
            if not is_extern:
                defined_names.add(filename)

    # Summary
    total_addresses = len(by_address)
    total_names = len(by_name)
    dupe_addrs = sum(1 for addr in by_address if len(set(e[1] for e in by_address[addr])) > 1)

    print()
    print("=" * 70)
    print("SUMMARY")
    print("=" * 70)
    print(f"  Total unique addresses:    {total_addresses}")
    print(f"  Total unique names:        {total_names}")
    print(f"  Addresses with duplicates: {dupe_addrs}")


if __name__ == '__main__':
    main()
