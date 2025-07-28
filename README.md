#  My Teacher is an Alien (1997) reconstruction  source code reconstruction using LLMs

This is a work in progress project to reconstruct the source code for the Windows version of the 7th Level/Byron Preiss Multimedia  game ["My Teacher is an Alien"](https://adventuregamers.com/games/my-teacher-is-an-alien). The original game was written in C++ and compiled with the Microsoft Visual C++ 4.x.

The reconstruction aims to be bug-for-bug faithful, and the C++ routines yield code that's identical to the original at the CPU instruction level (when compiled with the same compiler with appropriate flags), while allowing for layout differences in the executables.

This is just the reconstruction project; porting to a modern OS, potential improvements and bugfixes will be targeted by a separate project in the future.

This repository contains no game assets, executables or other copyrighted material (except for the demo executable for reference), it's a clean rewrite of the game's source code based on the analysis of the game binaries obtained from the CD-ROM disks, for preservation and historical research purposes, and as such should fall under the interoperability exemption of the DMCA.

The general idea of doing a source code reconstruction were based on [the excelent F-15 Strike Eagle 2 reconstruction](https://github.com/neuviemeporte/f15se2-re). Very recommended to take a look there if you want to see a professional (and advanced!) reconstruction project.

The main difference betweeh the f15se2-re project and this reconstruction is the experimentation using LLMs to iterate on the C+ code to make the process scalable.

# Executable

The game executables has the following sha256 signatures:

* `5c618148696472e4715031de408b5e206e0680662866284851d5a7929cb153e2  exe/demo/TEACHER.EXE`

# Status

As of the time of writing this, the reconstruction has not started yet. We are just testing the technology to see if it works and setting up the initial scripts to automatically compile the complete binary and compare it with the original one. It is unclear if the technology is good enough, but some results are promising. Check the [src/folder](./src) to see examples of reconstructed functions. All the reconstructored source code was producing using [Gemini 2.5](https://blog.google/technology/google-deepmind/gemini-model-thinking-updates-march-2025/).

# Building

Prerequisites:

* `wine`
* relatively recent (3.8-ish) Python installed

If you want to see the LLM workflow in action:

* [cline](https://cline.bot/)
* Some LLM API (Gemini 2.5 recommended)