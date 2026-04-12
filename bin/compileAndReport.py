#!/usr/bin/env python3

import os
import re
import sys
from compileAndCompare import get_similarity

def get_function_name(line):
    stripped = line.strip()
    if stripped.startswith('//') or stripped.startswith('/*') or stripped.startswith('*'):
        return None
    if stripped.startswith('if') or stripped.startswith('for') or stripped.startswith('while'):
        return None
    if stripped.startswith('{') or stripped.startswith('}') or stripped == '':
        return None

    # Class::Method or Class::~Method
    match = re.search(r'\b([a-zA-Z0-9_]+::~?[a-zA-Z0-9_]+)\s*\(', line)
    if match:
        return match.group(1)

    # __cdecl/__fastcall/__stdcall free functions
    match = re.search(r'(?:__cdecl|__fastcall|__stdcall)\s+\*?([a-zA-Z_][a-zA-Z0-9_]*)\s*\(', line)
    if match:
        return match.group(1)

    # Regular free functions: returntype functionname(
    match = re.search(r'^\s*(?:[\w\s\*]+)\s+\*?([a-zA-Z_][a-zA-Z0-9_]*)\s*\(', line)
    if match:
        func_name = match.group(1)
        if func_name not in ('if', 'for', 'while', 'switch', 'return', 'else', 'extern', 'typedef'):
            return func_name

    return None

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
    totals = {"count": 0, "sum": 0.0, "at100": 0, "above90": 0, "below90": 0, "errors": 0}

    for root, _, files in os.walk(src_dir):
        if map_skip in root:
            continue

        for file in sorted(files):
            if not file.endswith(".cpp"):
                continue
            if file_filter and file_filter not in file:
                continue
            filepath = os.path.join(root, file)
            with open(filepath, "r") as f:
                lines = f.readlines()

            for i, line in enumerate(lines):
                if "No assembly extracted" in line:
                    continue
                match = re.search(r"/\* Function start: 0x([0-9a-fA-F]+)", line)
                if not match:
                    continue
                address = match.group(1).upper()

                # Find function name on next non-empty lines
                func_name = None
                for j in range(i + 1, min(i + 5, len(lines))):
                    func_name = get_function_name(lines[j])
                    if func_name:
                        break

                if not func_name:
                    continue

                disassembled_file = f"{code_dir}/FUN_{address}.disassembled.txt"
                if not os.path.exists(disassembled_file):
                    report.append((file, func_name, address, "N/A"))
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
    print(f"  Average similarity: {avg:.2f}%")

if __name__ == "__main__":
    main()
