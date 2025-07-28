import re
import Levenshtein

from os import system
from time import sleep
from argparse import ArgumentParser


def read_assembly(function_name, file_path):
    # Read the  file with the assembly code
    with open(file_path, 'r') as file:
        assembly = file.read()

    # Parse assembly code
    if "::" in function_name:
        if f"; {function_name}, COMDAT" in assembly:
            assembly = assembly.split(f"; {function_name}, COMDAT")[1]
            assembly = assembly.split(f"; {function_name}")[0]
        else:
            # Use regex to handle variable whitespace
            match = re.search(fr"PROC NEAR\s*;\s*{re.escape(function_name)}", assembly)
            if match:
                assembly = assembly.split(match.group(0))[1]
                assembly = assembly.split(f"ENDP")[0]
            else:
                raise IndexError(f"Function '{function_name}' not found in assembly with non-COMDAT format.")
    else:
        if function_name.startswith("?"):
            match = re.search(f"{re.escape(function_name)} PROC NEAR", assembly)
            if match:
                assembly = assembly.split(f"{function_name} PROC NEAR")[1]
                assembly = assembly.split(f"ENDP")[0]
        elif function_name == "ShowError":
            assembly = assembly.split(f"?ShowError@@YAXPADZZ PROC NEAR")[1]
            assembly = assembly.split(f"?ShowError@@YAXPADZZ ENDP")[0]
        elif function_name == "_SetVideoMode":
            assembly = assembly.split(f"?_SetVideoMode@@YAHH@Z PROC NEAR")[1]
            assembly = assembly.split(f"?_SetVideoMode@@YAHH@Z ENDP")[0]
        elif function_name == "InvalidateVideoMode":
            assembly = assembly.split(f"?InvalidateVideoMode@@YAHXZ PROC NEAR")[1]
            assembly = assembly.split(f"?InvalidateVideoMode@@YAHXZ ENDP")[0]
        elif function_name == "ClearScreen":
            assembly = assembly.split(f"?ClearScreen@@YAXXZ PROC NEAR")[1]
            assembly = assembly.split(f"?ClearScreen@@YAXXZ ENDP")[0]
        else:
            if function_name == "FUN_004260f0":
                assembly = assembly.split(f"?FUN_004260f0@@YAXPAPAX0@Z PROC NEAR")[1]
                assembly = assembly.split(f"?FUN_004260f0@@YAXPAPAX0@Z ENDP")[0]
            else:
                assembly = assembly.split(f"_{function_name} PROC")[1]
                assembly = assembly.split(f"_{function_name} ENDP")[0]

    # Discard the lines that start with ";"
    assembly = "\n".join([line for line in assembly.split("\n") if not line.strip().startswith(";")])

    # Discard empty lines
    assembly = "\n".join([line for line in assembly.split("\n") if line.strip() != ""])

    # Remove the header
    if "PROC NEAR" in assembly:
        assembly = assembly.split("PROC NEAR")[1]
        assembly = assembly.split("ENDP")[0]

    #print(assembly)
    return assembly
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
            parts = line.split(None, 1)
            mnemonic = parts[0]
            if mnemonic in normalization_map:
                mnemonic = normalization_map[mnemonic]
            mnemonics.append(mnemonic)
    return mnemonics

def side_by_side(str1, str2, tab_size=4):
    """
    Combine two multi-line strings side by side with proper padding.
    Handles tabs in the input strings by expanding them to spaces.

    Args:
        str1 (str): First string
        str2 (str): Second string
        tab_size (int, optional): Number of spaces to replace each tab. Defaults to 4.

    Returns:
        str: Combined string with lines from both inputs side by side
    """
    # Replace tabs with spaces in both strings
    str1 = str1.replace('\t', ' ' * tab_size)
    str2 = str2.replace('\t', ' ' * tab_size)

    # Split strings into lines
    lines1 = str1.split('\n')
    lines2 = str2.split('\n')

    # Find the maximum length of lines in the first string
    max_length = max(len(line) for line in lines1) if lines1 else 0
    max_length = min(max_length, 48)

    # Determine how many lines to process
    max_lines = max(len(lines1), len(lines2))

    # Build the result
    result = []
    for i in range(max_lines):
        # Get lines or empty strings if we're past the end
        line1 = lines1[i] if i < len(lines1) else ""
        line2 = lines2[i] if i < len(lines2) else ""

        if (len(line1) > max_length - 2):
            line1 = line1[:max_length - 2] + ".." # Truncate line1 to max_length

        if (len(line2) > max_length - 2):
            # Truncate line2 to max_length
            line2 = line2[:max_length - 2] + ".."

        # Pad the first line to the maximum length
        padded_line1 = line1.ljust(max_length)

        # Combine the lines with separator
        combined = f"{padded_line1} | {line2}"
        result.append(combined)

    # Join all lines with newlines and return
    return '\n'.join(result)

def save_code(name, code, iteration):
    # Save code to src/recovered/name.iteration.c
    with open(f"src/recovered/{name}.{iteration}.c", 'w') as file:
        file.write(code)

def main():

    # Parse arguments using argparse
    parser = ArgumentParser(description="Recover high-level code from assembly code")
    parser.add_argument("function_name", help="Function name to compare")
    parser.add_argument("file_name", help="Path to reconstructed source code")
    parser.add_argument("disassembled_code", help="Path to the disassembled code from Ghidra")
    parser.add_argument("--mangled_name", help="Mangled name of the function to compare")

    args = parser.parse_args()

    system(f"wine bin/compile.bat {args.file_name} 2> /dev/null")
    produced_code = read_assembly(args.mangled_name if args.mangled_name else args.function_name, "out/code.asm.txt")

    with open(args.disassembled_code, 'rb') as file:
        target_code = file.read()
        target_code = target_code.decode('utf-8', errors='ignore').lower()
        target_code = "\n".join(target_code.splitlines()[3:])

    #print(target_code)
    print(side_by_side(produced_code, target_code))

    produced_mnemonics = parse_mnemonics(produced_code)
    target_mnemonics = parse_mnemonics(target_code)

    distance = Levenshtein.distance(produced_mnemonics, target_mnemonics)
    max_len = max(len(produced_mnemonics), len(target_mnemonics))
    if max_len == 0:
        similarity = 100.0
    else:
        similarity = (1 - distance / max_len) * 100

    print(f"\nSimilarity: {similarity:.2f}%")

if __name__ == "__main__":
    main()
