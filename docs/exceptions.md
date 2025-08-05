# Exception handler

The documentation describes:

* Background behind C++ exception metadata. It is recommended that users read this first.
* Interactive operation via the GUI, configuration file, and keyboard shortcuts.
* A list of helper calls that may appear in the output.
* A note about the boundaries of `try` and `__unwind` regions.
* Miscellaneous notes about the plugin.

**TRY, CATCH, AND THROW**

The C++ language provides the `try` scoped construct in which the developer expects that an exception might occur. `try` blocks must be followed by one or more scoped `catch` constructs for catching exceptions that may occur within. `catch` blocks may use `...` to catch any exception. Alternatively, `catch` blocks may name the type of an exception, such as `std::bad_alloc`. `catch` blocks with named types may or may not also catch the exception object itself. For example, `catch(std::bad_alloc *v10)` and `catch(std::bad_alloc *)` are both valid. The former can access the exception object through variable `v10`, whereas the latter cannot access the exception object.

C++ provides the `throw` keyword for throwing an exception, as in `std::bad_alloc ba; throw ba;`. This is represented in the output as (for example) `throw v10;`. C++ also allows code to rethrow the current exception via `throw;`. This is represented in the output as `throw;`.

**WIND AND UNWIND**

Exception metadata in C++ binaries is split into two categories: `try` and `catch` blocks, as discussed above, and so-called `wind` and `unwind` blocks. C++ does not have `wind` and `unwind` keywords, but the compiler creates these blocks implicitly. In most binaries, they outnumber `try` and `catch` blocks by about 20 to 1.

Consider the following code, which may or may not throw an `int` as an exception at three places:

```cpp
void may_throw() 
{
  // Point -1
  if ( rand() % 2 )
    throw -1;

  string s0 = "0";

  // Point 0
  if ( rand() % 2 )
    throw 0;

  string s1 = "1";

  // Point 1
  if ( rand() % 2 )
    throw 1;

  // Point 2
  printf("%s %sn",
    s0.c_str(),
    s1.c_str());

  // Implicit
  // destruction
  s1.~string();
  s0.~string();
}
```

If an exception is thrown at point -1, the function exits early without executing any of its remaining code. As no objects have been created on the stack, nothing needs to be cleaned up before the function returns.

If an exception is thrown at point 0, the function exits early as before. However, since `string s0` has been created on the stack, it needs to be destroyed before exiting the function. Similarly, if an exception is thrown at point 1, both `string s1` and `string s0` must be destroyed.

These destructor calls would normally happen at the end of their enclosing scope, i.e. the bottom of the function, where the compiler inserts implicitly-generated destructor calls. However, since the function does not have any `try` blocks, none of the function's remaining code will execute after the exception is thrown. Therefore, the destructor calls at the bottom will not execute. If there were no other mechanism for destructing `s0` and/or `s1`, the result would be memory leaks or other state management issues involving those objects. Therefore, the C++ exception management runtime provides another mechanism to invoke their destructors: `wind` blocks and their corresponding `unwind` handlers.

`wind` blocks are effectively `try` blocks that are inserted invisibly by the compiler. They begin immediately after constructing some object, and end immediately before destructing that object. Their `unwind` blocks play the role of `catch` handlers, calling the destructor upon the object when exceptional control flow would otherwise cause the destructor call to be skipped.

Microsoft Visual C++ effectively transforms the previous example as follows:

```cpp
void may_throw_transformed() 
{
  if ( rand() % 2 )
    throw -1;

  string s0 = "0";

  // Implicit tryMaking C++ Exception Handling Smaller on x64
  __wind {
    if ( rand() % 2 )
      throw 0;

    string s1 = "1";

    // Implicit try
    __wind 
    {
      if ( rand() % 2 )
        throw 1;

      printf("%s %sn",
        s0.c_str(),
        s1.c_str());
    }

    // Implicit catch
    __unwind
    {
      s1.~string();
    }
    s1.~string();
  }

  // Implicit catch
  __unwind 
  {
    s0.~string();
  }
  s0.~string();
}
```

`unwind` blocks always re-throw the current exception, unlike `catch` handlers, which may or may not re-throw it. Re-throwing the exception ensures that prior `wind` blocks will have a chance to execute. So, for example, if an exception is thrown at point 1, after the `unwind` handler destroys `string s1`, re-throwing the exception causes the unwind handler for point 0 to execute, thereby allowing it to destroy `string s0` before re-throwing the exception out of the function.

**STATE NUMBERS AND INSTRUCTION STATES**

As we have discussed, the primary components of Microsoft Visual C++ x64 exception metadata are `try` blocks, `catch` handlers, `wind` blocks, and `unwind` handlers. Generally speaking, these elements can be nested within one another. For example, in C++ code, it is legal for one `try` block to contain another, and a `catch` handler may contain `try` blocks of its own. The same is true for `wind` and `unwind` constructs: `wind` blocks may contain other `wind` blocks (as in the previous example) or `try` blocks, and `try` and `catch` blocks may contain `wind` blocks.

Exceptions must be processed in a particular sequence: namely, the most nested handlers must be consulted first. For example, if a `try` block contains another `try` block, any exceptions occurring within the latter region must be processed by the innermost `catch` handlers first. Only if none of the inner `catch` handlers can handle the exception should the outer `try` block's catch handlers be consulted. Similarly, as in the previous example, `unwind` handlers must destruct their corresponding objects before passing control to any previous exception handlers (such as `string s1`'s `unwind` handler passing control to `string s0`'s `unwind` handler).

Microsoft's solution to ensure that exceptions are processed in the proper sequence is simple. It assigns a "state number" to each exception-handling construct. Each exception state has a "parent" state number whose handler will be consulted if the current state's handler is unable to handle the exception. In the previous example, what we called "point 0" is assigned the state number 0, while "point 1" is assigned the state number 1. State 1 has a parent of 0. (State 0's parent is a dummy value, -1, that signifies that it has no parent.) Since `unwind` handlers always re-throw exceptions, if state 1's `unwind` handler is ever invoked, the exception handling machinery will always invoke state 0's `unwind` handler afterwards. Because state 0 has no parent, the exception machinery will re-throw the exception out of the current function. This same machinery ensures that the catch handlers for inner `try` blocks are consulted before outer `try` blocks.

There is only one more piece to the puzzle: given that an exception could occur anywhere, how does the exception machinery know which exception handler should be consulted first? I.e., for every address within a function with C++ exception metadata, what is the current exception state? Microsoft C++/x64 binaries provide this information in the `IPtoStateMap` metadata tables, which is an array of address ranges and their corresponding state numbers.