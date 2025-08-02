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

def print_ranges(all_funcs, implemented_funcs):
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
        status = "✅" if implemented else "❌"
        if start == end:
            print(f"  {status} 0x{start:x} ({count} function)")
        else:
            print(f"  {status} 0x{start:x} - 0x{end:x} ({count} functions)")

    print("-----------------------------------------")


if __name__ == "__main__":
    map_directory = "src/map"
    src_directory = "src"

    all_functions = get_all_functions(map_directory)
    implemented_functions = get_implemented_functions(src_directory)

    print_ranges(all_functions, implemented_functions)

    total_functions = len(all_functions)
    total_implemented = len(implemented_functions)

    if total_functions > 0:
        percentage = (total_implemented / total_functions) * 100
        print(f"\nProgress: {total_implemented} / {total_functions} ({percentage:.2f}%)")
    else:
        print("\nNo functions found in the map directory.")
