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
        has_symbolic = bool(re.search(r'_\w+\$\[', c_ops))
        has_seh = 'EHRec' in c_ops
        c_off = extract_offset(c_ops)
        o_off = extract_offset(o_ops)
        if c_off is not None and o_off is not None and c_off != o_off and not has_symbolic and not has_seh:
            # Check base registers match
            c_reg = re.search(r'\[\s*(\w+)', c_ops)
            o_reg = re.search(r'\[\s*(\w+)', o_ops)
            if c_reg and o_reg and c_reg.group(1).lower() == o_reg.group(1).lower():
                warnings.append(('offset', ci, oi, c_off, o_off, c_raw.strip(), o_raw.strip()))

        if c_mnem == 'push':
            c_imm = extract_immediate(c_ops)
            o_imm = extract_immediate(o_ops)
            c_str = extract_string_name(c_ops)
            o_str = strings_db.get(o_imm) if o_imm is not None else None

            if c_imm is not None and o_imm is not None and c_imm != o_imm:
                # Skip if both are string references (mangled name truncation/encoding)
                if c_str and o_str:
                    o_str_clean = normalize_string_for_cmp(o_str.strip().strip('"').strip('\\').strip('"'))
                    c_str_norm = normalize_string_for_cmp(c_str)
                    # Skip truncated prefixes, undecoded mangled chars, or matching strings
                    if not has_undecoded_mangled(c_str_norm) and not o_str_clean.startswith(c_str_norm) and c_str_norm != o_str_clean:
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
                    detail = f'  compiled: "{c_str}" vs original: "{o_str_clean}"'
                    warnings.append(('string_mismatch', ci, oi, c_str, o_str_clean, c_raw.strip(), o_raw.strip(), detail))

        if c_mnem in ('mov', 'cmp', 'add', 'sub', 'or', 'and', 'xor', 'test'):
            c_parts = c_ops.split(',')
            o_parts = o_ops.split(',')
            if len(c_parts) >= 2 and len(o_parts) >= 2:
                c_imm = extract_immediate(c_parts[-1])
                o_imm = extract_immediate(o_parts[-1])
                if c_imm is not None and o_imm is not None and c_imm != o_imm:
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
