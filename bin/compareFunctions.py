#!/usr/bin/env python3
"""
Compare function addresses between the original TEACHER.EXE and the reconstructed version.
Shows similarity metrics and identifies the first user-defined function.
"""
import os
import re
import struct
import argparse

# Library address ranges - functions in these ranges are CRT/library code
LIBRARY_RANGES = [(0x424540, 0x4304E0)]


def parse_pe_text_section(filename):
    """Parse PE and return .text section info and raw data"""
    with open(filename, 'rb') as f:
        f.seek(0x3C)
        pe_off = struct.unpack('<I', f.read(4))[0]
        
        f.seek(pe_off + 6)
        num_sections = struct.unpack('<H', f.read(2))[0]
        
        f.seek(pe_off + 20)
        opt_size = struct.unpack('<H', f.read(2))[0]
        
        f.seek(pe_off + 52)
        image_base = struct.unpack('<I', f.read(4))[0]
        
        section_start = pe_off + 24 + opt_size
        f.seek(section_start)
        
        for i in range(num_sections):
            name = f.read(8).rstrip(b'\x00').decode('ascii', errors='ignore')
            vsize = struct.unpack('<I', f.read(4))[0]
            va = struct.unpack('<I', f.read(4))[0]
            rawsize = struct.unpack('<I', f.read(4))[0]
            rawptr = struct.unpack('<I', f.read(4))[0]
            f.read(12)
            flags = struct.unpack('<I', f.read(4))[0]
            
            if name == '.text':
                f.seek(rawptr)
                data = f.read(rawsize)
                return image_base + va, vsize, data, image_base
        
        return None, None, None, None


def get_all_functions_from_map(map_dir):
    """Get all known function addresses from src/map/*.cpp files"""
    addresses = []
    for filename in os.listdir(map_dir):
        if filename.endswith(".cpp"):
            filepath = os.path.join(map_dir, filename)
            with open(filepath, 'r') as f:
                content = f.read()
            
            found = re.findall(r'/\* Function start: (0x[0-9a-fA-F]+) \*/', content)
            for addr in found:
                addresses.append(int(addr, 16))
    return sorted(set(addresses))


def get_implemented_functions(src_dir):
    """Get function addresses that have been implemented in src/*.cpp"""
    addresses = set()
    for root, dirs, files in os.walk(src_dir):
        if 'map' in dirs:
            dirs.remove('map')  # Exclude the map directory
        for filename in files:
            if filename.endswith(".cpp"):
                filepath = os.path.join(root, filename)
                with open(filepath, 'r') as f:
                    content = f.read()
                
                found = re.findall(r'/\* Function start: (0x[0-9a-fA-F]+) \*/', content)
                for addr in found:
                    addresses.add(int(addr, 16))
    return addresses


def get_library_functions(code_dir):
    """Get addresses marked as library functions in decompiled files"""
    lib_addresses = set()
    for filename in os.listdir(code_dir):
        if filename.endswith('.decompiled.txt'):
            filepath = os.path.join(code_dir, filename)
            try:
                with open(filepath, 'r') as f:
                    content = f.read()
            except Exception:
                continue
            
            if 'Library Function - Single Match' in content:
                m = re.search(r'Address:\s*0x([0-9A-Fa-f]+)', content)
                if m:
                    lib_addresses.add(int(m.group(1), 16))
    return lib_addresses


def is_library_function(addr, library_funcs, library_ranges):
    """Check if an address is a library function"""
    if addr in library_funcs:
        return True
    for start, end in library_ranges:
        if start <= addr <= end:
            return True
    return False


def find_function_prologues(text_data, text_base):
    """
    Heuristically find function prologues in the .text section.
    Common patterns for MSVC:
    - PUSH EBP; MOV EBP, ESP (55 8B EC)
    - PUSH ECX (51) - for small locals
    - PUSH ESI (56) - saving registers
    - MOV EAX, xxx; CALL __SEH_prolog (for SEH functions)
    """
    addresses = []
    
    # Pattern: PUSH EBP; MOV EBP, ESP (standard prologue)
    pattern1 = b'\x55\x8B\xEC'
    
    # Pattern: PUSH ESI; MOV ESI, ECX (thiscall saving this)
    pattern2 = b'\x56\x8B\xF1'
    
    # Pattern: PUSH -1 (SEH prologue start for many functions)
    pattern3 = b'\x6A\xFF'
    
    i = 0
    while i < len(text_data) - 3:
        if text_data[i:i+3] == pattern1:
            addresses.append(text_base + i)
        elif text_data[i:i+3] == pattern2:
            addresses.append(text_base + i)
        elif text_data[i:i+2] == pattern3:
            # Check if this looks like a function start (preceded by ret or int3)
            if i > 0 and text_data[i-1] in (0xC3, 0xCC, 0xC2):
                addresses.append(text_base + i)
        i += 1
    
    return sorted(set(addresses))


def main():
    parser = argparse.ArgumentParser(description='Compare function addresses between original and reconstructed EXE')
    parser.add_argument('--orig', default='data/demo/TEACHER.EXE', help='Original EXE path')
    parser.add_argument('--new', default='TEACHER.EXE', help='Reconstructed EXE path (optional)')
    parser.add_argument('--map-dir', default='src/map', help='Map directory with function addresses')
    parser.add_argument('--src-dir', default='src', help='Source directory')
    parser.add_argument('--code-dir', default='code', help='Code directory with decompiled files')
    args = parser.parse_args()
    
    print("=" * 70)
    print("Function Address Comparison: Original vs Reconstructed TEACHER.EXE")
    print("=" * 70)
    print()
    
    # Get all known functions from the map directory
    all_functions = get_all_functions_from_map(args.map_dir)
    print(f"Total functions in map directory: {len(all_functions)}")
    
    # Get implemented functions
    implemented_functions = get_implemented_functions(args.src_dir)
    print(f"Total implemented functions: {len(implemented_functions)}")
    
    # Get library functions
    library_functions = get_library_functions(args.code_dir)
    print(f"Library functions (from decompiled files): {len(library_functions)}")
    
    # Add functions in library ranges
    range_lib_funcs = set()
    for addr in all_functions:
        for start, end in LIBRARY_RANGES:
            if start <= addr <= end:
                range_lib_funcs.add(addr)
    
    all_library = library_functions | range_lib_funcs
    print(f"Library functions (including range 0x{LIBRARY_RANGES[0][0]:X}-0x{LIBRARY_RANGES[0][1]:X}): {len(all_library)}")
    print()
    
    # Categorize functions
    user_functions = [addr for addr in all_functions if not is_library_function(addr, library_functions, LIBRARY_RANGES)]
    user_implemented = [addr for addr in implemented_functions if not is_library_function(addr, library_functions, LIBRARY_RANGES)]
    
    print("-" * 70)
    print("USER-DEFINED FUNCTIONS ANALYSIS")
    print("-" * 70)
    print(f"Total user-defined functions: {len(user_functions)}")
    print(f"Implemented user-defined functions: {len(user_implemented)}")
    
    if user_functions:
        percentage = len(user_implemented) / len(user_functions) * 100
        print(f"Implementation progress: {percentage:.1f}%")
    print()
    
    # Find first user-defined function
    if user_functions:
        first_user_func = min(user_functions)
        print(f"🎯 FIRST USER-DEFINED FUNCTION: 0x{first_user_func:08X}")
        
        # Check if it's implemented
        if first_user_func in implemented_functions:
            print(f"   ✅ This function is implemented")
        else:
            print(f"   ❌ This function is NOT yet implemented")
        
        # Find the decompiled file for context
        decompiled_file = os.path.join(args.code_dir, f"FUN_{first_user_func:X}.decompiled.txt")
        if os.path.exists(decompiled_file):
            print(f"   📄 Decompiled file: {decompiled_file}")
            with open(decompiled_file, 'r') as f:
                lines = f.readlines()[:10]
                for line in lines:
                    if 'Function:' in line or 'void' in line or 'int' in line:
                        print(f"      {line.rstrip()}")
    print()
    
    # Show address distribution
    print("-" * 70)
    print("ADDRESS DISTRIBUTION BY PAGE (0x1000)")
    print("-" * 70)
    
    # Group by page
    pages = {}
    for addr in all_functions:
        page = addr & 0xFFFFF000
        if page not in pages:
            pages[page] = {'total': 0, 'user': 0, 'implemented': 0, 'lib': 0}
        pages[page]['total'] += 1
        if is_library_function(addr, library_functions, LIBRARY_RANGES):
            pages[page]['lib'] += 1
        else:
            pages[page]['user'] += 1
            if addr in implemented_functions:
                pages[page]['implemented'] += 1
    
    print(f"{'Page':<12} {'Total':>6} {'User':>6} {'Impl':>6} {'Lib':>6} {'Progress':>10}")
    print("-" * 52)
    for page in sorted(pages.keys()):
        p = pages[page]
        progress = f"{p['implemented']/max(1,p['user'])*100:.0f}%" if p['user'] > 0 else "N/A"
        print(f"0x{page:08X} {p['total']:>6} {p['user']:>6} {p['implemented']:>6} {p['lib']:>6} {progress:>10}")
    print()
    
    # Show first 20 user-defined functions and their status
    print("-" * 70)
    print("FIRST 20 USER-DEFINED FUNCTIONS")
    print("-" * 70)
    print(f"{'Address':<12} {'Status':<12} {'Notes'}")
    print("-" * 50)
    for addr in user_functions[:20]:
        status = "✅ Implemented" if addr in implemented_functions else "❌ Missing"
        
        # Check for decompiled file
        decompiled = os.path.join(args.code_dir, f"FUN_{addr:X}.decompiled.txt")
        disasm = os.path.join(args.code_dir, f"FUN_{addr:X}.disassembled.txt")
        
        notes = []
        if os.path.exists(decompiled):
            notes.append("decompiled")
        if os.path.exists(disasm):
            notes.append("disasm")
        
        print(f"0x{addr:08X} {status:<12} {', '.join(notes)}")
    print()
    
    # Summary
    print("=" * 70)
    print("SUMMARY")
    print("=" * 70)
    print(f"Total functions in map:        {len(all_functions)}")
    print(f"Library/CRT functions:         {len(all_library)}")
    print(f"User-defined functions:        {len(user_functions)}")
    print(f"Implemented (user-defined):    {len(user_implemented)}")
    print(f"Remaining to implement:        {len(user_functions) - len(user_implemented)}")
    print()
    print(f"First user-defined function:   0x{min(user_functions):08X}" if user_functions else "No user functions found")
    print(f"Last user-defined function:    0x{max(user_functions):08X}" if user_functions else "")
    
    # Calculate progress percentage
    if user_functions:
        pct = len(user_implemented) / len(user_functions) * 100
        bar_len = 40
        filled = int(bar_len * pct / 100)
        bar = '█' * filled + '░' * (bar_len - filled)
        print(f"\nProgress: [{bar}] {pct:.1f}%")


if __name__ == '__main__':
    main()
