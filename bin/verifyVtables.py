#!/usr/bin/env python3
"""
Verify vtables from the original executable against reimplemented source code.

For every vtable override (function pointer that differs from the parent class),
checks that a matching /* Function start: 0xXXXXXX */ comment exists in src/.
Trivial stubs (RET, return 0) are flagged as low priority.

Usage:
  python3 bin/verifyVtables.py            # Summary table
  python3 bin/verifyVtables.py --dump     # Full vtable dump
  python3 bin/verifyVtables.py --class SC_Timer
"""
import struct
import re
import os
import sys
import glob
import argparse

ORIG_EXE = "data/full/TEACHER.ORI.EXE"
SRC_DIR = "src"
CODE_FULL_DIR = "code-full"

# .rdata address range for the full game binary (TEACHER.ORI.EXE)
RDATA_MIN = 0x461000
RDATA_MAX = 0x468000

# ── PE parsing ──────────────────────────────────────────────────────────────

def parse_pe(filename):
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
        for _ in range(num_sections):
            name = f.read(8).rstrip(b'\x00').decode('ascii', errors='ignore')
            vsize = struct.unpack('<I', f.read(4))[0]
            va = struct.unpack('<I', f.read(4))[0]
            rawsize = struct.unpack('<I', f.read(4))[0]
            rawptr = struct.unpack('<I', f.read(4))[0]
            f.read(12)
            flags = struct.unpack('<I', f.read(4))[0]
            sections[name] = {
                'va_start': image_base + va,
                'va_end': image_base + va + vsize,
                'rawptr': rawptr,
                'rawsize': rawsize,
            }
    return image_base, sections


def read_bytes_at_va(f, sections, va, size):
    """Read raw bytes at a virtual address."""
    for sec in sections.values():
        if sec['va_start'] <= va < sec['va_end']:
            off = va - sec['va_start']
            if off + size > sec['rawsize']:
                return None
            f.seek(sec['rawptr'] + off)
            return f.read(size)
    return None


def read_dword(f, sections, va):
    data = read_bytes_at_va(f, sections, va, 4)
    return struct.unpack('<I', data)[0] if data else None


# ── Stub detection ──────────────────────────────────────────────────────────

def classify_function(f, sections, addr):
    """
    Classify a function as 'stub', 'ret0', 'error', or 'real'.
    Reads first few bytes to detect trivial patterns.
    """
    data = read_bytes_at_va(f, sections, addr, 16)
    if not data:
        return 'unknown'

    # Pure RET
    if data[0] == 0xC3:
        return 'stub'
    # RET imm16
    if data[0] == 0xC2:
        return 'stub'
    # XOR EAX,EAX; RET or RET imm
    if data[0:2] == b'\x33\xc0' and data[2] in (0xC3, 0xC2):
        return 'ret0'
    # MOV EAX,1; RET
    if data[0:5] == b'\xb8\x01\x00\x00\x00' and data[5] in (0xC3, 0xC2):
        return 'ret1'
    # PUSH string_addr; CALL ShowError pattern (error handler)
    if data[0] == 0x68 and data[5] == 0xE8:
        return 'error'

    return 'real'


# ── Vtable address collection ──────────────────────────────────────────────

def find_vtable_addrs_from_disasm(code_full_dir):
    """Find all vtable addresses from MOV patterns in disassembled constructors."""
    pat = re.compile(r'MOV dword ptr \[E[A-Z]+\],0x(46[0-9a-fA-F]{4})', re.IGNORECASE)
    addrs = set()
    for fn in glob.glob(os.path.join(code_full_dir, '*.disassembled.txt')):
        with open(fn, 'r') as fh:
            for line in fh:
                m = pat.search(line)
                if m:
                    addrs.add(int(m.group(1), 16))
    return addrs


def find_vtable_addrs_from_headers(src_dir):
    """Find vtable addresses from header comments (full game only)."""
    pat = re.compile(r'[Vv]table[:\s]+0x([0-9a-fA-F]+)', re.IGNORECASE)
    addrs = set()
    for hfile in glob.glob(os.path.join(src_dir, '*.h')):
        with open(hfile, 'r') as f:
            for line in f:
                m = pat.search(line)
                if m:
                    addr = int(m.group(1), 16)
                    if RDATA_MIN <= addr < RDATA_MAX:
                        addrs.add(addr)
    return addrs


# ── Source parsing ──────────────────────────────────────────────────────────

def parse_class_info(src_dir, code_full_dir):
    """
    Parse .h files for class hierarchy and vtable addresses.
    Uses constructor disassembly to fix incorrect/demo vtable addresses.
    """
    classes = {}

    vtable_pat = re.compile(r'[Vv]table[:\s]+0x([0-9a-fA-F]+)', re.IGNORECASE)
    class_pat = re.compile(r'class\s+(\w+)\s*:\s*public\s+(\w+)')
    ctor_pat = re.compile(r'Constructor[:\s]+0x([0-9a-fA-F]+)', re.IGNORECASE)
    mov_pat = re.compile(r'MOV dword ptr \[E[A-Z]+\],0x(46[0-9a-fA-F]{4})', re.IGNORECASE)

    for hfile in sorted(glob.glob(os.path.join(src_dir, '*.h'))):
        basename = os.path.basename(hfile)
        with open(hfile, 'r') as f:
            content = f.read()

        for m in class_pat.finditer(content):
            cls_name = m.group(1)
            parent_name = m.group(2)
            if cls_name in classes:
                continue

            pre_class = content[:m.start()]
            search_area = pre_class[-1500:] if len(pre_class) > 1500 else pre_class

            # Find vtable address from comments
            vtable_addr = None
            for vm in vtable_pat.finditer(search_area):
                addr = int(vm.group(1), 16)
                if RDATA_MIN <= addr < RDATA_MAX:
                    vtable_addr = addr

            # Find constructor address
            ctor_addr = None
            for cm in ctor_pat.finditer(search_area):
                ctor_addr = int(cm.group(1), 16)

            # Use constructor to find/verify vtable address
            if ctor_addr:
                ctor_file = os.path.join(code_full_dir, f"FUN_{ctor_addr:X}.disassembled.txt")
                if os.path.exists(ctor_file):
                    with open(ctor_file, 'r') as cf:
                        last_vtable = None
                        for line in cf:
                            mm = mov_pat.search(line)
                            if mm:
                                last_vtable = int(mm.group(1), 16)
                        if last_vtable:
                            vtable_addr = last_vtable

            if vtable_addr:
                classes[cls_name] = {
                    'vtable_addr': vtable_addr,
                    'parent': parent_name,
                    'header': basename,
                    'constructor': ctor_addr,
                }

    # ── Manual entries for classes not auto-detected ──
    # These either lack "class X : public Y" syntax, have no vtable comment,
    # or are only identifiable from constructor/destructor disassembly.
    manual = [
        # Root classes
        ('Parser',            0x461298, None,           'Parser.h',           0x4127C0),
        ('Handler',           0x461098, 'Parser',       'Handler.h',          None),
        # Classes whose headers lack full-game vtable comments
        ('RenderEntry',       0x46102C, None,           'RenderEntry.h',      0x414710),
        ('SoundCommand',      0x461030, None,           'SoundCommand.h',     None),
        ('Animation',         0x461370, None,           'Animation.h',        0x41A9D0),
        ('HotspotAction',     0x461378, 'Parser',       'HotspotAction.h',    0x41B320),
        ('QuestionInit',      0x461418, 'Parser',       'QuestionInit.h',     0x422880),
        ('GameLoop',          0x461458, 'Parser',       'GameLoop.h',         None),
        ('GameState',         0x461468, 'Parser',       'GameState.h',        0x4333D0),
        ('MouseControl',      0x4616B0, 'Parser',       'MouseControl.h',     0x4327C0),
        ('CDData',            0x4616C8, 'Parser',       'CDData.h',           0x432EC0),
        ('TargetList',        0x461960, 'Parser',       'Target.h',           0x4432F0),
        ('MMPlayer',          0x461970, 'Parser',       'MMPlayer.h',         0x4438A0),
        ('SoundEntry',        0x461A58, None,           'OnScreenMessage.h',  0x447FF0),
        ('Sprite',            0x461C10, 'Parser',       'Sprite.h',           0x44C660),
        # ZBufferManager render command subclasses (SoundCommand-derived, created in ZBufMgr methods)
        ('RenderCmd_DrawVBuf',  0x461038, 'SoundCommand', 'ZBufferManager.h', None),
        ('RenderCmd_DrawText',  0x461040, 'SoundCommand', 'ZBufferManager.h', None),
        ('RenderCmd_DrawRect',  0x461048, 'SoundCommand', 'ZBufferManager.h', None),
        # Handler6 (not instantiated in full game, COMDAT constructor)
        ('Handler6',          0x4616B0, 'Handler',      'Handler6.h',         0x4327C0),
        # Combat crosshair weapon (used by SC_FireAlarm, SC_Slime, SC_Fan, SC_Wahoo)
        ('CombatWeapon',      0x461118, 'Weapon',       '(unknown)',          None),
        # SC_SpaceShipNav's internal combat engine
        ('SpaceShipEngine',   0x461A10, 'SC_CombatBase','(unknown)',          None),
        # mCNavNode sub-vtables (parsing and runtime variants, set in LBLParse 0x44AF40)
        ('mCNavNode_Runtime',     0x461B30, 'Parser',   'mCNavNode.h',        0x44A900),
        ('mCNavNode_TypeA',       0x461B60, 'Parser',   'mCNavNode.h',        None),
        ('mCNavNode_TypeB',       0x461B80, 'Parser',   'mCNavNode.h',        None),
        ('mCNavNode_TypeC',       0x461BA0, 'Parser',   'mCNavNode.h',        None),
        ('mCNavNode_TypeD',       0x461BC0, 'Parser',   'mCNavNode.h',        None),
        ('mCNavNode_TypeE',       0x461BE0, 'Parser',   'mCNavNode.h',        None),
        # NavSubNode base (parsing-time vtable, overwritten by OnDir/BG constructors)
        ('NavSubNode_Base',   0x461AC8, 'Parser',       'NavSubNode.h',       0x449BD0),
    ]

    for name, vtaddr, parent, header, ctor in manual:
        if name not in classes:
            classes[name] = {
                'vtable_addr': vtaddr,
                'parent': parent,
                'header': header,
                'constructor': ctor,
            }

    # MouseControl and Handler6 share 0x4327C0 (COMDAT) — keep MouseControl
    if 'Handler6' in classes and 'MouseControl' in classes:
        if classes['Handler6']['vtable_addr'] == classes['MouseControl']['vtable_addr']:
            del classes['Handler6']

    return classes


def find_implemented_functions(src_dir):
    """
    Scan .cpp and .h files for /* Function start: 0xXXXXXX */ comments.
    Returns dict: address -> list of (file, line_number)
    """
    func_pat = re.compile(r'/\*\s*Function start:\s*0x([0-9a-fA-F]+)')
    implementations = {}

    for pattern in ['*.cpp', '*.h']:
        for srcfile in sorted(glob.glob(os.path.join(src_dir, pattern))):
            basename = os.path.basename(srcfile)
            with open(srcfile, 'r') as f:
                for lineno, line in enumerate(f, 1):
                    m = func_pat.search(line)
                    if m:
                        addr = int(m.group(1), 16)
                        if addr not in implementations:
                            implementations[addr] = []
                        implementations[addr].append((basename, lineno))

    return implementations


# ── Main ────────────────────────────────────────────────────────────────────

def read_vtable_entries(f, sections, vtable_addr, code_start, code_end, max_addr):
    """Read vtable entries, stopping at non-code or next vtable boundary."""
    entries = []
    addr = vtable_addr
    while addr < max_addr:
        val = read_dword(f, sections, addr)
        if val is None or not (code_start <= val < code_end):
            break
        entries.append(val)
        addr += 4
    return entries


def main():
    ap = argparse.ArgumentParser(description='Verify vtables against source')
    ap.add_argument('--dump', action='store_true', help='Show full vtable dump')
    ap.add_argument('--class', dest='filter_class', help='Filter to specific class')
    args = ap.parse_args()

    if not os.path.exists(ORIG_EXE):
        print(f"Error: {ORIG_EXE} not found")
        sys.exit(1)

    _, sections = parse_pe(ORIG_EXE)
    text = sections['.text']
    code_start, code_end = text['va_start'], text['va_end']

    # Collect all vtable boundary addresses
    disasm_addrs = find_vtable_addrs_from_disasm(CODE_FULL_DIR)
    hdr_addrs = find_vtable_addrs_from_headers(SRC_DIR)
    all_vtable_addrs = sorted(disasm_addrs | hdr_addrs)

    # Parse classes and implementations
    classes = parse_class_info(SRC_DIR, CODE_FULL_DIR)
    implementations = find_implemented_functions(SRC_DIR)

    print(f"Binary:  {ORIG_EXE}")
    print(f"Code:    0x{code_start:08X}..0x{code_end:08X}")
    print(f"Vtables: {len(all_vtable_addrs)} unique addresses")
    print(f"Classes: {len(classes)} with vtable info")
    print(f"Impls:   {len(implementations)} Function start comments")
    print()

    # Read vtables from binary
    vtables = {}
    func_types = {}  # cache: addr -> stub/ret0/real/...

    with open(ORIG_EXE, 'rb') as f:
        for cls_name, info in classes.items():
            addr = info['vtable_addr']
            idx = all_vtable_addrs.index(addr) if addr in all_vtable_addrs else -1
            max_addr = all_vtable_addrs[idx + 1] if (idx >= 0 and idx + 1 < len(all_vtable_addrs)) else addr + 0x100
            vtables[cls_name] = read_vtable_entries(f, sections, addr, code_start, code_end, max_addr)

        # Classify all functions referenced in vtables
        all_funcs = set()
        for entries in vtables.values():
            all_funcs.update(entries)
        for func_addr in all_funcs:
            func_types[func_addr] = classify_function(f, sections, func_addr)

    # Resolve parent vtable entries up the chain
    def get_parent_entries(cls_name, depth=0):
        if depth > 10:
            return []
        info = classes.get(cls_name)
        if not info or not info['parent']:
            return []
        parent = info['parent']
        if parent in vtables:
            return vtables[parent]
        return get_parent_entries(parent, depth + 1)

    # ── Verify ──
    total_slots = 0
    total_inherited = 0
    total_overrides = 0
    total_implemented = 0
    total_sdtors = 0
    total_stubs = 0
    total_missing_real = 0
    missing_real = []
    missing_stubs = []

    # Slot name mapping for Handler-derived classes
    HANDLER_SLOTS = {
        0: 'LBLParse', 1: 'OnProcessStart', 2: 'OnProcessEnd', 3: '~sdtor',
        4: 'Init', 5: 'AddMessage', 6: 'ShutDown', 7: 'Update',
        8: 'Exit', 9: 'Serialize', 10: 'OnInput',
    }
    PARSER_SLOTS = {
        0: 'LBLParse', 1: 'OnProcessStart', 2: 'OnProcessEnd', 3: '~sdtor',
    }

    sorted_classes = sorted(
        [c for c in vtables if not args.filter_class or c == args.filter_class],
        key=lambda c: classes[c]['vtable_addr']
    )

    if not args.dump:
        print("=" * 115)
        print(f"{'Class':<22} {'Vtable':<14} {'Parent':<18} {'#':<4} {'Inh':<4} {'Ovr':<4} {'OK':<4} {'Stub':<5} {'Miss':<5} {'Status'}")
        print("=" * 115)

    for cls_name in sorted_classes:
        info = classes[cls_name]
        entries = vtables[cls_name]
        parent_entries = get_parent_entries(cls_name)

        # Determine slot names based on hierarchy
        slot_names = {}
        parent_chain = []
        cur = cls_name
        while cur and cur in classes:
            parent_chain.append(cur)
            cur = classes[cur].get('parent')
        if 'Handler' in parent_chain or cls_name == 'Handler':
            slot_names = HANDLER_SLOTS
        elif 'Parser' in parent_chain or cls_name == 'Parser':
            slot_names = PARSER_SLOTS

        inherited = 0
        overrides = []
        for i, func_addr in enumerate(entries):
            if i < len(parent_entries) and parent_entries[i] == func_addr:
                inherited += 1
            else:
                overrides.append((i, func_addr))

        # Classify each override
        n_impl = 0
        n_sdtor = 0
        n_stub_missing = 0
        n_real_missing = 0
        cls_missing_real = []
        cls_missing_stubs = []

        for slot_idx, func_addr in overrides:
            is_impl = func_addr in implementations
            ftype = func_types.get(func_addr, 'unknown')

            if is_impl:
                n_impl += 1
            elif slot_idx == 3:
                n_sdtor += 1  # sdtor, skip
            elif ftype in ('stub', 'ret0', 'ret1'):
                n_stub_missing += 1
                cls_missing_stubs.append((slot_idx, func_addr, ftype))
            else:
                n_real_missing += 1
                cls_missing_real.append((slot_idx, func_addr, ftype))

        total_slots += len(entries)
        total_inherited += inherited
        total_overrides += len(overrides)
        total_implemented += n_impl
        total_sdtors += n_sdtor
        total_stubs += n_stub_missing
        total_missing_real += n_real_missing

        if args.dump:
            parent_str = info['parent'] or '(root)'
            print(f"\n{cls_name} (0x{info['vtable_addr']:08X}, parent={parent_str}, {info['header']}):")
            for i, func_addr in enumerate(entries):
                is_inherited = (i < len(parent_entries) and parent_entries[i] == func_addr)
                is_impl = func_addr in implementations
                ftype = func_types.get(func_addr, '?')
                slot_name = slot_names.get(i, '')

                marker = " " if is_inherited else "*"
                if is_inherited:
                    status = f"inherited ({parent_str})"
                elif is_impl:
                    locs = implementations[func_addr]
                    status = f"OK  <- {', '.join(f'{fl}:{ln}' for fl, ln in locs)}"
                elif i == 3:
                    status = "sdtor (compiler-generated)"
                elif ftype in ('stub', 'ret0', 'ret1'):
                    tag = {'stub': 'RET', 'ret0': 'return 0', 'ret1': 'return 1'}[ftype]
                    status = f"\033[33mMISSING ({tag})\033[0m"
                elif ftype == 'error':
                    status = f"\033[33mMISSING (ShowError call)\033[0m"
                else:
                    status = f"\033[31mMISSING\033[0m"

                name_str = f" {slot_name}" if slot_name else ""
                print(f"  {marker} [{i:2d}]{name_str:<18} 0x{func_addr:08X}  {status}")
        else:
            n_miss_total = n_real_missing + n_stub_missing
            if n_real_missing > 0:
                status_str = f"\033[31m{n_real_missing} MISSING\033[0m"
                if n_stub_missing > 0:
                    status_str += f" +{n_stub_missing} stubs"
            elif n_stub_missing > 0:
                status_str = f"\033[33m{n_stub_missing} stubs only\033[0m"
            else:
                status_str = "\033[32mOK\033[0m"

            parent_str = info['parent'] or '(root)'
            print(f"{cls_name:<22} 0x{info['vtable_addr']:08X}   {parent_str:<18} "
                  f"{len(entries):<4} {inherited:<4} {len(overrides):<4} {n_impl:<4} "
                  f"{n_stub_missing:<5} {n_real_missing:<5} {status_str}")

            for slot_idx, func_addr, ftype in cls_missing_real:
                slot_name = slot_names.get(slot_idx, '')
                disasm = os.path.exists(f"{CODE_FULL_DIR}/FUN_{func_addr:X}.disassembled.txt")
                note = " (disasm)" if disasm else ""
                name_str = f" {slot_name}" if slot_name else ""
                missing_real.append((cls_name, slot_idx, func_addr, info['header'], slot_name))
                print(f"    [{slot_idx:2d}]{name_str:<16} 0x{func_addr:08X}  [{ftype}]{note}")

            for slot_idx, func_addr, ftype in cls_missing_stubs:
                slot_name = slot_names.get(slot_idx, '')
                tag = {'stub': 'RET', 'ret0': 'return 0', 'ret1': 'return 1'}.get(ftype, ftype)
                name_str = f" {slot_name}" if slot_name else ""
                missing_stubs.append((cls_name, slot_idx, func_addr, info['header'], slot_name, tag))
                print(f"    [{slot_idx:2d}]{name_str:<16} 0x{func_addr:08X}  \033[2m({tag})\033[0m")

    if not args.dump:
        print("=" * 115)

    print()
    print(f"{'Total vtable slots:':<30} {total_slots}")
    print(f"{'  Inherited:':<30} {total_inherited}")
    print(f"{'  Overrides:':<30} {total_overrides}")
    print(f"{'    Implemented:':<30} {total_implemented}")
    print(f"{'    Sdtors (compiler):':<30} {total_sdtors}")
    print(f"{'    Stubs (RET/ret 0):':<30} \033[33m{total_stubs}\033[0m")
    print(f"{'    Missing (real code):':<30} \033[31m{total_missing_real}\033[0m")

    # Unmatched vtable addresses
    known_addrs = set(info['vtable_addr'] for info in classes.values())
    unmatched = sorted(set(all_vtable_addrs) - known_addrs)
    if unmatched:
        print(f"\nVtable addresses not matched to any class ({len(unmatched)}):")
        for addr in unmatched:
            print(f"  0x{addr:08X}")


if __name__ == '__main__':
    main()
