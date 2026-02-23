#  My Teacher is an Alien (1997) source code reconstruction using LLMs

This is a work in progress project to reconstruct the source code for the Windows version of the 7th Level/Byron Preiss Multimedia  game ["My Teacher is an Alien"](https://adventuregamers.com/games/my-teacher-is-an-alien). The original game was written in C++ and compiled with the Microsoft Visual C++ 4.x.

The reconstruction aims to be bug-for-bug faithful, and the C++ routines yield code that's identical to the original at the CPU instruction level (when compiled with the same compiler with appropriate flags), while allowing for layout differences in the executables.

This is just the reconstruction project; porting to a modern OS, potential improvements and bugfixes will be targeted by a separate project in the future.

This repository contains no game assets, executables or other copyrighted material (except for the demo executable and some sample game scripts for reference). This is a clean rewrite of the game's source code based on the analysis of the game binaries obtained from the CD-ROM disks, for preservation and historical research purposes, and as such should fall under the interoperability exemption of the DMCA.

The general idea of doing a source code reconstruction were based on [the excelent F-15 Strike Eagle 2 reconstruction](https://github.com/neuviemeporte/f15se2-re). Very recommended to take a look there if you want to see a professional (and advanced!) reconstruction project.

The main difference betweeh the f15se2-re project and this reconstruction is the experimentation using LLMs to iterate on the C+ code to make the process scalable.

# Executable

The game executables has the following sha256 signatures:

* `5c618148696472e4715031de408b5e206e0680662866284851d5a7929cb153e2  exe/demo/TEACHER.EXE`

# Status

Both the demo and full game binaries are being reconstructed. The demo is the primary target, with the full game build derived from it.

## Demo

**Progress: 831 / 876 (94.86%)** — 265 library functions excluded — 178 auto-complete functions marked

The intro video and the first screen from the demo are starting to run:

<img width="640" height="479" alt="image" src="https://github.com/user-attachments/assets/e92594ef-183c-472e-8bd6-101ee6b517a4" />

## Full Game

**Progress: 792 / 1273 (62.22%)** — 133 library functions excluded — 132 auto-complete functions marked

The full game build (`src-full/`) is derived from the demo sources with addresses remapped to match the full game binary. Both the demo and full game EXEs compile and link successfully.

# Building

Prerequisites:

* `wibo` (already included as a submodule)

Optionally, if you want to compute the similarity/progress reports:

* relatively recent (3.8-ish) Python installed
* [Levenshtein](https://pypi.org/project/Levenshtein/)

Clone the project and run `make` inside. If you want to see the LLM workflow in action, you need:

* [cline](https://cline.bot/) or Copilot.
* Some LLM API (Claude Opus recommended)

## Build targets

| Target | Command | Description |
|--------|---------|-------------|
| Compile demo | `make` | Compiles all demo sources (`src/` -> `out/`) |
| Compile full game | `make full` | Compiles all full game sources (`src-full/` -> `out-full/`) |
| Link demo EXE | `make TEACHER-DEMO.EXE` | Links the demo executable |
| Link full game EXE | `make TEACHER-FULL.EXE` | Links the full game executable |
| Demo progress | `make progress` | Shows function coverage for the demo |
| Full game progress | `make progress-full` | Shows function coverage for the full game |
| Clean demo | `make clean` | Removes demo build artifacts |
| Clean full game | `make clean-full` | Removes full game build artifacts |

Check the [src folder](./src) to see examples of reconstructed functions. All the reconstructed source code was producing using [Gemini 2.5](https://blog.google/technology/google-deepmind/gemini-model-thinking-updates-march-2025/) and Claude Opus.
