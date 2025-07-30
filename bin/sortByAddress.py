import os
import re

def sort_cpp_files_by_address(directory):
    for filename in os.listdir(directory):
        if filename.endswith(".cpp"):
            filepath = os.path.join(directory, filename)

            print(f"Processing file: {filepath}")

            with open(filepath, 'r') as f:
                content = f.read()

            functions = re.findall(r'/\* Function start: (0x[0-9a-fA-F]+) \*/\n(.*?)\n}\n', content, re.DOTALL)

            sorted_functions = sorted(functions, key=lambda x: int(x[0], 16))

            new_content = ""
            # Add back any file content that is not part of a function
            # This is a simplification and might not preserve all non-function content
            non_function_content = re.split(r'/\* Function start: 0x[0-9a-fA-F]+ \*/\n.*?\n}\n', content, flags=re.DOTALL)
            new_content += non_function_content[0]

            for addr, func_body in sorted_functions:
                new_content += f"/* Function start: {addr} */\n{func_body.strip()}\n}}\n\n"

            with open(filepath, 'w') as f:
                f.write(new_content)

if __name__ == "__main__":
    sort_cpp_files_by_address("src")
