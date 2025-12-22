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

As of the time of writing this, the reconstruction of the demo executable already started. The progress is reported using ranges of functions (not all of them are implemented with 100%, but still provides a sense of progress):

```--- Progress Report ---
  ❌ 0x401000 - 0x4012c0 (17 functions)
  ✅ 0x4012d0 - 0x401500 (4 functions)
  ❌ 0x401540 - 0x401550 (2 functions)
  ✅ 0x401560 (1 function)
  ❌ 0x40166b - 0x401690 (2 functions)
  ✅ 0x4016a0 (1 function)
  ❌ 0x4016ff (1 function)
  ✅ 0x401710 - 0x401790 (2 functions)
  ❌ 0x401810 (1 function)
  ✅ 0x401890 - 0x401910 (2 functions)
  ❌ 0x40197b - 0x401990 (2 functions)
  ✅ 0x4019a0 - 0x401b60 (2 functions)
  ❌ 0x401c80 - 0x401e20 (7 functions)
  ✅ 0x401e30 - 0x402700 (9 functions)
  ❌ 0x402730 - 0x405c72 (68 functions)
  ✅ 0x405c80 (1 function)
  ❌ 0x405cc0 - 0x405cf0 (2 functions)
  ✅ 0x405d20 (1 function)
  ❌ 0x405dcd - 0x4093ec (57 functions)
  ✅ 0x409400 (1 function)
  ❌ 0x409440 - 0x4094a0 (3 functions)
  ✅ 0x4094f0 - 0x409bf0 (4 functions)
  ❌ 0x409cc2 (1 function)
  ✅ 0x409cd0 - 0x409d50 (2 functions)
  ❌ 0x409f00 - 0x40a290 (9 functions)
  ✅ 0x40a2e0 - 0x40a410 (3 functions)
  ❌ 0x40a56c - 0x40a5d0 (5 functions)
  ✅ 0x40a5e0 (1 function)
  ❌ 0x40a7aa - 0x40d200 (50 functions)
  ✅ 0x40d230 - 0x40d28c (2 functions)
  ❌ 0x40d2a0 - 0x411c8e (93 functions)
  ✅ 0x411d60 (1 function)
  ❌ 0x411de0 - 0x413380 (38 functions)
  ✅ 0x413670 - 0x4136f0 (2 functions)
  ❌ 0x413804 (1 function)
  ✅ 0x413810 - 0x413cf0 (6 functions)
  ❌ 0x413d50 - 0x4189d0 (112 functions)
  ✅ 0x4189f0 - 0x418f50 (18 functions)
  ❌ 0x418f60 - 0x418f90 (2 functions)
  ✅ 0x419010 - 0x4192f0 (11 functions)
  ❌ 0x419390 - 0x4193e0 (2 functions)
  ✅ 0x419420 - 0x4195a0 (2 functions)
  ❌ 0x4195c0 - 0x419660 (3 functions)
  ✅ 0x419770 - 0x419780 (2 functions)
  ❌ 0x419800 - 0x419fd0 (7 functions)
  ✅ 0x41a150 (1 function)
  ❌ 0x41a3b9 (1 function)
  ✅ 0x41a3d0 - 0x41a670 (3 functions)
  ❌ 0x41a810 - 0x41a8c0 (2 functions)
  ✅ 0x41a9a0 - 0x41ad50 (19 functions)
  ❌ 0x41ae0c (1 function)
  ✅ 0x41ae20 - 0x41aee0 (4 functions)
  ❌ 0x41af9f - 0x41afb1 (2 functions)
  ✅ 0x41afc0 (1 function)
  ❌ 0x41b07f - 0x41b0a0 (3 functions)
  ✅ 0x41b110 (1 function)
  ❌ 0x41b29a - 0x41b2ac (2 functions)
  ✅ 0x41b2c0 - 0x41b310 (2 functions)
  ❌ 0x41b3b0 - 0x41cd30 (28 functions)
  ✅ 0x41cd50 - 0x41ce30 (2 functions)
  ❌ 0x41cef0 - 0x41cf08 (2 functions)
  ✅ 0x41cf10 - 0x41e580 (24 functions)
  ❌ 0x41e666 (1 function)
  ✅ 0x41e670 - 0x41eb70 (13 functions)
  ❌ 0x41eb90 - 0x41ebb0 (2 functions)
  ✅ 0x41ec60 - 0x41ef25 (4 functions)
  ❌ 0x41ef47 (1 function)
  ✅ 0x41ef50 - 0x41f360 (4 functions)
  ❌ 0x41f471 - 0x41f480 (2 functions)
  ✅ 0x41f4f0 (1 function)
  ❌ 0x41f560 (1 function)
  ✅ 0x41f800 (1 function)
  ❌ 0x41f9d0 (1 function)
  ✅ 0x41fa50 - 0x41fb70 (5 functions)
  ❌ 0x41fbd3 (1 function)
  ✅ 0x41fbe0 - 0x41fd80 (8 functions)
  ❌ 0x41fdfc (1 function)
  ✅ 0x41fe20 - 0x420480 (15 functions)
  ❌ 0x42055c (1 function)
  ✅ 0x420570 - 0x4209c0 (6 functions)
  ❌ 0x4209e0 - 0x420f10 (8 functions)
  ✅ 0x420f80 - 0x421010 (2 functions)
  ❌ 0x4210a8 - 0x4210bd (2 functions)
  ✅ 0x4210d0 (1 function)
  ❌ 0x421260 - 0x4213f0 (2 functions)
  ✅ 0x421420 (1 function)
  ❌ 0x42157a - 0x421680 (5 functions)
  ✅ 0x421700 (1 function)
  ❌ 0x421770 - 0x421ea0 (19 functions)
  ✅ 0x421eb0 - 0x421f40 (3 functions)
  ❌ 0x421f90 - 0x422090 (3 functions)
  ✅ 0x4220a0 (1 function)
  ❌ 0x422250 (1 function)
  ✅ 0x4223f0 (1 function)
  ❌ 0x422430 - 0x422510 (9 functions)
  ✅ 0x422520 (1 function)
  ❌ 0x422690 - 0x422ac3 (18 functions)
  ✅ 0x422d98 - 0x422e02 (3 functions)
  ❌ 0x422e1a - 0x422e8f (3 functions)
  ✅ 0x422f00 (1 function)
  ❌ 0x423076 (1 function)
  ✅ 0x423099 (1 function)
  ❌ 0x4230d9 - 0x423843 (13 functions)
  ✅ 0x4238d9 - 0x423905 (2 functions)
  ❌ 0x4239c8 - 0x424922 (35 functions)
  ✅ 0x424940 (1 function)
  ❌ 0x424950 (1 function)
  ✅ 0x4249c0 - 0x4249d0 (2 functions)
  ❌ 0x424a60 - 0x424ae0 (2 functions)
  ✅ 0x424b00 (1 function)
  ❌ 0x424b90 - 0x425e20 (23 functions)
  ✅ 0x425e50 (1 function)
  ❌ 0x425e70 - 0x426070 (9 functions)
  ✅ 0x4260f0 - 0x426110 (2 functions)
  ❌ 0x426140 - 0x426180 (2 functions)
  ✅ 0x4261c0 (1 function)
  ❌ 0x426320 - 0x427760 (22 functions)
  ✅ 0x4277e0 (1 function)
  ❌ 0x427800 - 0x428440 (9 functions)
  ✅ 0x428460 - 0x4284a0 (2 functions)
  ❌ 0x4285a0 - 0x42b150 (51 functions)
  ✅ 0x42b300 (1 function)
  ❌ 0x42b370 - 0x42c540 (27 functions)
  ✅ 0x42c5c0 (1 function)
  ❌ 0x42c5f0 - 0x430260 (112 functions)
  ✅ 0x430310 (1 function)
  ❌ 0x430360 - 0x4304e0 (4 functions)

Progress: 225 / 1142 (19.70%)
```

Check the [src folder](./src) to see examples of reconstructed functions. All the reconstructed source code was producing using [Gemini 2.5](https://blog.google/technology/google-deepmind/gemini-model-thinking-updates-march-2025/).

# Building

Prerequisites:

* `wine`

Optionally, if you want to compute the similarity/progress reports:

* relatively recent (3.8-ish) Python installed
* [Levenshtein](https://pypi.org/project/Levenshtein/)

Clone the project and run `make` inside. If you want to see the LLM workflow in action, you need:

* [cline](https://cline.bot/) or Copilot.
* Some LLM API (Claude recommended)
