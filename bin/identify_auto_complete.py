#!/usr/bin/env python3
"""
Identify which auto-completable functions (empty, SEH funclets, destructors, constructors)
are currently marked as unimplemented and should be marked complete.
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

def categorize_function(filepath):
    """Categorize a function."""
    with open(filepath, 'r') as f:
        content = f.read()
    
    if 'Library Function - Single Match' in content:
        return 'library'
    
    lines = content.split('\n')
    body_lines = []
    in_body = False
    for line in lines:
        if '{' in line:
            in_body = True
        if in_body:
            body_lines.append(line)
    
    body = '\n'.join(body_lines)
    
    # Empty function
    if re.search(r'{\s*return;\s*}', body, re.DOTALL):
        return 'empty'
    
    # SEH funclet
    if 'unaff_EBP' in body and re.search(r'FUN_\w+\(\*', body):
        func_calls = re.findall(r'FUN_\w+', body)
        if len(func_calls) <= 2:
            return 'seh_funclet'
    
    # Destructor
    if '__thiscall' in content and 'param_1 & 1' in body and 'FUN_00424940' in body:
        return 'destructor'
    
    # Constructor
    if '__thiscall' in content and body.count('*(undefined4 *)') >= 2:
        assignments = re.findall(r'\*\(undefined4 \*\)', body)
        if len(assignments) >= 2 and body.count('\n') < 15:
            return 'constructor'
    
    return 'other'

def main():
    map_dir = 'src/map'
    src_dir = 'src'
    code_dir = 'code'
    
    all_funcs = get_all_functions_from_map(map_dir)
    implemented_funcs = get_implemented_functions(src_dir)
    unimplemented_funcs = all_funcs - implemented_funcs
    
    # Categorize unimplemented functions
    auto_complete = []
    
    for addr in sorted(unimplemented_funcs):
        filename = f'FUN_{addr:06X}.decompiled.txt'
        filepath = os.path.join(code_dir, filename)
        
        if os.path.exists(filepath):
            category = categorize_function(filepath)
            if category in ['empty', 'seh_funclet', 'destructor', 'constructor']:
                auto_complete.append((addr, category))
    
    print(f"Found {len(auto_complete)} unimplemented functions that can be auto-completed:\n")
    
    # Group by category
    by_category = {}
    for addr, cat in auto_complete:
        if cat not in by_category:
            by_category[cat] = []
        by_category[cat].append(addr)
    
    for cat in ['empty', 'seh_funclet', 'destructor', 'constructor']:
        if cat in by_category:
            addrs = by_category[cat]
            print(f"\n{cat.upper().replace('_', ' ')} ({len(addrs)} functions):")
            for addr in addrs[:10]:
                print(f"  0x{addr:06X}")
            if len(addrs) > 10:
                print(f"  ... and {len(addrs) - 10} more")
    
    # Output addresses for script modification
    print(f"\n{'='*80}")
    print("ADDRESSES TO ADD TO AUTO-COMPLETE LIST:")
    print(f"{'='*80}")
    all_addrs = [f"0x{addr:06X}" for addr, _ in auto_complete]
    print(", ".join(all_addrs))

if __name__ == '__main__':
    main()
