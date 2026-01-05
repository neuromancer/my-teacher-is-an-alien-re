#!/usr/bin/env python3
"""
Compare global data sections between original and recompiled executables.
Compares the raw .data section content and reports similarity.
"""
import sys
import struct
import argparse
import os

def parse_pe(filename):
    """Parse PE and return image_base, sections list"""
    with open(filename, 'rb') as f:
        f.seek(0x3C)
        pe_off = struct.unpack('<I', f.read(4))[0]
        
        f.seek(pe_off + 6)
        num_sections = struct.unpack('<H', f.read(2))[0]
        
        f.seek(pe_off + 20)
        opt_size = struct.unpack('<H', f.read(2))[0]
        
        f.seek(pe_off + 52)
        image_base = struct.unpack('<I', f.read(4))[0]
        
        section_start = pe_off + 24 + opt_size
        f.seek(section_start)
        
        sections = {}
        for i in range(num_sections):
            name = f.read(8).rstrip(b'\x00').decode('ascii', errors='ignore')
            vsize = struct.unpack('<I', f.read(4))[0]
            va = struct.unpack('<I', f.read(4))[0]
            rawsize = struct.unpack('<I', f.read(4))[0]
            rawptr = struct.unpack('<I', f.read(4))[0]
            f.read(12)
            flags = struct.unpack('<I', f.read(4))[0]
            sections[name] = {
                'va': image_base + va,
                'vsize': vsize,
                'rawptr': rawptr,
                'rawsize': rawsize
            }
        
        return image_base, sections

def get_section_data(filename, section_name):
    """Read raw section data"""
    _, sections = parse_pe(filename)
    if section_name not in sections:
        return None, None
    sec = sections[section_name]
    with open(filename, 'rb') as f:
        f.seek(sec['rawptr'])
        return sec['va'], f.read(sec['rawsize'])

def find_printable_strings(data, min_len=6):
    """Find printable strings in raw data"""
    strings = []
    current = b''
    start = 0
    for i, b in enumerate(data):
        if 32 <= b < 127:
            if not current:
                start = i
            current += bytes([b])
        elif b == 0 and len(current) >= min_len:
            strings.append((start, current.decode('ascii')))
            current = b''
        else:
            current = b''
    return strings

def compare_strings(orig_strings, new_strings):
    """Compare string lists and return matches"""
    orig_set = set(s for _, s in orig_strings)
    new_set = set(s for _, s in new_strings)
    
    return {
        'matched': orig_set & new_set,
        'missing': orig_set - new_set,
        'extra': new_set - orig_set
    }

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('orig_exe')
    parser.add_argument('new_exe')
    parser.add_argument('--section', default='.data', help='Section to compare')
    args = parser.parse_args()
    
    print(f"Comparing {args.section} sections...")
    print(f"Original: {args.orig_exe}")
    print(f"New: {args.new_exe}")
    print()
    
    orig_va, orig_data = get_section_data(args.orig_exe, args.section)
    new_va, new_data = get_section_data(args.new_exe, args.section)
    
    if orig_data is None:
        print(f"Section {args.section} not found in original")
        return
    if new_data is None:
        print(f"Section {args.section} not found in new")
        return
        
    print(f"Original {args.section}: VA=0x{orig_va:08X}, Size={len(orig_data)} bytes")
    print(f"New {args.section}: VA=0x{new_va:08X}, Size={len(new_data)} bytes")
    print()
    
    # Analyze non-string data (dwords)
    def analyze_dwords(data):
        zero = 0
        nonzero = 0
        nonzero_vals = []
        for i in range(0, len(data) - 3, 4):
            val = struct.unpack('<I', data[i:i+4])[0]
            if val == 0:
                zero += 1
            else:
                nonzero += 1
                if len(nonzero_vals) < 50:
                    nonzero_vals.append((i, val))
        return zero, nonzero, nonzero_vals
    
    orig_zero, orig_nonzero, orig_vals = analyze_dwords(orig_data)
    new_zero, new_nonzero, new_vals = analyze_dwords(new_data)
    
    print(f"Non-String Global Data:")
    print(f"  Original: {orig_zero} zero dwords, {orig_nonzero} non-zero dwords")
    print(f"  New:      {new_zero} zero dwords, {new_nonzero} non-zero dwords")
    
    # Check how many non-zero values match (by value, not position)
    orig_val_set = set(v for _, v in orig_vals)
    new_val_set = set(v for _, v in new_vals)
    common_vals = orig_val_set & new_val_set
    print(f"  Common non-zero values (first 50): {len(common_vals)}/{min(len(orig_vals), len(new_vals))}")
    print()
    
    # Compare strings
    orig_strings = find_printable_strings(orig_data)
    new_strings = find_printable_strings(new_data)
    
    print(f"Original strings: {len(orig_strings)}")
    print(f"New strings: {len(new_strings)}")
    
    comparison = compare_strings(orig_strings, new_strings)
    
    print(f"\nString Content Match:")
    print(f"  Matched: {len(comparison['matched'])}")
    print(f"  Missing in new: {len(comparison['missing'])}")
    print(f"  Extra in new: {len(comparison['extra'])}")
    
    match_pct = len(comparison['matched']) / max(1, len(orig_strings)) * 100
    print(f"  Match percentage: {match_pct:.1f}%")
    
    # Check string order similarity
    orig_order = [s for _, s in orig_strings if s in comparison['matched']]
    new_order = [s for _, s in new_strings if s in comparison['matched']]
    
    # Create position map
    orig_positions = {s: i for i, s in enumerate(orig_order)}
    
    # Calculate how many strings are in the same relative order
    inversions = 0
    total_pairs = 0
    for i, s1 in enumerate(new_order):
        for j, s2 in enumerate(new_order[i+1:], i+1):
            if s1 in orig_positions and s2 in orig_positions:
                total_pairs += 1
                if orig_positions[s1] > orig_positions[s2]:
                    inversions += 1
    
    if total_pairs > 0:
        order_similarity = (1 - inversions / total_pairs) * 100
        print(f"  Order similarity: {order_similarity:.1f}%")
    
    # Show first few mismatched strings
    if comparison['missing']:
        print(f"\n--- First 10 Missing Strings ---")
        for s in sorted(comparison['missing'])[:10]:
            print(f"  {repr(s[:60])}")
    
    if comparison['extra']:
        print(f"\n--- First 10 Extra Strings ---")
        for s in sorted(comparison['extra'])[:10]:
            print(f"  {repr(s[:60])}")
    
    # Show both string orders for matching strings (first 30)
    print(f"\n--- String Order (first 30 common strings) ---")
    print(f"{'Orig Offset':<12} | {'New Offset':<12} | {'Content':<50}")
    print("-" * 80)
    
    common_in_orig = [(off, s) for off, s in orig_strings if s in comparison['matched']][:30]
    common_in_new = [(off, s) for off, s in new_strings if s in comparison['matched']][:30]
    
    for i in range(min(len(common_in_orig), len(common_in_new))):
        o_off, o_s = common_in_orig[i]
        n_off, n_s = common_in_new[i]
        match = "✓" if o_s == n_s else "✗"
        print(f"0x{o_off:08X}  | 0x{n_off:08X}  | {o_s[:45]:45} {match}")

if __name__ == '__main__':
    main()
