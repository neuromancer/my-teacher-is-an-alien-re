# My Teacher is an Alien (1997) -- Source Code Reconstruction

A source code reconstruction of the full Windows 95 point-and-click adventure game *My Teacher is an Alien*, developed by 7th Level / Byron Preiss Multimedia (1997). The original game was written in C++ and compiled with Microsoft Visual C++ 4.20.

The reconstruction is bug-for-bug faithful: the reimplemented C++ routines produce code identical to the original at the CPU instruction level when compiled with the same compiler and flags, while allowing for layout differences in the executables.

This is a reconstruction-only project. Porting to a modern OS, improvements, and bugfixes will be handled by a separate project in the future.

## Status

Both the full game and demo are fully playable. Some screenshots running in DREAMM:

<img width="378" height="304" alt="Screenshot 2026-05-28 at 16 38 32" src="https://github.com/user-attachments/assets/a0ac2b7e-36ea-4e8a-b6ad-d5e4988e2a83" />

<img width="378" height="304" alt="Screenshot 2026-05-28 at 16 38 49" src="https://github.com/user-attachments/assets/48145419-1bd6-4d46-8938-dbad3834d920" />

<img width="378" height="304" alt="Screenshot 2026-05-28 at 16 39 23" src="https://github.com/user-attachments/assets/92aa397b-e4b0-47cb-8451-0114167025bc" />

<img width="378" height="304" alt="Screenshot 2026-05-28 at 16 46 11" src="https://github.com/user-attachments/assets/8d31a2f7-9517-44fa-acb8-ca78bae252d0" />


### Full Game Target

**Progress: 1405 / 1405 (100.00%)** -- 584 auto-complete functions marked

Latest `binary-comp report --config config/binary-comp.json --target full` summary:

| Metric | Count |
|--------|-------|
| Compared functions | 1129 |
| Exact matches | 542 |
| Similarity >= 90% | 950 |
| Similarity < 90% | 179 |
| Errors / NOT FOUND | 0 |
| Missing Ghidra assembly | 1 |
| Average similarity | 92.99% |

### Demo Reference

**Progress: 817 / 876 (93.26%)** -- 265 library functions excluded, 178 auto-complete

## Methodology

The project uses a combination of manual reverse engineering and LLM-assisted iteration:

1. **Disassembly**: Functions are extracted from the original binary using Ghidra, producing both disassembly and decompiled output.
2. **Implementation**: Each function is reimplemented in C++ guided by the disassembly (the only source of truth -- decompiled output is treated as a hint, not authoritative).
3. **Comparison**: The reimplemented code is compiled with the original MSVC 4.20 compiler, then [`binary-comp`](https://github.com/neuromancer/binary-comp) compares rebuilt instructions, calls, globals, data, values, and vtables against the original binary and Ghidra exports.
4. **Iteration**: The code is refined until similarity reaches >= 90%, often 100%.

Modern Clang is also used as an analysis-only pass to find undefined behavior,
64-bit portability hazards, and possible reconstruction mistakes without
changing the MSVC preservation target. See [docs/clang-analysis.md](docs/clang-analysis.md).

LLMs ([Claude](https://www.anthropic.com/claude) and [Codex](https://openai.com/index/codex/)) are used to accelerate step 2 and 4, making the process scalable to hundreds of functions. The workflow is described in the `CLAUDE.md` file.

## Building

### Prerequisites

**To build the executable** -- only a C++ toolchain and CMake (for `wibo`):

- [wibo](https://github.com/neuromancer/wibo) -- Win32 PE loader for running MSVC on Linux/macOS (submodule, built by `make`)
- [MSVC 4.20](https://github.com/itsmattkc/MSVC420) -- the original compiler (submodule)

**To run the rebuilt game**, additionally:

- [DREAMM](https://dreamm.aarongiles.com/) -- Windows 95/98 compatibility layer (auto-downloaded into `.dreamm/` on first `make run`)
- `bsdtar` (libarchive) -- used once to extract `data/full/teacher.iso` (full game only; the demo data is fetched automatically)

**For the verification/comparison tools** (optional, used during development):

- Python 3.10+ and the packages in `requirements.txt`, which installs [`binary-comp`](https://github.com/neuromancer/binary-comp) from GitHub
- Clang, if you want to run the analysis-only warning/static-analyzer scripts

### Setup

Build the executable:

```bash
git clone --recursive <repo-url>
cd my-teacher-is-an-alien-re
make TEACHER.EXE
```

If you intend to run `make report` / `make progress` or any of the verifiers, also install the Python tooling:

```bash
python3 -m pip install -r requirements.txt
```

### Data layout

The repository ships no game assets, so `data/` starts empty. Each target manages its own subtree:

- **Demo** -- nothing to do. `make run-demo` downloads the bundle from ScummVM's mirror into `data/demo/CDDATA/`, swaps in the rebuilt `TEACHER-DEMO.EXE`, and launches DREAMM.

- **Full game** -- supply the retail CD-ROM image once:

  ```bash
  mkdir -p data/full
  cp /path/to/teacher.iso data/full/
  ```

  Then `make run`. The Makefile extracts `CDDATA/` from the ISO into `data/full/` (TEACHER.EXE renamed to TEACHER.ORI.EXE, plus MSS/Smacker DLLs and the `DATA/` tree), creates `data/full/hd/` as DREAMM's writable `C:` drive, copies the rebuilt `TEACHER.EXE` into place, and mounts the ISO as `D:` for the in-game CD check. Extraction requires `bsdtar` (libarchive); it runs once and is a no-op on later invocations.

### Build targets

Full game:

| Command | Description |
|---------|-------------|
| `make run` | Build and launch in DREAMM (requires `data/full/teacher.iso`) |
| `make debug` | Same as `make run`, with DREAMM's debugger attached |
| `make progress` | Function coverage |
| `make report` | Per-function similarity report |
| `make analyze` | Run Clang warning and static-analysis passes |
| `make clean` | Remove build artifacts |

Demo:

| Command | Description |
|---------|-------------|
| `make run-demo` | Build and launch in DREAMM (auto-downloads demo data) |
| `make progress-demo` | Function coverage |
| `make report-demo` | Per-function similarity report |
| `make clean-demo` | Remove build artifacts |

### Debugging in DREAMM

`make debug` boots the full game under DREAMM's integrated kernel debugger (WDEB386). It halts at the initial breakpoint before the game starts; the debugger console takes commands directly.

Recommended starting sequence:

```
bpex *    # break on every exception (access violations, divide-by-zero, etc.)
g         # go -- resume execution until a breakpoint or exception fires
```

`bpex *` is essential: the reconstructed binary will diverge from the original at the first faulting instruction, and without an exception breakpoint Windows 95 will swallow the fault and the game will appear to hang or silently exit. When the debugger breaks in, use `r` to inspect registers and `u eip` to disassemble around the fault.

## Target Binaries

| Version | SHA256 |
|---------|--------|
| Full game, English retail | `75e691050b7a92a7874318fed6be7069b49c4e1755a3ac03764e0a85a71c9e96` |
| Demo reference | `5c618148696472e4715031de408b5e206e0680662866284851d5a7929cb153e2` |

## Acknowledgments

- [F-15 Strike Eagle II reconstruction](https://github.com/neuviemeporte/f15se2-re) -- the inspiration for this project
- [decompals/wibo](https://github.com/decompals/wibo) -- Win32 PE loader
- [itsmattkc/MSVC420](https://github.com/itsmattkc/MSVC420) -- archived compiler

## Legal

This repository contains no game assets, executables, or other copyrighted material. It is a clean rewrite of the game's source code based on analysis of the game binaries obtained from the original CD-ROM discs, for preservation and historical research purposes, and as such should fall under the interoperability exemption of the DMCA.
