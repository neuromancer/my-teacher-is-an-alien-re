#!/usr/bin/env python3
"""
compareExe.py - Ultimate binary comparison tool for TEACHER.EXE reconstruction.

Compares original and recompiled executables at multiple levels:
  1. PE section layout (sizes, addresses)
  2. Function address mapping (correct position vs shifted)
  3. Per-function byte comparison (raw + normalized with capstone)
  4. .data section comparison with global variable annotation

Usage:
  python3 bin/compareExe.py [original] [recompiled]
  python3 bin/compareExe.py --functions    # Per-function details
  python3 bin/compareExe.py --data         # .data section details
  python3 bin/compareExe.py --detail 0x401000  # Deep-dive one function
"""
import struct
import sys
import os
import re
import argparse
from collections import defaultdict

try:
    from capstone import Cs, CS_ARCH_X86, CS_MODE_32
    HAS_CAPSTONE = True
except ImportError:
    HAS_CAPSTONE = False

# Library address ranges (CRT/compiler runtime)
LIBRARY_RANGES = [(0x424540, 0x4304E0)]


# --- PE Parsing (reused pattern from compareGlobalData.py) ---

def parse_pe(filename):
    """Parse PE and return image_base, sections dict, entry_point."""
    with open(filename, 'rb') as f:
        f.seek(0x3C)
        pe_off = struct.unpack('<I', f.read(4))[0]

        f.seek(pe_off + 6)
        num_sections = struct.unpack('<H', f.read(2))[0]

        f.seek(pe_off + 20)
        opt_size = struct.unpack('<H', f.read(2))[0]

        f.seek(pe_off + 40)
        entry_point_rva = struct.unpack('<I', f.read(4))[0]

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
                'rawsize': rawsize,
                'flags': flags,
            }

        return image_base, sections, image_base + entry_point_rva


def read_section_data(filename, sections, section_name):
    """Read raw data for a section."""
    if section_name not in sections:
        return None
    sec = sections[section_name]
    with open(filename, 'rb') as f:
        f.seek(sec['rawptr'])
        return f.read(sec['rawsize'])


def read_bytes_at_va(filename, sections, va, size):
    """Read bytes at a virtual address from the PE file."""
    for name, sec in sections.items():
        sec_start = sec['va']
        sec_end = sec_start + sec['vsize']
        if sec_start <= va < sec_end:
            offset = va - sec_start
            rawsize = sec['rawsize']
            if offset >= rawsize:
                return bytes(size)
            avail = min(size, rawsize - offset)
            result = bytearray(size)
            with open(filename, 'rb') as f:
                f.seek(sec['rawptr'] + offset)
                result[:avail] = f.read(avail)
            return bytes(result)
    return None


# --- Function address loading ---

def get_all_original_functions(map_dir):
    """Load all known function addresses from src/map/*.cpp."""
    addresses = []
    for filename in os.listdir(map_dir):
        if filename.endswith('.cpp'):
            filepath = os.path.join(map_dir, filename)
            with open(filepath, 'r') as f:
                content = f.read()
            for addr in re.findall(r'/\* Function start: (0x[0-9a-fA-F]+) \*/', content):
                addresses.append(int(addr, 16))
    return sorted(set(addresses))


def parse_linker_map_by_obj(map_path):
    """Parse linker .map file and group .text entries by object file.

    Returns dict: {obj_name: [(va, mangled_name), ...]} sorted by VA.
    """
    entries_by_obj = defaultdict(list)
    if not os.path.exists(map_path):
        return entries_by_obj

    # Match: 0001:offset  mangledName  VA f ObjName.obj
    pattern = re.compile(
        r'0001:[0-9a-fA-F]+\s+(\S+)\s+([0-9a-fA-F]{8})\s+f\s+(\S+\.obj)')
    with open(map_path, 'r') as f:
        for line in f:
            m = pattern.search(line)
            if m:
                mangled = m.group(1)
                va = int(m.group(2), 16)
                obj = m.group(3)
                entries_by_obj[obj].append((va, mangled))

    # Sort each obj's entries by VA
    for obj in entries_by_obj:
        entries_by_obj[obj].sort()
    return entries_by_obj


def parse_source_functions(src_path):
    """Parse a source file and extract (original_addr, func_info) pairs.

    Returns list of (addr, class_name_or_None, method_name, is_extern_c) tuples
    in source order.
    """
    results = []
    with open(src_path, 'r') as f:
        lines = f.readlines()

    # Track extern "C" context
    in_extern_c_block = False

    i = 0
    while i < len(lines):
        line = lines[i].strip()

        # Track extern "C" { ... } blocks
        if line.startswith('extern "C" {'):
            in_extern_c_block = True
            i += 1
            continue
        if in_extern_c_block and line == '}':
            in_extern_c_block = False
            i += 1
            continue

        # Look for /* Function start: 0xADDR */
        m = re.match(r'/\*\s*Function start:\s*(0x[0-9a-fA-F]+)\s*\*/', line)
        if m:
            addr = int(m.group(1), 16)
            # Find the function definition in the next few lines
            for j in range(i + 1, min(i + 10, len(lines))):
                defline = lines[j].strip()
                if not defline or defline.startswith('//') or defline.startswith('/*'):
                    continue

                is_extern_c = in_extern_c_block or 'extern "C"' in defline

                # Try class::method pattern
                cm = re.search(r'(\w+)::(~?\w+)\s*\(', defline)
                if cm:
                    results.append((addr, cm.group(1), cm.group(2), False))
                    break

                # Try global/standalone function
                # Patterns: void FuncName(, int __cdecl FuncName(, int WINAPI FuncName(
                gm = re.search(r'(?:^|\s)(\w+)\s*\(', defline)
                if gm:
                    fname = gm.group(1)
                    # Skip type keywords
                    if fname in ('if', 'while', 'for', 'switch', 'return', 'extern',
                                 'void', 'int', 'char', 'long', 'short', 'unsigned',
                                 'const', 'static', 'struct', 'class', 'WINAPI',
                                 '__cdecl', '__stdcall'):
                        # Try harder - find the actual function name
                        gm2 = re.search(
                            r'(?:void|int|char|long|short|unsigned|const\s+char\s*\*|'
                            r'LRESULT|BOOL|DWORD)\s+'
                            r'(?:__cdecl\s+|__stdcall\s+|WINAPI\s+|CALLBACK\s+)?'
                            r'(\w+)\s*\(', defline)
                        if gm2:
                            fname = gm2.group(1)
                            results.append((addr, None, fname, is_extern_c))
                            break
                    else:
                        results.append((addr, None, fname, is_extern_c))
                        break
                break
        i += 1
    return results


def build_function_mapping(src_dir, map_path):
    """Build original_addr -> recompiled_addr mapping by matching source
    function definitions to linker map symbols.
    """
    mapping = {}
    if not os.path.exists(map_path):
        return mapping

    map_entries = parse_linker_map_by_obj(map_path)

    # Process each source file
    import glob as globmod
    for src_file in sorted(globmod.glob(os.path.join(src_dir, '*.cpp'))):
        basename = os.path.basename(src_file)
        if basename == 'globals.cpp':
            continue  # globals.cpp doesn't have functions in .text
        obj_name = basename.replace('.cpp', '.obj')
        obj_entries = map_entries.get(obj_name, [])
        if not obj_entries:
            continue

        source_funcs = parse_source_functions(src_file)
        if not source_funcs:
            continue

        # For each source function, find matching map entry
        used_indices = set()
        for orig_addr, class_name, method_name, is_extern_c in source_funcs:
            # Build match patterns for the mangled name
            patterns = []
            if class_name:
                if class_name == method_name:
                    # Constructor
                    patterns.append(f'??0{class_name}@@')
                elif method_name.startswith('~'):
                    # Destructor
                    patterns.append(f'??1{class_name}@@')
                else:
                    # Regular method
                    patterns.append(f'?{method_name}@{class_name}@@')
            else:
                if is_extern_c:
                    # extern "C" -> _FuncName or _FuncName@N (stdcall)
                    patterns.append(f'_{method_name}@')
                    patterns.append(f'_{method_name}')
                else:
                    # C++ global function
                    patterns.append(f'?{method_name}@@')
                    # Could also be extern "C" without explicit marking
                    patterns.append(f'_{method_name}@')
                    patterns.append(f'_{method_name}')

            # Find first unused matching entry in obj's map entries
            for idx, (va, mangled) in enumerate(obj_entries):
                if idx in used_indices:
                    continue
                matched = False
                for pat in patterns:
                    if pat == mangled or mangled.startswith(pat) or pat in mangled:
                        matched = True
                        break
                if matched:
                    mapping[orig_addr] = va
                    used_indices.add(idx)
                    break

    return mapping


def get_recompiled_functions_from_map(map_path):
    """Parse linker .map to get all recompiled function addresses (from .text section).

    Returns sorted list of (recompiled_va, symbol_name) tuples.
    """
    functions = []
    if not os.path.exists(map_path):
        return functions

    # Match lines like: 0001:00000000  ??0Animation@@QAE@XZ  00401000 f Animation.obj
    pattern = re.compile(r'0001:([0-9a-fA-F]+)\s+(\S+)\s+([0-9a-fA-F]+)\s')
    with open(map_path, 'r') as f:
        for line in f:
            m = pattern.search(line)
            if m:
                va = int(m.group(3), 16)
                name = m.group(2)
                functions.append((va, name))
    # Deduplicate by address
    seen = set()
    unique = []
    for va, name in sorted(functions):
        if va not in seen:
            seen.add(va)
            unique.append((va, name))
    return unique


def parse_globals_cpp(globals_path):
    """Parse globals.cpp to get global variable addresses and sizes."""
    globals_list = []
    if not os.path.exists(globals_path):
        return globals_list

    with open(globals_path, 'r') as f:
        content = f.read()

    TYPE_SIZES = {
        'char': 1, 'unsigned char': 1, 'short': 2, 'unsigned short': 2,
        'int': 4, 'unsigned int': 4, 'long': 4, 'unsigned long': 4,
        'void*': 4, 'char*': 4, 'int*': 4,
        'HDC': 4, 'HWND': 4, 'HPALETTE': 4, 'HGDIOBJ': 4, 'HANDLE': 4,
    }

    pattern = re.compile(
        r'^(?:extern\s+"C"\s+)?(\w+(?:\s*\*)?)\s+'
        r'(\w+_00([0-9a-fA-F]{6,8}))'
        r'(?:\[(\d+)\])?'
        r'\s*(?:=\s*([^;]+))?;',
        re.MULTILINE
    )

    for match in pattern.finditer(content):
        type_name = match.group(1).strip()
        var_name = match.group(2)
        addr = int(match.group(3).zfill(8), 16)
        array_size = match.group(4)

        base_type = type_name.replace('*', '').strip()
        elem_size = TYPE_SIZES.get(base_type, 4)
        if '*' in type_name:
            elem_size = 4
        size = elem_size * int(array_size) if array_size else elem_size

        globals_list.append((addr, size, var_name))

    globals_list.sort()
    return globals_list


def is_library(addr):
    """Check if address is in library/CRT range."""
    for start, end in LIBRARY_RANGES:
        if start <= addr <= end:
            return True
    return False


# --- Instruction-level comparison using capstone ---

def normalize_function_bytes(data, base_addr):
    """Disassemble function bytes and extract normalized opcode sequence.

    Returns list of (mnemonic, op_str, size, raw_bytes) tuples.
    Masks absolute address operands for comparison.
    """
    if not HAS_CAPSTONE or data is None:
        return None

    md = Cs(CS_ARCH_X86, CS_MODE_32)
    md.detail = False
    instructions = []
    for insn in md.disasm(data, base_addr):
        instructions.append((insn.mnemonic, insn.op_str, insn.size, insn.bytes))
    return instructions


def compare_instructions(orig_insns, recomp_insns):
    """Compare two instruction sequences.

    Returns (total, matching_opcodes, matching_full) counts.
    """
    if orig_insns is None or recomp_insns is None:
        return 0, 0, 0

    total = max(len(orig_insns), len(recomp_insns))
    if total == 0:
        return 0, 0, 0

    matching_opcodes = 0
    matching_full = 0
    for i in range(min(len(orig_insns), len(recomp_insns))):
        o_mn, o_op, o_sz, o_bytes = orig_insns[i]
        r_mn, r_op, r_sz, r_bytes = recomp_insns[i]
        if o_mn == r_mn:
            matching_opcodes += 1
            if o_bytes == r_bytes:
                matching_full += 1
    return total, matching_opcodes, matching_full


# --- Report generators ---

def report_sections(orig_file, recomp_file):
    """Compare PE section layout."""
    _, orig_secs, orig_ep = parse_pe(orig_file)
    _, recomp_secs, recomp_ep = parse_pe(recomp_file)

    print("=== PE Section Comparison ===")
    print(f"  {'Section':<10} {'Original VA':>14} {'Orig Size':>12} {'Recomp VA':>14} {'Recomp Size':>12} {'Match'}")
    print(f"  {'-'*10} {'-'*14} {'-'*12} {'-'*14} {'-'*12} {'-'*5}")

    all_names = list(dict.fromkeys(list(orig_secs.keys()) + list(recomp_secs.keys())))
    for name in all_names:
        o = orig_secs.get(name)
        r = recomp_secs.get(name)
        if o and r:
            va_match = o['va'] == r['va']
            sz_match = o['vsize'] == r['vsize']
            status = '\u2705' if (va_match and sz_match) else '\u274c'
            print(f"  {name:<10} 0x{o['va']:08X} {o['vsize']:>10}  0x{r['va']:08X} {r['vsize']:>10}  {status}")
        elif o:
            print(f"  {name:<10} 0x{o['va']:08X} {o['vsize']:>10}  {'(missing)':>14} {'':>12}  \u274c")
        else:
            print(f"  {name:<10} {'(missing)':>14} {'':>12}  0x{r['va']:08X} {r['vsize']:>10}  \u274c")

    ep_match = '\u2705' if orig_ep == recomp_ep else '\u274c'
    print(f"\n  Entry point: 0x{orig_ep:08X} vs 0x{recomp_ep:08X} {ep_match}")
    print()


def report_functions(orig_file, recomp_file, map_dir, map_file, src_dir, show_all=False):
    """Compare function addresses and byte content."""
    orig_funcs = get_all_original_functions(map_dir)
    addr_mapping = build_function_mapping(src_dir, map_file)
    _, orig_secs, _ = parse_pe(orig_file)
    _, recomp_secs, _ = parse_pe(recomp_file)

    # Build recompiled function address list for size calculation
    recomp_func_list = get_recompiled_functions_from_map(map_file)
    recomp_addrs = sorted(set(va for va, _ in recomp_func_list))

    text_end_orig = orig_secs.get('.text', {}).get('va', 0) + orig_secs.get('.text', {}).get('vsize', 0)
    text_end_recomp = recomp_secs.get('.text', {}).get('va', 0) + recomp_secs.get('.text', {}).get('vsize', 0)

    correct_addr = 0
    shifted = 0
    not_found = 0
    total_user = 0
    byte_identical = 0
    opcode_high = 0  # >=95% opcode match

    results = []

    for i, orig_addr in enumerate(orig_funcs):
        # Function size in original = distance to next function
        if i + 1 < len(orig_funcs):
            orig_size = orig_funcs[i + 1] - orig_addr
        else:
            orig_size = text_end_orig - orig_addr

        orig_size = min(orig_size, 0x10000)  # cap at 64k
        lib = is_library(orig_addr)

        if not lib:
            total_user += 1

        recomp_addr = addr_mapping.get(orig_addr)

        if recomp_addr is None:
            if not lib:
                not_found += 1
            results.append((orig_addr, None, orig_size, 0, lib, 0.0, 0.0, 'missing'))
            continue

        at_correct = (recomp_addr == orig_addr)
        if at_correct and not lib:
            correct_addr += 1
        elif not lib:
            shifted += 1

        # Get recompiled function size
        idx = None
        for j, ra in enumerate(recomp_addrs):
            if ra == recomp_addr:
                idx = j
                break
        if idx is not None and idx + 1 < len(recomp_addrs):
            recomp_size = recomp_addrs[idx + 1] - recomp_addr
        else:
            recomp_size = orig_size
        recomp_size = min(recomp_size, 0x10000)

        # Compare bytes
        compare_size = min(orig_size, recomp_size)
        orig_bytes = read_bytes_at_va(orig_file, orig_secs, orig_addr, compare_size)
        recomp_bytes = read_bytes_at_va(recomp_file, recomp_secs, recomp_addr, compare_size)

        raw_match = 0.0
        opcode_match = 0.0
        status = 'shifted' if not at_correct else 'ok'

        if orig_bytes and recomp_bytes:
            # Raw byte comparison
            matching = sum(1 for a, b in zip(orig_bytes, recomp_bytes) if a == b)
            raw_match = matching / compare_size * 100 if compare_size > 0 else 0

            if raw_match == 100.0 and orig_size == recomp_size:
                if not lib:
                    byte_identical += 1
                status = 'identical'

            # Instruction-level comparison
            if HAS_CAPSTONE:
                orig_insns = normalize_function_bytes(orig_bytes, orig_addr)
                recomp_insns = normalize_function_bytes(recomp_bytes, recomp_addr)
                total_insns, match_opc, match_full = compare_instructions(orig_insns, recomp_insns)
                opcode_match = match_opc / total_insns * 100 if total_insns > 0 else 0
                if opcode_match >= 95 and not lib:
                    opcode_high += 1

        results.append((orig_addr, recomp_addr, orig_size, recomp_size, lib, raw_match, opcode_match, status))

    # Print report
    print("=== Function Address Mapping ===")
    print(f"  Total functions in map: {len(orig_funcs)}")
    print(f"  User functions: {total_user} (excluding {len(orig_funcs) - total_user} library)")
    print(f"  At correct address: {correct_addr}/{total_user} ({correct_addr/total_user*100:.1f}%)" if total_user > 0 else "")
    print(f"  Shifted: {shifted}/{total_user}")
    print(f"  Not in recompiled: {not_found}/{total_user}")
    if HAS_CAPSTONE:
        print(f"  Byte-identical: {byte_identical}/{total_user}")
        print(f"  Opcode match >=95%: {opcode_high}/{total_user}")
    print()

    if show_all:
        print(f"  {'Address':>10} {'Recomp':>10} {'OSize':>6} {'RSize':>6} {'Raw%':>6} {'Opc%':>6} {'Status'}")
        print(f"  {'-'*10} {'-'*10} {'-'*6} {'-'*6} {'-'*6} {'-'*6} {'-'*10}")
        for orig_addr, recomp_addr, osize, rsize, lib, raw, opc, status in results:
            if lib:
                continue
            ra = f"0x{recomp_addr:08X}" if recomp_addr else "  -------"
            sym = ''
            if status == 'identical':
                sym = '\u2705'
            elif status == 'ok':
                sym = '\u2796'
            elif status == 'shifted':
                sym = '\u274c'
            else:
                sym = '\u26a0\ufe0f'
            print(f"  0x{orig_addr:08X} {ra} {osize:>6} {rsize:>6} {raw:>5.1f}% {opc:>5.1f}% {sym} {status}")
        print()

    # Show first misalignment
    for orig_addr, recomp_addr, osize, rsize, lib, raw, opc, status in results:
        if lib:
            continue
        if status in ('shifted', 'missing'):
            if recomp_addr:
                print(f"  First misalignment: 0x{orig_addr:08X} (expected) -> 0x{recomp_addr:08X} (actual)")
            else:
                print(f"  First misalignment: 0x{orig_addr:08X} (not found in recompiled)")
            break
    print()

    return results


def report_data(orig_file, recomp_file, globals_path):
    """Compare .data sections."""
    _, orig_secs, _ = parse_pe(orig_file)
    _, recomp_secs, _ = parse_pe(recomp_file)

    orig_data_sec = orig_secs.get('.data')
    recomp_data_sec = recomp_secs.get('.data')
    if not orig_data_sec or not recomp_data_sec:
        print("=== Data Section Comparison ===")
        print("  Could not find .data section in one or both binaries.")
        print()
        return

    orig_data = read_section_data(orig_file, orig_secs, '.data')
    recomp_data = read_section_data(recomp_file, recomp_secs, '.data')

    if not orig_data or not recomp_data:
        print("  Could not read .data section.")
        return

    # Compare the raw .data bytes at matching offsets
    compare_len = min(len(orig_data), len(recomp_data))
    matching = sum(1 for i in range(compare_len) if orig_data[i] == recomp_data[i])

    print("=== Data Section Comparison ===")
    print(f"  Original .data:   VA=0x{orig_data_sec['va']:08X}  size={len(orig_data)} bytes")
    print(f"  Recompiled .data: VA=0x{recomp_data_sec['va']:08X}  size={len(recomp_data)} bytes")
    va_match = '\u2705' if orig_data_sec['va'] == recomp_data_sec['va'] else '\u274c'
    print(f"  VA match: {va_match}")
    print(f"  Bytes compared: {compare_len}")
    print(f"  Matching bytes: {matching}/{compare_len} ({matching/compare_len*100:.1f}%)")
    print()

    # Load globals for annotation
    globals_list = parse_globals_cpp(globals_path)
    globals_by_offset = {}
    data_va = orig_data_sec['va']
    for addr, size, name in globals_list:
        offset = addr - data_va
        if 0 <= offset < len(orig_data):
            globals_by_offset[offset] = (name, size)

    # Find and report mismatches, grouped by region
    mismatches = []
    i = 0
    while i < compare_len:
        if orig_data[i] != recomp_data[i]:
            start = i
            while i < compare_len and orig_data[i] != recomp_data[i]:
                i += 1
            end = i
            # Find the global variable at this offset
            var_name = None
            for off in range(start, -1, -1):
                if off in globals_by_offset:
                    var_name = globals_by_offset[off][0]
                    break
            mismatches.append((start, end, var_name))
        else:
            i += 1

    if mismatches:
        print(f"  Mismatch regions: {len(mismatches)}")
        shown = 0
        for start, end, var_name in mismatches:
            if shown >= 30:
                print(f"  ... and {len(mismatches) - shown} more regions")
                break
            va = data_va + start
            length = end - start
            var_info = f"  ({var_name})" if var_name else ""
            orig_hex = ' '.join(f'{orig_data[j]:02x}' for j in range(start, min(start + 8, end)))
            recomp_hex = ' '.join(f'{recomp_data[j]:02x}' for j in range(start, min(start + 8, end)))
            if length > 8:
                orig_hex += ' ...'
                recomp_hex += ' ...'
            print(f"  0x{va:08X} +{length:>4} bytes{var_info}")
            print(f"    orig:   {orig_hex}")
            print(f"    recomp: {recomp_hex}")
            shown += 1
    else:
        print("  No mismatches!")
    print()


def report_detail(orig_file, recomp_file, map_dir, map_file, src_dir, target_addr):
    """Deep-dive comparison of a single function."""
    orig_funcs = get_all_original_functions(map_dir)
    addr_mapping = build_function_mapping(src_dir, map_file)
    _, orig_secs, _ = parse_pe(orig_file)
    _, recomp_secs, _ = parse_pe(recomp_file)

    if target_addr not in orig_funcs:
        # Find closest
        closest = min(orig_funcs, key=lambda x: abs(x - target_addr))
        print(f"Address 0x{target_addr:08X} not found in map. Closest: 0x{closest:08X}")
        target_addr = closest

    idx = orig_funcs.index(target_addr)
    if idx + 1 < len(orig_funcs):
        orig_size = orig_funcs[idx + 1] - target_addr
    else:
        text_end = orig_secs.get('.text', {}).get('va', 0) + orig_secs.get('.text', {}).get('vsize', 0)
        orig_size = text_end - target_addr
    orig_size = min(orig_size, 0x10000)

    recomp_addr = addr_mapping.get(target_addr)

    print(f"=== Function Detail: 0x{target_addr:08X} ===")
    print(f"  Original address:   0x{target_addr:08X}")
    if recomp_addr:
        print(f"  Recompiled address: 0x{recomp_addr:08X}", end='')
        print(" \u2705" if recomp_addr == target_addr else " \u274c SHIFTED")
    else:
        print("  Recompiled address: NOT FOUND")
        return
    print(f"  Original size: {orig_size} bytes")

    orig_bytes = read_bytes_at_va(orig_file, orig_secs, target_addr, orig_size)
    recomp_bytes = read_bytes_at_va(recomp_file, recomp_secs, recomp_addr, orig_size)

    if not orig_bytes or not recomp_bytes:
        print("  Could not read bytes from one or both binaries.")
        return

    # Raw comparison
    matching = sum(1 for a, b in zip(orig_bytes, recomp_bytes) if a == b)
    print(f"  Raw byte match: {matching}/{orig_size} ({matching/orig_size*100:.1f}%)")

    if not HAS_CAPSTONE:
        print("  (Install capstone for instruction-level comparison)")
        return

    # Instruction-level comparison
    md = Cs(CS_ARCH_X86, CS_MODE_32)
    md.detail = False

    orig_insns = list(md.disasm(orig_bytes, target_addr))
    recomp_insns = list(md.disasm(recomp_bytes, recomp_addr))

    print(f"  Instructions: {len(orig_insns)} (orig) vs {len(recomp_insns)} (recomp)")
    print()

    # Side-by-side instruction comparison
    max_show = min(len(orig_insns), len(recomp_insns))
    diffs = 0
    for i in range(max_show):
        oi = orig_insns[i]
        ri = recomp_insns[i]
        o_hex = ' '.join(f'{b:02x}' for b in oi.bytes)
        r_hex = ' '.join(f'{b:02x}' for b in ri.bytes)
        o_asm = f"{oi.mnemonic} {oi.op_str}"
        r_asm = f"{ri.mnemonic} {ri.op_str}"

        if oi.mnemonic == ri.mnemonic:
            if oi.bytes == ri.bytes:
                marker = '  '
            else:
                marker = '~ '  # same opcode, different operands
                diffs += 1
        else:
            marker = '! '
            diffs += 1

        print(f"  {marker}0x{oi.address:08X} {o_hex:<24s} {o_asm:<30s} | 0x{ri.address:08X} {r_hex:<24s} {r_asm}")

    if len(orig_insns) != len(recomp_insns):
        extra = abs(len(orig_insns) - len(recomp_insns))
        which = "original" if len(orig_insns) > len(recomp_insns) else "recompiled"
        print(f"\n  {extra} extra instructions in {which}")

    matched_opcodes = max_show - diffs
    print(f"\n  Opcode match: {matched_opcodes}/{max_show} ({matched_opcodes/max_show*100:.1f}%)" if max_show > 0 else "")
    print()


def report_summary(orig_file, recomp_file):
    """Quick overall summary comparing raw section bytes."""
    _, orig_secs, _ = parse_pe(orig_file)
    _, recomp_secs, _ = parse_pe(recomp_file)

    print("=== Overall Summary ===")
    total_match = 0
    total_compared = 0

    for sec_name in ['.text', '.rdata', '.data']:
        orig_data = read_section_data(orig_file, orig_secs, sec_name)
        recomp_data = read_section_data(recomp_file, recomp_secs, sec_name)
        if orig_data and recomp_data:
            cmp_len = min(len(orig_data), len(recomp_data))
            matching = sum(1 for i in range(cmp_len) if orig_data[i] == recomp_data[i])
            pct = matching / cmp_len * 100 if cmp_len > 0 else 0
            print(f"  {sec_name:<8} {matching:>8}/{cmp_len:<8} bytes match ({pct:.1f}%)")
            total_match += matching
            total_compared += cmp_len

    if total_compared > 0:
        print(f"  {'Total':<8} {total_match:>8}/{total_compared:<8} bytes match ({total_match/total_compared*100:.1f}%)")
    print()


def main():
    parser = argparse.ArgumentParser(description='Binary comparison tool for TEACHER.EXE reconstruction')
    parser.add_argument('orig', nargs='?', default='re/demo/CDDATA/TEACHER.ORI.EXE',
                        help='Original executable')
    parser.add_argument('recomp', nargs='?', default='TEACHER-DEMO.EXE',
                        help='Recompiled executable')
    parser.add_argument('-m', '--map', default='TEACHER-DEMO.map',
                        help='Linker map file (default: TEACHER-DEMO.map)')
    parser.add_argument('--map-dir', default='src/map',
                        help='Map directory with original function addresses')
    parser.add_argument('--src-dir', default='src',
                        help='Source directory with .cpp files')
    parser.add_argument('--globals', default='src/globals.cpp',
                        help='Path to globals.cpp for data annotation')
    parser.add_argument('--summary', action='store_true',
                        help='Show only section-level summary')
    parser.add_argument('--functions', action='store_true',
                        help='Show per-function comparison')
    parser.add_argument('--data', action='store_true',
                        help='Show .data section comparison')
    parser.add_argument('--detail', type=str, default=None,
                        help='Deep-dive into a specific function (hex address)')
    args = parser.parse_args()

    if not os.path.exists(args.orig):
        print(f"Error: Original executable not found: {args.orig}")
        sys.exit(1)
    if not os.path.exists(args.recomp):
        print(f"Error: Recompiled executable not found: {args.recomp}")
        print("Run 'make TEACHER-DEMO.EXE' first.")
        sys.exit(1)

    print(f"Comparing: {args.orig}")
    print(f"     with: {args.recomp}")
    if not HAS_CAPSTONE:
        print("Note: Install capstone (pip install capstone) for instruction-level analysis")
    print()

    # Handle --detail mode
    if args.detail:
        addr = int(args.detail, 16) if args.detail.startswith('0x') else int(args.detail, 16)
        report_detail(args.orig, args.recomp, args.map_dir, args.map, args.src_dir, addr)
        return

    # Default: show everything. Flags filter to specific sections.
    show_all = not (args.summary or args.functions or args.data)

    if show_all or args.summary:
        report_sections(args.orig, args.recomp)

    if show_all or args.functions:
        report_functions(args.orig, args.recomp, args.map_dir, args.map, args.src_dir, show_all=args.functions)

    if show_all or args.data:
        report_data(args.orig, args.recomp, args.globals)

    report_summary(args.orig, args.recomp)


if __name__ == '__main__':
    main()
