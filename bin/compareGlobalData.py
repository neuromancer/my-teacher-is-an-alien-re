#!/usr/bin/env python3
"""
Compare initialized global data between original and recompiled executables.
Focuses on specific known global addresses and their initial values.

Uses the TEACHER.map file to resolve symbol addresses in the recompiled binary.
"""
import sys
import struct
import argparse
import os
import re

# Known globals with their expected sizes and descriptions
# Format: (virtual_address, size_bytes, name, description)
KNOWN_GLOBALS = [
    # LOGPALETTE structure
    (0x437720, 4, "DAT_00437720", "LOGPALETTE header (palVersion, palNumEntries)"),
    (0x437720, 1028, "DAT_00437720_full", "Full LOGPALETTE buffer"),
    (0x437b48, 1028, "DAT_00437b48", "RGBQUAD palette buffer"),

    # Video/Graphics globals
    (0x437f4c, 4, "DAT_00437f4c", "DIB header size (runtime set)"),
    (0x437f50, 4, "DAT_00437f50", "Color depth flag"),
    (0x437f54, 4, "DAT_00437f54", "Current video buffer index"),
    (0x437f62, 4, "DAT_00437f62", "Video buffer size"),
    (0x437f66, 4, "DAT_00437f66", "Video buffer pointer"),

    # Palette identity map
    (0x437520, 256, "DAT_00437520", "Palette identity map"),
    (0x437620, 256, "DAT_00437620", "State flags / Palette data"),

    # Video buffer tables
    (0x43826c, 128, "DAT_0043826c", "Video buffer table (32 entries)"),
    (0x437fec, 128, "DAT_00437fec", "DIB handle table"),
    (0x437f6c, 128, "DAT_00437f6c", "Memory handle table"),
    (0x4383ec, 16, "DAT_004383ec", "Graphics state"),

    # Global pointers (should be 0 at init)
    (0x436960, 64, "g_GlobalPointers", "Global pointer area (0x436960-0x4369a0)"),

    # Game state
    (0x4373b8, 4, "DAT_004373b8", "DoubleClickTime"),
    (0x4373bc, 4, "DAT_004373bc", "WaitForInput var"),

    # Window globals
    (0x43de88, 4, "DAT_0043de88", "Window width"),
    (0x43de8c, 4, "DAT_0043de8c", "Window height"),
    (0x43de90, 4, "DAT_0043de90", "Windowed mode flag"),
]

def parse_map_file(map_path):
    """Parse TEACHER.map to build original_va -> recompiled_va mapping.

    Map file format:
    0003:00000df4       ?DAT_00437f4c@@3HA         0041edf4   globals.obj

    The symbol name contains the original address (DAT_00437f4c -> 0x437f4c)
    The last hex value is the address in the recompiled binary (0x41edf4)
    """
    mapping = {}
    if not os.path.exists(map_path):
        return mapping

    # Pattern matches lines like: 0003:00000df4  ?DAT_00437f4c@@3HA  0041edf4  globals.obj
    # or _DAT_004374b2  0042365c  globals.obj
    pattern = re.compile(r'[\?_]DAT_([0-9a-fA-F]{8})@@?\S*\s+([0-9a-fA-F]{8})')

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

def compare_globals(orig_file, new_file, map_file, verbose=False):
    """Compare known globals between two PE files"""

    orig_base, orig_sections = parse_pe(orig_file)
    new_base, new_sections = parse_pe(new_file)

    # Parse map file to get address mapping
    addr_map = parse_map_file(map_file)

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

    matches = 0
    mismatches = 0
    not_in_recompiled = 0

    print("=" * 110)
    print(f"{'Orig Addr':<12} {'New Addr':<12} {'Name':<25} {'Status':<10} {'Description'}")
    print("=" * 110)

    for va, size, name, desc in KNOWN_GLOBALS:
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
    parser.add_argument('new_exe', nargs='?', default='TEACHER.EXE',
                        help='Recompiled executable (default: TEACHER.EXE)')
    parser.add_argument('-m', '--map', default='TEACHER.map',
                        help='Map file for symbol resolution (default: TEACHER.map)')
    parser.add_argument('-v', '--verbose', action='store_true',
                        help='Show all data, not just mismatches')
    parser.add_argument('-a', '--address', type=str,
                        help='Check specific address (hex, e.g., 0x437720)')
    parser.add_argument('-s', '--size', type=int, default=32,
                        help='Size to read when using -a (default: 32)')
    args = parser.parse_args()

    if not os.path.exists(args.orig_exe):
        print(f"Error: Original file not found: {args.orig_exe}")
        sys.exit(1)
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
        success = compare_globals(args.orig_exe, args.new_exe, args.map, args.verbose)
        sys.exit(0 if success else 1)

if __name__ == '__main__':
    main()
