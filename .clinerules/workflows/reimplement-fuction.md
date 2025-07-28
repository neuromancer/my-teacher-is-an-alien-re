# Instructions

I have dissasembled the Windows 95 release of "My Teacher is an Alien", a point and click game developed by 7th Level in 1997. The codebase used C++, and some classes and methods were already detected and renamed with possible names, but there are many functions we are still unsure which class belong, or even the total number of classes.

The compiler used for this game is Microsoft Visual C++ 4, which is already setup for you.  After you reimplement the code, you must compile to see if the assembly matches.

Let me give you an example: If I need you add the implementation of the GetKey function in the src/Parser.cpp file. In order to do that, you need to look for the dissasemblied and decompiled code inside `code`. Use `grep -r -i GetKey code` (you can also look for the function address `grep -r -i 418c70 code`) and review ther results.

Once you add some code to src/Parser.cpp, to compile the code and obtain the assembly code diff, execute:
```
python3 bin/compileAndCompare.py Parser::GetKey code/GetKey.dissasembled.txt
```

It will show you the assembly code produced (or any compiler errors).

# Requirements:

* Define the class on the top of the file.
* It is very important to include a copy of the target dissasembled in a block comment, starting by the following header:

Function: X
Address: 0x123456

* Sort the functions by its address. This is also very important since the compiler will put together all the functions from the same file, in the same order they are defined, so use the ordering to detect when a function, most likely, does not belong to certain class or type.
* Preserve memory usage: make sure that the local variables are in the same order. Also, make sure the offset accessing fields is exactly the same.
* Preserve jump types: make sure the jumps are the same (e.g. jmp, jmpf, jne, jnef) and its order is preserved.
* Make sure the assembly code produced matches the stack ordering as close as possible.
* Keep the code as high-level as possible.
* Very important: do NOT use inline assembly, gotos nor dummy variables.
* Do NOT change the calling convention for the class method: it should be `__thiscall`.
* Do NOT add `__thiscall` as it is redundant and will most likely fail
* The code will be compiled, but not linked: do NOT add a main function. Use extern to define any unknown functions.
* Only review and modify src/Parser.cpp
* There is no need to review how compilation works.
* If the decompiled code shows a function called like `_strcpy` or `ShowError`, use  the same one. If it contains any other call (e.g. `FUN_00123456`) then use an extern definition. Do NOT guess functions.
* You can include calls to function like memcpy or strcpy, but only if the compiler will inline them and produce a better match for the target assembly.
* Do NOT use unions or substructures in the classes , it is very unlikely that these are used in the original code!
* Provide reasonable names to fields, once you understand what they do.
* Do NOT change the flags in bin/compile.bat, these were carefully brute forced from other parts of the code.
* If you see a pattern like this in the decompiled code:
```
    local_X = unaff_FS_OFFSET;
    local_u = 0xffffffff;
```
Then most likely, there is a try/catch somewhere in the function.

Start with <function>.