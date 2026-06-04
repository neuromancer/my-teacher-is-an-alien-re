# Clang Analysis

This repository preserves the original MSVC 4.20 reconstruction as the source
of truth. The Clang analysis pass is an additional diagnostic layer: it helps
find undefined behavior, 64-bit portability hazards, and source-only
reconstruction mistakes without changing the preservation target.

The analysis scripts use compatibility headers in `analysis/include/` and define
`TEACHER_ANALYSIS`. Those headers are intentionally analysis-only shims for
Win32/MSVC APIs, Miles, Smacker, and a few compiler attributes. They must not be
used by the normal MSVC build.

## Commands

Run the syntax-only warning pass:

```sh
make analyze-clang
```

Run the Clang Static Analyzer pass:

```sh
make analyze-static
```

Run both passes:

```sh
make analyze
```

Run either pass on a smaller set of files while triaging:

```sh
make analyze-clang ANALYZE_FILES="src/Parser.cpp src/SoundList.cpp"
make analyze-static ANALYZE_FILES=src/ZBufferManager.cpp
```

The underlying scripts can still be called directly as
`tools/analyze_clang.sh` and `tools/analyze_static.sh`.

Generated logs are written to `analysis/out/`, which is ignored by git. The
curated triage notes live in `analysis/RESULTS.md`.

## How To Triage Findings

Treat Clang findings as leads, not automatic fixes. For every high-signal
warning:

1. Check the corresponding dumped function in `code-full/FUN_*.disassembled.txt`.
2. If the warning is a reconstruction mistake, fix the reconstructed source and
   verify similarity with `binary-comp compare`.
3. If the warning is present in the original binary, preserve the behavior and
   add a short source comment noting the original bug.
4. If the warning is only modern portability debt, record it separately instead
   of changing the reconstruction.

Useful high-signal buckets include missing format arguments, uninitialized
values, divide-by-zero paths, fixed-address/null dereferences, incomplete
deletes, and unusual object/global `memset` patterns.

Lower-priority buckets are usually 64-bit portability or C++-modernization
noise: pointer/int casts, `%ld`/`%lu` width differences on LP64, writable string
literals, non-virtual destructor deletes, and repeated POD-like reset `memset`
calls. These are still useful for a future port, but they are not proof of an
original game bug.
