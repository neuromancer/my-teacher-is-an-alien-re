#!/usr/bin/env python3
"""Run checkValues analysis on all implemented functions."""

import argparse
import os
import re
import subprocess
import sys
from compileAndCompare import get_similarity, read_assembly
from checkValues import (
    parse_full_instructions, lcs_align, load_strings,
    extract_offset, extract_immediate, extract_string_name
)
from cppSourceParser import parse_source_functions
from projectConfig import ConfigError, add_config_argument, config_or_arg, load_config, mode_paths


def normalize_string_for_cmp(s):
    """Normalize a string for comparison — decode C escapes.
    strings.txt double-escapes: \\\\\\\\ in file = \\\\ captured = one actual backslash."""
    # First collapse all multi-backslashes down to single
    while '\\\\' in s:
        s = s.replace('\\\\', '\\')
    s = s.replace('\\n', '\n')
    s = s.replace('\\t', '\t')
    s = s.replace('\\r', '\r')
    # Also strip surrounding escaped quotes from strings.txt
    s = s.strip('\\"')
    return s


def has_undecoded_mangled(s):
    """Check if string has undecoded MSVC mangled sequences."""
    return bool(re.search(r'\?\$[A-Z]{2}', s) or re.search(r'\?[0-9]', s))


def has_symbolic_memory_operand(ops):
    """True when MSVC emitted a symbol plus an index/base register."""
    if not ('[' in ops and ']' in ops):
        return False
    # Locals look like _name$123[esp+20]. The numeric suffix is optional.
    if re.search(r'_\w+\$\d*\[', ops):
        return True
    # Globals/member arrays can look like ?symbol@@...[eax+4].
    return '?' in ops or '@@' in ops


def is_memory_operand(op):
    return '[' in op and ']' in op


def extract_standalone_immediate(op):
    """Extract an immediate only when the operand is not a memory reference."""
    if is_memory_operand(op):
        return None
    return extract_immediate(op)


def split_operands(ops):
    return [part.strip() for part in ops.split(',')]


def is_stack_memory(op):
    reg = re.search(r'\[\s*(\w+)', op)
    return bool(reg and reg.group(1).lower() in ('esp', 'ebp'))


def lhs_operands_comparable(c_lhs, o_lhs):
    """Avoid comparing immediates from LCS pairs with unrelated operands."""
    c_lhs = c_lhs.strip()
    o_lhs = o_lhs.strip()
    c_mem = is_memory_operand(c_lhs)
    o_mem = is_memory_operand(o_lhs)
    if c_mem != o_mem:
        return False
    if not c_mem:
        return c_lhs.lower() == o_lhs.lower()
    if has_symbolic_memory_operand(c_lhs) or has_symbolic_memory_operand(o_lhs):
        return False
    c_reg = re.search(r'\[\s*(\w+)', c_lhs)
    o_reg = re.search(r'\[\s*(\w+)', o_lhs)
    if not c_reg or not o_reg:
        return False
    if c_reg.group(1).lower() != o_reg.group(1).lower():
        return False
    if c_reg.group(1).lower() in ('esp', 'ebp'):
        return False
    c_off = extract_offset(c_lhs)
    o_off = extract_offset(o_lhs)
    return c_off == o_off


def comparable_offset_operands(c_ops, o_ops):
    c_parts = split_operands(c_ops)
    o_parts = split_operands(o_ops)
    if len(c_parts) != len(o_parts):
        return False

    c_mem_indexes = [i for i, part in enumerate(c_parts) if is_memory_operand(part)]
    o_mem_indexes = [i for i, part in enumerate(o_parts) if is_memory_operand(part)]
    if len(c_mem_indexes) != 1 or c_mem_indexes != o_mem_indexes:
        return False

    for i, (c_part, o_part) in enumerate(zip(c_parts, o_parts)):
        if i == c_mem_indexes[0]:
            continue
        c_imm = extract_standalone_immediate(c_part)
        o_imm = extract_standalone_immediate(o_part)
        if c_imm is not None or o_imm is not None:
            if c_imm != o_imm:
                return False
        elif c_part.lower() != o_part.lower():
            return False
    return True


def is_double_push_half_mismatch(c_imm, o_imm):
    return {c_imm, o_imm} == {0, 0x3ff00000}


def next_mnemonic(instrs, idx):
    if idx + 1 >= len(instrs):
        return None
    return normalize_branch_mnemonic(instrs[idx + 1][0])


def normalize_branch_mnemonic(mnem):
    aliases = {
        'jnl': 'jge',
        'jnle': 'jg',
        'jng': 'jle',
        'jnge': 'jl',
        'jnb': 'jae',
        'jnbe': 'ja',
        'jna': 'jbe',
        'jnae': 'jb',
    }
    mnem = mnem.lower()
    return aliases.get(mnem, mnem)


def equivalent_integer_threshold(c_imm, o_imm, compiled_instrs, original_instrs, ci, oi):
    """MSVC can express >= N as > N-1, and similarly for nearby integer bounds."""
    c_next = next_mnemonic(compiled_instrs, ci)
    o_next = next_mnemonic(original_instrs, oi)
    if c_next is None or o_next is None:
        return False

    if (c_next, o_next) in (('jge', 'jg'), ('jae', 'ja')):
        return c_imm == o_imm + 1
    if (c_next, o_next) in (('jg', 'jge'), ('ja', 'jae')):
        return o_imm == c_imm + 1
    if (c_next, o_next) in (('jl', 'jle'), ('jb', 'jbe')):
        return c_imm == o_imm + 1
    if (c_next, o_next) in (('jle', 'jl'), ('jbe', 'jb')):
        return o_imm == c_imm + 1
    return False


def memory_base_register(ops):
    match = re.search(r'\[\s*(\w+)', ops)
    if match:
        return match.group(1).lower()
    return None


def extract_memory_displacement(op):
    """Extract the additive displacement from a memory operand, including complex LEAs."""
    direct = extract_offset(op)
    if direct is not None:
        return direct
    match = re.search(r'\[(.*?)\]', op)
    if not match:
        return None
    inner = match.group(1).lower()
    inner = re.sub(r'\*\s*(?:0x[0-9a-f]+|\d+)', '', inner)
    total = 0
    found = False
    for sign, token in re.findall(r'([+-]?)\s*(0x[0-9a-f]+|\d+)', inner):
        value = int(token, 0)
        if sign == '-':
            value = -value
        total += value
        found = True
    return total if found else 0


def recent_lea_base_offset(instrs, idx, reg, window=8):
    """Return the offset of a recent `lea reg, [...]`, if that register defines the base."""
    reg = reg.lower()
    start = max(0, idx - window)
    for j in range(idx - 1, start - 1, -1):
        mnem, ops, _ = instrs[j]
        if mnem != 'lea':
            continue
        parts = split_operands(ops)
        if len(parts) != 2:
            continue
        if parts[0].strip().lower() != reg:
            continue
        return extract_memory_displacement(parts[1]) or 0
    return None


def recent_base_offset(instrs, idx, reg, window=96):
    """Return a recent base offset produced by LEA or by add/sub on the base register."""
    reg = reg.lower()
    start = max(0, idx - window)
    for j in range(idx - 1, start - 1, -1):
        mnem, ops, _ = instrs[j]
        if mnem == 'call' and reg in ('eax', 'ecx', 'edx'):
            return None
        parts = split_operands(ops)
        if len(parts) < 2:
            if mnem in ('pop', 'inc', 'dec', 'neg', 'not') and parts and parts[0].strip().lower() == reg:
                return None
            continue
        if parts[0].strip().lower() != reg:
            continue
        if mnem == 'lea':
            return extract_memory_displacement(parts[1]) or 0
        if mnem in ('add', 'sub'):
            imm = extract_standalone_immediate(parts[1])
            if imm is not None:
                return imm if mnem == 'add' else -imm
        return None
    return None


def recent_lea_effective_offsets_match(compiled_instrs, original_instrs, ci, oi, c_ops, o_ops):
    c_reg = memory_base_register(c_ops)
    o_reg = memory_base_register(o_ops)
    if not c_reg or c_reg != o_reg:
        return False
    c_off = extract_memory_displacement(c_ops)
    o_off = extract_memory_displacement(o_ops)
    if c_off is None or o_off is None:
        return False
    c_base = recent_base_offset(compiled_instrs, ci, c_reg)
    o_base = recent_base_offset(original_instrs, oi, o_reg)
    if c_base is None and o_base is None:
        return False
    return (c_base or 0) + c_off == (o_base or 0) + o_off


def recent_base_offsets_differ(compiled_instrs, original_instrs, ci, oi, c_ops, o_ops):
    c_reg = memory_base_register(c_ops)
    o_reg = memory_base_register(o_ops)
    if not c_reg or c_reg != o_reg:
        return False
    c_base = recent_base_offset(compiled_instrs, ci, c_reg)
    o_base = recent_base_offset(original_instrs, oi, o_reg)
    return c_base is not None and o_base is not None and c_base != o_base


def nearby_has_offset(instrs, idx, offset, mnem, window=10):
    start = max(0, idx - window)
    end = min(len(instrs), idx + window + 1)
    for j in range(start, end):
        if instrs[j][0] == mnem and extract_offset(instrs[j][1]) == offset:
            return True
    return False


def nearby_offset_swap(compiled_instrs, original_instrs, ci, oi, c_off, o_off, mnem):
    return (nearby_has_offset(compiled_instrs, ci, o_off, mnem) and
            nearby_has_offset(original_instrs, oi, c_off, mnem))


def nearby_has_memory_offset(instrs, idx, offset, window=24):
    start = max(0, idx - window)
    end = min(len(instrs), idx + window + 1)
    for j in range(start, end):
        if extract_memory_displacement(instrs[j][1]) == offset:
            return True
    return False


def nearby_lea_member_reorder(compiled_instrs, original_instrs, ci, oi, c_off, o_off):
    return (nearby_has_memory_offset(compiled_instrs, ci, o_off) and
            nearby_has_memory_offset(original_instrs, oi, c_off))


def nearby_push_immediates(instrs, idx, window=24):
    values = set()
    start = max(0, idx - window)
    end = min(len(instrs), idx + window + 1)
    for j in range(start, end):
        if instrs[j][0] != 'push':
            continue
        imm = extract_immediate(instrs[j][1])
        if imm is None:
            imm = recent_register_immediate(instrs, j, instrs[j][1].strip())
        if imm is not None:
            values.add(imm)
    return values


def recent_register_immediate(instrs, idx, reg, window=6):
    reg = reg.lower()
    if not re.fullmatch(r'e?[abcd]x|e?[sd]i|e?[sb]p', reg):
        return None
    start = max(0, idx - window)
    for j in range(idx - 1, start - 1, -1):
        mnem, ops, _ = instrs[j]
        parts = split_operands(ops)
        if len(parts) < 2:
            continue
        lhs = parts[0].lower()
        if lhs != reg:
            continue
        if mnem != 'mov':
            return None
        return extract_standalone_immediate(parts[-1])
    return None


def normalize_string_value(s):
    return normalize_string_for_cmp(s.strip().strip('"').strip('\\').strip('"'))


def nearby_push_strings(instrs, idx, strings_db=None, window=8):
    values = set()
    start = max(0, idx - window)
    end = min(len(instrs), idx + window + 1)
    for j in range(start, end):
        if instrs[j][0] != 'push':
            continue
        ops = instrs[j][1]
        text = extract_string_name(ops)
        if text is None and strings_db is not None:
            imm = extract_immediate(ops)
            if imm is not None:
                text = strings_db.get(imm)
        if text is not None:
            values.add(normalize_string_value(text))
    return values


def nearby_push_value_swap(compiled_instrs, original_instrs, ci, oi, c_imm, o_imm):
    c_values = nearby_push_immediates(compiled_instrs, ci)
    o_values = nearby_push_immediates(original_instrs, oi)
    return o_imm in c_values and c_imm in o_values


def nearby_push_string_swap(compiled_instrs, original_instrs, ci, oi, c_str, o_str, strings_db):
    c_norm = normalize_string_value(c_str)
    o_norm = normalize_string_value(o_str)
    c_values = nearby_push_strings(compiled_instrs, ci)
    o_values = nearby_push_strings(original_instrs, oi, strings_db)
    return o_norm in c_values and c_norm in o_values


def is_range_guard_cmp(instrs, idx):
    if idx + 1 >= len(instrs):
        return False
    mnem, ops, _ = instrs[idx]
    if mnem != 'cmp':
        return False
    parts = split_operands(ops)
    if len(parts) < 2:
        return False
    branch = normalize_branch_mnemonic(instrs[idx + 1][0])
    if branch not in ('ja', 'jae', 'jg', 'jge', 'jb', 'jbe', 'jl', 'jle'):
        return False
    lhs = parts[0].strip().lower()
    start = max(0, idx - 3)
    for j in range(idx - 1, start - 1, -1):
        prev_mnem, prev_ops, _ = instrs[j]
        prev_parts = split_operands(prev_ops)
        if len(prev_parts) < 2:
            continue
        if prev_parts[0].strip().lower() != lhs:
            continue
        if prev_mnem in ('add', 'sub') and extract_standalone_immediate(prev_parts[1]) is not None:
            return True
        return False
    return False


def check_values_for(function_name, disasm_path, out_dir, strings_db):
    """Run value check for a single function. Returns list of warnings."""
    similarity, _, _ = get_similarity(function_name, disasm_path, clean_build=False, out_dir=out_dir)
    if similarity is None:
        return None, []

    # Find compiled assembly
    produced_code = None
    for filename in os.listdir(out_dir):
        if filename.endswith(".asm"):
            filepath = os.path.join(out_dir, filename)
            with open(filepath, 'r') as f:
                content = f.read()
                if function_name in content:
                    result = read_assembly(function_name, filepath)
                    if result is not None:
                        produced_code, _ = result
                        break

    if produced_code is None:
        return similarity, []

    with open(disasm_path, 'rb') as f:
        target_code = f.read().decode('utf-8', errors='ignore').lower()
        target_code = "\n".join(target_code.splitlines()[3:])

    compiled_instrs = parse_full_instructions(produced_code)
    original_instrs = parse_full_instructions(target_code)

    compiled_mnemonics = [i[0] for i in compiled_instrs]
    original_mnemonics = [i[0] for i in original_instrs]

    matches = lcs_align(compiled_mnemonics, original_mnemonics)

    warnings = []
    for ci, oi in matches:
        c_mnem, c_ops, c_raw = compiled_instrs[ci]
        o_mnem, o_ops, o_raw = original_instrs[oi]

        if c_mnem != o_mnem:
            continue
        if c_mnem.startswith('j') or c_mnem == 'call':
            continue
        if 'FLAT:$L' in c_ops or 'FLAT:$T' in c_ops:
            continue

        # Skip offset comparisons when:
        # - Compiled uses symbolic variable names (offset relative to variable, not esp)
        # - Different base registers (misaligned LCS match, not comparable)
        # - SEH exception handler records
        has_symbolic = has_symbolic_memory_operand(c_ops)
        has_seh = 'EHRec' in c_ops
        c_off = extract_offset(c_ops)
        o_off = extract_offset(o_ops)
        if (c_off is not None and o_off is not None and c_off != o_off and
                not has_symbolic and not has_seh and comparable_offset_operands(c_ops, o_ops)):
            # Check base registers match
            c_reg = re.search(r'\[\s*(\w+)', c_ops)
            o_reg = re.search(r'\[\s*(\w+)', o_ops)
            same_base = c_reg and o_reg and c_reg.group(1).lower() == o_reg.group(1).lower()
            stack_base = same_base and c_reg.group(1).lower() in ('esp', 'ebp')
            if same_base and not stack_base:
                if (nearby_offset_swap(compiled_instrs, original_instrs, ci, oi, c_off, o_off, c_mnem) or
                        recent_lea_effective_offsets_match(compiled_instrs, original_instrs, ci, oi, c_ops, o_ops) or
                        recent_base_offsets_differ(compiled_instrs, original_instrs, ci, oi, c_ops, o_ops) or
                        (c_mnem == 'lea' and nearby_lea_member_reorder(compiled_instrs, original_instrs, ci, oi, c_off, o_off))):
                    continue
                warnings.append(('offset', ci, oi, c_off, o_off, c_raw.strip(), o_raw.strip()))

        if c_mnem == 'push':
            c_imm = extract_immediate(c_ops)
            o_imm = extract_immediate(o_ops)
            c_str = extract_string_name(c_ops)
            o_str = strings_db.get(o_imm) if o_imm is not None else None

            if c_imm is not None and o_imm is not None and c_imm != o_imm:
                if is_double_push_half_mismatch(c_imm, o_imm):
                    continue
                if nearby_push_value_swap(compiled_instrs, original_instrs, ci, oi, c_imm, o_imm):
                    continue
                # Skip if both are string references (mangled name truncation/encoding)
                if c_str and o_str:
                    o_str_clean = normalize_string_for_cmp(o_str.strip().strip('"').strip('\\').strip('"'))
                    c_str_norm = normalize_string_for_cmp(c_str)
                    # Skip truncated prefixes, undecoded mangled chars, or matching strings
                    if not has_undecoded_mangled(c_str_norm) and not o_str_clean.startswith(c_str_norm) and c_str_norm != o_str_clean:
                        if nearby_push_string_swap(compiled_instrs, original_instrs, ci, oi, c_str, o_str_clean, strings_db):
                            continue
                        detail = f'  compiled: "{c_str}" vs original: "{o_str_clean}"'
                        warnings.append(('string_mismatch', ci, oi, c_str, o_str_clean, c_raw.strip(), o_raw.strip(), detail))
                else:
                    detail = ""
                    if c_str:
                        detail += f'  compiled string: "{c_str}"'
                    if o_str:
                        detail += f'  original string: {o_str}'
                    warnings.append(('push_value', ci, oi, c_imm, o_imm, c_raw.strip(), o_raw.strip(), detail))
            elif c_str is not None and o_str is not None:
                o_str_clean = normalize_string_for_cmp(o_str.strip().strip('"').strip('\\').strip('"'))
                c_str_norm = normalize_string_for_cmp(c_str)
                # Skip truncated prefixes, undecoded mangled chars, or matching strings
                if not has_undecoded_mangled(c_str_norm) and not o_str_clean.startswith(c_str_norm) and c_str_norm != o_str_clean:
                    if nearby_push_string_swap(compiled_instrs, original_instrs, ci, oi, c_str, o_str_clean, strings_db):
                        continue
                    detail = f'  compiled: "{c_str}" vs original: "{o_str_clean}"'
                    warnings.append(('string_mismatch', ci, oi, c_str, o_str_clean, c_raw.strip(), o_raw.strip(), detail))

        if c_mnem in ('mov', 'cmp', 'add', 'sub', 'or', 'and', 'xor', 'test'):
            c_parts = split_operands(c_ops)
            o_parts = split_operands(o_ops)
            if len(c_parts) >= 2 and len(o_parts) >= 2:
                if c_mnem in ('add', 'sub') and c_parts[0].lower() == 'esp' and o_parts[0].lower() == 'esp':
                    continue
                if 'EHRec' in c_ops or 'EHRec' in o_ops:
                    continue
                if not lhs_operands_comparable(c_parts[0], o_parts[0]):
                    continue
                c_imm = extract_standalone_immediate(c_parts[-1])
                o_imm = extract_standalone_immediate(o_parts[-1])
                if c_imm is not None and o_imm is not None and c_imm != o_imm:
                    if c_mnem == 'cmp' and equivalent_integer_threshold(c_imm, o_imm, compiled_instrs, original_instrs, ci, oi):
                        continue
                    if c_mnem == 'cmp' and (is_range_guard_cmp(compiled_instrs, ci) or is_range_guard_cmp(original_instrs, oi)):
                        continue
                    if (is_memory_operand(c_parts[0]) and is_memory_operand(o_parts[0]) and
                            recent_base_offsets_differ(compiled_instrs, original_instrs, ci, oi, c_parts[0], o_parts[0])):
                        continue
                    if c_imm < 0x400000 and o_imm < 0x400000:
                        warnings.append(('imm_value', ci, oi, c_imm, o_imm, c_raw.strip(), o_raw.strip()))

    return similarity, warnings


def format_warning(w):
    kind = w[0]
    if kind == 'offset':
        _, ci, oi, cv, ov, c_raw, o_raw = w
        return f"    OFFSET 0x{cv:X} vs 0x{ov:X}: {c_raw}  |  {o_raw}"
    elif kind == 'push_value':
        _, ci, oi, cv, ov, c_raw, o_raw, detail = w
        line = f"    PUSH 0x{cv:X} vs 0x{ov:X}: {c_raw}  |  {o_raw}"
        if detail:
            line += f"\n      {detail.strip()}"
        return line
    elif kind == 'string_mismatch':
        _, ci, oi, cv, ov, c_raw, o_raw, detail = w
        return f"    STRING: {detail.strip()}"
    elif kind == 'imm_value':
        _, ci, oi, cv, ov, c_raw, o_raw = w
        return f"    IMM {cv} vs {ov}: {c_raw}  |  {o_raw}"
    return str(w)


def parse_args():
    parser = argparse.ArgumentParser(description="Run value checks for all implemented functions.")
    add_config_argument(parser)
    parser.add_argument("--demo", action="store_true", help="Use demo paths from config.")
    parser.add_argument("--filter", dest="file_filter", help="Only include files whose names contain this text.")
    parser.add_argument("--src-dir", help="Source directory override.")
    parser.add_argument("--code-dir", help="Ghidra export/disassembly directory override.")
    parser.add_argument("--out-dir", help="Compiled assembly output directory override.")
    parser.add_argument("--map-skip", help="Directory subtree to skip while scanning sources.")
    parser.add_argument("--strings", dest="strings_path", help="strings.txt path override.")
    parser.add_argument("--clean-target", help="Make target used to clean before rebuilding.")
    parser.add_argument("--build-target", help="Make target used to build assembly.")
    parser.add_argument("--jobs", type=int, help="Parallel make jobs.")
    parser.add_argument("--no-build", action="store_true", help="Skip clean/build and use existing output.")
    parser.add_argument("--min-similarity", type=float, default=0.0,
                        help="Only show functions whose similarity is at least this percentage (0-100). Default: 0.")
    return parser.parse_args()


def main():
    args = parse_args()
    try:
        config = load_config(args.config)
        paths = mode_paths(config, demo=args.demo)
        mode = "demo" if args.demo else "full"
        src_dir = config_or_arg(args.src_dir, paths, "src_dir", f"paths.{mode}.src_dir")
        code_dir = config_or_arg(args.code_dir, paths, "code_dir", f"paths.{mode}.code_dir")
        out_dir = config_or_arg(args.out_dir, paths, "out_dir", f"paths.{mode}.out_dir")
        clean_target = config_or_arg(args.clean_target, paths, "clean_target", f"paths.{mode}.clean_target")
        build_target = config_or_arg(args.build_target, paths, "build_target", f"paths.{mode}.build_target")
        map_skip = config_or_arg(args.map_skip, paths, "map_skip", f"paths.{mode}.map_skip")
        strings_path = args.strings_path or os.path.join(code_dir, "strings.txt")
        jobs = args.jobs if args.jobs is not None else int(config.get("build", {}).get("jobs", 1))
    except ConfigError as exc:
        print(f"error: {exc}", file=sys.stderr)
        sys.exit(2)

    if not args.no_build:
        print("Building...", file=sys.stderr)
        subprocess.run(["make", clean_target], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, check=False)
        build_command = ["make", build_target]
        if jobs and jobs > 1:
            build_command.append(f"-j{jobs}")
        ret = subprocess.run(build_command, check=False)
        if ret.returncode != 0:
            print("Build failed!", file=sys.stderr)
            sys.exit(1)

    strings_db = load_strings(strings_path)

    total = 0
    with_mismatches = 0
    total_warnings = 0
    skipped_below_threshold = 0
    threshold = args.min_similarity

    for root, _, files in os.walk(src_dir):
        if map_skip in root:
            continue
        for file in sorted(files):
            if not file.endswith(".cpp"):
                continue
            if args.file_filter and args.file_filter not in file:
                continue
            filepath = os.path.join(root, file)
            for source_function in parse_source_functions(filepath):
                address = source_function.address
                func_name = source_function.name

                disasm_file = f"{code_dir}/FUN_{address}.disassembled.txt"
                if not os.path.exists(disasm_file):
                    continue

                total += 1
                try:
                    similarity, warnings = check_values_for(func_name, disasm_file, out_dir, strings_db)
                    if warnings:
                        if similarity is not None and similarity < threshold:
                            skipped_below_threshold += 1
                            continue
                        with_mismatches += 1
                        total_warnings += len(warnings)
                        sim_str = f"{similarity:.1f}%" if similarity is not None else "?"
                        print(f"\n{func_name} (0x{address}, {sim_str}) - {len(warnings)} mismatch(es):")
                        for w in warnings:
                            print(format_warning(w))
                except Exception as e:
                    print(f"\n{func_name} (0x{address}): ERROR - {e}", file=sys.stderr)

    print(f"\n--- Summary ---")
    print(f"Functions checked: {total}")
    print(f"With value mismatches: {with_mismatches}")
    print(f"Total mismatches: {total_warnings}")
    if threshold > 0:
        print(f"Skipped (similarity < {threshold:.1f}%): {skipped_below_threshold}")


if __name__ == "__main__":
    main()
