#!/usr/bin/env python3

import re
import Levenshtein
import os
import sys

from os import system
from time import sleep
from argparse import ArgumentParser

def read_assembly(function_name, file_path):
    # Read the file with the assembly code
    with open(file_path, 'r') as file:
        assembly = file.read()

    mangled_name = function_name
    if '::' in function_name:
        name, clss = function_name.split('::')
        mangled_name = f"{clss}@{name}"

    if f";	COMDAT ?{mangled_name}@" in assembly:
        print("Stack positions in assembly code:", end="")
        # If the function is a COMDAT, we need to handle it differently
        symbols = assembly.split(f";	COMDAT ?{mangled_name}@")[2]
        symbols = symbols.split(f"_TEXT	SEGMENT")[1]
        symbols = symbols.split(f"?{mangled_name}@")[0]
        print(symbols, end="")

    # Parse assembly code
    if f"; {function_name}, COMDAT" in assembly:
        assembly = assembly.split(f"; {function_name}, COMDAT")[1]
        assembly = assembly.split(f"; {function_name}")[0]
        # Also remove lines containing ENDP (since PROC NEAR was before the split point)
        lines = assembly.split("\n")
        assembly = "\n".join([line for line in lines if "ENDP" not in line])
    elif f"_{function_name} PROC NEAR" in assembly:
        assembly = assembly.split(f"_{function_name} PROC NEAR")[1]
        assembly = assembly.split(f"_{function_name} ENDP")[0]
    else:
        # Try to find stdcall decoration _Name@N
        match = re.search(f"_{function_name}@[0-9]+ PROC NEAR", assembly)
        if match:
            start_marker = match.group(0)
            end_marker = start_marker.replace("PROC NEAR", "ENDP")
            if start_marker in assembly and end_marker in assembly:
                assembly = assembly.split(start_marker)[1]
                assembly = assembly.split(end_marker)[0]
            else:
                return None
        else:
            return None

    # Discard the lines that start with ";"
    assembly = "\n".join([line for line in assembly.split("\n") if not line.strip().startswith(";")])

    # Discard empty lines
    assembly = "\n".join([line for line in assembly.split("\n") if line.strip() != ""])

    # Remove the header
    if "PROC NEAR" in assembly:
        assembly = assembly.split("PROC NEAR")[1]
        assembly = assembly.split("ENDP")[0]

    # Remove SEH handler code that appears after the main function body
    # SEH handlers come after the final ret and have patterns like:
    #   $L30694:  mov eax, OFFSET FLAT:$Lxxxxx  ret 0
    #   $L30695:  mov eax, OFFSET FLAT:$Txxxxx  jmp ___CxxFrameHandler
    # We find the last "ret" that's followed only by SEH handlers and remove everything after
    lines = assembly.split("\n")
    stripped_lines = [line.strip() for line in lines]

    # First, identify labels that are referenced by jump tables (switch statements)
    # These should NOT be treated as SEH handlers
    jump_table_targets = set()
    for line in stripped_lines:
        # Match DD OFFSET FLAT:$Lxxxx patterns (jump table entries)
        match = re.search(r'DD\s+OFFSET\s+FLAT:(\$L\d+)', line)
        if match:
            jump_table_targets.add(match.group(1))
        # Also check for indirect jump patterns like jmp DWORD PTR $Lxxxx[eax*4]
        match = re.search(r'jmp\s+DWORD\s+PTR\s+(\$L\d+)\[', line, re.IGNORECASE)
        if match:
            # This is a jump table reference - the table itself will have DD entries
            pass

    # Find all ret instruction indices
    ret_indices = []
    for i, line in enumerate(stripped_lines):
        # Strip inline comments before checking for ret
        line_no_comment = line.split(';')[0].strip()
        # Match ret with optional whitespace and optional number (ret, ret 0, ret 4, etc.)
        if re.match(r'^ret(\s+\d+)?$', line_no_comment):
            ret_indices.append(i)

    # For each ret from the end, check if everything after is just SEH handlers or data
    # We want the LAST ret after which only SEH code or data follows
    cutoff_idx = len(lines)
    for ret_idx in reversed(ret_indices):
        # Check what's after this ret
        remaining = stripped_lines[ret_idx+1:]
        is_seh_or_data_only = True
        i = 0
        while i < len(remaining):
            line = remaining[i]
            if not line:  # Empty line
                i += 1
                continue
            # Check for label pattern
            label_match = re.match(r'(\$L\d+):', line)
            if label_match:
                label_name = label_match.group(1)
                # If this label is a jump table target, it's switch case code, not SEH
                if label_name in jump_table_targets:
                    is_seh_or_data_only = False
                    break
                # Look at next two instructions for SEH handler pattern
                if i + 1 < len(remaining):
                    next_line = remaining[i + 1]
                    if 'OFFSET FLAT:$' in next_line or 'jmp' in next_line.lower():
                        i += 1
                        continue
                i += 1
                continue
            elif 'OFFSET FLAT:$' in line:
                i += 1
                continue
            elif line.startswith('ret') or line == 'ret 0':
                i += 1
                continue
            elif 'jmp' in line.lower():
                # SEH handlers jump to CxxFrameHandler or to cleanup funclets (mangled names)
                # Mangled names start with ? or ?? and contain @@
                if 'CxxFrameHandler' in line or '??' in line or ('?' in line and '@@' in line):
                    i += 1
                    continue
                else:
                    is_seh_or_data_only = False
                    break
            elif line.startswith('mov') or line.startswith('push') or line.startswith('call') or line.startswith('add'):
                # After SEH label, mov/push/call/add instructions are part of the cleanup funclet
                i += 1
                continue
            # Check for jump table data directives (DD, DB, npad, etc.)
            elif line.startswith('DD') or line.startswith('DB') or line.startswith('DW'):
                i += 1
                continue
            elif line.startswith('npad'):
                i += 1
                continue
            else:
                # Non-SEH/non-data code found after ret
                is_seh_or_data_only = False
                break

        if is_seh_or_data_only and len(remaining) > 0:
            cutoff_idx = ret_idx + 1
            break

    assembly = "\n".join(lines[:cutoff_idx])
    seh_code = "\n".join(lines[cutoff_idx:])

    return assembly, seh_code

def parse_mnemonics(assembly_code):
    normalization_map = {
        'je': 'jz',
        'jne': 'jnz',
        'jb': 'jc',
        'jae': 'jnc',
        'jg': 'jnle',
        'jge': 'jnl',
        'jl': 'jnge',
        'jle': 'jng',
    }
    mnemonics = []
    for line in assembly_code.split('\n'):
        line = line.strip()
        if line and not line.endswith(':') and not line.startswith('?'):
            parts = line.split()
            if not parts:
                continue
            
            mnemonic = parts[0]

            # Handle Ghidra style .rep suffix
            if mnemonic.endswith('.rep'):
                 mnemonic = 'rep ' + mnemonic[:-4]
            
            # Handle MSVC style rep prefix
            # If the mnemonic is a prefix, we include the next token as part of the mnemonic
            if mnemonic in ['rep', 'repe', 'repne', 'lock'] and len(parts) > 1:
                # Ensure the next part is not a comment
                if not parts[1].startswith(';'):
                    mnemonic = f"{mnemonic} {parts[1]}"

            if mnemonic in normalization_map:
                mnemonic = normalization_map[mnemonic]
            mnemonics.append(mnemonic)
    return mnemonics

def side_by_side(str1, str2, tab_size=4):
    str1 = str1.replace('\t', ' ' * tab_size)
    str2 = str2.replace('\t', ' ' * tab_size)
    lines1 = str1.split('\n')
    lines2 = str2.split('\n')
    max_length = max(len(line) for line in lines1) if lines1 else 0
    max_length = min(max_length, 48)
    max_lines = max(len(lines1), len(lines2))
    result = []
    for i in range(max_lines):
        line1 = lines1[i] if i < len(lines1) else ""
        line2 = lines2[i] if i < len(lines2) else ""
        if (len(line1) > max_length - 2):
            line1 = line1[:max_length - 2] + ".."
        if (len(line2) > max_length - 2):
            line2 = line2[:max_length - 2] + ".."
        padded_line1 = line1.ljust(max_length)
        combined = f"{padded_line1} | {line2}"
        result.append(combined)
    return '\n'.join(result)

def get_similarity(function_name, disassembled_code_path, clean_build=True):
    if clean_build:
        system("make clean")
        if system("make all -j12") != 0:
            print("Make failed")
            exit(1)

    asm_file_path = None
    produced_code = None
    for filename in os.listdir("out"):
        if filename.endswith(".asm"):
            filepath = os.path.join("out", filename)
            with open(filepath, 'r') as f:
                content = f.read()
                if function_name in content:
                    asm_file_path = filepath
                    result = read_assembly(function_name, filepath)
                    if result is not None:
                        produced_code, seh_code = result
                        break

    if asm_file_path is None:
        return None, "Function not found in any .asm file.", None

    if produced_code is None:
        return None, "Function found but could not extract assembly.", None

    with open(disassembled_code_path, 'rb') as file:
        target_code = file.read()
        target_code = target_code.decode('utf-8', errors='ignore').lower()
        target_code = "\n".join(target_code.splitlines()[3:])

    produced_mnemonics = parse_mnemonics(produced_code)
    target_mnemonics = parse_mnemonics(target_code)

    distance = Levenshtein.distance(produced_mnemonics, target_mnemonics)
    max_len = max(len(produced_mnemonics), len(target_mnemonics))
    if max_len == 0:
        similarity = 100.0
    else:
        similarity = (1 - distance / max_len) * 100

    return similarity, side_by_side(produced_code, target_code), seh_code

def main():
    parser = ArgumentParser(description="Recover high-level code from assembly code")
    parser.add_argument("function_name", help="Function name to compare")
    parser.add_argument("disassembled_code", help="Path to the disassembled code from Ghidra")
    args = parser.parse_args()

    similarity, comparison_text, seh_code = get_similarity(args.function_name, args.disassembled_code)

    print (f"Comparison for function '{args.function_name}':")
    if similarity is None:
        print(comparison_text)
        sys.exit(1)

    print(comparison_text)
    print(f"\nSimilarity: {similarity:.2f}%")

    if seh_code and seh_code.strip():
        print(f"\nDetected SEH code (excluded from comparison):")
        print(seh_code)

if __name__ == "__main__":
    main()
