#!/usr/bin/env python3

import os
import sys
from compileAndCompare import get_similarity
from cppSourceParser import parse_source_functions

def main():
    demo_mode = '--demo' in sys.argv
    file_filter = None
    if '--filter' in sys.argv:
        idx = sys.argv.index('--filter')
        if idx + 1 < len(sys.argv):
            file_filter = sys.argv[idx + 1]
    if demo_mode:
        src_dir = "src-demo"
        code_dir = "code"
        out_dir = "out-demo"
        clean_target = "clean-demo"
        build_target = "demo"
        map_skip = "src-demo/map"
    else:
        src_dir = "src"
        code_dir = "code-full"
        out_dir = "out"
        clean_target = "clean"
        build_target = "all"
        map_skip = "src/map"

    # Clean and build once
    print("Building...", file=sys.stderr)
    os.system(f"make {clean_target} > /dev/null 2>&1")
    ret = os.system(f"make {build_target} -j12")
    if ret != 0:
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
            if file_filter and file_filter not in file:
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
