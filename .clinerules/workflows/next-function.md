# Instructions

Continue implementing the next function. Before starting, review all the current files and determine if there is any class where the function could be added as a method. If the function is already implemented, make sure it is properly used by the other files (not declared as extern). Make sure the name is good.

If similarity is >= 90%, you can stop. If you give up, make sure you still provide the code with the best similarity. If the function is complex, even obtaining a 60% can be good enough, there will another pass later to improve it.

Important mote: `ShowError` is a function that should be marked by the compiler as "no return" as it calls some internal functions that eventualy ends the process, so it will not produce instructions after the call (e.g. stack clean up). However, right now, the implementation is not complete and the compiler will not optimize that. Do NOT try to workaround this (e.g. using attributes or other code), this will be fixed later.

# Requirements:

* Remember that the assembly code and the strings are the only source of truth, the decompiled code could be wrong or mislabeled.
* Define the class on the top of the file.
* It is very important to include the following header before each reimplemented function with the address of the function:

/* Function start: 0x1234.. */

* Sort the functions by its address. This is also very important since the compiler will put together all the functions from the same file, in the same order they are defined, so use the ordering to detect when a function, most likely, does not belong to certain class or type.

In order to know which functions are next to each other, check the files inside the `src/map` directory. These files will NOT be compiled.

There is a very important file located in `code/strings.txt`. This contains the addresses and strings extracted from the binary. It is very important to review it when you see a string, as it have valuable information. Include the full strings in the reimplemented code as constants, do NOT use explicit pointers to strings that are constants.

Another very important set of of files is located in `data/demo/mis`. These are "game scripts" in .txt format. Please take a look to them so you can understand what kind of data is parsed and use them to understand which class will need which code to be parsed.

* Preserve memory usage: make sure that the local variables are in the same order. Also, make sure the offset accessing fields is exactly the same.
* Preserve jump types: make sure the jumps are the same (e.g. jmp, jmpf, jne, jnef) and its order is preserved.
* Make sure the assembly code produced matches the stack ordering as close as possible.
* Keep the code as high-level as possible.
* Do NOT show me the code once you finish.
* Very important: do NOT use inline assembly, gotos nor dummy variables.
* Do NOT change the calling convention for the class method: it should be `__thiscall`. Also do NOT add `__thiscall` as it is redundant and will most likely fail
* The code will be compiled, but not linked: do NOT add a main function. Use extern to define any unknown functions.
* There is no need to review how compilation works. Also do NOT change the flags in `bin/compile.bat`, these were carefully brute forced from other parts of the code.
* If the decompiled code shows a standard function called like `_strcpy`, then use `strcpy` imported from the corresponding header.
* Every time you found a standard function such as `fsetpos`, carefully review each parameter and try to rename/retype the fields of the class using that information.
* Do NOT use `new` as it looks like it was not used in this codebase.
* If the decompiled code shows a function such as `ShowError`, use the one already implemented (check all the source files). If it contains any other call that are not yet implemented (e.g. `FUN_00123456`) then use an extern definition. Do NOT guess functions.
* Do NOT inline class methods
* You can include calls to function like `memcpy` or `strcpy`, but only if the compiler will inline them and produce a better match for the target assembly.
* Do NOT use unions or substructures in the classes , it is very unlikely that these are used in the original code!
* Provide reasonable names to fields, once you understand what they do.
* Do NOT create .c files
* If you see a pattern like this in the decompiled code:
```
    local_X = unaff_FS_OFFSET;
    local_u = 0xffffffff;
```
Then most likely, there is a try/catch somewhere in the function.
* Do not try to emulate vtables with C++ code manually. If the function is related with vtables, skip it.
* Do NOT add "vtable" fields, these are going to be handled automatically by the compiler
