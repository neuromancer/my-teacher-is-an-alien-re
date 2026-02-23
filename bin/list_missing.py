#!/usr/bin/env python3
"""
List unimplemented developer functions (excluding auto-complete and library).
"""

import os
import re

def get_all_functions_from_map(map_dir):
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

def load_auto_complete_functions(filepath):
    """Load auto-completable function addresses from external file."""
    addresses = set()
    if not os.path.exists(filepath):
        return addresses
    with open(filepath, 'r') as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith('#'):
                continue
            try:
                addr = int(line, 16)
                addresses.add(addr)
            except ValueError:
                pass
    return addresses

AUTO_COMPLETE_FILE = os.path.join(os.path.dirname(__file__), '..', 'src', 'auto_complete.txt')
LIBRARY_RANGES = [(0x424540, 0x4304E0)]

def is_library(addr):
    for start, end in LIBRARY_RANGES:
        if start <= addr <= end:
            return True
    return False

all_funcs = get_all_functions_from_map('src/map')
implemented_funcs = get_implemented_functions('src')
auto_complete = load_auto_complete_functions(AUTO_COMPLETE_FILE)
unimplemented_funcs = all_funcs - implemented_funcs - auto_complete
developer_funcs = sorted([addr for addr in unimplemented_funcs if not is_library(addr)])

print(f"Total unimplemented developer functions: {len(developer_funcs)}\n")
print("Address list:")
for addr in developer_funcs:
    print(f"0x{addr:06X}")
