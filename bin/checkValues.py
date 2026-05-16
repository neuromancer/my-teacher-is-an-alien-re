#!/usr/bin/env python3
"""Check operand value differences between compiled and original assembly.

Usage: python3 bin/checkValues.py "Class::Method" code-full/FUN_XXXX.disassembled.txt

The similarity tool only compares opcodes (push == push regardless of operand).
This script finds lines where opcodes match but operand VALUES differ,
revealing bugs like wrong string constants, wrong offsets, or wrong immediates.
"""

import re
import sys
import os
from compileAndCompare import get_similarity, read_assembly, parse_mnemonics

def parse_full_instructions(assembly_code):
    """Parse assembly into list of (mnemonic, operands, raw_line) tuples."""
    normalization_map = {
        'je': 'jz', 'jne': 'jnz', 'jb': 'jc', 'jae': 'jnc',
        'jg': 'jnle', 'jge': 'jnl', 'jl': 'jnge', 'jle': 'jng',
    }
    instructions = []
    for line in assembly_code.split('\n'):
        line = line.strip()
        if line and not line.endswith(':') and not line.startswith('?'):
            parts = line.split()
            if not parts:
                continue
            mnemonic = parts[0]
            if mnemonic == 'npad':
                continue
            # Handle rep/repne prefixes
            if mnemonic.endswith('.rep'):
                mnemonic = 'rep ' + mnemonic[:-4]
            elif mnemonic.endswith('.repne'):
                mnemonic = 'repnz ' + mnemonic[:-6]
            elif mnemonic.endswith('.repe'):
                mnemonic = 'repe ' + mnemonic[:-5]
            if mnemonic in ['rep', 'repe', 'repne', 'repnz', 'lock'] and len(parts) > 1:
                if not parts[1].startswith(';'):
                    mnemonic = f"{mnemonic} {parts[1]}"
            if mnemonic in normalization_map:
                mnemonic = normalization_map[mnemonic]
            # Get operands (everything after mnemonic, before comment)
            operand_str = ' '.join(parts[1:] if mnemonic.count(' ') == 0 else parts[2:])
            operand_str = operand_str.split(';')[0].strip()
            instructions.append((mnemonic, operand_str, line))
    return instructions


def normalize_operand(op):
    """Normalize an operand for comparison - strip type prefixes, normalize hex."""
    op = op.strip()
    # Remove DWORD PTR, BYTE PTR, WORD PTR etc.
    op = re.sub(r'\b(DWORD|BYTE|WORD|QWORD)\s+PTR\s+', '', op, flags=re.IGNORECASE)
    op = re.sub(r'\bdword\s+ptr\s+', '', op, flags=re.IGNORECASE)
    # Remove OFFSET FLAT:
    op = re.sub(r'OFFSET\s+FLAT:', '', op)
    # Remove segment prefixes
    op = re.sub(r'\b[CDEFGS]S:', '', op, flags=re.IGNORECASE)
    return op.strip()


def extract_immediate(op):
    """Extract immediate numeric value from operand if present."""
    op = normalize_operand(op)
    # Match hex: 0x1234 or 1234H or plain decimal
    m = re.search(r'\b0x([0-9a-fA-F]+)\b', op)
    if m:
        return int(m.group(1), 16)
    m = re.search(r'\b([0-9a-fA-F]+)H\b', op, re.IGNORECASE)
    if m:
        try:
            return int(m.group(1), 16)
        except ValueError:
            pass
    # Plain decimal in compiled output (e.g., "push 144")
    m = re.match(r'^(\d+)$', op)
    if m:
        return int(m.group(1))
    return None


def extract_offset(op):
    """Extract memory offset like [reg + 0x90] or [reg + 144]."""
    op = normalize_operand(op)
    # Match [anything + number] or [anything - number]
    m = re.search(r'\[\s*\w+\s*[+\-]\s*(?:0x)?([0-9a-fA-F]+)\s*\]', op, re.IGNORECASE)
    if m:
        try:
            val = m.group(1)
            if '0x' in op[m.start():m.end()].lower():
                return int(val, 16)
            return int(val)
        except ValueError:
            pass
    return None


def extract_string_name(op):
    """Extract string content from MSVC mangled string name."""
    # ??_C@_0BJ@EFEM@illegal?5modual?5insertion?$AA@
    # ?5 = space, ?$AA = null terminator, ?3 = /
    m = re.search(r'\?\?_C@[^@]+@[^@]+@([^@]+)@', op)
    if m:
        s = m.group(1)
        s = s.replace('?$CF', '%').replace('?$CB', '!').replace('?$AA', '')
        s = s.replace('?$CC', '"').replace('?$CI', '(').replace('?$CJ', ')')
        s = s.replace('?5', ' ').replace('?3', ':').replace('?2', '\\')
        s = s.replace('?1', '/').replace('?9', '-').replace('?8', "'")
        s = s.replace('?4', '.').replace('?0', ',').replace('?7', '\t')
        s = s.replace('?6', '\n')
        s = s.replace('?$FL', '[').replace('?$FN', ']')
        return s
    return None


def lcs_align(seq1, seq2):
    """Align two sequences using LCS. Returns list of (i1, i2) pairs or (i, None)/(None, i)."""
    n, m = len(seq1), len(seq2)
    # Build LCS table
    dp = [[0] * (m + 1) for _ in range(n + 1)]
    for i in range(1, n + 1):
        for j in range(1, m + 1):
            if seq1[i-1] == seq2[j-1]:
                dp[i][j] = dp[i-1][j-1] + 1
            else:
                dp[i][j] = max(dp[i-1][j], dp[i][j-1])

    # Backtrack to find alignment
    alignment = []
    i, j = n, m
    matches = []
    while i > 0 and j > 0:
        if seq1[i-1] == seq2[j-1]:
            matches.append((i-1, j-1))
            i -= 1
            j -= 1
        elif dp[i-1][j] > dp[i][j-1]:
            i -= 1
        else:
            j -= 1
    matches.reverse()
    return matches


def load_strings(strings_path):
    """Load address->string mapping from strings.txt."""
    strings = {}
    if not os.path.exists(strings_path):
        return strings
    with open(strings_path, 'r') as f:
        for line in f:
            m = re.match(r'0x([0-9a-fA-F]+):\s*"(.*)"', line.strip())
            if m:
                addr = int(m.group(1), 16)
                strings[addr] = m.group(2)
    return strings


def main():
    if len(sys.argv) < 3:
        print(f"Usage: {sys.argv[0]} 'Class::Method' code-full/FUN_XXXX.disassembled.txt")
        sys.exit(1)

    function_name = sys.argv[1]
    disasm_path = sys.argv[2]
    demo = '--demo' in sys.argv

    out_dir = "out-demo" if demo else "out"

    # Build first
    similarity, comparison, seh_code = get_similarity(function_name, disasm_path, out_dir=out_dir)
    if similarity is None:
        print(f"Error: {comparison}")
        sys.exit(1)

    print(f"Similarity: {similarity:.2f}%\n")

    # Re-extract the assembly
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

    with open(disasm_path, 'rb') as f:
        target_code = f.read().decode('utf-8', errors='ignore').lower()
        target_code = "\n".join(target_code.splitlines()[3:])

    # Parse full instructions
    compiled_instrs = parse_full_instructions(produced_code)
    original_instrs = parse_full_instructions(target_code)

    # Get mnemonics for alignment
    compiled_mnemonics = [i[0] for i in compiled_instrs]
    original_mnemonics = [i[0] for i in original_instrs]

    # Align using LCS on mnemonics
    matches = lcs_align(compiled_mnemonics, original_mnemonics)

    # Load strings
    strings_path = "code-full/strings.txt" if not demo else "code/strings.txt"
    strings_db = load_strings(strings_path)

    # Check value differences in matched pairs
    warnings = []
    for ci, oi in matches:
        c_mnem, c_ops, c_raw = compiled_instrs[ci]
        o_mnem, o_ops, o_raw = original_instrs[oi]

        if c_mnem != o_mnem:
            continue  # shouldn't happen in LCS match, but safety

        # Skip branch/jump/call - addresses always differ
        if c_mnem.startswith('j') or c_mnem == 'call':
            continue
        # Skip label refs and SEH
        if 'FLAT:$L' in c_ops or 'FLAT:$T' in c_ops:
            continue

        # Compare offsets in memory operands
        c_off = extract_offset(c_ops)
        o_off = extract_offset(o_ops)
        if c_off is not None and o_off is not None and c_off != o_off:
            warnings.append(('offset', ci, oi, c_off, o_off, c_raw.strip(), o_raw.strip()))

        # Compare push values - handle both numeric and string references
        if c_mnem == 'push':
            c_imm = extract_immediate(c_ops)
            o_imm = extract_immediate(o_ops)
            c_str = extract_string_name(c_ops)
            o_str = strings_db.get(o_imm) if o_imm is not None else None

            # Case 1: both numeric - compare values
            if c_imm is not None and o_imm is not None and c_imm != o_imm:
                detail = ""
                if c_str:
                    detail += f'  compiled string: "{c_str}"'
                if o_str:
                    detail += f'  original string: {o_str}'
                warnings.append(('push_value', ci, oi, c_imm, o_imm, c_raw.strip(), o_raw.strip(), detail))

            # Case 2: compiled is string symbol, original is address - compare string content
            elif c_str is not None and o_str is not None:
                # Normalize: strings.txt has format: "\"actual string\""
                o_str_clean = o_str.strip().strip('"').strip('\\').strip('"')
                if c_str != o_str_clean:
                    detail = f'  compiled string: "{c_str}"\n    original string: "{o_str_clean}"'
                    c_val = c_str
                    o_val = o_str_clean
                    warnings.append(('string_mismatch', ci, oi, c_val, o_val, c_raw.strip(), o_raw.strip(), detail))

        # Compare mov/cmp immediate values
        if c_mnem in ('mov', 'cmp', 'add', 'sub', 'or', 'and', 'xor', 'test'):
            # Extract the immediate from the last operand
            c_parts = c_ops.split(',')
            o_parts = o_ops.split(',')
            if len(c_parts) >= 2 and len(o_parts) >= 2:
                c_imm = extract_immediate(c_parts[-1])
                o_imm = extract_immediate(o_parts[-1])
                if c_imm is not None and o_imm is not None and c_imm != o_imm:
                    # Filter out address/label differences (large values are likely addresses)
                    if c_imm < 0x400000 and o_imm < 0x400000:
                        warnings.append(('imm_value', ci, oi, c_imm, o_imm, c_raw.strip(), o_raw.strip()))

    if not warnings:
        print("No operand value differences found in matched instructions.")
    else:
        print(f"Found {len(warnings)} operand value difference(s):\n")
        for w in warnings:
            kind = w[0]
            if kind == 'offset':
                _, ci, oi, cv, ov, c_raw, o_raw = w
                print(f"  OFFSET MISMATCH (compiled 0x{cv:X} vs original 0x{ov:X}):")
                print(f"    compiled [{ci:3d}]: {c_raw}")
                print(f"    original [{oi:3d}]: {o_raw}")
                print()
            elif kind == 'push_value':
                _, ci, oi, cv, ov, c_raw, o_raw, detail = w
                print(f"  PUSH VALUE MISMATCH (compiled 0x{cv:X} vs original 0x{ov:X}):")
                print(f"    compiled [{ci:3d}]: {c_raw}")
                print(f"    original [{oi:3d}]: {o_raw}")
                if detail:
                    print(f"   {detail}")
                print()
            elif kind == 'string_mismatch':
                _, ci, oi, cv, ov, c_raw, o_raw, detail = w
                print(f"  STRING MISMATCH:")
                print(f"    compiled [{ci:3d}]: {c_raw}")
                print(f"    original [{oi:3d}]: {o_raw}")
                if detail:
                    print(f"   {detail}")
                print()
            elif kind == 'imm_value':
                _, ci, oi, cv, ov, c_raw, o_raw = w
                print(f"  IMMEDIATE MISMATCH (compiled {cv} vs original {ov}):")
                print(f"    compiled [{ci:3d}]: {c_raw}")
                print(f"    original [{oi:3d}]: {o_raw}")
                print()


if __name__ == '__main__':
    main()
