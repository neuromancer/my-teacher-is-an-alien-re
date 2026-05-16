#!/usr/bin/env python3
"""
Verify vtables using Capstone-backed disassembly.

This is a binary-backed companion to verifyVtables.py. It keeps the original
script intact, but replaces text-assembly parsing for constructor vtable writes
and constructor hierarchy evidence with Capstone over the original executable.
Function starts from code-full/FUN_*.disassembled.txt are used only as boundary
markers.
"""
import argparse
import glob
import os
import re
import sys
from dataclasses import dataclass

try:
    from capstone import Cs, CS_ARCH_X86, CS_MODE_32
    from capstone.x86_const import X86_OP_IMM, X86_OP_MEM, X86_OP_REG
except ImportError as exc:
    raise SystemExit("error: capstone is required. Install it with: python3 -m pip install capstone") from exc

import verifyVtables as legacy
from projectConfig import (
    ConfigError,
    add_config_argument,
    config_or_arg,
    load_config,
    mode_paths,
    parse_int,
    parse_range,
)


DEFAULT_MAX_FUNCTION_BYTES = 0x4000


@dataclass(frozen=True)
class CapstoneVtableConfig:
    max_function_bytes: int
    stack_registers: frozenset
    object_write_displacements: frozenset


@dataclass(frozen=True)
class ConstructorEvidence:
    instruction_count: int
    this_reg: str
    per_reg_vtables: dict
    calls: frozenset
    vtable_writes: frozenset


def unsigned32(value):
    return value & 0xFFFFFFFF


def parse_config_int(value, label, default=None):
    if value in (None, ""):
        if default is not None:
            return default
        raise ConfigError(f"missing required configuration value: {label}")
    if isinstance(value, int):
        return value
    if isinstance(value, str):
        try:
            return int(value, 0)
        except ValueError:
            raise ConfigError(f"{label} must be an integer or integer string") from None
    raise ConfigError(f"{label} must be an integer or integer string")


def parse_config_int_list(value, label, default):
    if value is None:
        value = default
    if not isinstance(value, list):
        raise ConfigError(f"{label} must be a list")
    return frozenset(parse_config_int(item, f"{label}[]") for item in value)


def parse_string_list(value, label, default):
    if value is None:
        value = default
    if not isinstance(value, list) or any(not isinstance(item, str) for item in value):
        raise ConfigError(f"{label} must be a list of strings")
    return frozenset(item.lower() for item in value)


def load_capstone_vtable_config(config):
    section = config.get("vtables_capstone", {})
    if section is None:
        section = {}
    if not isinstance(section, dict):
        raise ConfigError("vtables_capstone must be an object")
    return CapstoneVtableConfig(
        max_function_bytes=parse_config_int(
            section.get("max_function_bytes"),
            "vtables_capstone.max_function_bytes",
            DEFAULT_MAX_FUNCTION_BYTES,
        ),
        stack_registers=parse_string_list(
            section.get("stack_registers"),
            "vtables_capstone.stack_registers",
            ["esp", "ebp"],
        ),
        object_write_displacements=parse_config_int_list(
            section.get("object_write_displacements"),
            "vtables_capstone.object_write_displacements",
            [0],
        ),
    )


def load_function_starts(code_dir, code_start, code_end):
    starts = set()
    if not code_dir or not os.path.isdir(code_dir):
        return []
    for path in glob.glob(os.path.join(code_dir, "FUN_*.disassembled.txt")):
        match = re.search(r"FUN_([0-9A-Fa-f]+)\.disassembled\.txt$", os.path.basename(path))
        if not match:
            continue
        addr = int(match.group(1), 16)
        if code_start <= addr < code_end:
            starts.add(addr)
    return sorted(starts)


def next_function_boundary(starts, start, code_end):
    for candidate in starts:
        if candidate > start:
            return candidate
    return code_end


def function_bytes(f, sections, starts, start, code_end, max_function_bytes):
    end = min(next_function_boundary(starts, start, code_end), code_end, start + max_function_bytes)
    if end <= start:
        return None
    return legacy.read_bytes_at_va(f, sections, start, end - start)


def disassemble_function(f, sections, starts, start, code_end, max_function_bytes, md):
    data = function_bytes(f, sections, starts, start, code_end, max_function_bytes)
    if not data:
        return []
    return list(md.disasm(data, start))


def is_rdata_address(value, rdata_min, rdata_max):
    return rdata_min <= unsigned32(value) < rdata_max


def operand_reg_name(insn, operand):
    return insn.reg_name(operand.reg) if operand.type == X86_OP_REG else ""


def operand_mem_base_name(insn, operand):
    if operand.type != X86_OP_MEM or not operand.mem.base:
        return ""
    return insn.reg_name(operand.mem.base)


def is_object_memory_operand(insn, operand, policy):
    if operand.type != X86_OP_MEM:
        return False
    base = operand_mem_base_name(insn, operand)
    if not base or base in policy.stack_registers:
        return False
    if operand.mem.index:
        return False
    return operand.mem.disp in policy.object_write_displacements


def vtable_store(insn, rdata_min, rdata_max, policy):
    if insn.mnemonic != "mov":
        return None
    operands = list(insn.operands)
    if len(operands) < 2:
        return None
    left, right = operands[0], operands[1]
    if left.type != X86_OP_MEM or right.type != X86_OP_IMM:
        return None
    imm = unsigned32(right.imm)
    if not is_rdata_address(imm, rdata_min, rdata_max):
        return None
    if not is_object_memory_operand(insn, left, policy):
        return None
    return operand_mem_base_name(insn, left), imm


def find_vtable_addrs_from_capstone(exe_path, sections, starts, code_start, code_end, rdata_min, rdata_max, policy):
    addrs = set()
    md = Cs(CS_ARCH_X86, CS_MODE_32)
    md.detail = True
    with open(exe_path, "rb") as f:
        for start in starts:
            for insn in disassemble_function(f, sections, starts, start, code_end, policy.max_function_bytes, md):
                hit = vtable_store(insn, rdata_min, rdata_max, policy)
                if hit is not None:
                    _, addr = hit
                    addrs.add(addr)
    return addrs


def analyze_constructor(f, sections, starts, ctor_addr, code_end, rdata_min, rdata_max, policy, md):
    instrs = disassemble_function(f, sections, starts, ctor_addr, code_end, policy.max_function_bytes, md)
    if not instrs:
        return None

    this_reg = None
    per_reg_vtables = {}
    calls = set()
    vtable_writes = set()

    for insn in instrs:
        operands = list(insn.operands)
        if insn.mnemonic == "mov" and len(operands) >= 2:
            if this_reg is None and operands[0].type == X86_OP_REG and operands[1].type == X86_OP_REG:
                dst = operand_reg_name(insn, operands[0])
                src = operand_reg_name(insn, operands[1])
                if src == "ecx":
                    this_reg = dst

            hit = vtable_store(insn, rdata_min, rdata_max, policy)
            if hit is not None:
                reg, addr = hit
                per_reg_vtables[reg] = addr
                vtable_writes.add(addr)

        if insn.mnemonic == "call" and operands and operands[0].type == X86_OP_IMM:
            calls.add(unsigned32(operands[0].imm))

    return ConstructorEvidence(
        instruction_count=len(instrs),
        this_reg=this_reg,
        per_reg_vtables=per_reg_vtables,
        calls=frozenset(calls),
        vtable_writes=frozenset(vtable_writes),
    )


def selected_constructor_vtable(evidence):
    if evidence is None:
        return None
    if evidence.this_reg:
        selected = evidence.per_reg_vtables.get(evidence.this_reg)
        if selected is not None:
            return selected
    if evidence.per_reg_vtables:
        return next(reversed(evidence.per_reg_vtables.values()))
    return None


def parse_class_info_capstone(src_dir, exe_path, sections, starts, code_end, rdata_min, rdata_max, policy):
    classes = {}
    hierarchy, class_header, _, explicit_vtables, constructors = legacy.parse_header_metadata(src_dir)
    md = Cs(CS_ARCH_X86, CS_MODE_32)
    md.detail = True

    with open(exe_path, "rb") as f:
        for cls_name, parent_name in hierarchy.items():
            if cls_name in classes:
                continue

            vtable_addr = explicit_vtables.get(cls_name)
            if vtable_addr is not None and not is_rdata_address(vtable_addr, rdata_min, rdata_max):
                vtable_addr = None

            ctor_addr = constructors.get(cls_name)
            if ctor_addr:
                evidence = analyze_constructor(
                    f,
                    sections,
                    starts,
                    ctor_addr,
                    code_end,
                    rdata_min,
                    rdata_max,
                    policy,
                    md,
                )
                chosen_vtable = selected_constructor_vtable(evidence)
                if chosen_vtable:
                    vtable_addr = chosen_vtable

            if vtable_addr:
                classes[cls_name] = {
                    "vtable_addr": vtable_addr,
                    "parent": parent_name,
                    "header": class_header.get(cls_name, "(unknown)"),
                    "constructor": ctor_addr,
                }

    for item in legacy.VTABLE_MANUAL_CLASSES:
        name = item["name"]
        if name not in classes:
            classes[name] = {
                "vtable_addr": parse_int(item["vtable_addr"]),
                "parent": item.get("parent"),
                "header": item.get("header", "(unknown)"),
                "constructor": parse_int(item.get("constructor")),
            }

    for item in legacy.VTABLE_DUPLICATE_CLASS_DROPS:
        drop = item.get("drop")
        keep = item.get("keep")
        if drop in classes and keep in classes:
            if not item.get("when_same_vtable") or classes[drop]["vtable_addr"] == classes[keep]["vtable_addr"]:
                del classes[drop]

    for class_name, override in legacy.VTABLE_CLASS_OVERRIDES.items():
        if class_name in classes:
            if "vtable_addr" in override:
                classes[class_name]["vtable_addr"] = parse_int(override["vtable_addr"])
            if "parent" in override:
                classes[class_name]["parent"] = override["parent"]
            if "header" in override:
                classes[class_name]["header"] = override["header"]

    return classes


def find_constructor_parent_warnings_capstone(
        classes, exe_path, sections, starts, code_end, rdata_min, rdata_max, policy, filter_class=None):
    warnings = []
    stats = {
        "checked": 0,
        "missing_constructor": 0,
        "missing_parent_evidence": 0,
        "missing_disassembly": 0,
        "empty_disassembly": 0,
    }
    md = Cs(CS_ARCH_X86, CS_MODE_32)
    md.detail = True

    with open(exe_path, "rb") as f:
        for cls_name, info in sorted(classes.items()):
            if filter_class and cls_name != filter_class:
                continue

            parent_name = info.get("parent")
            ctor_addr = info.get("constructor")
            if not parent_name:
                continue
            if not ctor_addr:
                stats["missing_constructor"] += 1
                continue

            parent_info = classes.get(parent_name)
            if not parent_info:
                continue

            parent_ctor = parent_info.get("constructor")
            parent_vtable = parent_info.get("vtable_addr")
            if not parent_ctor and not parent_vtable:
                stats["missing_parent_evidence"] += 1
                continue

            evidence = analyze_constructor(
                f,
                sections,
                starts,
                ctor_addr,
                code_end,
                rdata_min,
                rdata_max,
                policy,
                md,
            )
            if evidence is None:
                stats["missing_disassembly"] += 1
                continue
            if evidence.instruction_count == 0:
                stats["empty_disassembly"] += 1
                continue

            stats["checked"] += 1
            if parent_ctor and parent_ctor in evidence.calls:
                continue
            if parent_vtable and parent_vtable in evidence.vtable_writes:
                continue

            warnings.append({
                "class_name": cls_name,
                "parent_name": parent_name,
                "constructor": ctor_addr,
                "parent_constructor": parent_ctor,
                "parent_vtable": parent_vtable,
            })

    return warnings, stats


def first_non_nop(instrs):
    for instr in instrs:
        if instr.mnemonic not in {"nop", "int3"}:
            return instr
    return None


def next_non_nop(instrs, idx):
    for instr in instrs[idx + 1:]:
        if instr.mnemonic not in {"nop", "int3"}:
            return instr
    return None


def is_ret(instr):
    return instr is not None and instr.mnemonic == "ret"


def is_eax_zeroing(instr):
    if instr is None or instr.mnemonic != "xor":
        return False
    operands = list(instr.operands)
    if len(operands) < 2 or operands[0].type != X86_OP_REG or operands[1].type != X86_OP_REG:
        return False
    left = operand_reg_name(instr, operands[0])
    right = operand_reg_name(instr, operands[1])
    return left == right and left in {"eax", "ax", "al"}


def mov_return_constant(instr):
    if instr is None or instr.mnemonic != "mov":
        return None
    operands = list(instr.operands)
    if len(operands) < 2 or operands[0].type != X86_OP_REG or operands[1].type != X86_OP_IMM:
        return None
    reg = operand_reg_name(instr, operands[0])
    if reg not in {"eax", "ax", "al"}:
        return None
    return unsigned32(operands[1].imm)


def is_field_getter(instr):
    if instr is None or instr.mnemonic not in {"mov", "movzx", "movsx"}:
        return False
    operands = list(instr.operands)
    if len(operands) < 2 or operands[0].type != X86_OP_REG or operands[1].type != X86_OP_MEM:
        return False
    dst = operand_reg_name(instr, operands[0])
    base = operand_mem_base_name(instr, operands[1])
    return dst in {"eax", "ax", "al"} and base == "ecx"


def classify_function_capstone(f, sections, starts, addr, code_end, policy):
    md = Cs(CS_ARCH_X86, CS_MODE_32)
    md.detail = True
    instrs = disassemble_function(f, sections, starts, addr, code_end, 32, md)
    if not instrs:
        return "unknown"

    first = first_non_nop(instrs)
    if first is None:
        return "unknown"
    first_idx = instrs.index(first)
    second = next_non_nop(instrs, first_idx)

    if first.mnemonic == "jmp":
        operands = list(first.operands)
        if operands and operands[0].type == X86_OP_MEM:
            return "thunk"
    if is_ret(first):
        return "stub"
    if is_eax_zeroing(first) and is_ret(second):
        return "ret0"

    ret_constant = mov_return_constant(first)
    if ret_constant is not None and is_ret(second):
        return "ret1" if ret_constant == 1 else "retconst"
    if is_field_getter(first) and is_ret(second):
        return "fieldget"

    if first.mnemonic == "push" and second is not None and second.mnemonic == "call":
        return "error"

    return "real"


def format_symbol_list(symbols):
    if not symbols:
        return "(unresolved)"
    return ", ".join(f"{sym['class_name']}::{sym['method_name']}" for sym in symbols)


def main():
    ap = argparse.ArgumentParser(description="Verify vtables against source using Capstone for binary disassembly.")
    add_config_argument(ap)
    ap.add_argument("--exe", help="Original executable to read vtables from")
    ap.add_argument("--src-dir", help="Source directory")
    ap.add_argument("--code-dir", help="Function-boundary directory with FUN_*.disassembled.txt filenames")
    ap.add_argument("--rdata-min", type=lambda value: int(value, 0), help="Fallback .rdata start address")
    ap.add_argument("--rdata-max", type=lambda value: int(value, 0), help="Fallback .rdata end address")
    ap.add_argument("--dump", action="store_true", help="Show full vtable dump")
    ap.add_argument("--class", dest="filter_class", help="Filter to specific class")
    args = ap.parse_args()

    try:
        config = load_config(args.config)
        legacy.configure_vtable_checker(config)
        capstone_config = load_capstone_vtable_config(config)
        paths = mode_paths(config, demo=False)
        vtables_config = config.get("vtables", {})
        exe_path = config_or_arg(args.exe, paths, "exe", "paths.full.exe")
        src_dir = config_or_arg(args.src_dir, paths, "src_dir", "paths.full.src_dir")
        code_dir = config_or_arg(args.code_dir, paths, "code_dir", "paths.full.code_dir")
        configured_rdata_min, configured_rdata_max = parse_range(vtables_config.get("rdata_range"), "vtables.rdata_range")
        fallback_rdata_min = args.rdata_min if args.rdata_min is not None else configured_rdata_min
        fallback_rdata_max = args.rdata_max if args.rdata_max is not None else configured_rdata_max
    except ConfigError as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 2

    if not os.path.exists(exe_path):
        print(f"Error: {exe_path} not found")
        return 1

    _, sections = legacy.parse_pe(exe_path)
    text = sections[".text"]
    rdata = sections.get(".rdata")
    code_start, code_end = text["va_start"], text["va_end"]
    rdata_min = rdata["va_start"] if rdata else fallback_rdata_min
    rdata_max = rdata["va_end"] if rdata else fallback_rdata_max
    function_starts = load_function_starts(code_dir, code_start, code_end)
    if not function_starts:
        print(f"error: no function boundary markers found in {code_dir}", file=sys.stderr)
        return 2

    capstone_addrs = find_vtable_addrs_from_capstone(
        exe_path,
        sections,
        function_starts,
        code_start,
        code_end,
        rdata_min,
        rdata_max,
        capstone_config,
    )
    hdr_addrs = legacy.find_vtable_addrs_from_headers(src_dir, rdata_min, rdata_max)
    all_vtable_addrs = sorted(capstone_addrs | hdr_addrs)

    classes = parse_class_info_capstone(
        src_dir,
        exe_path,
        sections,
        function_starts,
        code_end,
        rdata_min,
        rdata_max,
        capstone_config,
    )
    invalid_parents = sorted(
        (cls_name, info["parent"])
        for cls_name, info in classes.items()
        if info.get("parent") and info["parent"] not in classes
    )
    constructor_parent_warnings, constructor_parent_stats = find_constructor_parent_warnings_capstone(
        classes,
        exe_path,
        sections,
        function_starts,
        code_end,
        rdata_min,
        rdata_max,
        capstone_config,
        args.filter_class,
    )
    implementations = legacy.find_implemented_functions(src_dir)
    function_symbols = legacy.find_source_function_symbols(src_dir)
    _, _, class_methods, _ = legacy.parse_header_classes(src_dir)
    expected_slots = {}
    for cls_name in classes:
        legacy.build_expected_vtable_slots(cls_name, classes, class_methods, expected_slots)

    print(f"Binary:  {exe_path}")
    print(f"Code:    0x{code_start:08X}..0x{code_end:08X}")
    print(f"Rdata:   0x{rdata_min:08X}..0x{rdata_max:08X}")
    print(f"Vtables: {len(all_vtable_addrs)} unique addresses")
    print(f"Classes: {len(classes)} with vtable info")
    print(f"Parents: {len(invalid_parents)} invalid references")
    print(f"Ctor hierarchy: {constructor_parent_stats['checked']} checked, {len(constructor_parent_warnings)} warnings")
    skipped_ctor_checks = sum(
        count for key, count in constructor_parent_stats.items()
        if key != "checked"
    )
    if skipped_ctor_checks:
        print(f"Ctor hierarchy skipped: {skipped_ctor_checks} without constructor evidence")
    print(f"Impls:   {len(implementations)} Function start comments")
    print()

    if invalid_parents:
        print("Invalid parent references:")
        for cls_name, parent in invalid_parents:
            print(f"  {cls_name} -> {parent}")
        print()

    if constructor_parent_warnings:
        print("Constructor hierarchy warnings:")
        for item in constructor_parent_warnings:
            parent_vtable = item["parent_vtable"]
            parent_vtable_str = f"0x{parent_vtable:08X}" if parent_vtable else "(none)"
            print(
                f"  {item['class_name']} declares parent {item['parent_name']}, "
                f"but ctor 0x{item['constructor']:08X} neither calls parent ctor "
                f"0x{item['parent_constructor']:08X} nor writes parent vtable {parent_vtable_str}"
            )
        print()

    vtables = {}
    func_types = {}

    with open(exe_path, "rb") as f:
        for cls_name, info in classes.items():
            addr = info["vtable_addr"]
            idx = all_vtable_addrs.index(addr) if addr in all_vtable_addrs else -1
            max_addr = all_vtable_addrs[idx + 1] if (idx >= 0 and idx + 1 < len(all_vtable_addrs)) else addr + 0x100
            vtables[cls_name] = legacy.read_vtable_entries(f, sections, addr, code_start, code_end, max_addr)

        all_funcs = set()
        for entries in vtables.values():
            all_funcs.update(entries)
        for func_addr in all_funcs:
            func_types[func_addr] = classify_function_capstone(
                f,
                sections,
                function_starts,
                func_addr,
                code_end,
                capstone_config,
            )

    def get_parent_entries(cls_name, depth=0):
        if depth > 10:
            return []
        info = classes.get(cls_name)
        if not info or not info["parent"]:
            return []
        parent = info["parent"]
        if parent in vtables:
            return vtables[parent]
        return get_parent_entries(parent, depth + 1)

    total_slots = 0
    total_inherited = 0
    total_overrides = 0
    total_implemented = 0
    total_sdtors = 0
    total_stubs = 0
    total_missing_real = 0
    total_symbol_mismatch = 0
    missing_real = []
    missing_stubs = []

    sorted_classes = sorted(
        [c for c in vtables if not args.filter_class or c == args.filter_class],
        key=lambda c: classes[c]["vtable_addr"]
    )

    if not args.dump:
        print("=" * 124)
        print(f"{'Class':<22} {'Vtable':<14} {'Parent':<18} {'#':<4} {'Inh':<4} {'Ovr':<4} {'OK':<4} {'Stub':<5} {'Miss':<5} {'Bad':<4} {'Status'}")
        print("=" * 124)

    for cls_name in sorted_classes:
        info = classes[cls_name]
        entries = vtables[cls_name]
        parent_entries = get_parent_entries(cls_name)

        slot_names = {}
        parent_chain = []
        cur = cls_name
        while cur and cur in classes:
            parent_chain.append(cur)
            cur = classes[cur].get("parent")
        for item in legacy.VTABLE_SLOT_NAME_SETS:
            if item["class_or_ancestor"] in parent_chain:
                slot_names = item["slots"]
                break

        inherited = 0
        overrides = []
        for i, func_addr in enumerate(entries):
            if i < len(parent_entries) and parent_entries[i] == func_addr:
                inherited += 1
            else:
                overrides.append((i, func_addr))

        n_impl = 0
        n_sdtor = 0
        n_stub_missing = 0
        n_real_missing = 0
        n_symbol_mismatch = 0
        cls_missing_real = []
        cls_missing_stubs = []
        cls_symbol_mismatch = []
        slot_expectations = expected_slots.get(cls_name, [])

        def expected_for_slot(slot_idx):
            if slot_idx < len(slot_expectations):
                return slot_expectations[slot_idx]
            return None

        def slot_label(slot_idx):
            expected = expected_for_slot(slot_idx)
            if expected is not None:
                return expected["method_name"]
            return slot_names.get(slot_idx, "")

        def is_sdtor_slot(slot_idx):
            expected = expected_for_slot(slot_idx)
            if expected is not None and expected["signature_key"] == "__dtor__":
                return True
            return slot_idx == 3

        def matches_expected_symbol(slot_idx, expected, symbols):
            if expected is None:
                return True
            aliases = legacy.SLOT_SYMBOL_ALIASES.get((cls_name, slot_idx), set())
            for sym in symbols:
                candidate = (sym["class_name"], sym["method_name"])
                if candidate == (expected["class_name"], expected["method_name"]):
                    return True
                if candidate in aliases:
                    return True
            return False

        for slot_idx, func_addr in overrides:
            is_impl = func_addr in implementations
            ftype = func_types.get(func_addr, "unknown")
            expected = expected_for_slot(slot_idx)
            symbols = function_symbols.get(func_addr, [])

            if is_impl:
                n_impl += 1
                if expected is not None and expected["signature_key"] != "__dtor__" and not legacy.is_trivial_function_type(ftype):
                    matches_expected = matches_expected_symbol(slot_idx, expected, symbols)
                    if not matches_expected:
                        n_symbol_mismatch += 1
                        cls_symbol_mismatch.append((slot_idx, func_addr, expected, symbols))
            elif is_sdtor_slot(slot_idx):
                n_sdtor += 1
            elif legacy.is_trivial_function_type(ftype):
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
        total_symbol_mismatch += n_symbol_mismatch

        if args.dump:
            parent_str = info["parent"] or "(root)"
            print(f"\n{cls_name} (0x{info['vtable_addr']:08X}, parent={parent_str}, {info['header']}):")
            for i, func_addr in enumerate(entries):
                is_inherited = (i < len(parent_entries) and parent_entries[i] == func_addr)
                is_impl = func_addr in implementations
                ftype = func_types.get(func_addr, "?")
                slot_name = slot_label(i)
                expected = expected_for_slot(i)
                symbols = function_symbols.get(func_addr, [])

                marker = " " if is_inherited else "*"
                if is_inherited:
                    status = f"inherited ({parent_str})"
                elif is_impl:
                    matches_expected = True
                    if expected is not None and expected["signature_key"] != "__dtor__" and not legacy.is_trivial_function_type(ftype):
                        matches_expected = matches_expected_symbol(i, expected, symbols)
                    if matches_expected:
                        locs = implementations[func_addr]
                        status = f"OK  <- {', '.join(f'{fl}:{ln}' for fl, ln in locs)}"
                    else:
                        status = (
                            f"\033[31mMISMATCH\033[0m expected {expected['display_name']} "
                            f"got {format_symbol_list(symbols)}"
                        )
                elif is_sdtor_slot(i):
                    status = "sdtor (compiler-generated)"
                elif legacy.is_trivial_function_type(ftype):
                    tag = {
                        "stub": "RET",
                        "ret0": "return 0",
                        "ret1": "return 1",
                        "retconst": "return constant",
                        "fieldget": "simple field getter",
                        "thunk": "jump thunk",
                    }.get(ftype, ftype)
                    status = f"\033[33mMISSING ({tag})\033[0m"
                elif ftype == "error":
                    status = f"\033[33mMISSING (ShowError call)\033[0m"
                else:
                    status = f"\033[31mMISSING\033[0m"

                name_str = f" {slot_name}" if slot_name else ""
                print(f"  {marker} [{i:2d}]{name_str:<18} 0x{func_addr:08X}  {status}")
        else:
            if n_real_missing > 0:
                status_str = f"\033[31m{n_real_missing} MISSING\033[0m"
                if n_symbol_mismatch > 0:
                    status_str += f" +{n_symbol_mismatch} bad slots"
                if n_stub_missing > 0:
                    status_str += f" +{n_stub_missing} stubs"
            elif n_symbol_mismatch > 0:
                status_str = f"\033[31m{n_symbol_mismatch} bad slots\033[0m"
                if n_stub_missing > 0:
                    status_str += f" +{n_stub_missing} stubs"
            elif n_stub_missing > 0:
                status_str = f"\033[33m{n_stub_missing} stubs only\033[0m"
            else:
                status_str = "\033[32mOK\033[0m"

            parent_str = info["parent"] or "(root)"
            print(f"{cls_name:<22} 0x{info['vtable_addr']:08X}   {parent_str:<18} "
                  f"{len(entries):<4} {inherited:<4} {len(overrides):<4} {n_impl:<4} "
                  f"{n_stub_missing:<5} {n_real_missing:<5} {n_symbol_mismatch:<4} {status_str}")

            for slot_idx, func_addr, ftype in cls_missing_real:
                slot_name = slot_label(slot_idx)
                disasm = os.path.exists(f"{code_dir}/FUN_{func_addr:X}.disassembled.txt")
                note = " (disasm)" if disasm else ""
                name_str = f" {slot_name}" if slot_name else ""
                missing_real.append((cls_name, slot_idx, func_addr, info["header"], slot_name))
                print(f"    [{slot_idx:2d}]{name_str:<16} 0x{func_addr:08X}  [{ftype}]{note}")

            for slot_idx, func_addr, ftype in cls_missing_stubs:
                slot_name = slot_label(slot_idx)
                tag = {
                    "stub": "RET",
                    "ret0": "return 0",
                    "ret1": "return 1",
                    "retconst": "return constant",
                    "fieldget": "simple field getter",
                    "thunk": "jump thunk",
                }.get(ftype, ftype)
                name_str = f" {slot_name}" if slot_name else ""
                missing_stubs.append((cls_name, slot_idx, func_addr, info["header"], slot_name, tag))
                print(f"    [{slot_idx:2d}]{name_str:<16} 0x{func_addr:08X}  \033[2m({tag})\033[0m")

            for slot_idx, func_addr, expected, symbols in cls_symbol_mismatch:
                print(
                    f"    [{slot_idx:2d}] {slot_label(slot_idx):<15} 0x{func_addr:08X}  "
                    f"expected {expected['display_name']} got {format_symbol_list(symbols)}"
                )

    if not args.dump:
        print("=" * 124)

    print()
    print(f"{'Total vtable slots:':<30} {total_slots}")
    print(f"{'  Inherited:':<30} {total_inherited}")
    print(f"{'  Overrides:':<30} {total_overrides}")
    print(f"{'    Implemented:':<30} {total_implemented}")
    print(f"{'    Sdtors (compiler):':<30} {total_sdtors}")
    print(f"{'    Trivial helpers/thunks:':<30} \033[33m{total_stubs}\033[0m")
    print(f"{'    Missing (real code):':<30} \033[31m{total_missing_real}\033[0m")
    print(f"{'    Implemented but wrong slot:':<30} \033[31m{total_symbol_mismatch}\033[0m")

    known_addrs = set(info["vtable_addr"] for info in classes.values())
    unmatched = sorted(set(all_vtable_addrs) - known_addrs)
    if unmatched:
        print(f"\nVtable addresses not matched to any class ({len(unmatched)}):")
        for addr in unmatched:
            print(f"  0x{addr:08X}")

    if invalid_parents or constructor_parent_warnings or total_missing_real or total_symbol_mismatch:
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
