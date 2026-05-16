#!/usr/bin/env python3
"""Check operand value differences by disassembling PE bytes with Capstone.

This is a binary-backed companion to checkValuesAll.py. It keeps the old text
assembly verifier untouched, but avoids depending on Ghidra/MSVC assembly text
for operands.
"""

import argparse
import glob
import json
import os
import re
import struct
import subprocess
import sys
from dataclasses import dataclass

try:
    from capstone import Cs, CS_ARCH_X86, CS_MODE_32
    from capstone.x86_const import X86_OP_IMM, X86_OP_MEM, X86_OP_REG
except ImportError as exc:
    raise SystemExit("error: capstone is required. Install it with: python3 -m pip install capstone") from exc

from cppSourceParser import parse_source_function_groups
from projectConfig import ConfigError, add_config_argument, config_or_arg, load_config, mode_paths


EXECUTABLE_FLAG = 0x20000000
READABLE_FLAG = 0x40000000
DEFAULT_CAPSTONE_CONFIG_PATH = "config/checkValuesCapstone.json"


@dataclass(frozen=True)
class Section:
    name: str
    start: int
    end: int
    rawptr: int
    rawsize: int
    flags: int


@dataclass(frozen=True)
class Operand:
    kind: str
    text: str
    reg: str = ""
    imm: int = 0
    base: str = ""
    index: str = ""
    scale: int = 0
    disp: int = 0
    size: int = 0


@dataclass(frozen=True)
class Instruction:
    address: int
    mnemonic: str
    op_str: str
    operands: tuple
    raw: str


@dataclass(frozen=True)
class FunctionGroup:
    source_path: str
    name: str
    line: int
    original_addrs: tuple
    rebuilt_addr: int
    rebuilt_symbol: str


@dataclass(frozen=True)
class CheckResult:
    original_addr: int
    rebuilt_addr: int
    similarity: float
    original_count: int
    rebuilt_count: int
    warnings: tuple


@dataclass(frozen=True)
class VerifierPolicy:
    max_disassembly_bytes: int
    max_member_displacement: int
    nearby_window: int
    small_immediate_min: int
    small_immediate_max: int
    value_mnemonics: frozenset
    padding_mnemonics: frozenset
    stack_registers: frozenset
    diagnostic_functions: frozenset
    allowed_one_char_strings: frozenset
    rejected_string_substrings: tuple


@dataclass(frozen=True)
class CompareContext:
    enabled_kinds: frozenset
    policy: VerifierPolicy
    compiled_diagnostic_targets: frozenset
    original_diagnostic_targets: frozenset


class PEImage:
    def __init__(self, path, policy):
        self.path = path
        self.policy = policy
        with open(path, "rb") as f:
            self.data = f.read()

        if self.data[:2] != b"MZ":
            raise ValueError(f"not an MZ executable: {path}")
        peoff = struct.unpack_from("<I", self.data, 0x3C)[0]
        if self.data[peoff:peoff + 4] != b"PE\0\0":
            raise ValueError(f"not a PE executable: {path}")

        num_sections = struct.unpack_from("<H", self.data, peoff + 6)[0]
        opt_size = struct.unpack_from("<H", self.data, peoff + 20)[0]
        opt = peoff + 24
        self.image_base = struct.unpack_from("<I", self.data, opt + 28)[0]

        self.sections = []
        section_start = opt + opt_size
        for i in range(num_sections):
            offset = section_start + i * 40
            name = self.data[offset:offset + 8].split(b"\0", 1)[0].decode("ascii", errors="ignore")
            vsize, rva, rawsize, rawptr = struct.unpack_from("<IIII", self.data, offset + 8)
            flags = struct.unpack_from("<I", self.data, offset + 36)[0]
            start = self.image_base + rva
            end = start + max(vsize, rawsize)
            self.sections.append(Section(name, start, end, rawptr, rawsize, flags))

    def section_for_va(self, va):
        for section in self.sections:
            if section.start <= va < section.end:
                return section
        return None

    def section_end_for_va(self, va):
        section = self.section_for_va(va)
        return section.end if section else None

    def read(self, va, size):
        section = self.section_for_va(va)
        if section is None:
            return None
        offset = va - section.start
        if offset >= section.rawsize:
            return b"\0" * size
        available = min(size, section.rawsize - offset)
        result = bytearray(size)
        result[:available] = self.data[section.rawptr + offset:section.rawptr + offset + available]
        return bytes(result)

    def c_string_at(self, va, max_len=160):
        section = self.section_for_va(va)
        if section is None:
            return None
        if section.flags & EXECUTABLE_FLAG:
            return None
        if section.flags and not (section.flags & READABLE_FLAG):
            return None

        data = self.read(va, max_len)
        if not data:
            return None
        if not (32 <= data[0] <= 126):
            return None
        end = data.find(b"\0")
        if end <= 0:
            return None
        raw = data[:end]
        if any(ch < 32 or ch > 126 for ch in raw):
            return None
        text = raw.decode("ascii", errors="ignore")
        if not looks_like_literal_string(text, self.policy):
            return None
        return text


def parse_config_int(value, label):
    if isinstance(value, int):
        return value
    if isinstance(value, str):
        try:
            return int(value, 0)
        except ValueError:
            raise ConfigError(f"{label} must be an integer or integer string") from None
    raise ConfigError(f"{label} must be an integer or integer string")


def required_config(config, key, label):
    if not isinstance(config, dict):
        raise ConfigError(f"{label} must be a JSON object")
    if key not in config:
        raise ConfigError(f"missing required capstone value-check config field: {label}.{key}")
    return config[key]


def string_list_config(config, key, label):
    value = required_config(config, key, label)
    if not isinstance(value, list) or any(not isinstance(item, str) for item in value):
        raise ConfigError(f"{label}.{key} must be a list of strings")
    return value


def value_checker_config(config, demo):
    section = config.get("values_capstone", {})
    if section is None:
        return {}, {}
    if not isinstance(section, dict):
        raise ConfigError("values_capstone must be an object")

    mode = "demo" if demo else "full"
    mode_section = section.get(mode, {})
    if mode_section is None:
        return section, {}
    if not isinstance(mode_section, dict):
        raise ConfigError(f"values_capstone.{mode} must be an object")
    return section, mode_section


def load_policy(path):
    if not path:
        path = DEFAULT_CAPSTONE_CONFIG_PATH
    if not os.path.exists(path):
        raise ConfigError(f"capstone value-check config file not found: {path}")

    with open(path, "r", encoding="utf-8") as f:
        config = json.load(f)

    literal_config = required_config(config, "literal_strings", "capstone")
    return VerifierPolicy(
        max_disassembly_bytes=parse_config_int(
            required_config(config, "max_disassembly_bytes", "capstone"),
            "capstone.max_disassembly_bytes",
        ),
        max_member_displacement=parse_config_int(
            required_config(config, "max_member_displacement", "capstone"),
            "capstone.max_member_displacement",
        ),
        nearby_window=parse_config_int(
            required_config(config, "nearby_window", "capstone"),
            "capstone.nearby_window",
        ),
        small_immediate_min=parse_config_int(
            required_config(config, "small_immediate_min", "capstone"),
            "capstone.small_immediate_min",
        ),
        small_immediate_max=parse_config_int(
            required_config(config, "small_immediate_max", "capstone"),
            "capstone.small_immediate_max",
        ),
        value_mnemonics=frozenset(m.lower() for m in string_list_config(config, "value_mnemonics", "capstone")),
        padding_mnemonics=frozenset(m.lower() for m in string_list_config(config, "padding_mnemonics", "capstone")),
        stack_registers=frozenset(r.lower() for r in string_list_config(config, "stack_registers", "capstone")),
        diagnostic_functions=frozenset(string_list_config(config, "diagnostic_functions", "capstone")),
        allowed_one_char_strings=frozenset(string_list_config(literal_config, "allowed_one_char", "capstone.literal_strings")),
        rejected_string_substrings=tuple(string_list_config(literal_config, "reject_substrings", "capstone.literal_strings")),
    )


def signed32(value):
    value &= 0xFFFFFFFF
    return value - 0x100000000 if value & 0x80000000 else value


def unsigned32(value):
    return value & 0xFFFFFFFF


def normalize_mnemonic(mnemonic):
    aliases = {
        "je": "jz",
        "jne": "jnz",
        "jb": "jc",
        "jnae": "jc",
        "jae": "jnc",
        "jnb": "jnc",
        "jg": "jnle",
        "jnle": "jnle",
        "jge": "jnl",
        "jnl": "jnl",
        "jl": "jnge",
        "jnge": "jnge",
        "jle": "jng",
        "jng": "jng",
    }
    return aliases.get(mnemonic.lower(), mnemonic.lower())


def lcs_align(left, right):
    """Align two mnemonic sequences using longest common subsequence."""
    rows = len(left)
    cols = len(right)
    dp = [[0] * (cols + 1) for _ in range(rows + 1)]
    for i in range(1, rows + 1):
        for j in range(1, cols + 1):
            if left[i - 1] == right[j - 1]:
                dp[i][j] = dp[i - 1][j - 1] + 1
            else:
                dp[i][j] = max(dp[i - 1][j], dp[i][j - 1])

    matches = []
    i = rows
    j = cols
    while i > 0 and j > 0:
        if left[i - 1] == right[j - 1]:
            matches.append((i - 1, j - 1))
            i -= 1
            j -= 1
        elif dp[i - 1][j] > dp[i][j - 1]:
            i -= 1
        else:
            j -= 1
    matches.reverse()
    return matches


def is_branch_or_call(mnemonic):
    return mnemonic.startswith("j") or mnemonic in {"call", "loop", "loope", "loopne", "jecxz"}


def normalize_string(value):
    value = value.replace("\\\\", "\\")
    value = value.replace("\\n", "\n").replace("\\t", "\t").replace("\\r", "\r")
    return value


def normalize_string_for_compare(value):
    if value is None:
        return None
    value = normalize_string(value)
    value = value.strip().strip('"').strip("\\").strip('"')
    return value


def equivalent_format_strings(left, right):
    if "%" not in left or "%" not in right:
        return False
    return "".join(left.split()) == "".join(right.split())


def looks_like_literal_string(value, policy):
    """Reject common false positives from decoding vtable/code pointers as text."""
    for rejected in policy.rejected_string_substrings:
        if rejected in value:
            return False
    if len(value) == 1:
        return value in policy.allowed_one_char_strings
    if len(value) <= 3:
        if "%" in value:
            return True
        if value.isalpha() and (value.islower() or value.isupper()):
            return True
        return False
    return any(ch.isalpha() for ch in value)


def split_name_parameters(name):
    return name.split("(", 1)[0]


def canonical_function_name(name):
    return split_name_parameters(name)


def symbol_patterns_for_function(name):
    base = split_name_parameters(name)
    if "::" in base:
        class_name, method_name = base.rsplit("::", 1)
        class_leaf = class_name.rsplit("::", 1)[-1]
        if method_name == class_leaf:
            return [f"??0{class_leaf}@@"]
        if method_name.startswith("~"):
            return [f"??1{class_leaf}@@"]
        return [f"?{method_name}@{class_leaf}@@"]
    return [f"?{base}@@", f"_{base}@", f"_{base}"]


def symbol_matches(mangled, patterns):
    return any(pattern == mangled or mangled.startswith(pattern) or pattern in mangled for pattern in patterns)


def parse_linker_map_by_obj(map_path):
    entries_by_obj = {}
    if not os.path.exists(map_path):
        return entries_by_obj

    pattern = re.compile(r"0001:[0-9a-fA-F]+\s+(\S+)\s+([0-9a-fA-F]{8})\s+f\s+(\S+\.obj)")
    with open(map_path, "r", encoding="latin1", errors="ignore") as f:
        for line in f:
            match = pattern.search(line)
            if not match:
                continue
            mangled = match.group(1)
            va = int(match.group(2), 16)
            obj = match.group(3)
            entries_by_obj.setdefault(obj, []).append((va, mangled))

    for obj in entries_by_obj:
        entries_by_obj[obj].sort()
    return entries_by_obj


def load_source_groups(src_dir, map_skip=None):
    groups_by_source = {}
    for root, _, files in os.walk(src_dir):
        if map_skip and map_skip in root:
            continue
        for filename in sorted(files):
            if not filename.endswith(".cpp"):
                continue
            path = os.path.join(root, filename)
            groups = parse_source_function_groups(
                path,
                include_no_assembly=False,
                disambiguate_overloads=True,
            )
            if groups:
                groups_by_source[path] = groups
    return groups_by_source


def map_source_groups(groups_by_source, map_path):
    entries_by_obj = parse_linker_map_by_obj(map_path)
    mapped = []
    missing = []

    for source_path in sorted(groups_by_source):
        obj = os.path.basename(source_path).replace(".cpp", ".obj")
        obj_entries = entries_by_obj.get(obj, [])
        used = set()

        for group in groups_by_source[source_path]:
            patterns = symbol_patterns_for_function(group.name)
            hit = None
            for idx, (va, mangled) in enumerate(obj_entries):
                if idx in used:
                    continue
                if symbol_matches(mangled, patterns):
                    hit = (idx, va, mangled)
                    break
            if hit is None:
                missing.append((source_path, group))
                continue
            idx, va, mangled = hit
            used.add(idx)
            original_addrs = tuple(int(addr, 16) for addr in group.addresses)
            mapped.append(FunctionGroup(source_path, group.name, group.line, original_addrs, va, mangled))

    return mapped, missing, entries_by_obj


def build_diagnostic_targets(function_groups, policy):
    original_targets = set()
    rebuilt_targets = set()
    for group in function_groups:
        if canonical_function_name(group.name) not in policy.diagnostic_functions:
            continue
        original_targets.update(group.original_addrs)
        rebuilt_targets.add(group.rebuilt_addr)
    return frozenset(original_targets), frozenset(rebuilt_targets)


def load_original_boundary_starts(code_dir, function_groups):
    starts = set()
    if code_dir and os.path.isdir(code_dir):
        for path in glob.glob(os.path.join(code_dir, "FUN_*.disassembled.txt")):
            match = re.search(r"FUN_([0-9A-Fa-f]+)\.disassembled\.txt$", path)
            if match:
                starts.add(int(match.group(1), 16))
    for group in function_groups:
        starts.update(group.original_addrs)
    return sorted(starts)


def load_rebuilt_boundary_starts(entries_by_obj):
    starts = set()
    for entries in entries_by_obj.values():
        for va, _ in entries:
            starts.add(va)
    return sorted(starts)


def next_boundary(starts, start):
    for candidate in starts:
        if candidate > start:
            return candidate
    return None


def function_bytes(image, start, starts):
    section_end = image.section_end_for_va(start)
    if section_end is None:
        return None, None
    end = next_boundary(starts, start)
    if end is None or end > section_end:
        end = section_end
    if end <= start:
        return None, None
    size = min(end - start, image.policy.max_disassembly_bytes)
    return image.read(start, size), start + size


def make_operand(insn, op):
    if op.type == X86_OP_REG:
        return Operand("reg", insn.reg_name(op.reg), reg=insn.reg_name(op.reg), size=op.size)
    if op.type == X86_OP_IMM:
        imm = signed32(op.imm)
        return Operand("imm", str(imm), imm=imm, size=op.size)
    if op.type == X86_OP_MEM:
        base = insn.reg_name(op.mem.base) if op.mem.base else ""
        index = insn.reg_name(op.mem.index) if op.mem.index else ""
        return Operand(
            "mem",
            "",
            base=base,
            index=index,
            scale=op.mem.scale,
            disp=signed32(op.mem.disp),
            size=op.size,
        )
    return Operand("other", "")


def disassemble(image, start, starts):
    data, end = function_bytes(image, start, starts)
    if not data:
        return []

    md = Cs(CS_ARCH_X86, CS_MODE_32)
    md.detail = True
    instructions = []
    for insn in md.disasm(data, start):
        mnemonic = normalize_mnemonic(insn.mnemonic)
        if mnemonic in image.policy.padding_mnemonics:
            continue
        operands = tuple(make_operand(insn, op) for op in insn.operands)
        instructions.append(Instruction(
            address=insn.address,
            mnemonic=mnemonic,
            op_str=insn.op_str,
            operands=operands,
            raw=f"{insn.mnemonic} {insn.op_str}".strip(),
        ))

    data_ranges = switch_jump_table_ranges(image, instructions, start, end)
    if data_ranges:
        instructions = [
            instr for instr in instructions
            if not address_in_ranges(instr.address, data_ranges)
        ]

    instructions = trim_seh_cleanup_funclets(instructions)
    while instructions and instructions[-1].mnemonic in image.policy.padding_mnemonics:
        instructions.pop()
    return instructions


def address_in_ranges(address, ranges):
    return any(start <= address < end for start, end in ranges)


def switch_jump_table_ranges(image, instrs, func_start, func_end):
    """Find MSVC switch tables embedded in a function body.

    Function bounds come from MAP/Ghidra starts, so jump tables placed after the
    final RET still belong to the function range. Linear disassembly decodes
    those table bytes as bogus instructions unless we remove the data range.
    """
    ranges = []
    for instr in instrs:
        if instr.mnemonic != "jmp":
            continue
        if len(instr.operands) != 1:
            continue

        operand = instr.operands[0]
        if operand.kind != "mem":
            continue
        if operand.base or not operand.index or operand.scale != 4:
            continue

        table_start = unsigned32(operand.disp)
        if not (func_start <= table_start < func_end):
            continue

        table_end = switch_jump_table_end(image, table_start, func_start, func_end)
        if table_end is not None:
            ranges.append((table_start, table_end))

    return merge_ranges(ranges)


def switch_jump_table_end(image, table_start, func_start, func_end):
    cursor = table_start
    entries = 0
    while cursor + 4 <= func_end:
        raw = image.read(cursor, 4)
        if raw is None or len(raw) != 4:
            break
        target = struct.unpack("<I", raw)[0]
        if not (func_start <= target < func_end):
            break
        entries += 1
        cursor += 4

    if entries == 0:
        return None
    return cursor


def merge_ranges(ranges):
    if not ranges:
        return []
    merged = []
    for start, end in sorted(ranges):
        if not merged or start > merged[-1][1]:
            merged.append([start, end])
            continue
        merged[-1][1] = max(merged[-1][1], end)
    return tuple((start, end) for start, end in merged)


def has_msvc_seh_frame(instrs):
    """MSVC places EH cleanup funclets after the main body RET."""
    scan = instrs[:12]
    saw_fs_read = any(instr.mnemonic == "mov" and "fs:" in instr.op_str.lower() for instr in scan)
    saw_sentinel = any(
        instr.mnemonic == "push"
        and instr.operands
        and instr.operands[0].kind == "imm"
        and instr.operands[0].imm == -1
        for instr in scan
    )
    saw_fs_write = any(instr.mnemonic == "mov" and "fs:" in instr.op_str.lower() for instr in instrs[:24])
    return saw_fs_read and saw_sentinel and saw_fs_write


def trim_seh_cleanup_funclets(instrs):
    if not has_msvc_seh_frame(instrs):
        return instrs
    for idx, instr in enumerate(instrs):
        if instr.mnemonic == "ret":
            return instrs[:idx + 1]
    return instrs


def operand_signature(operand):
    if operand.kind == "reg":
        return ("reg", operand.reg)
    if operand.kind == "mem":
        return ("mem", operand.base, operand.index, operand.scale)
    return (operand.kind,)


def comparable_lhs(left, right, policy):
    if left.kind != right.kind:
        return False
    if left.kind == "reg":
        return left.reg == right.reg
    if left.kind != "mem":
        return False
    if left.base != right.base or left.index != right.index or left.scale != right.scale:
        return False
    if left.base in policy.stack_registers:
        return False
    if not left.base:
        return False
    return True


def comparable_operands(compiled, original):
    if len(compiled.operands) != len(original.operands):
        return False
    for c_op, o_op in zip(compiled.operands, original.operands):
        if c_op.kind == "imm" or o_op.kind == "imm":
            continue
        if operand_signature(c_op) != operand_signature(o_op):
            return False
    return True


def immediate_operands(instr):
    return [(idx, op) for idx, op in enumerate(instr.operands) if op.kind == "imm"]


def memory_operands(instr):
    return [(idx, op) for idx, op in enumerate(instr.operands) if op.kind == "mem"]


def immediate_string(image, operand):
    return image.c_string_at(unsigned32(operand.imm))


def small_numeric_immediate(operand, policy):
    value = operand.imm
    return policy.small_immediate_min <= value < policy.small_immediate_max


def has_pointer_immediate(instr):
    return any(unsigned32(operand.imm) >= 0x400000 for _, operand in immediate_operands(instr))


def member_displacement(value, policy):
    return -policy.max_member_displacement < value < policy.max_member_displacement


def lhs_is_stack_memory(instr, policy):
    if not instr.operands:
        return False
    operand = instr.operands[0]
    return operand.kind == "mem" and operand.base in policy.stack_registers


def nearby_immediate_values(instrs, image, idx, policy):
    values = set()
    start = max(0, idx - policy.nearby_window)
    end = min(len(instrs), idx + policy.nearby_window + 1)
    for j in range(start, end):
        for _, operand in immediate_operands(instrs[j]):
            if small_numeric_immediate(operand, policy):
                values.add(operand.imm)
    return values


def nearby_strings(instrs, image, idx, policy):
    values = set()
    start = max(0, idx - policy.nearby_window)
    end = min(len(instrs), idx + policy.nearby_window + 1)
    for j in range(start, end):
        for _, operand in immediate_operands(instrs[j]):
            string = immediate_string(image, operand)
            if string is not None:
                values.add(normalize_string_for_compare(string))
    return values


def nearby_calls_target(instrs, idx, targets, window=8):
    if not targets:
        return False
    end = min(len(instrs), idx + window + 1)
    for j in range(idx + 1, end):
        instr = instrs[j]
        if instr.mnemonic != "call":
            continue
        for _, operand in immediate_operands(instr):
            if unsigned32(operand.imm) in targets:
                return True
    return False


def nearby_memory_displacements(instrs, idx, policy, mnemonic=None):
    values = set()
    start = max(0, idx - policy.nearby_window)
    end = min(len(instrs), idx + policy.nearby_window + 1)
    aliases = {}
    for j in range(start, end):
        instr = instrs[j]
        if mnemonic is None or instr.mnemonic == mnemonic:
            for _, operand in memory_operands(instr):
                if member_displacement(operand.disp, policy):
                    values.add(operand.disp)
                alias = aliases.get(operand.base)
                if alias is not None and not operand.index:
                    effective_disp = alias.disp + operand.disp
                    if member_displacement(effective_disp, policy):
                        values.add(effective_disp)

        update_register_aliases(aliases, instr, policy)

    return values


def writes_register(instr, reg):
    if not instr.operands:
        return False
    operand = instr.operands[0]
    return operand.kind == "reg" and operand.reg == reg


def update_register_aliases(aliases, instr, policy):
    if instr.mnemonic == "call":
        for reg in ("eax", "ecx", "edx"):
            aliases.pop(reg, None)
        return

    if instr.mnemonic == "lea" and len(instr.operands) >= 2:
        dst = instr.operands[0]
        src = instr.operands[1]
        if dst.kind == "reg" and src.kind == "mem":
            if src.base and src.base not in policy.stack_registers and member_displacement(src.disp, policy):
                aliases[dst.reg] = src
            else:
                aliases.pop(dst.reg, None)
        return

    if instr.mnemonic in {"add", "sub"} and len(instr.operands) >= 2:
        dst = instr.operands[0]
        src = instr.operands[1]
        if dst.kind == "reg" and src.kind == "imm" and member_displacement(src.imm, policy):
            current = aliases.get(dst.reg, Operand("mem", "", base=dst.reg, scale=1))
            delta = src.imm if instr.mnemonic == "add" else -src.imm
            aliases[dst.reg] = Operand(
                "mem",
                "",
                base=current.base,
                index=current.index,
                scale=current.scale,
                disp=current.disp + delta,
            )
            return

    register_write_mnemonics = {
        "mov", "xor", "or", "and", "imul",
        "shl", "shr", "sar", "inc", "dec",
    }
    if instr.mnemonic in register_write_mnemonics and instr.operands and instr.operands[0].kind == "reg":
        aliases.pop(instr.operands[0].reg, None)


def recent_pointer_alias(instrs, idx, reg, policy, max_window=8):
    start = max(0, idx - max_window)
    for j in range(idx - 1, start - 1, -1):
        instr = instrs[j]
        if instr.mnemonic == "call" or is_branch_or_call(instr.mnemonic):
            return None
        if not writes_register(instr, reg):
            continue
        if instr.mnemonic == "lea" and len(instr.operands) >= 2:
            src = instr.operands[1]
            if src.kind != "mem" or not src.base or src.base in policy.stack_registers:
                return None
            if not member_displacement(src.disp, policy):
                return None
            return src
        if instr.mnemonic in {"add", "sub"} and len(instr.operands) >= 2:
            src = instr.operands[1]
            if src.kind != "imm" or not member_displacement(src.imm, policy):
                return None
            delta = src.imm if instr.mnemonic == "add" else -src.imm
            return Operand("mem", "", base=reg, scale=1, disp=delta)
        return None
    return None


def register_alias_at(instrs, idx, reg, policy):
    aliases = {}
    for j in range(0, idx):
        update_register_aliases(aliases, instrs[j], policy)
    return aliases.get(reg)


def same_effective_lea_displacement(compiled_instrs, original_instrs, ci, oi, operand_idx, c_op, o_op, policy):
    c_alias = recent_pointer_alias(compiled_instrs, ci, c_op.base, policy)
    o_alias = recent_pointer_alias(original_instrs, oi, o_op.base, policy)
    if c_alias is None:
        c_alias = register_alias_at(compiled_instrs, ci, c_op.base, policy)
    if o_alias is None:
        o_alias = register_alias_at(original_instrs, oi, o_op.base, policy)
    if c_alias is None and o_alias is None:
        return False
    if c_alias is not None and o_alias is not None:
        if c_alias.base != o_alias.base or c_alias.index != o_alias.index or c_alias.scale != o_alias.scale:
            return False
        return c_alias.disp + c_op.disp == o_alias.disp + o_op.disp
    if c_alias is not None:
        return c_alias.disp + c_op.disp == o_op.disp
    return c_op.disp == o_alias.disp + o_op.disp


def shifted_memory_base_match_count(compiled_instrs, original_instrs, ci, oi, operand_idx, delta, policy):
    """Count nearby same-shaped memory operands using one consistent base shift."""
    if delta == 0:
        return 0

    count = 0
    start = -policy.nearby_window
    end = policy.nearby_window + 1
    for rel in range(start, end):
        c_idx = ci + rel
        o_idx = oi + rel
        if c_idx < 0 or o_idx < 0:
            continue
        if c_idx >= len(compiled_instrs) or o_idx >= len(original_instrs):
            continue

        compiled = compiled_instrs[c_idx]
        original = original_instrs[o_idx]
        if compiled.mnemonic != original.mnemonic:
            continue
        if operand_idx >= len(compiled.operands) or operand_idx >= len(original.operands):
            continue

        c_op = compiled.operands[operand_idx]
        o_op = original.operands[operand_idx]
        if c_op.kind != "mem" or o_op.kind != "mem":
            continue
        if not comparable_lhs(c_op, o_op, policy):
            continue
        if not member_displacement(c_op.disp, policy) or not member_displacement(o_op.disp, policy):
            continue
        if c_op.disp - o_op.disp != delta:
            continue

        count += 1

    return count


def equivalent_shifted_memory_base(compiled_instrs, original_instrs, ci, oi, operand_idx, c_op, o_op, policy):
    # MSVC often keeps an interior pointer in a register and addresses all fields
    # around it. Require several neighboring accesses before treating the
    # displacement delta as a pointer-basis choice rather than a layout bug.
    delta = c_op.disp - o_op.disp
    return shifted_memory_base_match_count(
        compiled_instrs,
        original_instrs,
        ci,
        oi,
        operand_idx,
        delta,
        policy,
    ) >= 3


def next_mnemonic(instrs, idx):
    if idx + 1 >= len(instrs):
        return None
    return instrs[idx + 1].mnemonic


def equivalent_integer_threshold(c_imm, o_imm, compiled_instrs, original_instrs, ci, oi):
    c_next = next_mnemonic(compiled_instrs, ci)
    o_next = next_mnemonic(original_instrs, oi)
    if c_next is None or o_next is None:
        return False

    if (c_next, o_next) in (("jnl", "jnle"), ("jnc", "ja")):
        return c_imm == o_imm + 1
    if (c_next, o_next) in (("jnle", "jnl"), ("ja", "jnc")):
        return o_imm == c_imm + 1
    if (c_next, o_next) in (("jnge", "jng"), ("jc", "jbe")):
        return c_imm == o_imm + 1
    if (c_next, o_next) in (("jng", "jnge"), ("jbe", "jc")):
        return o_imm == c_imm + 1
    return False


def starts_boolean_mask_after_cmp(instrs, idx):
    if idx + 2 >= len(instrs):
        return False

    mov_instr = instrs[idx + 1]
    adc_instr = instrs[idx + 2]
    if mov_instr.mnemonic != "mov" or adc_instr.mnemonic != "adc":
        return False
    if len(mov_instr.operands) < 2 or len(adc_instr.operands) < 2:
        return False

    mov_dst = mov_instr.operands[0]
    mov_src = mov_instr.operands[1]
    adc_dst = adc_instr.operands[0]
    adc_src = adc_instr.operands[1]
    return (
        mov_dst.kind == "reg"
        and mov_src.kind == "imm"
        and mov_src.imm == 0
        and adc_dst.kind == "reg"
        and adc_dst.reg == mov_dst.reg
        and adc_src.kind == "imm"
        and adc_src.imm == -1
    )


def equivalent_boolean_zero_test(c_imm, o_imm, compiled_instrs, original_instrs, ci, oi):
    if {c_imm, o_imm} != {0, 1}:
        return False
    return starts_boolean_mask_after_cmp(compiled_instrs, ci) or starts_boolean_mask_after_cmp(original_instrs, oi)


def is_stack_adjustment(instr):
    if instr.mnemonic not in {"add", "sub"} or len(instr.operands) < 2:
        return False
    return instr.operands[0].kind == "reg" and instr.operands[0].reg == "esp"


def report_string_mismatch(compiled_instrs, original_instrs, compiled_image, original_image, ci, oi, c_str, o_str, context):
    if c_str is None or o_str is None:
        return None
    c_norm = normalize_string_for_compare(c_str)
    o_norm = normalize_string_for_compare(o_str)
    if c_norm == o_norm:
        return None
    if c_norm.lower() == o_norm.lower():
        return None
    if c_norm and o_norm and (c_norm.startswith(o_norm) or o_norm.startswith(c_norm)):
        return None
    if c_norm is not None and o_norm is not None and equivalent_format_strings(c_norm, o_norm):
        return None
    if c_norm is not None and o_norm is not None:
        if (o_norm in nearby_strings(compiled_instrs, compiled_image, ci, context.policy) and
                c_norm in nearby_strings(original_instrs, original_image, oi, context.policy)):
            return None
    if (nearby_calls_target(compiled_instrs, ci, context.compiled_diagnostic_targets) and
            nearby_calls_target(original_instrs, oi, context.original_diagnostic_targets)):
        return None
    return ("string", c_str, o_str, compiled_instrs[ci], original_instrs[oi])


def compare_instruction_pair(compiled_instrs, original_instrs, compiled_image, original_image, ci, oi, context):
    compiled = compiled_instrs[ci]
    original = original_instrs[oi]
    warnings = []

    if compiled.mnemonic != original.mnemonic:
        return warnings
    if is_branch_or_call(compiled.mnemonic):
        return warnings
    if compiled.mnemonic not in context.policy.value_mnemonics:
        return warnings
    if not comparable_operands(compiled, original):
        return warnings

    c_imms = dict(immediate_operands(compiled))
    o_imms = dict(immediate_operands(original))
    for idx in sorted(set(c_imms) & set(o_imms)):
        c_op = c_imms[idx]
        o_op = o_imms[idx]
        if c_op.imm == o_op.imm:
            continue
        c_str = immediate_string(compiled_image, c_op)
        o_str = immediate_string(original_image, o_op)
        if c_str is not None or o_str is not None:
            if "strings" not in context.enabled_kinds:
                continue
            if compiled.mnemonic != "push" and lhs_is_stack_memory(compiled, context.policy):
                continue
            warning = report_string_mismatch(
                compiled_instrs, original_instrs,
                compiled_image, original_image,
                ci, oi, c_str, o_str, context,
            )
            if warning is not None:
                warnings.append(warning)
            continue

        if "immediates" not in context.enabled_kinds:
            continue
        if compiled.mnemonic != "push" and lhs_is_stack_memory(compiled, context.policy):
            continue
        if is_stack_adjustment(compiled) and is_stack_adjustment(original):
            continue
        if not small_numeric_immediate(c_op, context.policy) or not small_numeric_immediate(o_op, context.policy):
            continue
        if compiled.mnemonic == "cmp" and equivalent_integer_threshold(
                c_op.imm, o_op.imm, compiled_instrs, original_instrs, ci, oi):
            continue
        if compiled.mnemonic == "cmp" and equivalent_boolean_zero_test(
                c_op.imm, o_op.imm, compiled_instrs, original_instrs, ci, oi):
            continue
        if (o_op.imm in nearby_immediate_values(compiled_instrs, compiled_image, ci, context.policy) and
                c_op.imm in nearby_immediate_values(original_instrs, original_image, oi, context.policy)):
            continue
        warnings.append(("imm", c_op.imm, o_op.imm, compiled, original))

    c_mems = dict(memory_operands(compiled))
    o_mems = dict(memory_operands(original))
    if "offsets" in context.enabled_kinds:
        for idx in sorted(set(c_mems) & set(o_mems)):
            c_op = c_mems[idx]
            o_op = o_mems[idx]
            if c_op.disp == o_op.disp:
                continue
            if not member_displacement(c_op.disp, context.policy) or not member_displacement(o_op.disp, context.policy):
                continue
            if not comparable_lhs(c_op, o_op, context.policy):
                continue
            c_near = nearby_memory_displacements(compiled_instrs, ci, context.policy, compiled.mnemonic)
            o_near = nearby_memory_displacements(original_instrs, oi, context.policy, original.mnemonic)
            if o_op.disp in c_near and c_op.disp in o_near:
                continue
            if same_effective_lea_displacement(
                    compiled_instrs, original_instrs, ci, oi, idx, c_op, o_op, context.policy):
                continue
            if equivalent_shifted_memory_base(
                    compiled_instrs, original_instrs, ci, oi, idx, c_op, o_op, context.policy):
                continue
            if (c_op.disp == 0 or o_op.disp == 0) and (has_pointer_immediate(compiled) or has_pointer_immediate(original)):
                continue
            warnings.append(("offset", c_op.disp, o_op.disp, compiled, original))

    return warnings


def check_candidate(compiled_instrs, original_instrs, compiled_image, original_image, context):
    c_mnemonics = [instr.mnemonic for instr in compiled_instrs]
    o_mnemonics = [instr.mnemonic for instr in original_instrs]
    if not c_mnemonics or not o_mnemonics:
        return 0.0, ()

    matches = lcs_align(c_mnemonics, o_mnemonics)
    similarity = 100.0 * len(matches) / max(len(c_mnemonics), len(o_mnemonics))

    warnings = []
    for ci, oi in matches:
        warnings.extend(compare_instruction_pair(
            compiled_instrs,
            original_instrs,
            compiled_image,
            original_image,
            ci,
            oi,
            context,
        ))
    return similarity, tuple(warnings)


def check_function(group, original_image, rebuilt_image, original_starts, rebuilt_starts, context):
    rebuilt_instrs = disassemble(rebuilt_image, group.rebuilt_addr, rebuilt_starts)
    if not rebuilt_instrs:
        return None

    results = []
    for original_addr in group.original_addrs:
        original_instrs = disassemble(original_image, original_addr, original_starts)
        if not original_instrs:
            continue
        similarity, warnings = check_candidate(
            rebuilt_instrs,
            original_instrs,
            rebuilt_image,
            original_image,
            context,
        )
        results.append(CheckResult(
            original_addr=original_addr,
            rebuilt_addr=group.rebuilt_addr,
            similarity=similarity,
            original_count=len(original_instrs),
            rebuilt_count=len(rebuilt_instrs),
            warnings=warnings,
        ))

    if not results:
        return None
    return sorted(results, key=lambda result: (result.similarity, -len(result.warnings)), reverse=True)[0]


def format_warning(warning):
    kind, compiled_value, original_value, compiled, original = warning
    if kind == "string":
        return (
            f"    STRING compiled {compiled_value!r} vs original {original_value!r}: "
            f"0x{compiled.address:08X} {compiled.raw}  |  "
            f"0x{original.address:08X} {original.raw}"
        )
    if kind == "imm":
        return (
            f"    IMM {compiled_value} vs {original_value}: "
            f"0x{compiled.address:08X} {compiled.raw}  |  "
            f"0x{original.address:08X} {original.raw}"
        )
    if kind == "offset":
        return (
            f"    OFFSET 0x{compiled_value & 0xFFFFFFFF:X} vs 0x{original_value & 0xFFFFFFFF:X}: "
            f"0x{compiled.address:08X} {compiled.raw}  |  "
            f"0x{original.address:08X} {original.raw}"
        )
    return str(warning)


def default_rebuilt_exe(demo):
    return "TEACHER-DEMO.EXE" if demo else "TEACHER.EXE"


def default_map_path(demo):
    return "TEACHER-DEMO.map" if demo else "TEACHER.map"


def parse_args():
    parser = argparse.ArgumentParser(description="Run Capstone-backed value checks for implemented functions.")
    add_config_argument(parser)
    parser.add_argument("--demo", action="store_true", help="Use demo paths from config.")
    parser.add_argument("--filter", dest="file_filter", help="Only include files whose names contain this text.")
    parser.add_argument("--src-dir", help="Source directory override.")
    parser.add_argument("--code-dir", help="Ghidra export/disassembly directory override, used only for original boundaries.")
    parser.add_argument("--original-exe", help="Original executable override.")
    parser.add_argument("--recompiled-exe", help="Recompiled executable override.")
    parser.add_argument("--map", dest="map_path", help="Recompiled linker .map override.")
    parser.add_argument("--capstone-config",
                        help=f"Capstone verifier policy JSON override. Default: {DEFAULT_CAPSTONE_CONFIG_PATH}.")
    parser.add_argument("--map-skip", help="Directory subtree to skip while scanning sources.")
    parser.add_argument("--clean-target", help="Make target used to clean before rebuilding.")
    parser.add_argument("--build-target", help="Make target used to build assembly/executable.")
    parser.add_argument("--jobs", type=int, help="Parallel make jobs.")
    parser.add_argument("--no-build", action="store_true", help="Skip build and use existing executable/map.")
    parser.add_argument("--min-similarity", type=float, default=0.0,
                        help="Only show functions whose similarity is at least this percentage (0-100). Default: 0.")
    parser.add_argument("--boundary-report", action="store_true", help="Print function-boundary inventory.")
    parser.add_argument("--strings-only", action="store_true",
                        help="Only report string literal mismatches.")
    parser.add_argument("--no-strings", action="store_true", help="Do not report string literal mismatches.")
    parser.add_argument("--no-immediates", action="store_true", help="Do not report small numeric immediate mismatches.")
    parser.add_argument("--no-offsets", action="store_true", help="Do not report member displacement mismatches.")
    return parser.parse_args()


def main():
    args = parse_args()
    try:
        config = load_config(args.config)
        paths = mode_paths(config, demo=args.demo)
        value_config, value_mode_config = value_checker_config(config, args.demo)
        mode = "demo" if args.demo else "full"
        src_dir = config_or_arg(args.src_dir, paths, "src_dir", f"paths.{mode}.src_dir")
        code_dir = config_or_arg(args.code_dir, paths, "code_dir", f"paths.{mode}.code_dir")
        original_exe = config_or_arg(args.original_exe, paths, "exe", f"paths.{mode}.exe")
        map_skip = config_or_arg(args.map_skip, paths, "map_skip", f"paths.{mode}.map_skip")
        clean_target = config_or_arg(args.clean_target, paths, "clean_target", f"paths.{mode}.clean_target")
        build_target = (
            args.build_target
            or value_mode_config.get("build_target")
            or config_or_arg(None, paths, "build_target", f"paths.{mode}.build_target")
        )
        rebuilt_exe = args.recompiled_exe or value_mode_config.get("recompiled_exe") or default_rebuilt_exe(args.demo)
        map_path = args.map_path or value_mode_config.get("map") or default_map_path(args.demo)
        jobs = args.jobs if args.jobs is not None else int(config.get("build", {}).get("jobs", 1))
        capstone_config_path = (
            args.capstone_config
            or value_config.get("config", DEFAULT_CAPSTONE_CONFIG_PATH)
        )
        policy = load_policy(capstone_config_path)
    except ConfigError as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 2

    enabled_kinds = {"strings", "immediates", "offsets"}
    if args.strings_only:
        enabled_kinds = {"strings"}
    else:
        if args.no_strings:
            enabled_kinds.discard("strings")
        if args.no_immediates:
            enabled_kinds.discard("immediates")
        if args.no_offsets:
            enabled_kinds.discard("offsets")

    if not args.no_build:
        print("Building...", file=sys.stderr)
        subprocess.run(["make", clean_target], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, check=False)
        build_command = ["make", build_target]
        if jobs and jobs > 1:
            build_command.append(f"-j{jobs}")
        ret = subprocess.run(build_command, check=False)
        if ret.returncode != 0:
            print("Build failed!", file=sys.stderr)
            return 1

    for path, label in ((original_exe, "original executable"), (rebuilt_exe, "recompiled executable"), (map_path, "linker map")):
        if not os.path.exists(path):
            print(f"error: missing {label}: {path}", file=sys.stderr)
            return 2

    groups_by_source = load_source_groups(src_dir, map_skip)
    mapped_groups, missing_groups, entries_by_obj = map_source_groups(groups_by_source, map_path)

    if args.file_filter:
        mapped_groups = [
            group for group in mapped_groups
            if args.file_filter in os.path.basename(group.source_path) or args.file_filter in group.name
        ]

    original_starts = load_original_boundary_starts(code_dir, mapped_groups)
    rebuilt_starts = load_rebuilt_boundary_starts(entries_by_obj)

    if args.boundary_report:
        marker_count = sum(len(group.original_addrs) for group in mapped_groups)
        print("--- Boundary Inventory ---")
        print(f"Source function groups: {sum(len(groups) for groups in groups_by_source.values())}")
        print(f"Mapped groups: {len(mapped_groups)}")
        print(f"Original marker candidates in mapped groups: {marker_count}")
        print(f"Original boundary starts: {len(original_starts)}")
        print(f"Rebuilt boundary starts from map: {len(rebuilt_starts)}")
        print(f"Missing map groups: {len(missing_groups)}")

    original_image = PEImage(original_exe, policy)
    rebuilt_image = PEImage(rebuilt_exe, policy)
    original_diag_targets, rebuilt_diag_targets = build_diagnostic_targets(mapped_groups, policy)
    context = CompareContext(
        enabled_kinds=frozenset(enabled_kinds),
        policy=policy,
        compiled_diagnostic_targets=rebuilt_diag_targets,
        original_diagnostic_targets=original_diag_targets,
    )

    total = 0
    with_mismatches = 0
    total_warnings = 0
    skipped_below_threshold = 0
    skipped_no_bytes = 0

    for group in mapped_groups:
        total += 1
        result = check_function(group, original_image, rebuilt_image, original_starts, rebuilt_starts, context)
        if result is None:
            skipped_no_bytes += 1
            continue
        if not result.warnings:
            continue
        if result.similarity < args.min_similarity:
            skipped_below_threshold += 1
            continue

        with_mismatches += 1
        total_warnings += len(result.warnings)
        print(
            f"\n{group.name} "
            f"(orig 0x{result.original_addr:X}, rebuilt 0x{result.rebuilt_addr:X}, "
            f"{result.similarity:.1f}%) - {len(result.warnings)} mismatch(es):"
        )
        for warning in result.warnings:
            print(format_warning(warning))

    print("\n--- Summary ---")
    print(f"Functions checked: {total}")
    print(f"With value mismatches: {with_mismatches}")
    print(f"Total mismatches: {total_warnings}")
    print(f"Skipped (no bytes/disassembly): {skipped_no_bytes}")
    if args.min_similarity > 0:
        print(f"Skipped (similarity < {args.min_similarity:.1f}%): {skipped_below_threshold}")
    if missing_groups and not args.file_filter:
        print(f"Unmapped source groups: {len(missing_groups)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
