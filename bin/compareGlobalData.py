#!/usr/bin/env python3
"""
Compare initialized global data between original and recompiled executables.
Dynamically extracts globals from globals.cpp and uses TEACHER.map for address resolution.
"""
import sys
import struct
import argparse
import os
import re

# Type sizes for C types
TYPE_SIZES = {
    'char': 1, 'unsigned char': 1, 'signed char': 1,
    'short': 2, 'unsigned short': 2, 'signed short': 2,
    'int': 4, 'unsigned int': 4, 'signed int': 4, 'long': 4, 'unsigned long': 4,
    'void*': 4, 'char*': 4, 'int*': 4,  # pointers
    'HDC': 4, 'HWND': 4, 'HPALETTE': 4, 'HGDIOBJ': 4, 'HANDLE': 4, 'HMODULE': 4,
}

def parse_globals_cpp(globals_path):
    """Parse globals.cpp to extract global variables with addresses in their names.

    Returns list of (address, size, name, initializer) tuples.
    """
    globals_list = []
    if not os.path.exists(globals_path):
        return globals_list

    with open(globals_path, 'r') as f:
        content = f.read()

    # Pattern to match global variable declarations with address in name
    # Matches: type name_00XXXXXX = value; or type name_00XXXXXX; or type name_00XXXXXX[size];
    # Examples:
    #   int DAT_00437f4c = 0;
    #   char* g_Buffer_00436960 = 0;
    #   int DAT_0043826c[32];
    #   char DAT_00437720[1028] = {0x00, 0x03};
    pattern = re.compile(
        r'^(?:extern\s+"C"\s+)?'  # optional extern "C"
        r'(\w+(?:\s*\*)?)\s+'     # type (with optional pointer)
        r'(\w+_00([0-9a-fA-F]{6,8}))'  # name with address
        r'(?:\[(\d+)\])?'        # optional array size
        r'\s*(?:=\s*([^;]+))?;', # optional initializer
        re.MULTILINE
    )

    for match in pattern.finditer(content):
        type_name = match.group(1).strip()
        var_name = match.group(2)
        addr_str = match.group(3)
        array_size = match.group(4)
        initializer = match.group(5)

        # Parse address (add leading zeros if needed)
        addr_str = addr_str.zfill(8)
        address = int(addr_str, 16)

        # Calculate size
        base_type = type_name.replace('*', '').strip()
        if base_type in TYPE_SIZES:
            element_size = TYPE_SIZES[base_type]
        elif '*' in type_name:
            element_size = 4  # pointer
        else:
            element_size = 4  # default to int size

        if array_size:
            size = element_size * int(array_size)
        else:
            size = element_size

        # Determine description from initializer
        if initializer:
            init_str = initializer.strip()[:30]
            if len(initializer.strip()) > 30:
                init_str += '...'
            desc = f"init: {init_str}"
        else:
            desc = "uninitialized (BSS)"

        globals_list.append((address, size, var_name, desc))

    # Sort by address
    globals_list.sort(key=lambda x: x[0])
    return globals_list

def parse_map_file(map_path):
    """Parse TEACHER.map to build original_va -> recompiled_va mapping.

    Map file format:
    0003:00000df4       ?DAT_00437f4c@@3HA         0041edf4   globals.obj
    0003:00000100       ?g_Buffer_00436960@@3PADA  0041e100   globals.obj

    The symbol name contains the original address (e.g., DAT_00437f4c -> 0x437f4c)
    The last hex value is the address in the recompiled binary (0x41edf4)
    """
    mapping = {}
    if not os.path.exists(map_path):
        return mapping

    # Pattern matches symbols with 6-8 hex digit address at end of name:
    # - ?DAT_00437f4c@@3HA -> captures 00437f4c
    # - _DAT_004374b2 -> captures 004374b2
    # - ?g_Buffer_00436960@@3PADA -> captures 00436960
    # - _g_TimedEventPool1_00436984 -> captures 00436984
    # - _PTR_s_Setup_cfg_00437454 -> captures 00437454
    pattern = re.compile(r'[\?_]\w+_([0-9a-fA-F]{6,8})(?:@@\S*)?\s+([0-9a-fA-F]{8})')

    with open(map_path, 'r') as f:
        for line in f:
            match = pattern.search(line)
            if match:
                orig_addr = int(match.group(1), 16)
                new_addr = int(match.group(2), 16)
                mapping[orig_addr] = new_addr

    return mapping

def parse_pe(filename):
    """Parse PE and return image_base, sections dict"""
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

def read_at_va(filename, sections, image_base, va, size):
    """Read data at virtual address from the PE file.

    Handles BSS sections correctly - returns zeros for uninitialized data
    (where offset_in_section >= rawsize).
    """
    # Find which section contains this VA
    for name, sec in sections.items():
        sec_start = sec['va']
        sec_end = sec_start + sec['vsize']
        if sec_start <= va < sec_end:
            offset_in_section = va - sec_start
            rawsize = sec['rawsize']
            rawptr = sec['rawptr']

            # Check if this is in BSS (uninitialized) portion
            if offset_in_section >= rawsize:
                # BSS - return zeros
                return bytes(size)

            # Check if read extends into BSS
            bytes_in_file = min(size, rawsize - offset_in_section)

            result = bytearray(size)
            if bytes_in_file > 0 and rawptr > 0:
                file_offset = rawptr + offset_in_section
                with open(filename, 'rb') as f:
                    f.seek(file_offset)
                    result[:bytes_in_file] = f.read(bytes_in_file)
            # Rest of result is already zeros (for BSS portion)
            return bytes(result)
    return None

def format_bytes(data, max_bytes=32):
    """Format bytes for display"""
    if data is None:
        return "(not found)"
    if len(data) <= max_bytes:
        return ' '.join(f'{b:02x}' for b in data)
    else:
        return ' '.join(f'{b:02x}' for b in data[:max_bytes]) + f' ... ({len(data)} bytes)'

def format_value(data):
    """Format as various types"""
    if data is None or len(data) < 4:
        return "N/A"
    val = struct.unpack('<I', data[:4])[0]
    return f"0x{val:08x} ({val})"

def find_missing_globals(orig_file, globals_path):
    """Scan original binary's .data section for initialized data not in globals.cpp.

    Looks for non-zero 4-byte aligned values that might be globals we're missing.
    """
    orig_base, orig_sections = parse_pe(orig_file)

    # Get .data section info
    data_sec = orig_sections.get('.data')
    if not data_sec:
        print("Error: No .data section found in original binary")
        return

    # Parse globals.cpp to get covered addresses
    globals_list = parse_globals_cpp(globals_path)
    covered_ranges = []
    for addr, size, name, _ in globals_list:
        covered_ranges.append((addr, addr + size))

    def is_covered(addr):
        for start, end in covered_ranges:
            if start <= addr < end:
                return True
        return False

    # Read entire .data section
    data_start = data_sec['va']
    data_size = data_sec['rawsize']  # Only initialized portion

    print(f"Scanning original .data section: 0x{data_start:08x} - 0x{data_start + data_size:08x}")
    print(f"Known globals from globals.cpp: {len(globals_list)}")
    print()

    missing = []

    with open(orig_file, 'rb') as f:
        f.seek(data_sec['rawptr'])
        data = f.read(data_size)

    # Scan for non-zero dwords at 4-byte aligned addresses
    for offset in range(0, len(data) - 3, 4):
        addr = data_start + offset
        val = struct.unpack('<I', data[offset:offset+4])[0]

        if val != 0 and not is_covered(addr):
            # Check if this looks like a string pointer (points into .data or .rdata)
            is_ptr = (0x400000 <= val <= 0x500000)
            # Check if this is ASCII text
            chunk = data[offset:offset+16] if offset + 16 <= len(data) else data[offset:]
            is_text = all(32 <= b < 127 or b == 0 for b in chunk[:4]) and chunk[0] != 0

            missing.append((addr, val, is_ptr, is_text, chunk[:16]))

    # Group consecutive missing addresses
    print(f"Found {len(missing)} non-zero untracked dwords")
    print()
    print("=" * 100)
    print(f"{'Address':<12} {'Value':<12} {'Type':<10} {'Data'}")
    print("=" * 100)

    prev_addr = 0
    for addr, val, is_ptr, is_text, chunk in missing:
        # Show separator if gap > 16 bytes
        if prev_addr and addr - prev_addr > 16:
            print()

        type_str = "PTR?" if is_ptr else ("TEXT?" if is_text else "DATA")
        hex_str = ' '.join(f'{b:02x}' for b in chunk)
        ascii_str = ''.join(chr(b) if 32 <= b < 127 else '.' for b in chunk)

        print(f"0x{addr:08x}   0x{val:08x}   {type_str:<10} {hex_str}  {ascii_str}")
        prev_addr = addr

    print("=" * 100)
    print(f"\nTotal: {len(missing)} potential missing globals")

def compare_globals(orig_file, new_file, map_file, globals_path, verbose=False):
    """Compare known globals between two PE files"""

    orig_base, orig_sections = parse_pe(orig_file)
    new_base, new_sections = parse_pe(new_file)

    # Parse map file to get address mapping
    addr_map = parse_map_file(map_file)

    # Parse globals.cpp to get list of globals
    globals_list = parse_globals_cpp(globals_path)

    print(f"Original: {orig_file}")
    print(f"  Image base: 0x{orig_base:08x}")
    print(f"  .data section: VA=0x{orig_sections.get('.data', {}).get('va', 0):08x}")
    print()
    print(f"Recompiled: {new_file}")
    print(f"  Image base: 0x{new_base:08x}")
    print(f"  .data section: VA=0x{new_sections.get('.data', {}).get('va', 0):08x}")
    print()
    print(f"Map file: {map_file}")
    print(f"  Symbols found: {len(addr_map)}")
    print()
    print(f"Globals source: {globals_path}")
    print(f"  Globals found: {len(globals_list)}")
    print()

    matches = 0
    mismatches = 0
    not_in_recompiled = 0

    print("=" * 110)
    print(f"{'Orig Addr':<12} {'New Addr':<12} {'Name':<25} {'Status':<10} {'Description'}")
    print("=" * 110)

    for va, size, name, desc in globals_list:
        orig_data = read_at_va(orig_file, orig_sections, orig_base, va, size)

        # Look up the recompiled address from the map file
        new_va = addr_map.get(va)
        if new_va is not None:
            new_data = read_at_va(new_file, new_sections, new_base, new_va, size)
            new_addr_str = f"0x{new_va:08x}"
        else:
            new_data = None
            new_addr_str = "(not mapped)"

        if orig_data is None:
            status = "NOT FOUND"
            color = "\033[33m"  # Yellow
        elif new_va is None:
            status = "NO SYMBOL"
            color = "\033[33m"  # Yellow
            not_in_recompiled += 1
        elif new_data is None:
            status = "MISSING"
            color = "\033[31m"  # Red
            mismatches += 1
        elif orig_data == new_data:
            status = "OK"
            color = "\033[32m"  # Green
            matches += 1
        else:
            status = "MISMATCH"
            color = "\033[31m"  # Red
            mismatches += 1

        reset = "\033[0m"
        print(f"0x{va:08x}   {new_addr_str:<12} {name:<25} {color}{status:<10}{reset} {desc}")

        if verbose or (status == "MISMATCH"):
            if size <= 32:
                print(f"             Original: {format_bytes(orig_data)}")
                print(f"             New:      {format_bytes(new_data)}")
            else:
                print(f"             Original: {format_bytes(orig_data, 32)}")
                print(f"             New:      {format_bytes(new_data, 32)}")
            if size == 4:
                print(f"             Original value: {format_value(orig_data)}")
                print(f"             New value:      {format_value(new_data)}")
            print()

    print("=" * 110)
    print(f"\nSummary: {matches} matches, {mismatches} mismatches, {not_in_recompiled} not in recompiled")

    return mismatches == 0

def main():
    parser = argparse.ArgumentParser(description='Compare initialized global data between PE files')
    parser.add_argument('orig_exe', nargs='?', default='re/demo/CDDATA/TEACHER.ORI.EXE',
                        help='Original executable (default: re/demo/CDDATA/TEACHER.ORI.EXE)')
    parser.add_argument('new_exe', nargs='?', default='TEACHER-DEMO.EXE',
                        help='Recompiled executable (default: TEACHER-DEMO.EXE)')
    parser.add_argument('-m', '--map', default='TEACHER-DEMO.map',
                        help='Map file for symbol resolution (default: TEACHER-DEMO.map)')
    parser.add_argument('-g', '--globals', default='src/globals.cpp',
                        help='Path to globals.cpp (default: src/globals.cpp)')
    parser.add_argument('-v', '--verbose', action='store_true',
                        help='Show all data, not just mismatches')
    parser.add_argument('-a', '--address', type=str,
                        help='Check specific address (hex, e.g., 0x437720)')
    parser.add_argument('-s', '--size', type=int, default=32,
                        help='Size to read when using -a (default: 32)')
    parser.add_argument('--find-missing', action='store_true',
                        help='Scan original binary for globals not in globals.cpp')
    args = parser.parse_args()

    if not os.path.exists(args.orig_exe):
        print(f"Error: Original file not found: {args.orig_exe}")
        sys.exit(1)
    if not os.path.exists(args.globals):
        print(f"Error: Globals source file not found: {args.globals}")
        sys.exit(1)

    # Find missing mode - only needs original binary
    if args.find_missing:
        find_missing_globals(args.orig_exe, args.globals)
        sys.exit(0)

    if not os.path.exists(args.new_exe):
        print(f"Error: Recompiled file not found: {args.new_exe}")
        sys.exit(1)

    # Parse map file for address translation
    addr_map = parse_map_file(args.map)

    if args.address:
        # Single address mode
        va = int(args.address, 16)
        size = args.size

        orig_base, orig_sections = parse_pe(args.orig_exe)
        new_base, new_sections = parse_pe(args.new_exe)

        orig_data = read_at_va(args.orig_exe, orig_sections, orig_base, va, size)

        # Look up recompiled address from map file
        new_va = addr_map.get(va, va)  # Fall back to same address if not found
        new_data = read_at_va(args.new_exe, new_sections, new_base, new_va, size)

        print(f"Original address: 0x{va:08x}, Size: {size} bytes")
        if new_va != va:
            print(f"Recompiled address: 0x{new_va:08x} (from map file)")
        else:
            print(f"Recompiled address: 0x{new_va:08x} (not in map, using same)")
        print()
        print(f"Original ({args.orig_exe}):")
        if orig_data:
            for i in range(0, len(orig_data), 16):
                chunk = orig_data[i:i+16]
                hex_str = ' '.join(f'{b:02x}' for b in chunk)
                ascii_str = ''.join(chr(b) if 32 <= b < 127 else '.' for b in chunk)
                print(f"  {va+i:08x}: {hex_str:<48} {ascii_str}")
        else:
            print("  (not found in any section)")

        print()
        print(f"Recompiled ({args.new_exe}):")
        if new_data:
            for i in range(0, len(new_data), 16):
                chunk = new_data[i:i+16]
                hex_str = ' '.join(f'{b:02x}' for b in chunk)
                ascii_str = ''.join(chr(b) if 32 <= b < 127 else '.' for b in chunk)
                print(f"  {new_va+i:08x}: {hex_str:<48} {ascii_str}")
        else:
            print("  (not found in any section)")

        if orig_data and new_data:
            if orig_data == new_data:
                print("\n\033[32mData matches!\033[0m")
            else:
                print("\n\033[31mData differs!\033[0m")
    else:
        # Full comparison mode
        success = compare_globals(args.orig_exe, args.new_exe, args.map, args.globals, args.verbose)
        sys.exit(0 if success else 1)

if __name__ == '__main__':
    main()
