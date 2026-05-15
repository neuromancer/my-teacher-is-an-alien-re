#!/usr/bin/env python3

import argparse
import os
import subprocess
import sys
from compileAndCompare import get_similarity
from cppSourceParser import parse_source_functions
from projectConfig import ConfigError, add_config_argument, config_or_arg, load_config, mode_paths


def parse_args():
    parser = argparse.ArgumentParser(description="Build and report source/original function similarity.")
    add_config_argument(parser)
    parser.add_argument("--demo", action="store_true", help="Use demo paths from config.")
    parser.add_argument("--filter", dest="file_filter", help="Only include files whose names contain this text.")
    parser.add_argument("--src-dir", help="Source directory override.")
    parser.add_argument("--code-dir", help="Ghidra export/disassembly directory override.")
    parser.add_argument("--out-dir", help="Compiled assembly output directory override.")
    parser.add_argument("--map-skip", help="Directory subtree to skip while scanning sources.")
    parser.add_argument("--clean-target", help="Make target used to clean before rebuilding.")
    parser.add_argument("--build-target", help="Make target used to build assembly.")
    parser.add_argument("--jobs", type=int, help="Parallel make jobs.")
    parser.add_argument("--no-build", action="store_true", help="Skip clean/build and use existing output.")
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
        jobs = args.jobs if args.jobs is not None else int(config.get("build", {}).get("jobs", 1))
    except ConfigError as exc:
        print(f"error: {exc}", file=sys.stderr)
        sys.exit(2)

    # Clean and build once
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

    report = []
    missing_asm = []
    totals = {
        "count": 0,
        "sum": 0.0,
        "at100": 0,
        "above90": 0,
        "below90": 0,
        "errors": 0,
        "missing_asm": 0,
    }

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

                disassembled_file = f"{code_dir}/FUN_{address}.disassembled.txt"
                if not os.path.exists(disassembled_file):
                    pct = "MISSING ASM"
                    totals["missing_asm"] += 1
                    missing_asm.append((file, func_name, address, disassembled_file))
                    report.append((file, func_name, address, pct))
                    continue

                try:
                    similarity, _, _ = get_similarity(func_name, disassembled_file, clean_build=False, out_dir=out_dir)
                    if similarity is not None:
                        pct = f"{similarity:.2f}%"
                        totals["count"] += 1
                        totals["sum"] += similarity
                        if similarity >= 99.99:
                            totals["at100"] += 1
                        if similarity >= 90.0:
                            totals["above90"] += 1
                        else:
                            totals["below90"] += 1
                    else:
                        pct = "NOT FOUND"
                        totals["errors"] += 1
                except Exception as e:
                    pct = f"ERR: {e}"
                    totals["errors"] += 1

                report.append((file, func_name, address, pct))

    # Print report
    print("\n--- Similarity Report ---")
    current_file = None
    for file, func_name, address, similarity in report:
        if file != current_file:
            print(f"\n=== {file} ===")
            current_file = file
        print(f"  {func_name:45s} 0x{address}  {similarity}")

    # Summary
    n = totals["count"]
    avg = totals["sum"] / n if n > 0 else 0
    print(f"\n--- Summary ---")
    print(f"Total compared: {n}")
    print(f"  100%%: {totals['at100']}")
    print(f"  >=90%%: {totals['above90']}")
    print(f"  <90%%: {totals['below90']}")
    print(f"  Errors/NOT FOUND: {totals['errors']}")
    print(f"  Missing Ghidra asm: {totals['missing_asm']}")
    print(f"  Average similarity: {avg:.2f}%")

    if missing_asm:
        print("\n--- Missing Ghidra assembly exports ---")
        for file, func_name, address, disassembled_file in missing_asm:
            print(f"  {file:30s} {func_name:45s} 0x{address}  {disassembled_file}")

if __name__ == "__main__":
    main()
