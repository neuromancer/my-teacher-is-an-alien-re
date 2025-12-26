## Project Context

You are working with a disassembled Windows 95 release of "My Teacher is an Alien" (1997), a point-and-click game developed by 7th Level.

- Original codebase: C++
- Compiler: Microsoft Visual C++ 4 (already set up)
- Many classes and methods have been partially identified and renamed.
- Several functions still have unknown ownership (class/function mapping is incomplete).
- Decompiled code exists, but it may be incorrect or mislabeled.

IMPORTANT: The assembly output is the only source of truth.

---

## Absolute Rules (Do Not Violate)

- DO NOT remove or modify existing functions or classes, especially those identified by a comment header containing a function address.
- DO NOT remove existing code.
- DO NOT change calling conventions (class methods must remain __thiscall; do not explicitly specify it).
- DO NOT add:
  - main()
  - inline assembly
  - goto
  - dummy variables
  - new helper functions or extra methods
  - constructors for base classes
  - vtable fields or manual vtable handling
  - unions or substructures
  - .c files
- DO NOT use new (it appears unused in the original codebase).
- DO NOT inline class methods.
- DO NOT show the final code once you finish.
- If you are out of ideas, stop — do not break any rules.

---

## Your Task

YOUR TASK: <FILL ME>

Before starting:
1. Review all existing files.
2. Determine whether the target function belongs to an existing class.
3. If the function already exists, ensure it is properly used, not declared as extern.
4. Ensure naming is reasonable and consistent.

You may stop once assembly similarity is >= 90%.
If you give up, still provide the best possible implementation.

---

## Workflow

### Locating a Function

To implement a function at a specific address (e.g. 0x418C70):

grep -r -i 418C70 code

Always use -i to ensure case-insensitive matches.

### Compiling & Comparing Assembly

After implementing code in a file (e.g. Class::Name):

python3 bin/compileAndCompare.py Class::Name code/FUN_418C70.dissasembled.txt

This shows compiler errors or the generated assembly diff.

---

## Sources of Truth

- Assembly code
- Extracted strings

Decompiled code is NOT authoritative.

---

## Required Files & Documentation

- src/map  
  Sorted address lists showing which functions are adjacent in the binary.

- code/strings.txt  
  Address-to-string mappings. Always review when strings appear.

- docs/game.txt  
  Gameplay description; useful for understanding puzzles and logic.

- docs/exceptions.md  
  Explains how MSVC compiles exceptions. Required reading if exception patterns are suspected.

- data/demo/mis  
  Game script files used to understand parsed data and class responsibilities.

---

## Implementation Requirements

### Function Structure

- Define the class at the top of the file.
- Add this header before every reimplemented function:

/* Function start: 0x1234.. */

- Sort functions by address within each file.
  - MSVC emits functions in source order.
  - Ordering helps detect incorrect class ownership.

### Accuracy Constraints

- Preserve:
  - Local variable order
  - Field offsets
  - Stack layout
  - Jump types and order (jmp, jne, etc.)
- Assembly output must match stack usage as closely as possible.
- Keep code high-level, but faithful.

### Strings & Standard Functions

- Include full string literals as constants (no pointer aliases).
- If decompiled code calls _strcpy, use strcpy from the proper header.
- Standard functions (e.g. fsetpos) must be reviewed carefully to infer field types and meanings.
- You may use memcpy or strcpy only if they improve assembly matching via compiler inlining.

### External Functions

- If a function is referenced but not implemented (e.g. FUN_00123456), declare it as extern.
- Do not guess implementations.
- If ShowError is referenced, use the existing implementation.

---

## Special Cases & Patterns

- Pattern:

local_X = unaff_FS_OFFSET;
local_u = 0xffffffff;

This usually indicates a try/catch.

- If a function involves vtables, skip it.
- The code will be compiled but not linked.
- Do not modify compiler flags in bin/compile.bat.

---

## Important Notes

- ShowError is effectively no-return, but the compiler currently cannot optimize it as such.
- Do not use attributes or workarounds.
- This will be fixed later.

---

## Final Reminder

If similarity is >= 90%, stop.
If you are stuck, stop.
Never break the rules above.
