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


def load_auto_complete_functions(filepath):
    """Load auto-completable function addresses from external file.

    File format: one hex address per line (0x prefix), comments start with #
    """
    addresses = set()
    if not os.path.exists(filepath):
        return addresses

    with open(filepath, 'r') as f:
        for line in f:
            line = line.strip()
            # Skip empty lines and comments
            if not line or line.startswith('#'):
                continue
            # Parse hex address
            try:
                addr = int(line, 16)
                addresses.add(addr)
            except ValueError:
                pass  # Skip invalid lines
    return addresses


# Optional address ranges that should be treated as library functions even if
# the decompiled files don't explicitly mark them. Add tuples as (start, end).
LIBRARY_RANGES = [ (0x424540, 0x4304E0) ]  # CRT/library range: EH, FDIV workarounds, stdio, FP, locale, IAT thunks

# Path to auto-complete functions file (relative to script location)
AUTO_COMPLETE_FILE = os.path.join(os.path.dirname(__file__), '..', 'src', 'auto_complete.txt')


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

        if not implemented and lib_count == count:
            # Entire range is library — use 📚 icon and simplified text
            if start == end:
                print(f"  📚 0x{start:x} ({count} library function)")
            else:
                print(f"  📚 0x{start:x} - 0x{end:x} ({count} library functions)")
        else:
            status = "✅" if implemented else "❌"
            lib_marker = " 📚" if lib_count > 0 else ""
            lib_info = f", {lib_count} library" if lib_count > 0 else ""

            if start == end:
                print(f"  {status} 0x{start:x} ({count} function{lib_info}){lib_marker}")
            else:
                print(f"  {status} 0x{start:x} - 0x{end:x} ({count} functions{lib_info}){lib_marker}")

    print("-----------------------------------------")


def run(map_directory, src_directory, code_directory, library_ranges, auto_complete_file):
    all_functions = get_all_functions(map_directory)
    implemented_functions = get_implemented_functions(src_directory)
    library_functions = get_library_functions(code_directory)

    # Load auto-completable functions (compiler-generated)
    auto_complete_functions = load_auto_complete_functions(auto_complete_file)
    auto_complete_in_map = auto_complete_functions.intersection(all_functions)
    implemented_functions = implemented_functions.union(auto_complete_in_map)

    # Add any functions that fall inside configured library ranges
    range_funcs = set()
    for start, end in library_ranges:
        for a in all_functions:
            if start <= a <= end:
                range_funcs.add(a)

    if range_funcs:
        # merge explicit and range-derived library addresses
        library_functions = library_functions.union(range_funcs)

    # User functions that happen to sit in the CRT range should not be
    # classified as library.  If we implemented them (or they are in the
    # auto-complete list) they are user code.
    library_functions = library_functions - implemented_functions

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
            if library_ranges:
                ranges_str = ', '.join(f"0x{start:x}-0x{end:x}" for start, end in library_ranges)
                range_note = f" (ranges applied: {ranges_str})"
            auto_note = f" — {total_auto_complete} auto-complete functions marked" if total_auto_complete > 0 else ""
            print(f"\nProgress: {total_implemented} / {total_counted} ({percentage:.2f}%) — {total_library} library functions excluded{range_note}{auto_note}")
        else:
            print(f"\nNo non-library functions to implement; {total_library} library functions found and excluded.")
    else:
        print("\nNo functions found in the map directory.")


if __name__ == "__main__":
    import sys

    if '--full' in sys.argv:
        FULL_LIBRARY_RANGES = [(0x454100, 0x460480)]
        auto_complete_full = os.path.join(os.path.dirname(__file__), '..', 'src-full', 'auto_complete.txt')
        run("src-full/map", "src-full", "code-full", FULL_LIBRARY_RANGES, auto_complete_full)
    else:
        run("src/map", "src", "code", LIBRARY_RANGES, AUTO_COMPLETE_FILE)
