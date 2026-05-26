# My Teacher is an Alien (1997) -- Source Code Reconstruction

A source code reconstruction of the full Windows 95 point-and-click adventure game *My Teacher is an Alien*, developed by 7th Level / Byron Preiss Multimedia (1997). The original game was written in C++ and compiled with Microsoft Visual C++ 4.20.

The reconstruction is bug-for-bug faithful: the reimplemented C++ routines produce code identical to the original at the CPU instruction level when compiled with the same compiler and flags, while allowing for layout differences in the executables.

This is a reconstruction-only project. Porting to a modern OS, improvements, and bugfixes will be handled by a separate project in the future.

## Legal

This repository contains no game assets, executables, or other copyrighted material. It is a clean rewrite of the game's source code based on analysis of the game binaries obtained from the original CD-ROM discs, for preservation and historical research purposes, and as such should fall under the interoperability exemption of the DMCA.

## Status

The full retail game (`src/`) is the primary reconstruction target. The demo (`src-demo/`) is kept as a secondary reference because it shares many systems and is useful for cross-checking behavior.

### Full Game Target

**Progress: 1404 / 1405 (99.93%)** -- 583 auto-complete functions marked

Latest `binary-comp report --config config/binary-comp.json --target full` summary:

| Metric | Count |
|--------|-------|
| Compared functions | 1125 |
| Exact matches | 496 |
| Similarity >= 90% | 873 |
| Similarity < 90% | 252 |
| Errors / NOT FOUND | 0 |
| Missing Ghidra assembly | 0 |
| Average similarity | 89.88% |

### Demo Reference

**Progress: 817 / 876 (93.26%)** -- 265 library functions excluded, 178 auto-complete

## Methodology

The project uses a combination of manual reverse engineering and LLM-assisted iteration:

1. **Disassembly**: Functions are extracted from the original binary using Ghidra, producing both disassembly and decompiled output.
2. **Implementation**: Each function is reimplemented in C++ guided by the disassembly (the only source of truth -- decompiled output is treated as a hint, not authoritative).
3. **Comparison**: The reimplemented code is compiled with the original MSVC 4.20 compiler, then [`binary-comp`](https://github.com/neuromancer/binary-comp) compares rebuilt instructions, calls, globals, data, values, and vtables against the original binary and Ghidra exports.
4. **Iteration**: The code is refined until similarity reaches >= 90%, often 100%.

LLMs ([Claude](https://www.anthropic.com/claude) and [Codex](https://openai.com/index/codex/)) are used to accelerate step 2 and 4, making the process scalable to hundreds of functions. The workflow is described in the `CLAUDE.md` file.

### Key tools

| Tool | Purpose |
|------|---------|
| `binary-comp compare` | Compare a single rebuilt function against the original |
| `binary-comp report` | Build everything and produce a per-function similarity report |
| `bin/showProgress.py` | Show overall function coverage |
| `binary-comp globals` | Verify globals and audit auto-completed CRT/compiler functions that write global storage |
| `binary-comp calls` | Verify that reconstructed functions call the expected original targets |
| `binary-comp vtables` | Verify class inheritance, vtable slots, and slot implementations |
| `binary-comp values` | Compare extracted constants and string references against original disassembly |
| `binary-comp exe` | Compare rebuilt and original executable layout and bytes |
| `binary-comp data` | Compare global/static data sections |

[`binary-comp`](https://github.com/neuromancer/binary-comp) is included in this repository under `binary-comp/` and is installed as an editable local package by `requirements.txt`.

The Makefile-facing report and verifier tools read project-specific paths, address ranges, aliases, type sizes, and known allowances from `config/binary-comp.json`. See `docs/verification-scripts.md` for the reusable tool/config layout.

## Building

### Prerequisites

- [wibo](https://github.com/decompals/wibo) -- Win32 PE loader for running MSVC on Linux/macOS (included as a submodule)
- [MSVC 4.20](https://github.com/itsmattkc/MSVC420) -- the original compiler (included as a submodule)
- Python 3.10+ and the packages in `requirements.txt`, including the local `binary-comp` CLI and its analyzer dependencies
- [DREAMM](https://dreamm.aarongiles.com/) -- Windows 95/98 compatibility layer for running the rebuilt executable with `make run` or `make run-demo`

### Setup

```bash
git clone --recursive <repo-url>
cd my-teacher-is-an-alien-re
python3 -m pip install -r requirements.txt
make
```

### Build targets

Primary full-game targets:

| Target | Command | Description |
|--------|---------|-------------|
| Full game (default) | `make` | Compile full game sources (`src/` -> `out/`) |
| Link full game | `make TEACHER.EXE` | Link the full game executable |
| Run full game | `make run` | Build and run the full game in DREAMM (requires `data/full/teacher.iso`) |
| Full game progress | `make progress` | Show function coverage for the full game |
| Full game report | `binary-comp report --config config/binary-comp.json --target full` | Per-function similarity report for the full game |
| Verify globals | `binary-comp globals --config config/binary-comp.json --target full --fail-on-issues --fail-on-warnings` | Check global declarations, addresses, types, and initialized values |
| Verify call targets | `binary-comp calls --config config/binary-comp.json --target full` | Check reconstructed call targets against original disassembly |
| Verify vtables | `binary-comp vtables --config config/binary-comp.json --target full` | Check inheritance, vtable slots, and slot implementations |
| Compare globals | `binary-comp data --config config/binary-comp.json --target full` | Compare global data sections |
| Clean full game | `make clean` | Remove full game build artifacts |

Secondary demo/reference targets:

| Target | Command | Description |
|--------|---------|-------------|
| Demo | `make demo` | Compile demo sources (`src-demo/` -> `out-demo/`) |
| Link demo | `make TEACHER-DEMO.EXE` | Link the demo executable |
| Run demo | `make run-demo` | Build, download data if needed, and run the demo in DREAMM |
| Demo progress | `make progress-demo` | Show function coverage for the demo |
| Demo report | `binary-comp report --config config/binary-comp.json --target demo` | Per-function similarity report for the demo |
| Compare demo executable | `binary-comp exe --config config/binary-comp.json --target demo` | Byte-level comparison of original vs rebuilt demo |
| Compare demo functions | `binary-comp exe --config config/binary-comp.json --target demo --functions` | Function-level comparison of original vs rebuilt demo |
| Clean demo | `make clean-demo` | Remove demo build artifacts |

### Comparing a single function

```bash
binary-comp compare --config config/binary-comp.json --target full ClassName::MethodName code-full/FUN_XXXXXX.disassembled.txt
```

This compiles the project and shows a side-by-side diff of rebuilt and original instructions with a similarity percentage.

## Target Binaries

| Version | SHA256 |
|---------|--------|
| Full game, English retail | `75e691050b7a92a7874318fed6be7069b49c4e1755a3ac03764e0a85a71c9e96` |
| Demo reference | `5c618148696472e4715031de408b5e206e0680662866284851d5a7929cb153e2` |

## Acknowledgments

- [F-15 Strike Eagle II reconstruction](https://github.com/neuviemeporte/f15se2-re) -- the inspiration for this project
- [decompals/wibo](https://github.com/decompals/wibo) -- Win32 PE loader
- [itsmattkc/MSVC420](https://github.com/itsmattkc/MSVC420) -- archived compiler
