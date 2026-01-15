#!/usr/bin/env python3
"""
Compare global data (strings and uninitialized data) between original and recompiled executables.
Uses string content matching rather than absolute addresses.
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
        
        sections = []
        for i in range(num_sections):
            name = f.read(8).rstrip(b'\x00').decode('ascii', errors='ignore')
            vsize = struct.unpack('<I', f.read(4))[0]
            va = struct.unpack('<I', f.read(4))[0]
            rawsize = struct.unpack('<I', f.read(4))[0]
            rawptr = struct.unpack('<I', f.read(4))[0]
            f.read(12)
            flags = struct.unpack('<I', f.read(4))[0]
            sections.append({
                'name': name,
                'va': image_base + va,
                'vsize': vsize,
                'rawptr': rawptr,
                'rawsize': rawsize
            })
        
        return image_base, sections

def extract_strings(filename, min_length=4):
    """Extract printable strings from .data and .rdata sections"""
    image_base, sections = parse_pe(filename)
    
    strings = []
    
    with open(filename, 'rb') as f:
        for sec in sections:
            if sec['name'] not in ['.data', '.rdata']:
                continue
                
            f.seek(sec['rawptr'])
            data = f.read(sec['rawsize'])
            
            # Find null-terminated ASCII strings
            current_str = b''
            str_start = 0
            
            for i, byte in enumerate(data):
                if 32 <= byte < 127:  # Printable ASCII
                    if len(current_str) == 0:
                        str_start = i
                    current_str += bytes([byte])
                elif byte == 0 and len(current_str) >= min_length:
                    # End of string
                    va = sec['va'] + str_start
                    strings.append({
                        'va': va,
                        'content': current_str.decode('ascii'),
                        'section': sec['name']
                    })
                    current_str = b''
                else:
                    current_str = b''
    
    return strings

def main():
    parser = argparse.ArgumentParser(description='Compare globals between original and recompiled PE files')
    parser.add_argument('orig_exe', help='Original executable')
    parser.add_argument('new_exe', help='Recompiled executable')
    parser.add_argument('--min-length', type=int, default=6, help='Minimum string length')
    parser.add_argument('--filter', type=str, default='', help='Filter strings containing this text')
    args = parser.parse_args()
    
    if not os.path.exists(args.orig_exe):
        print(f"Error: {args.orig_exe} not found")
        sys.exit(1)
    if not os.path.exists(args.new_exe):
        print(f"Error: {args.new_exe} not found")
        sys.exit(1)
    
    print(f"Extracting strings from {args.orig_exe}...")
    orig_strings = extract_strings(args.orig_exe, args.min_length)
    
    print(f"Extracting strings from {args.new_exe}...")
    new_strings = extract_strings(args.new_exe, args.min_length)
    
    # Create lookup by content
    orig_by_content = {s['content']: s for s in orig_strings}
    new_by_content = {s['content']: s for s in new_strings}
    
    # Filter out CRT/Windows strings
    skip_prefixes = ['Microsoft', 'GetLast', 'GetActive', 'Message', 'user32', 
                     '(null)', 'runtime', 'KERNEL32', 'IsProcessor', 'Program:', '<program']
    
    def should_include(s):
        if args.filter and args.filter not in s:
            return False
        for prefix in skip_prefixes:
            if s.startswith(prefix):
                return False
        return True
    
    # Find important strings (game-specific)
    game_strings_orig = [s for s in orig_strings if should_include(s['content'])]
    game_strings_new = [s for s in new_strings if should_include(s['content'])]
    
    print(f"\nOriginal: {len(game_strings_orig)} game strings")
    print(f"Recompiled: {len(game_strings_new)} game strings")
    
    # Compare
    matched = 0
    missing_in_new = []
    missing_in_orig = []
    
    orig_contents = set(s['content'] for s in game_strings_orig)
    new_contents = set(s['content'] for s in game_strings_new)
    
    common = orig_contents & new_contents
    matched = len(common)
    missing_in_new = orig_contents - new_contents
    missing_in_orig = new_contents - orig_contents
    
    print(f"\n{'Content Match Summary':}")
    print(f"  Matched: {matched}")
    print(f"  Missing in recompiled: {len(missing_in_new)}")
    print(f"  Extra in recompiled: {len(missing_in_orig)}")
    
    if missing_in_new:
        print(f"\n--- Missing in recompiled (first 30) ---")
        for s in sorted(missing_in_new)[:30]:
            orig_s = orig_by_content[s]
            print(f"  0x{orig_s['va']:08X}: {repr(s[:60])}")
    
    if missing_in_orig:
        print(f"\n--- Extra in recompiled (first 30) ---")
        for s in sorted(missing_in_orig)[:30]:
            new_s = new_by_content[s]
            print(f"  0x{new_s['va']:08X}: {repr(s[:60])}")
    
    # Check ordering of matched strings
    print(f"\n--- String Order Comparison (sample) ---")
    orig_ordered = [(s['va'], s['content']) for s in game_strings_orig if s['content'] in common]
    new_ordered = [(s['va'], s['content']) for s in game_strings_new if s['content'] in common]
    
    # Sort by VA within each list
    orig_ordered.sort(key=lambda x: x[0])
    new_ordered.sort(key=lambda x: x[0])
    
    # Get content order
    orig_order = [c for va, c in orig_ordered]
    new_order = [c for va, c in new_ordered]
    
    # Calculate order similarity (Kendall-tau-like)
    order_matches = 0
    total_pairs = 0
    for i, (orig_c, new_c) in enumerate(zip(orig_order[:100], new_order[:100])):
        if orig_c == new_c:
            order_matches += 1
        total_pairs += 1
    
    if total_pairs > 0:
        order_pct = (order_matches / total_pairs) * 100
        print(f"Order similarity (first 100): {order_pct:.1f}%")
    
    # Show first few ordering differences
    print(f"\nFirst 20 strings in each binary:")
    print(f"{'Orig VA':<12} | {'New VA':<12} | {'Content':<50}")
    print("-" * 80)
    for i in range(min(20, len(orig_ordered), len(new_ordered))):
        orig_va, orig_c = orig_ordered[i]
        new_va, new_c = new_ordered[i]
        match = "✓" if orig_c == new_c else "✗"
        print(f"0x{orig_va:08X} | 0x{new_va:08X} | {orig_c[:45]:45} {match}")

if __name__ == '__main__':
    main()
