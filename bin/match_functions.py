#!/usr/bin/env python3
"""
Match demo functions to full game functions by comparing normalized assembly.

Normalizes instructions by replacing absolute addresses with placeholders,
then compares using exact matching first, followed by fuzzy matching.

Library/CRT functions are excluded using the same approach as showProgress.py:
address ranges + "Library Function" markers in decompiled files.
"""

import os
import re
import sys
import glob
import hashlib
from difflib import SequenceMatcher
from collections import defaultdict

DEMO_DIR = "code"
FULL_DIR = "code-full"
SRC_DIR = "src"

# CRT/library address ranges (start, end) — functions in these ranges are excluded
DEMO_LIBRARY_RANGES = [(0x424540, 0x4304E0)]
FULL_LIBRARY_RANGES = [(0x454100, 0x460480)]

# Addresses in the binary's address space (code + data segments)
# Anything >= 0x400000 and <= 0x4FFFFF is almost certainly a relocated address
ADDR_RE = re.compile(r'\b0x0*(?:4[0-9a-fA-F]{5})\b')
# Also match LAB_ labels which contain absolute addresses
LABEL_RE = re.compile(r'LAB_[0-9a-fA-F]+')
# Function header pattern
FUNC_RE = re.compile(r'^Function:\s+FUN_([0-9a-fA-F]+)', re.IGNORECASE)


def normalize_addr(addr_str):
    """Strip leading zeros from address and uppercase it. '00401000' -> '401000'."""
    return addr_str.lstrip('0').upper() or '0'


def addr_int(addr_str):
    """Convert normalized address string to int."""
    return int(addr_str, 16)


def is_library_by_range(addr_str, ranges):
    """Check if an address falls within any library range."""
    val = addr_int(addr_str)
    for start, end in ranges:
        if start <= val <= end:
            return True
    return False


def get_library_from_decompiled(code_dir):
    """Scan decompiled files for 'Library Function' markers."""
    lib_addrs = set()
    for filename in os.listdir(code_dir):
        if filename.endswith('.decompiled.txt'):
            filepath = os.path.join(code_dir, filename)
            try:
                with open(filepath, 'r') as f:
                    content = f.read(4096)  # Only need to check header
            except Exception:
                continue
            if 'Library Function' in content:
                m = re.search(r'FUN_([0-9a-fA-F]+)', filename, re.IGNORECASE)
                if m:
                    lib_addrs.add(normalize_addr(m.group(1)))
    return lib_addrs


def parse_disasm(filepath):
    """Parse a disassembly file, return (address, list of instruction lines)."""
    with open(filepath, 'r') as f:
        lines = f.readlines()

    addr = None
    instructions = []

    for line in lines:
        line = line.rstrip()
        if not line:
            continue

        # Parse header
        m = FUNC_RE.match(line)
        if m:
            addr = normalize_addr(m.group(1))
            continue
        if line.startswith('Address:'):
            continue

        # Skip label-only lines (they just mark positions)
        if LABEL_RE.match(line.strip()) and line.strip().endswith(':'):
            continue

        instructions.append(line)

    return addr, instructions


def normalize_instructions(instructions):
    """
    Normalize instructions by replacing absolute addresses with placeholders.
    Keeps opcodes, registers, stack offsets, and small constants intact.
    """
    addr_map = {}
    addr_counter = 0

    normalized = []
    for inst in instructions:
        def replace_label(m):
            nonlocal addr_counter
            val = m.group(0)
            if val not in addr_map:
                addr_map[val] = f'LBL_{addr_counter}'
                addr_counter += 1
            return addr_map[val]

        line = LABEL_RE.sub(replace_label, inst)

        def replace_addr(m):
            nonlocal addr_counter
            val = m.group(0).lower()
            if val not in addr_map:
                addr_map[val] = f'ADDR_{addr_counter}'
                addr_counter += 1
            return addr_map[val]

        line = ADDR_RE.sub(replace_addr, line)
        normalized.append(line)

    return normalized


def get_fingerprint(normalized_lines):
    """Create a hash fingerprint from normalized instruction lines."""
    content = '\n'.join(normalized_lines)
    return hashlib.md5(content.encode()).hexdigest()


def similarity(norm_a, norm_b):
    """Compute similarity ratio between two normalized instruction sequences."""
    return SequenceMatcher(None, norm_a, norm_b).ratio()


def find_implemented_functions():
    """Scan src/*.cpp files for implemented function addresses."""
    implemented = {}  # addr -> src_file
    for cpp in glob.glob(os.path.join(SRC_DIR, '*.cpp')):
        basename = os.path.splitext(os.path.basename(cpp))[0]
        with open(cpp, 'r') as f:
            for line in f:
                m = re.search(r'Function start:\s*0x([0-9a-fA-F]+)', line, re.IGNORECASE)
                if m:
                    addr = normalize_addr(m.group(1))
                    implemented[addr] = basename
    return implemented


def load_functions(directory, library_ranges):
    """Load all disassembly files from a directory, classifying library functions."""
    functions = {}
    library = set()

    # Also detect library functions from decompiled markers
    lib_marked = get_library_from_decompiled(directory)

    pattern = os.path.join(directory, 'FUN_*.disassembled.txt')
    files = glob.glob(pattern)

    for filepath in files:
        addr, instructions = parse_disasm(filepath)
        if not addr or not instructions:
            continue

        # Check if this is a library function
        if is_library_by_range(addr, library_ranges) or addr in lib_marked:
            library.add(addr)
            continue

        normalized = normalize_instructions(instructions)
        fp = get_fingerprint(normalized)
        functions[addr] = {
            'normalized': normalized,
            'fingerprint': fp,
            'filepath': filepath,
            'size': len(instructions),
        }

    return functions, library


def main():
    min_similarity = 0.80
    if len(sys.argv) > 1:
        try:
            min_similarity = float(sys.argv[1])
        except ValueError:
            pass

    print("Loading demo functions...")
    demo_funcs, demo_lib = load_functions(DEMO_DIR, DEMO_LIBRARY_RANGES)
    print(f"  Loaded {len(demo_funcs)} game functions, {len(demo_lib)} library (excluded)")

    print("Loading full game functions...")
    full_funcs, full_lib = load_functions(FULL_DIR, FULL_LIBRARY_RANGES)
    print(f"  Loaded {len(full_funcs)} game functions, {len(full_lib)} library (excluded)")

    print("Loading implemented functions from src/...")
    implemented = find_implemented_functions()
    print(f"  Found {len(implemented)} implemented functions")

    # Phase 1: Exact matches by fingerprint
    print("\n--- Phase 1: Exact fingerprint matches ---")

    full_by_fp = defaultdict(list)
    for addr, data in full_funcs.items():
        full_by_fp[data['fingerprint']].append(addr)

    exact_matches = []  # (demo_addr, full_addr, similarity)
    matched_demo = set()
    matched_full = set()

    for demo_addr, demo_data in demo_funcs.items():
        fp = demo_data['fingerprint']
        if fp in full_by_fp:
            candidates = full_by_fp[fp]
            best = None
            for c in candidates:
                if c not in matched_full:
                    if c == demo_addr:
                        best = c
                        break
                    if best is None:
                        best = c
            if best:
                exact_matches.append((demo_addr, best, 1.0))
                matched_demo.add(demo_addr)
                matched_full.add(best)

    print(f"  Found {len(exact_matches)} exact matches")

    # Phase 2: Fuzzy matches for remaining functions
    print("\n--- Phase 2: Fuzzy matching (this may take a while) ---")

    unmatched_demo = {a: d for a, d in demo_funcs.items() if a not in matched_demo}
    unmatched_full = {a: d for a, d in full_funcs.items() if a not in matched_full}

    full_by_size = defaultdict(list)
    for addr, data in unmatched_full.items():
        full_by_size[data['size']].append(addr)

    fuzzy_matches = []
    total = len(unmatched_demo)
    checked = 0

    for demo_addr, demo_data in sorted(unmatched_demo.items()):
        checked += 1
        if checked % 50 == 0:
            print(f"  Progress: {checked}/{total} ({len(fuzzy_matches)} fuzzy matches so far)")

        demo_size = demo_data['size']
        demo_norm = demo_data['normalized']

        # Skip very small functions (< 4 instructions) - too many false positives
        if demo_size < 4:
            continue

        # Size window: +-25%
        min_size = int(demo_size * 0.75)
        max_size = int(demo_size * 1.25)

        best_match = None
        best_sim = min_similarity

        for size in range(min_size, max_size + 1):
            for full_addr in full_by_size.get(size, []):
                if full_addr in matched_full:
                    continue

                full_norm = unmatched_full[full_addr]['normalized']
                sim = similarity(demo_norm, full_norm)
                if sim > best_sim:
                    best_sim = sim
                    best_match = full_addr

        if best_match:
            fuzzy_matches.append((demo_addr, best_match, best_sim))
            matched_demo.add(demo_addr)
            matched_full.add(best_match)

    print(f"  Found {len(fuzzy_matches)} fuzzy matches (>= {min_similarity:.0%})")

    # Combine and sort all matches
    all_matches = exact_matches + fuzzy_matches
    all_matches.sort(key=lambda x: x[0])

    # Print results
    print("\n" + "=" * 100)
    print(f"{'DEMO ADDR':<14} {'FULL ADDR':<14} {'SIM':>6} {'SIZE':>5} {'IMPL?':<20} {'STATUS'}")
    print("=" * 100)

    stats = {'exact': 0, 'fuzzy': 0, 'implemented': 0, 'not_implemented': 0}

    for demo_addr, full_addr, sim in all_matches:
        demo_size = demo_funcs[demo_addr]['size']
        impl_file = implemented.get(demo_addr, '')
        is_exact = sim >= 0.999

        if is_exact:
            stats['exact'] += 1
            status = 'EXACT'
        else:
            stats['fuzzy'] += 1
            status = f'FUZZY ({sim:.1%})'

        if impl_file:
            stats['implemented'] += 1
            impl_str = impl_file
        else:
            stats['not_implemented'] += 1
            impl_str = ''

        print(f"0x{demo_addr:<12} 0x{full_addr:<12} {sim:>5.1%} {demo_size:>5} {impl_str:<20} {status}")

    # Summary
    print("\n" + "=" * 100)
    print("SUMMARY")
    print("=" * 100)
    print(f"Total demo functions (game):   {len(demo_funcs)}  (+ {len(demo_lib)} library excluded)")
    print(f"Total full game functions:     {len(full_funcs)}  (+ {len(full_lib)} library excluded)")
    print(f"Matched (exact):               {stats['exact']}")
    print(f"Matched (fuzzy >= {min_similarity:.0%}):       {stats['fuzzy']}")
    print(f"Total matched:                 {stats['exact'] + stats['fuzzy']}")
    print(f"Unmatched demo functions:      {len(demo_funcs) - len(all_matches)}")
    print(f"New full-game-only functions:  {len(full_funcs) - len(matched_full)}")
    print()
    print(f"Of matched functions:")
    print(f"  Already implemented in src/: {stats['implemented']}")
    print(f"  Not yet implemented:         {stats['not_implemented']}")

    # Show unmatched demo functions
    unmatched_demo_addrs = sorted(set(demo_funcs.keys()) - matched_demo)
    if unmatched_demo_addrs:
        print(f"\n--- Unmatched demo functions ({len(unmatched_demo_addrs)}) ---")
        print("These demo functions have NO match in the full game (demo-only or heavily changed):")
        for addr in unmatched_demo_addrs:
            impl_file = implemented.get(addr, '')
            size = demo_funcs[addr]['size']
            print(f"  0x{addr}  size={size:>4}  {impl_file}")

    # Show new full-game-only functions (not matched to any demo function)
    new_full_addrs = sorted(set(full_funcs.keys()) - matched_full)
    print(f"\n--- New full-game-only functions ({len(new_full_addrs)}) ---")
    print("(showing first 50)")
    for addr in new_full_addrs[:50]:
        size = full_funcs[addr]['size']
        print(f"  0x{addr}  size={size:>4}")
    if len(new_full_addrs) > 50:
        print(f"  ... and {len(new_full_addrs) - 50} more")

    # Group matches by source file for easy overview
    print("\n" + "=" * 100)
    print("MATCHES GROUPED BY SOURCE FILE")
    print("=" * 100)

    file_matches = defaultdict(list)
    for demo_addr, full_addr, sim in all_matches:
        impl_file = implemented.get(demo_addr, None)
        if impl_file:
            file_matches[impl_file].append((demo_addr, full_addr, sim))

    file_unmatched = defaultdict(list)
    unmatched_demo_set = set(demo_funcs.keys()) - matched_demo
    for addr in unmatched_demo_set:
        impl_file = implemented.get(addr, None)
        if impl_file:
            file_unmatched[impl_file].append(addr)

    all_impl_files = sorted(set(list(file_matches.keys()) + list(file_unmatched.keys())))

    for src_file in all_impl_files:
        matches = file_matches.get(src_file, [])
        unmatched = file_unmatched.get(src_file, [])
        total_impl = len(matches) + len(unmatched)
        exact = sum(1 for _, _, s in matches if s >= 0.999)
        fuzzy = sum(1 for _, _, s in matches if s < 0.999)

        print(f"\n  {src_file}.cpp: {len(matches)}/{total_impl} matched "
              f"({exact} exact, {fuzzy} fuzzy), {len(unmatched)} unmatched")

        for demo_addr, full_addr, sim in sorted(matches):
            tag = "EXACT" if sim >= 0.999 else f"{sim:.0%}"
            print(f"    0x{demo_addr} -> 0x{full_addr}  [{tag}]")
        for addr in sorted(unmatched):
            size = demo_funcs[addr]['size']
            print(f"    0x{addr}  [NO MATCH]  size={size}")

    # Implementation transfer summary
    impl_no_match = sum(1 for a in unmatched_demo_set if a in implemented)
    impl_matched = stats['implemented']
    print(f"\n--- Implementation transfer summary ---")
    print(f"Implemented demo functions with full-game match:    {impl_matched}")
    print(f"Implemented demo functions with NO full-game match: {impl_no_match}")
    print(f"  (these may be demo-only or heavily reworked in the full game)")


if __name__ == '__main__':
    main()
