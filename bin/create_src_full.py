#!/usr/bin/env python3
"""
Create src-full/ directory from src/ with addresses remapped to the full game binary.

Uses the matching logic from match_functions.py to map demo addresses to full-game
addresses, then copies all source files with the address comments updated.

Also generates a new map/ directory with all full-game function addresses.
"""

import os
import re
import sys
import glob
import shutil
from collections import defaultdict

# Import matching logic
sys.path.insert(0, os.path.dirname(__file__))
from match_functions import (
    load_functions, normalize_addr, get_library_from_decompiled,
    is_library_by_range, DEMO_DIR, FULL_DIR,
    DEMO_LIBRARY_RANGES, FULL_LIBRARY_RANGES
)

SRC_DIR = "src"
DST_DIR = "src-full"
FULL_CODE_DIR = "code-full"

FUNC_COMMENT_RE = re.compile(
    r'/\* Function start: 0x([0-9a-fA-F]+) \*/(.*)'
)


def build_address_map(min_similarity=0.80):
    """Run the matching pipeline and return a dict of demo_addr -> (full_addr, similarity)."""
    from match_functions import (
        get_fingerprint, normalize_instructions, similarity
    )

    print("Loading demo functions...")
    demo_funcs, demo_lib = load_functions(DEMO_DIR, DEMO_LIBRARY_RANGES)
    print(f"  {len(demo_funcs)} game functions, {len(demo_lib)} library excluded")

    print("Loading full game functions...")
    full_funcs, full_lib = load_functions(FULL_DIR, FULL_LIBRARY_RANGES)
    print(f"  {len(full_funcs)} game functions, {len(full_lib)} library excluded")

    # Phase 1: Exact matches
    full_by_fp = defaultdict(list)
    for addr, data in full_funcs.items():
        full_by_fp[data['fingerprint']].append(addr)

    addr_map = {}  # demo_addr -> (full_addr, similarity)
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
                addr_map[demo_addr] = (best, 1.0)
                matched_demo.add(demo_addr)
                matched_full.add(best)

    print(f"  Phase 1: {len(addr_map)} exact matches")

    # Phase 2: Fuzzy matches
    unmatched_demo = {a: d for a, d in demo_funcs.items() if a not in matched_demo}
    unmatched_full = {a: d for a, d in full_funcs.items() if a not in matched_full}

    full_by_size = defaultdict(list)
    for addr, data in unmatched_full.items():
        full_by_size[data['size']].append(addr)

    fuzzy_count = 0
    total = len(unmatched_demo)
    checked = 0

    for demo_addr, demo_data in sorted(unmatched_demo.items()):
        checked += 1
        if checked % 100 == 0:
            print(f"  Phase 2: {checked}/{total}...")

        demo_size = demo_data['size']
        demo_norm = demo_data['normalized']

        if demo_size < 4:
            continue

        min_size = int(demo_size * 0.75)
        max_size = int(demo_size * 1.25)

        best_match = None
        best_sim = min_similarity

        for size in range(min_size, max_size + 1):
            for full_addr in full_by_size.get(size, []):
                if full_addr in matched_full:
                    continue
                full_norm = unmatched_full[full_addr]['normalized']
                from difflib import SequenceMatcher
                sim = SequenceMatcher(None, demo_norm, full_norm).ratio()
                if sim > best_sim:
                    best_sim = sim
                    best_match = full_addr

        if best_match:
            addr_map[demo_addr] = (best_match, best_sim)
            matched_demo.add(demo_addr)
            matched_full.add(best_match)
            fuzzy_count += 1

    print(f"  Phase 2: {fuzzy_count} fuzzy matches")
    print(f"  Total: {len(addr_map)} mapped, {len(demo_funcs) - len(addr_map)} unmapped")

    return addr_map


def remap_file(src_path, dst_path, addr_map):
    """Copy a source file, replacing function address comments."""
    with open(src_path, 'r') as f:
        content = f.read()

    replacements = 0
    unmatched = 0

    def replace_func_comment(m):
        nonlocal replacements, unmatched
        demo_addr = normalize_addr(m.group(1))
        rest = m.group(2)

        if demo_addr in addr_map:
            full_addr, sim = addr_map[demo_addr]
            replacements += 1
            if sim >= 0.999:
                return f"/* Function start: 0x{full_addr} */{rest}"
            else:
                return f"/* Function start: 0x{full_addr} */ /* ~{sim:.0%} match */{rest}"
        else:
            unmatched += 1
            return f"/* Function start: 0x{m.group(1)} */ /* DEMO ONLY - no full game match */{rest}"

    new_content = FUNC_COMMENT_RE.sub(replace_func_comment, content)

    os.makedirs(os.path.dirname(dst_path), exist_ok=True)
    with open(dst_path, 'w') as f:
        f.write(new_content)

    return replacements, unmatched


def generate_full_map(dst_map_dir):
    """Generate map files for the full game from code-full/ disassembly files."""
    # Collect all full-game function addresses (excluding library)
    lib_marked = get_library_from_decompiled(FULL_CODE_DIR)
    all_addrs = []

    for filepath in glob.glob(os.path.join(FULL_CODE_DIR, 'FUN_*.disassembled.txt')):
        m = re.search(r'FUN_([0-9a-fA-F]+)', os.path.basename(filepath), re.IGNORECASE)
        if m:
            addr_str = normalize_addr(m.group(1))
            if is_library_by_range(addr_str, FULL_LIBRARY_RANGES):
                continue
            if addr_str in lib_marked:
                continue
            all_addrs.append(addr_str)

    all_addrs.sort(key=lambda a: int(a, 16))
    print(f"  Full game map: {len(all_addrs)} game functions")

    # Group by page prefix (first 3 hex chars of address -> e.g., "401", "40A")
    pages = defaultdict(list)
    for addr in all_addrs:
        # Use same naming as demo: FUN_XXX.cpp where XXX is the first 3 hex digits
        prefix = addr[:3].upper()
        pages[prefix].append(addr)

    # Remove old map files
    if os.path.exists(dst_map_dir):
        shutil.rmtree(dst_map_dir)
    os.makedirs(dst_map_dir)

    for prefix, addrs in sorted(pages.items()):
        map_path = os.path.join(dst_map_dir, f"FUN_{prefix}.cpp")
        with open(map_path, 'w') as f:
            for addr in addrs:
                f.write(f"/* Function start: 0x{addr} */\n\n")

    print(f"  Generated {len(pages)} map files in {dst_map_dir}/")


def main():
    if os.path.exists(DST_DIR):
        print(f"Removing existing {DST_DIR}/...")
        shutil.rmtree(DST_DIR)

    # Build address mapping
    addr_map = build_address_map()

    # Copy and remap all files
    print(f"\nCopying {SRC_DIR}/ to {DST_DIR}/ with address remapping...")

    total_replaced = 0
    total_unmatched = 0
    files_processed = 0

    for root, dirs, files in os.walk(SRC_DIR):
        # Skip the map directory (we'll regenerate it)
        if 'map' in dirs:
            dirs.remove('map')

        for filename in files:
            src_path = os.path.join(root, filename)
            rel_path = os.path.relpath(src_path, SRC_DIR)
            dst_path = os.path.join(DST_DIR, rel_path)

            if filename.endswith('.cpp') or filename.endswith('.h'):
                replaced, unmatched = remap_file(src_path, dst_path, addr_map)
                total_replaced += replaced
                total_unmatched += unmatched
                files_processed += 1
                if replaced > 0 or unmatched > 0:
                    print(f"  {rel_path}: {replaced} remapped, {unmatched} unmatched")
            else:
                # Copy non-source files as-is (e.g., auto_complete.txt)
                os.makedirs(os.path.dirname(dst_path), exist_ok=True)
                shutil.copy2(src_path, dst_path)

    print(f"\nProcessed {files_processed} source files")
    print(f"  Addresses remapped:  {total_replaced}")
    print(f"  Addresses unmatched: {total_unmatched}")

    # Generate new map directory for full game
    print("\nGenerating full-game map...")
    generate_full_map(os.path.join(DST_DIR, "map"))

    # Update auto_complete.txt - remap addresses there too
    auto_complete_path = os.path.join(DST_DIR, "auto_complete.txt")
    if os.path.exists(auto_complete_path):
        remap_auto_complete(auto_complete_path, addr_map)

    print("\nDone!")


def remap_auto_complete(filepath, addr_map):
    """Remap addresses in auto_complete.txt."""
    with open(filepath, 'r') as f:
        lines = f.readlines()

    remapped = 0
    new_lines = []
    for line in lines:
        stripped = line.strip()
        if not stripped or stripped.startswith('#'):
            new_lines.append(line)
            continue

        try:
            addr_val = int(stripped, 16)
            addr_str = normalize_addr(format(addr_val, 'X'))
            if addr_str in addr_map:
                full_addr, sim = addr_map[addr_str]
                new_lines.append(f"0x{full_addr}\n")
                remapped += 1
            else:
                new_lines.append(f"{stripped}  # DEMO ONLY\n")
        except ValueError:
            new_lines.append(line)

    with open(filepath, 'w') as f:
        f.writelines(new_lines)

    print(f"  auto_complete.txt: {remapped} addresses remapped")


if __name__ == '__main__':
    main()
