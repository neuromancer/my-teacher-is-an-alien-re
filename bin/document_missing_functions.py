#!/usr/bin/env python3
"""
Document unimplemented developer-written functions at a high level.
"""

import os
import re

def get_all_functions_from_map(map_dir):
    """Get all function addresses from src/map."""
    addresses = []
    for filename in os.listdir(map_dir):
        if filename.endswith(".cpp"):
            filepath = os.path.join(map_dir, filename)
            with open(filepath, 'r') as f:
                content = f.read()
            found_addresses = re.findall(r'/\* Function start: (0x[0-9a-fA-F]+) \*/', content)
            for addr in found_addresses:
                addresses.append(int(addr, 16))
    return set(addresses)

def get_implemented_functions(src_dir):
    """Get implemented function addresses from src/."""
    addresses = set()
    for root, dirs, files in os.walk(src_dir):
        if 'map' in dirs:
            dirs.remove('map')
        for filename in files:
            if filename.endswith(".cpp"):
                filepath = os.path.join(root, filename)
                with open(filepath, 'r') as f:
                    content = f.read()
                found_functions = re.findall(r'/\* Function start: (0x[0-9a-fA-F]+) \*/', content)
                for addr in found_functions:
                    addresses.add(int(addr, 16))
    return addresses

AUTO_COMPLETE = {
    0x4010E0, 0x4010F0, 0x40166B, 0x4016FF, 0x402DA4, 0x402EA1, 0x405760, 0x405A82, 
    0x405A94, 0x405B56, 0x405BA6, 0x405C72, 0x407A40, 0x408931, 0x4093D4, 0x4093EC, 
    0x409CC2, 0x40A13D, 0x40A592, 0x40B130, 0x40BABA, 0x40CD15, 0x40CD1D, 0x40D5D2, 
    0x40D5EA, 0x40D6A0, 0x40EB1E, 0x40EB2C, 0x40EB3A, 0x40EB48, 0x40EB56, 0x40EB6E, 
    0x40F1B0, 0x40F9CD, 0x40FDF8, 0x40FE06, 0x40FE2C, 0x4100CD, 0x410740, 0x4107DF, 
    0x4107ED, 0x410805, 0x4111B0, 0x411223, 0x41190A, 0x411922, 0x4119A2, 0x4119BA, 
    0x411A29, 0x411C63, 0x411C6E, 0x411C79, 0x411C8E, 0x4122B8, 0x4122C6, 0x4122D4, 
    0x4122EC, 0x412A70, 0x412AD7, 0x412E30, 0x412F2A, 0x413804, 0x413FF6, 0x413FFE, 
    0x41400C, 0x41401A, 0x414028, 0x414036, 0x41404E, 0x41421B, 0x4145BC, 0x414F26, 
    0x41556A, 0x415D30, 0x4166F0, 0x416817, 0x416825, 0x416833, 0x416841, 0x41684F, 
    0x41685D, 0x41686F, 0x416D33, 0x416D41, 0x416D4F, 0x416D67, 0x417170, 0x417652, 
    0x4188A0, 0x4199EA, 0x419A02, 0x41AF9F, 0x41AFB1, 0x41B07F, 0x41B091, 0x41B29A, 
    0x41B2AC, 0x41B55D, 0x41BAE6, 0x41C58C, 0x41C72C, 0x41C94C, 0x41CB15, 0x41CB2A, 
    0x41CEF0, 0x41CF08, 0x41EF25, 0x41EF47, 0x41F471, 0x41FBD3, 0x42055C, 0x4210A8, 
    0x4210BD, 0x42157A, 0x42158C, 0x421671, 0x421881, 0x42196A, 0x421A24, 0x422791, 
    0x426CF0, 0x42B980, 0x42D240,
    0x401147, 0x4011EF, 0x401282, 0x401DE3, 0x402E86, 0x402EB9, 0x403561, 0x404635, 
    0x404FE3, 0x405DCD, 0x406257, 0x406920, 0x40722E, 0x408B51, 0x40AE6F,
    0x401190, 0x401200, 0x401C80, 0x402CB0, 0x4034A0, 0x404490, 0x404540, 0x404E40, 
    0x4061C0, 0x407050, 0x408940, 0x40AD50, 0x40B8F0, 0x40F8D0, 0x40FD50,
    0x401290, 0x404470, 0x408860, 0x40ACA0, 0x4189D0, 0x41CD10, 0x41CD30,
}

LIBRARY_RANGES = [(0x424540, 0x4304E0)]

def is_library(addr):
    """Check if address is in library range."""
    for start, end in LIBRARY_RANGES:
        if start <= addr <= end:
            return True
    return False

def analyze_function(filepath):
    """Analyze a decompiled function to extract high-level description."""
    with open(filepath, 'r') as f:
        content = f.read()
    
    # Extract function signature
    sig_match = re.search(r'(void|int|undefined\d*|[a-zA-Z_][a-zA-Z0-9_]*)\s+(__thiscall|__fastcall|__stdcall)?\s*FUN_[0-9A-Fa-f]+\([^)]*\)', content)
    signature = sig_match.group(0) if sig_match else 'unknown'
    
    # Count significant operations
    loops = len(re.findall(r'\b(while|for|do)\s*\(', content))
    conditions = len(re.findall(r'\bif\s*\(', content))
    calls = len(re.findall(r'FUN_[0-9A-Fa-f]+\(', content))
    
    # Look for string references
    strings = re.findall(r's_[A-Z_\d]+_[0-9a-f]+', content)
    
    # Look for keywords that hint at functionality
    hints = []
    if 'malloc' in content or 'new' in content:
        hints.append('memory allocation')
    if 'free' in content or 'delete' in content:
        hints.append('memory deallocation')
    if 'strcpy' in content or 'sprintf' in content or 'strcat' in content:
        hints.append('string operations')
    if 'fopen' in content or 'fread' in content or 'fwrite' in content:
        hints.append('file I/O')
    if 'DrawText' in content or 'BitBlt' in content or 'SetPixel' in content:
        hints.append('graphics/drawing')
    if 'SendMessage' in content or 'PostMessage' in content:
        hints.append('Windows messaging')
    if 'CreateWindow' in content or 'ShowWindow' in content:
        hints.append('window management')
    
    return {
        'signature': signature,
        'loops': loops,
        'conditions': conditions,
        'calls': calls,
        'strings': strings[:3],  # First 3 strings
        'hints': hints
    }

def main():
    map_dir = 'src/map'
    src_dir = 'src'
    code_dir = 'code'
    
    all_funcs = get_all_functions_from_map(map_dir)
    implemented_funcs = get_implemented_functions(src_dir)
    unimplemented_funcs = all_funcs - implemented_funcs - AUTO_COMPLETE
    
    # Filter out library functions
    developer_funcs = [addr for addr in unimplemented_funcs if not is_library(addr)]
    
    print(f"Analyzing {len(developer_funcs)} unimplemented developer functions...\n")
    
    # Analyze each function
    functions_info = []
    for addr in sorted(developer_funcs):
        filename = f'FUN_{addr:06X}.decompiled.txt'
        filepath = os.path.join(code_dir, filename)
        
        if os.path.exists(filepath):
            info = analyze_function(filepath)
            info['addr'] = addr
            functions_info.append(info)
    
    # Write markdown documentation
    doc_file = 'docs/unimplemented_functions.md'
    with open(doc_file, 'w') as f:
        f.write("# Unimplemented Functions Documentation\n\n")
        f.write("This document provides a high-level overview of unimplemented developer-written functions.\n")
        f.write("Auto-completable functions (compiler-generated code) are not included here.\n\n")
        f.write(f"**Total unimplemented developer functions: {len(functions_info)}**\n\n")
        f.write("---\n\n")
        
        # Group by address range
        current_range_start = None
        range_funcs = []
        
        for func_info in functions_info:
            addr = func_info['addr']
            
            # Start new range if this is first function or gap > 0x100
            if current_range_start is None or addr - range_funcs[-1]['addr'] > 0x100:
                if range_funcs:
                    # Write previous range
                    write_range_section(f, current_range_start, range_funcs)
                    range_funcs = []
                current_range_start = addr
            
            range_funcs.append(func_info)
        
        # Write last range
        if range_funcs:
            write_range_section(f, current_range_start, range_funcs)
    
    print(f"✅ Documentation written to {doc_file}")
    print(f"📊 Documented {len(functions_info)} functions")

def write_range_section(f, start_addr, funcs):
    """Write a section for a range of functions."""
    end_addr = funcs[-1]['addr']
    f.write(f"## Range: 0x{start_addr:06X} - 0x{end_addr:06X}\n\n")
    f.write(f"**Functions in this range: {len(funcs)}**\n\n")
    
    for func_info in funcs:
        addr = func_info['addr']
        f.write(f"### 0x{addr:06X}\n\n")
        f.write(f"- **Signature**: `{func_info['signature']}`\n")
        f.write(f"- **Complexity**: {func_info['loops']} loops, {func_info['conditions']} conditionals, {func_info['calls']} function calls\n")
        
        if func_info['hints']:
            f.write(f"- **Likely purpose**: {', '.join(func_info['hints'])}\n")
        
        if func_info['strings']:
            f.write(f"- **Referenced strings**: {', '.join(f'`{s}`' for s in func_info['strings'])}\n")
        
        f.write("\n")
    
    f.write("---\n\n")

if __name__ == '__main__':
    main()
