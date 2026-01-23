import os
import re


def get_all_functions(map_dir):
    addresses = []
    for filename in os.listdir(map_dir):
        if filename.endswith(".cpp"):
            filepath = os.path.join(map_dir, filename)
            with open(filepath, 'r') as f:
                content = f.read()

            found_addresses = re.findall(r'/\* Function start: (0x[0-9a-fA-F]+) \*/', content)
            for addr in found_addresses:
                addresses.append(int(addr, 16))
    return sorted(addresses)


def get_implemented_functions(src_dir):
    addresses = set()
    for root, dirs, files in os.walk(src_dir):
        if 'map' in dirs:
            dirs.remove('map')  # Exclude the map directory
        for filename in files:
            if filename.endswith(".cpp"):
                filepath = os.path.join(root, filename)
                with open(filepath, 'r') as f:
                    content = f.read()

                found_functions = re.findall(r'/\* Function start: (0x[0-9a-fA-F]+) \*/', content)
                for addr in found_functions:
                    addresses.add(int(addr, 16))
    return addresses


# Optional address ranges that should be treated as library functions even if
# the decompiled files don't explicitly mark them. Add tuples as (start, end).
LIBRARY_RANGES = [ (0x42A3D0, 0x4304E0) ]  # common library range identified by inspection

# Auto-completable functions: compiler-generated code (empty functions, SEH funclets, 
# simple destructors, simple constructors) that don't need manual implementation
AUTO_COMPLETE_FUNCTIONS = {
    # Empty functions (compiler-generated)
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
    # SEH exception handler funclets
    0x401147, 0x4011EF, 0x401282, 0x401DE3, 0x402E86, 0x402EB9, 0x403561, 0x404635, 
    0x404FE3, 0x405DCD, 0x406257, 0x406920, 0x40722E, 0x408B51, 0x40AE6F,
    # Destructors with conditional delete
    0x401190, 0x401200, 0x401C80, 0x402CB0, 0x4034A0, 0x404490, 0x404540, 0x404E40, 
    0x4061C0, 0x407050, 0x408940, 0x40AD50, 0x40B8F0, 0x40F8D0, 0x40FD50,
    # Simple constructors/initializers  
    0x401290, 0x404470, 0x408860, 0x40ACA0, 0x4189D0, 0x41CD10, 0x41CD30,
}


def get_library_functions(code_dir):
    """Scan decompiled files in `code/` and return addresses that are library functions.
    Library functions are identified by the comment 'Library Function - Single Match' in the decompiled files.
    This function only returns addresses explicitly marked in decompiled files; ranges from
    `LIBRARY_RANGES` are applied later against the set of all functions in the map.
    """
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


def print_ranges(all_funcs, implemented_funcs, library_funcs):
    if not all_funcs:
        return

    ranges = []

    start_range = all_funcs[0]
    is_implemented = start_range in implemented_funcs

    for i in range(1, len(all_funcs)):
        current_is_implemented = all_funcs[i] in implemented_funcs
        if current_is_implemented != is_implemented:
            end_range = all_funcs[i-1]
            ranges.append((start_range, end_range, is_implemented))
            start_range = all_funcs[i]
            is_implemented = current_is_implemented

    end_range = all_funcs[-1]
    ranges.append((start_range, end_range, is_implemented))

    print("--- Progress Report ---")

    for start, end, implemented in ranges:
        start_index = all_funcs.index(start)
        end_index = all_funcs.index(end)
        count = end_index - start_index + 1
        # Count how many of these are library functions
        lib_count = sum(1 for a in all_funcs[start_index:end_index+1] if a in library_funcs)
        status = "✅" if implemented else "❌"
        lib_marker = " 📚" if lib_count > 0 else ""
        lib_info = f", {lib_count} library" if lib_count > 0 else ""

        if start == end:
            print(f"  {status} 0x{start:x} ({count} function{lib_info}){lib_marker}")
        else:
            print(f"  {status} 0x{start:x} - 0x{end:x} ({count} functions{lib_info}){lib_marker}")

    print("-----------------------------------------")


if __name__ == "__main__":
    map_directory = "src/map"
    src_directory = "src"
    code_directory = "code"

    all_functions = get_all_functions(map_directory)
    implemented_functions = get_implemented_functions(src_directory)
    library_functions = get_library_functions(code_directory)

    # Add any functions that fall inside configured library ranges
    range_funcs = set()
    for start, end in LIBRARY_RANGES:
        for a in all_functions:
            if start <= a <= end:
                range_funcs.add(a)

    if range_funcs:
        # merge explicit and range-derived library addresses
        library_functions = library_functions.union(range_funcs)

    # Add auto-completable functions (compiler-generated) to implemented set
    auto_complete_in_map = AUTO_COMPLETE_FUNCTIONS.intersection(all_functions)
    implemented_functions = implemented_functions.union(auto_complete_in_map)
    
    # Exclude library functions from the implemented set for percentage calculations
    implemented_no_lib = implemented_functions - library_functions

    print_ranges(all_functions, implemented_no_lib, library_functions)

    total_functions = len(all_functions)
    total_library = len(library_functions)
    total_counted = total_functions - total_library

    total_implemented = len(implemented_no_lib)
    total_auto_complete = len(auto_complete_in_map)

    if total_functions > 0:
        if total_counted > 0:
            percentage = (total_implemented / total_counted) * 100
            range_note = ''
            if LIBRARY_RANGES:
                ranges_str = ', '.join(f"0x{start:x}-0x{end:x}" for start, end in LIBRARY_RANGES)
                range_note = f" (ranges applied: {ranges_str})"
            auto_note = f" — {total_auto_complete} auto-complete functions marked" if total_auto_complete > 0 else ""
            print(f"\nProgress: {total_implemented} / {total_counted} ({percentage:.2f}%) — {total_library} library functions excluded{range_note}{auto_note}")
        else:
            print(f"\nNo non-library functions to implement; {total_library} library functions found and excluded.")
    else:
        print("\nNo functions found in the map directory.")
