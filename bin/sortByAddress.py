import os
import re

def sort_cpp_files_by_address(directory):
    for filename in os.listdir(directory):
        if filename.endswith(".cpp"):
            filepath = os.path.join(directory, filename)

            print(f"Processing file: {filepath}")

            with open(filepath, 'r') as f:
                content = f.read()

            functions = re.findall(r'(/\* Function start: (0x[0-9a-fA-F]+) \*/\n(.*?)\n}\n)', content, re.DOTALL)

            sorted_functions = sorted(functions, key=lambda x: int(x[1], 16))

            # Get content that is not a function
            non_function_content = re.sub(r'/\* Function start: 0x[0-9a-fA-F]+ \*/\n.*?\n}\n', '', content, flags=re.DOTALL).strip()

            new_content = non_function_content + "\n\n"

            for full_match, addr, func_body in sorted_functions:
                new_content += f"{full_match.strip()}\n\n"

            with open(filepath, 'w') as f:
                f.write(new_content)

if __name__ == "__main__":
    sort_cpp_files_by_address("src")
