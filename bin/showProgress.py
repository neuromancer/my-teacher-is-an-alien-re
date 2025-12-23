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

    # Exclude library functions from the implemented set for percentage calculations
    implemented_no_lib = implemented_functions - library_functions

    print_ranges(all_functions, implemented_no_lib, library_functions)

    total_functions = len(all_functions)
    total_library = len(library_functions)
    total_counted = total_functions - total_library

    total_implemented = len(implemented_no_lib)

    if total_functions > 0:
        if total_counted > 0:
            percentage = (total_implemented / total_counted) * 100
            range_note = ''
            if LIBRARY_RANGES:
                ranges_str = ', '.join(f"0x{start:x}-0x{end:x}" for start, end in LIBRARY_RANGES)
                range_note = f" (ranges applied: {ranges_str})"
            print(f"\nProgress: {total_implemented} / {total_counted} ({percentage:.2f}%) — {total_library} library functions excluded{range_note}")
        else:
            print(f"\nNo non-library functions to implement; {total_library} library functions found and excluded.")
    else:
        print("\nNo functions found in the map directory.")
