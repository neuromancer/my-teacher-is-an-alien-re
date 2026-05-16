#!/usr/bin/env python3
"""
Verify that reimplemented functions call the correct functions compared to the original assembly.

The mnemonic-based similarity checker doesn't verify call destinations - a CALL instruction
matches any other CALL regardless of target. This script compares the multiset of called
functions between original and reimplemented code.

Vtable calls (CALL dword ptr [reg+offset]) are compared by offset to verify correct vtable slot.
"""

import argparse
import glob
import os
import re
import struct
import subprocess
import sys
from collections import Counter
from cppSourceParser import parse_source_function_groups
from projectConfig import (
    ConfigError,
    add_config_argument,
    config_or_arg,
    load_config,
    mode_paths,
    parse_int_map,
)

# Project-specific call target data is loaded from config/verification.json.
KNOWN_CRT_BY_MODE = {}
CANONICAL = {}
CALL_COUNT_ALLOWANCES = {}
FUNCTION_POINTER_TARGETS = {}
ORDINAL_IMPORTS = {}
SKIP_CALL_TOKENS = set()
TRIVIAL_CALL_TOKENS = set()


def configure_call_checker(config):
    global KNOWN_CRT_BY_MODE, CANONICAL, CALL_COUNT_ALLOWANCES, FUNCTION_POINTER_TARGETS
    global ORDINAL_IMPORTS, SKIP_CALL_TOKENS, TRIVIAL_CALL_TOKENS

    calls_config = config.get("calls", {})
    KNOWN_CRT_BY_MODE = {
        mode: parse_int_map(mapping)
        for mode, mapping in calls_config.get("known_crt", {}).items()
    }
    CANONICAL = dict(calls_config.get("canonical_aliases", {}))
    CALL_COUNT_ALLOWANCES = {
        (item["function"], item["target"]): (int(item.get("missing", 0)), int(item.get("extra", 0)))
        for item in calls_config.get("call_count_allowances", [])
    }
    FUNCTION_POINTER_TARGETS = parse_int_map(calls_config.get("function_pointer_globals", {}))
    ORDINAL_IMPORTS = {
        (item["dll"].upper(), int(item["ordinal"])): item["name"]
        for item in calls_config.get("ordinal_imports", [])
    }
    SKIP_CALL_TOKENS = set(calls_config.get("skip_tokens", []))
    TRIVIAL_CALL_TOKENS = set(calls_config.get("trivial_tokens", []))


def canonicalize(name):
    if name.startswith("FUN_"):
        name = name.upper()
    name = CANONICAL.get(name, name)
    return name


def apply_call_count_allowances(func_name, only_orig, only_compiled):
    for (allowed_func, name), (missing_allowed, extra_allowed) in CALL_COUNT_ALLOWANCES.items():
        if func_name != allowed_func:
            continue
        if missing_allowed:
            remaining = only_orig.get(name, 0) - missing_allowed
            if remaining > 0:
                only_orig[name] = remaining
            else:
                only_orig.pop(name, None)
        if extra_allowed:
            remaining = only_compiled.get(name, 0) - extra_allowed
            if remaining > 0:
                only_compiled[name] = remaining
            else:
                only_compiled.pop(name, None)


def iter_source_functions(cpp_file, include_no_assembly=False):
    """Yield ([start_addrs], function_name) pairs from source annotations."""
    for group in parse_source_function_groups(
        cpp_file,
        include_no_assembly=include_no_assembly,
        disambiguate_overloads=True,
    ):
        yield [int(address, 16) for address in group.addresses], group.name


def auto_detect_crt(code_dir):
    """Auto-detect CRT/library function addresses from Ghidra decompiled text."""
    crt_map = {}
    for decompiled in glob.glob(f"{code_dir}/FUN_*.decompiled.txt"):
        try:
            with open(decompiled, 'r') as f:
                content = f.read(2000)  # Only need header
        except:
            continue
        if "Library Function" not in content:
            continue
        # Extract address from filename
        basename = os.path.basename(decompiled)
        m = re.match(r'FUN_([0-9a-fA-F]+)\.decompiled\.txt', basename)
        if not m:
            continue
        addr = int(m.group(1), 16)
        header = re.match(r'Function:\s*([A-Za-z_][\w]*)', content)
        if header and not header.group(1).startswith("FUN_"):
            crt_map[addr] = normalize_import_name(header.group(1))
            continue
        # Extract function name from line after "Library Function"
        lines = content.split('\n')
        for i, line in enumerate(lines):
            if "Library Function" in line:
                # Name is usually on next non-empty line, stripped of leading _
                for j in range(i+1, min(i+3, len(lines))):
                    name = lines[j].strip()
                    if name and not name.startswith('*') and not name.startswith('/'):
                        # Clean up: "_sprintf" -> "sprintf", "__strcmpi" -> "_strcmpi"
                        if name.startswith('_') and not name.startswith('__'):
                            name = name[1:]
                        crt_map[addr] = name
                        break
                break
    return crt_map


def build_address_to_name_map(src_dir, code_dir, mode):
    """Build address-to-name map from configured CRT names, auto-detected CRT, and source annotations."""
    addr_map = dict(KNOWN_CRT_BY_MODE.get(mode, {}))
    if mode != "demo":
        # Also auto-detect CRT from decompiled texts
        auto_crt = auto_detect_crt(code_dir)
        for addr, name in auto_crt.items():
            if addr not in addr_map:
                addr_map[addr] = name

    for cpp_file in glob.glob(f"{src_dir}/*.cpp"):
        for addrs, func_name in iter_source_functions(cpp_file, include_no_assembly=True):
            for addr in addrs:
                addr_map[addr] = func_name
    return addr_map


# Import Address Table entries loaded from the original executable. Many calls
# go through indirect IAT slots, while the rebuild may emit named __imp__ calls
# or direct CRT thunks, so both sides are normalized to the import name.
IAT_ADDRESSES = {}
IAT_ADDRESS_RANGES = []


def normalize_import_name(name):
    name = re.sub(r'@\d+$', '', name)
    if name.startswith('_') and not name.startswith('__'):
        return name[1:]
    return name


def parse_c_string(data, offset):
    end = data.find(b'\0', offset)
    if end < 0:
        end = len(data)
    return data[offset:end].decode("ascii", errors="replace")


def parse_pe_iat_addresses(exe_path):
    """Return {virtual_iat_address: import_name} for a PE32 executable."""
    if not exe_path or not os.path.exists(exe_path):
        return {}, []

    with open(exe_path, "rb") as f:
        data = f.read()

    def u16(offset):
        return struct.unpack_from("<H", data, offset)[0]

    def u32(offset):
        return struct.unpack_from("<I", data, offset)[0]

    if len(data) < 0x40 or data[:2] != b"MZ":
        raise ValueError(f"not a PE executable: {exe_path}")

    pe_offset = u32(0x3C)
    if pe_offset + 0x18 >= len(data) or data[pe_offset:pe_offset + 4] != b"PE\0\0":
        raise ValueError(f"invalid PE header: {exe_path}")

    file_header = pe_offset + 4
    section_count = u16(file_header + 2)
    optional_size = u16(file_header + 16)
    optional_header = file_header + 20
    magic = u16(optional_header)
    if magic != 0x10B:
        raise ValueError(f"unsupported PE optional header: {exe_path}")

    image_base = u32(optional_header + 28)
    data_dir = optional_header + 96
    import_rva = u32(data_dir + 8)
    import_size = u32(data_dir + 12)
    if import_rva == 0:
        return {}, []

    sections_offset = optional_header + optional_size
    sections = []
    for index in range(section_count):
        section_offset = sections_offset + (index * 40)
        if section_offset + 40 > len(data):
            break
        virtual_size = u32(section_offset + 8)
        virtual_address = u32(section_offset + 12)
        raw_size = u32(section_offset + 16)
        raw_pointer = u32(section_offset + 20)
        sections.append((virtual_address, max(virtual_size, raw_size), raw_pointer, raw_size))

    def rva_to_offset(rva):
        for virtual_address, size, raw_pointer, raw_size in sections:
            if virtual_address <= rva < virtual_address + size:
                offset = raw_pointer + (rva - virtual_address)
                if offset < raw_pointer + raw_size or raw_size == 0:
                    return offset
        if rva < len(data):
            return rva
        raise ValueError(f"RVA 0x{rva:X} is outside mapped sections")

    imports = {}
    ranges = []
    descriptor = rva_to_offset(import_rva)
    while descriptor + 20 <= len(data):
        original_first_thunk = u32(descriptor)
        name_rva = u32(descriptor + 12)
        first_thunk = u32(descriptor + 16)
        if original_first_thunk == 0 and name_rva == 0 and first_thunk == 0:
            break

        dll_name = parse_c_string(data, rva_to_offset(name_rva)).upper() if name_rva else ""
        thunk_rva = original_first_thunk or first_thunk
        index = 0
        while True:
            thunk_offset = rva_to_offset(thunk_rva + (index * 4))
            thunk = u32(thunk_offset)
            if thunk == 0:
                break
            if thunk & 0x80000000:
                import_name = ORDINAL_IMPORTS.get((dll_name, thunk & 0xFFFF))
                if import_name:
                    imports[image_base + first_thunk + (index * 4)] = import_name
            else:
                import_name_offset = rva_to_offset(thunk) + 2
                import_name = normalize_import_name(parse_c_string(data, import_name_offset))
                imports[image_base + first_thunk + (index * 4)] = import_name
            index += 1

        if index:
            ranges.append((image_base + first_thunk, image_base + first_thunk + (index * 4)))
        descriptor += 20

    return imports, ranges


def is_iat_address(addr):
    if addr in IAT_ADDRESSES:
        return True
    return any(start <= addr < end for start, end in IAT_ADDRESS_RANGES)

def parse_vtable_call(line):
    """Parse a vtable/indirect call and return a normalized form.
    Original: CALL dword ptr [EAX + 0x4] -> vtable[0x4]
    Compiled: call DWORD PTR [eax+4] -> vtable[0x4]
    Absolute IAT: CALL dword ptr [0x00476370] -> name or __import__
    Absolute known function pointer: CALL dword ptr [0x0046e01c] -> WinGBitBlt
    """
    # Absolute-address indirect call (IAT import): CALL dword ptr [0xADDR]
    m = re.match(r'CALL\s+dword\s+ptr\s*\[\s*(?:0x)?([0-9a-fA-F]+)\s*\]', line, re.IGNORECASE)
    if m:
        try:
            addr = int(m.group(1), 16)
            if is_iat_address(addr):
                return IAT_ADDRESSES.get(addr, "__import__")
            if addr in FUNCTION_POINTER_TARGETS:
                return FUNCTION_POINTER_TARGETS[addr]
            if addr >= 0x400000:
                return "__funcptr__"
        except ValueError:
            pass

    # Original format: CALL dword ptr [REG + 0xOFFSET] or CALL dword ptr [REG]
    m = re.match(r'CALL\s+dword\s+ptr\s*\[\s*(\w+)\s*(?:\+\s*(?:0x)?([0-9a-fA-F]+))?\s*\]', line, re.IGNORECASE)
    if m:
        offset = int(m.group(2), 16) if m.group(2) else 0
        return f"vtable[0x{offset:x}]"

    # Compiled format: call DWORD PTR [reg+offset]
    m = re.match(r'call\s+DWORD\s+PTR\s*\[\s*(\w+)\s*(?:\+\s*(\d+))?\s*\]', line, re.IGNORECASE)
    if m:
        offset = int(m.group(2)) if m.group(2) else 0
        return f"vtable[0x{offset:x}]"

    return None


def extract_calls_from_original(disasm_path):
    """Extract CALL targets from original disassembly.

    Tracks simple MOV REG, [IAT_ADDR] hoists so that subsequent CALL REG
    resolves to the IAT symbol. The original compiler frequently hoists
    rand/GetTickCount into EBX/EDI inside a function, while our rebuild
    emits the direct CRT call.
    """
    calls = []
    reg_map = {}  # e.g. "EBX" -> "rand"
    mov_iat_re = re.compile(
        r'MOV\s+(E[A-D]X|ESI|EDI|EBP)\s*,\s*dword\s+ptr\s*\[\s*(?:0x)?([0-9a-fA-F]+)\s*\]',
        re.IGNORECASE,
    )
    call_reg_re = re.compile(r'CALL\s+(E[A-D]X|ESI|EDI|EBP)\s*$', re.IGNORECASE)
    try:
        with open(disasm_path, 'r') as f:
            for line in f:
                line = line.strip()

                # Track MOV REG, [0xADDR] for IAT hoisting
                mv = mov_iat_re.match(line)
                if mv:
                    try:
                        addr = int(mv.group(2), 16)
                        if is_iat_address(addr):
                            reg_map[mv.group(1).upper()] = IAT_ADDRESSES.get(addr, "__import__")
                        else:
                            reg_map.pop(mv.group(1).upper(), None)
                    except ValueError:
                        reg_map.pop(mv.group(1).upper(), None)
                    continue

                if not line.upper().startswith('CALL'):
                    continue

                # Register-indirect call: check hoist table first
                cr = call_reg_re.match(line)
                if cr:
                    reg = cr.group(1).upper()
                    if reg in reg_map:
                        calls.append(reg_map[reg])
                    else:
                        calls.append("__indirect__")
                    continue

                # Vtable/indirect call
                vt = parse_vtable_call(line)
                if vt:
                    calls.append(vt)
                    continue

                # Direct call to address
                m = re.match(r'CALL\s+(?:0x)?0*([0-9a-fA-F]+)\s*$', line)
                if m:
                    addr = int(m.group(1), 16)
                    if addr < 0x1000:
                        calls.append("__import__")
                    else:
                        calls.append(addr)
                    continue

                # Call to label
                m = re.match(r'CALL\s+LAB_0*([0-9a-fA-F]+)', line)
                if m:
                    addr = int(m.group(1), 16)
                    calls.append(addr)
                    continue

                # Other indirect
                calls.append("__indirect__")
    except:
        pass
    return calls


def extract_calls_from_compiled(asm_path, function_name, occurrence_index=0):
    """Extract CALL target names from compiled assembly."""
    calls = []
    try:
        with open(asm_path, 'r') as f:
            lines = f.readlines()
    except:
        return calls

    # Find the Nth function body by exact comment or symbol match. The
    # occurrence index keeps overloaded methods tied to the right PROC body.
    func_lines = []
    current_index = -1
    in_func = False

    for line in lines:
        stripped = line.strip()
        if not in_func:
            if 'PROC' not in stripped:
                continue
            matched = False
            if '; ' in stripped:
                comment = stripped.split('; ', 1)[1].strip()
                # Strip trailing ", COMDAT" added by MSVC asm listing
                if comment.endswith(', COMDAT'):
                    comment = comment[:-len(', COMDAT')].strip()
                matched = comment == function_name
            if not matched:
                symbol = stripped.split(None, 1)[0]
                matched = normalize_compiled(symbol) == function_name
            if not matched:
                continue
            current_index += 1
            if current_index != occurrence_index:
                continue
            in_func = True
            continue

        if re.search(r'\bENDP\b', stripped):
            break
        func_lines.append(stripped)

    if not func_lines:
        return calls

    func_section = '\n'.join(func_lines)

    # Track MOV reg, DWORD PTR __imp__XXX@N hoists so that subsequent `call reg`
    # resolves to the imported symbol (mirrors the original-side tracker).
    # Only the names that round-trip through IAT_ADDRESSES are kept as real
    # symbols; everything else collapses to "__import__" so the unknown-IAT
    # buckets balance against the original side.
    known_imports = set(IAT_ADDRESSES.values())
    compiled_reg_map = {}
    mov_imp_re = re.compile(
        r'mov\s+(eax|ebx|ecx|edx|esi|edi|ebp)\s*,\s*DWORD\s+PTR\s+__imp__([A-Za-z_][\w]*)',
        re.IGNORECASE,
    )

    for line in func_section.split('\n'):
        line = line.strip()

        mv = mov_imp_re.match(line)
        if mv:
            name = normalize_import_name(mv.group(2))
            compiled_reg_map[mv.group(1).lower()] = name if name in known_imports else "__import__"
            continue

        if not line.lower().startswith('call'):
            continue

        full_target = line[4:].strip()

        # Check for vtable call
        vt_m = re.match(r'DWORD\s+PTR\s*\[\s*(\w+)\s*(?:\+\s*(\d+))?\s*\]', full_target, re.IGNORECASE)
        if vt_m:
            offset = int(vt_m.group(2)) if vt_m.group(2) else 0
            calls.append(f"vtable[0x{offset:x}]")
            continue

        # Register calls — consult the hoist table first
        if full_target in ('eax', 'ebx', 'ecx', 'edx', 'esi', 'edi', 'ebp', 'esp'):
            if full_target in compiled_reg_map:
                calls.append(compiled_reg_map[full_target])
            else:
                calls.append("__indirect__")
            continue

        # Label calls
        if full_target.startswith('$L') or full_target.startswith('$T'):
            calls.append("__label__")
            continue

        # Import calls (IAT indirect). Only named-resolve symbols that the
        # original-side tracker also resolves (via IAT_ADDRESSES); for every
        # other import both sides emit the neutral "__import__" token so the
        # counts match regardless of naming.
        if '__imp__' in full_target:
            m = re.search(r'__imp__([A-Za-z_][\w]*)', full_target)
            name = normalize_import_name(m.group(1)) if m else None
            if name and name in set(IAT_ADDRESSES.values()):
                calls.append(name)
            else:
                calls.append("__import__")
            continue

        # Indirect call through a named function pointer global.
        m = re.match(r'DWORD\s+PTR\s+(.+)', full_target, re.IGNORECASE)
        if m:
            calls.append(m.group(1).split(';', 1)[0].strip())
            continue

        # Prefer the decorated symbol when present; MSVC comments erase
        # overload signatures that the decorated name still preserves.
        target_symbol = full_target.split(';', 1)[0].strip()
        if target_symbol.startswith('?') or target_symbol.startswith('@'):
            normalized_symbol = normalize_compiled(target_symbol)
            if normalized_symbol != target_symbol:
                calls.append(normalized_symbol)
                continue

        # Use comment if available
        if ';' in full_target:
            target = full_target.split(';')[1].strip()
        else:
            target = full_target

        calls.append(target)
    return calls


def normalize_compiled(name):
    """Normalize compiled name."""
    name = name.strip()
    m = re.match(r'\?(PopSafe|Pop)@TimedEventPool@@QAEPAV(SC_MessageParser|SpriteAction)@@PAV2@@Z', name)
    if m:
        return f"TimedEventPool::{m.group(1)}({m.group(2)}*)"
    if name.startswith('??0') and '@@' in name:
        m = re.match(r'\?\?0(\w+)@@', name)
        if m: return f"{m.group(1)}::{m.group(1)}"
    if name.startswith('??1') and '@@' in name:
        m = re.match(r'\?\?1(\w+)@@', name)
        if m: return f"{m.group(1)}::~{m.group(1)}"
    if name.startswith('??2@'): return "operator_new"
    if name.startswith('??3@'): return "operator_delete"
    if name.startswith('?') and '@@' in name:
        m = re.match(r'\?(\w+)@(\w+)@@', name)
        if m: return f"{m.group(2)}::{m.group(1)}"
        m = re.match(r'\?(\w+)@@', name)
        if m: return m.group(1)
    if name.startswith('_') and '::' not in name and '@' not in name:
        return name[1:]
    m = re.match(r'@([\w]+)@\d+', name)
    if m: return m.group(1)
    m = re.match(r'_?(\w+)@\d+$', name)
    if m: return m.group(1)
    if 'eh vector constructor iterator' in name:
        return "__eh_vec_ctor__"
    return name


def parse_args():
    parser = argparse.ArgumentParser(
        description="Verify that compiled functions call the same targets as the original disassembly."
    )
    add_config_argument(parser)
    parser.add_argument("filters", nargs="*", help="Optional function name filters (exact or substring).")
    parser.add_argument("--demo", action="store_true", help="Use demo paths from config.")
    parser.add_argument("--all", action="store_true", help="Show all mismatches, including unresolved FUN_ entries.")
    parser.add_argument("--src-dir", default=None, help="Override source directory.")
    parser.add_argument("--code-dir", default=None, help="Override disassembly directory.")
    parser.add_argument("--out-dir", default=None, help="Compiled assembly output directory.")
    parser.add_argument("--exe", default=None, help="Original executable used to resolve IAT call targets.")
    parser.add_argument("--clean-target", default=None, help="Make target used to clean before rebuilding.")
    parser.add_argument("--build-target", default=None, help="Make target used to build assembly.")
    parser.add_argument("--jobs", type=int, default=None, help="Parallel make jobs.")
    parser.add_argument("--no-build", action="store_true", help="Skip rebuilding before verification.")
    return parser.parse_args()


def matches_filter(func_name, cpp_file, filters):
    if not filters:
        return True
    basename = os.path.basename(cpp_file)
    for item in filters:
        if func_name == item:
            return True
        if item in func_name:
            return True
        if item in basename:
            return True
    return False


def has_disassembly_body(disasm_path):
    body_lines = []
    try:
        with open(disasm_path, 'r') as f:
            for line in f:
                line = line.strip()
                if not line or line.startswith(('Function:', 'Address:')):
                    continue
                body_lines.append(line)
    except:
        return False
    if not body_lines:
        return False

    # Some exported disassembly files are truncated at the top of the
    # function. Treat them like missing disassembly rather than reporting
    # every rebuilt call as an EXTRA mismatch.
    for line in body_lines:
        if line.upper().startswith('RET'):
            return True
    return body_lines[-1].upper().startswith('JMP')


def main():
    global IAT_ADDRESSES, IAT_ADDRESS_RANGES

    args = parse_args()

    try:
        config = load_config(args.config)
        configure_call_checker(config)
        paths = mode_paths(config, demo=args.demo)
        mode = "demo" if args.demo else "full"
        src_dir = config_or_arg(args.src_dir, paths, "src_dir", f"paths.{mode}.src_dir")
        code_dir = config_or_arg(args.code_dir, paths, "code_dir", f"paths.{mode}.code_dir")
        out_dir = config_or_arg(args.out_dir, paths, "out_dir", f"paths.{mode}.out_dir")
        exe_path = config_or_arg(args.exe, paths, "exe", f"paths.{mode}.exe")
        clean_target = config_or_arg(args.clean_target, paths, "clean_target", f"paths.{mode}.clean_target")
        build_target = config_or_arg(args.build_target, paths, "build_target", f"paths.{mode}.build_target")
        jobs = args.jobs if args.jobs is not None else int(config.get("build", {}).get("jobs", 1))
    except ConfigError as exc:
        print(f"error: {exc}", file=sys.stderr)
        sys.exit(2)

    show_all = args.all

    if not os.path.isdir(src_dir):
        print(f"error: source directory not found: {src_dir}", file=sys.stderr)
        sys.exit(1)
    if not os.path.isdir(code_dir):
        print(f"error: disassembly directory not found: {code_dir}", file=sys.stderr)
        sys.exit(1)
    if not os.path.isdir(out_dir):
        print(f"error: assembly output directory not found: {out_dir}", file=sys.stderr)
        sys.exit(1)

    try:
        IAT_ADDRESSES, IAT_ADDRESS_RANGES = parse_pe_iat_addresses(exe_path)
    except Exception as exc:
        IAT_ADDRESSES, IAT_ADDRESS_RANGES = {}, []
        print(f"warning: could not load IAT entries from {exe_path}: {exc}", file=sys.stderr)

    if not IAT_ADDRESSES:
        print(f"warning: no IAT entries loaded from {exe_path}; import calls will be compared generically", file=sys.stderr)

    addr_map = build_address_to_name_map(src_dir, code_dir, mode)

    functions = []
    skipped_no_disasm = []
    for cpp_file in sorted(glob.glob(f"{src_dir}/*.cpp")):
        func_occurrences = {}
        for addrs, func_name in iter_source_functions(cpp_file):
            if not matches_filter(func_name, cpp_file, args.filters):
                continue
            occurrence_index = func_occurrences.get(func_name, 0)
            func_occurrences[func_name] = occurrence_index + 1

            # Consecutive Function start comments describe one source body
            # split by compiler prologue/thunk code. Compare the body address.
            addr = addrs[-1]
            addr_hex = f"{addr:X}"
            disasm_path = None
            for p in [f"{code_dir}/FUN_{addr_hex}.disassembled.txt",
                      f"{code_dir}/FUN_{addr_hex.lower()}.disassembled.txt",
                      f"{code_dir}/FUN_00{addr_hex}.disassembled.txt",
                      f"{code_dir}/FUN_00{addr_hex.lower()}.disassembled.txt"]:
                if os.path.exists(p):
                    disasm_path = p
                    break
            if disasm_path is None:
                skipped_no_disasm.append((func_name, addr, cpp_file))
                continue
            if not has_disassembly_body(disasm_path):
                skipped_no_disasm.append((func_name, addr, cpp_file))
                continue

            asm_path = f"{out_dir}/{os.path.splitext(os.path.basename(cpp_file))[0]}.asm"
            functions.append((cpp_file, func_name, occurrence_index, addr, disasm_path, asm_path))

    if not functions:
        print("error: no functions selected for verification.", file=sys.stderr)
        if skipped_no_disasm:
            print(f"note: {len(skipped_no_disasm)} candidate functions were skipped because no usable disassembly was found in {code_dir}.", file=sys.stderr)
        sys.exit(1)

    if not args.no_build:
        print("Building project to generate assembly...")
        subprocess.run(
            ["make", clean_target],
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL,
            check=False,
        )
        build_command = ["make", build_target, "COMPARE=1"]
        if jobs and jobs > 1:
            build_command.append(f"-j{jobs}")
        ret = subprocess.run(
            build_command,
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL,
            check=False,
        )
        if ret.returncode != 0:
            print("Build failed!", file=sys.stderr)
            sys.exit(1)

    total_checked = 0
    mismatched_funcs = []

    for cpp_file, func_name, occurrence_index, orig_addr, disasm_path, asm_path in functions:
        if not os.path.exists(asm_path):
            continue

        orig_raw = extract_calls_from_original(disasm_path)
        compiled_raw = extract_calls_from_compiled(asm_path, func_name, occurrence_index)

        if not orig_raw and not compiled_raw:
            continue
        total_checked += 1

        # Resolve original addresses to names
        orig_resolved = []
        for call in orig_raw:
            if isinstance(call, int):
                name = addr_map.get(call, f"FUN_{call:08X}")
                orig_resolved.append(name)
            else:
                orig_resolved.append(call)

        # Normalize compiled names
        compiled_resolved = [normalize_compiled(n) if not n.startswith('vtable[') and not n.startswith('__') else n
                            for n in compiled_raw]

        # Canonicalize
        orig_canon = [canonicalize(n) for n in orig_resolved]
        compiled_canon = [canonicalize(n) for n in compiled_resolved]

        # Filter out configured non-comparable compiler/runtime artifacts.
        orig_filtered = [n for n in orig_canon if n not in SKIP_CALL_TOKENS]
        compiled_filtered = [n for n in compiled_canon if n not in SKIP_CALL_TOKENS]

        # Normalize funcptr/vtable[0x0] equivalence on both sides:
        # the original sometimes calls through vtable[0x0] (a register loaded
        # with a function pointer) while the rebuild uses a vtable virtual
        # destructor call. Treat both as a generic function-pointer target
        # before comparing counts.
        orig_filtered = ["__funcptr__" if n == "vtable[0x0]" else n for n in orig_filtered]
        compiled_filtered = ["__funcptr__" if n == "vtable[0x0]" else n for n in compiled_filtered]

        # Compare as multisets
        orig_counter = Counter(orig_filtered)
        compiled_counter = Counter(compiled_filtered)

        only_orig = orig_counter - compiled_counter
        only_compiled = compiled_counter - orig_counter
        apply_call_count_allowances(func_name, only_orig, only_compiled)

        if not only_orig and not only_compiled:
            continue

        # Filter trivial differences (new/delete counts from SEH)
        real_orig = {k: v for k, v in only_orig.items() if k not in TRIVIAL_CALL_TOKENS}
        real_compiled = {k: v for k, v in only_compiled.items() if k not in TRIVIAL_CALL_TOKENS}

        if not real_orig and not real_compiled:
            continue

        # Separate FUN_ (unresolved) from resolved-name mismatches
        unresolved_orig = {k: v for k, v in real_orig.items() if k.startswith("FUN_")}
        resolved_orig = {k: v for k, v in real_orig.items() if not k.startswith("FUN_")}

        # If not --all, only show functions with resolved-name mismatches or
        # mismatched EXTRA calls that can't be explained by unresolved FUN_ entries
        if not show_all:
            # Count unresolved originals vs extra compiled — if they balance, skip
            unresolved_count = sum(unresolved_orig.values())
            extra_count = sum(real_compiled.values())
            # If all mismatches are just unresolved FUN_ entries balanced by extras, skip
            if not resolved_orig and unresolved_count >= extra_count:
                continue

        mismatched_funcs.append((func_name, orig_addr, os.path.basename(cpp_file),
                                  dict(real_orig), dict(real_compiled)))

    # Print report
    print(f"\n{'='*70}")
    print(f"CALL TARGET VERIFICATION REPORT")
    if show_all:
        print(f"  (showing ALL mismatches including unresolved FUN_ entries)")
    else:
        print(f"  (showing only resolved-name mismatches; use --all for everything)")
    print(f"{'='*70}")
    print(f"Functions selected: {len(functions)}")
    print(f"Functions checked: {total_checked}")
    print(f"Functions with call target mismatches: {len(mismatched_funcs)}")
    if skipped_no_disasm:
        print(f"Functions skipped (no usable disassembly found): {len(skipped_no_disasm)}")
    print()

    if total_checked == 0:
        print("error: zero functions were actually checked. Verify that assembly output exists and the function filter is correct.", file=sys.stderr)
        sys.exit(1)

    if mismatched_funcs:
        mismatched_funcs.sort(key=lambda x: x[0])
        for func_name, addr, filename, only_orig, only_compiled in mismatched_funcs:
            print(f"{func_name} (0x{addr:X}) [{filename}]")
            for name, count in sorted(only_orig.items()):
                tag = " (unresolved)" if name.startswith("FUN_") else ""
                print(f"  MISSING: {name} x{count}{tag}")
            for name, count in sorted(only_compiled.items()):
                print(f"  EXTRA:   {name} x{count}")
            print()
    else:
        print("All call targets match!")


if __name__ == "__main__":
    main()
