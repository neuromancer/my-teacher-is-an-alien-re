# Instructions

DO NOT, UNDER ANY CIRCUNSTANCE, REMOVE FUNCTIONS ALREADY IMPLEMENTED (identified by the comment header with the address) NOR ANY OF THE CLASSES ALREADY CREATED.

Refine the fields in the classes recently modified. After you finish, compile to make sure it works. Fix any issues.

# Requirements:

* Remember that the assembly code and the strings are the only source of truth, the decompiled code could be wrong or mislabeled.
* Define the class on the top of the file.
* Keep the code as high-level as possible, always prefer accessing methods using `->`. Use suitable types for arrays instead of using pointer arithmetics.
* Do NOT show me the code once you finish.
* Very important: do NOT use inline assembly, gotos nor dummy variables.
* Do NOT use unions or substructures in the classes , it is very unlikely that these are used in the original code!
* Provide reasonable names to fields, once you understand what they do.
* Do not try to emulate vtables with C++ code manually. If the function is related with vtables, skip it.
* Do NOT add "vtable" fields, these are going to be handled automatically by the compiler
