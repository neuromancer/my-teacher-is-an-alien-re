# HINTS — getting good assembly-similarity scores (MSVC 4.20 / *My Teacher is an Alien*)

Practical tips for re-implementing the full game so the C++ source compiles to assembly
that matches the original `TEACHER.EXE`. Apply them when a function reads cleanly but
stalls below 95%.

```
make TEACHER.EXE -j12                                       # incremental rebuild + relink
binary-comp compare --config config/binary-comp.json --target full <Name> code-full/FUN_XXXXXX.disassembled.txt --no-build
make report                                                 # whole-program similarity table
python3 bin/ndiff.py --no-build Name:0xADDR ...             # normalized-mnemonic diff (only the divergent blocks)
python3 bin/triage.py /tmp/report.txt --min 85 --max 95 --no-build --sort pol
```
The fast loop is: edit → `make TEACHER.EXE -j12` (incremental, sub-second) → `binary-comp
compare … --no-build`. Run `--no-build` so you don't trigger a full `make clean` rebuild.

> **Provenance / how to read this file.** These notes were originally collected on a
> *different* project — *Arthur's 1st Grade*, a **C** codebase built with **MSVC 6.0**. They
> were ported here and **validated against this project** (C++, **MSVC 4.20**, the
> `binary-comp` comparator). Hints that did not survive validation were removed; the ones
> that changed meaning under MSVC 4.20 / C++ were corrected and are flagged
> **[MSVC4.20]**. The biggest differences from the source project: this is **C++ not C**
> (so K&R/`#include`-promotion hints are gone), the build is **`/Og /Oi /Ot /Oy /Ob1 /Gs
> /Gf /GX`** (not `/O2`) and **uniform** (no per-function `#pragma optimize`), and the
> comparator disassembles **both** sides from PE bytes with capstone.

---

## 0. The single most important fact: the comparator is *mnemonic-only*

`binary-comp` scores similarity as a **Levenshtein distance over the normalized mnemonic
sequence** (`binary_comp/core/disasm.py`, `analyzers/function_compare.py`). It **ignores**
operands, registers, immediates, addresses, symbol names, and operand sizes. Consequences:

- `mov eax,[x]` ≡ `mov ecx,[y]` ≡ `mov word ptr [z],5` — all just `mov`. **You do not need
  the right registers, offsets, or constants** — only the right *instruction in the right
  place*. A wrong stride/scale that adds **no extra instruction** costs nothing
  (`lea [b+i*2]` ≡ `lea [b+i]`; `imul eax,0x18` ≡ `imul eax,0x36`).
- **Branch mnemonics are normalized** to a canonical form, so condition *spelling* never
  matters: `je↔jz`, `jne↔jnz`, `jb↔jc`, `jae↔jnc`, `jg↔jnle`, `jge↔jnl`, `jl↔jnge`,
  `jle↔jng`. There is **no branch-naming floor** — a `<100%` score is a genuine
  mnemonic-sequence difference (different/extra/missing instruction or different
  control-flow shape) and is worth a structural look.
- **Both sides are disassembled from the actual PE bytes with capstone.** The Ghidra export
  (`code-full/FUN_*.disassembled.txt`) is used only to locate the function's block
  boundaries; the mnemonics compared come from re-disassembling `TEACHER.ORI.EXE` and your
  `TEACHER.EXE`. So tokenization (incl. `rep` prefixes) is **symmetric** — there is no
  "Ghidra spells it differently" asymmetry to worry about. **[corrected]** (`sete`/`setz`
  are **not** normalized — but they tokenize identically on both sides anyway.)
- Padding/data is skipped: `db`, `dd`, `dw`, `npad`.
- **The real residual floors** (genuinely hard to close from clean C++) are
  **register-allocation/spills**, **instruction scheduling (transpositions)**, and the
  `sub esp,N` frame-size / `add esp,N` arg-cleanup counts. On this project, *most* small
  sub-95 functions are stuck on exactly these (see §10) — verify before sinking time in.

## 0a. Operand-level bugs the score can't see — use the *native* verifiers

The similarity score is behaviour-blind: `<` vs `<=`, `if(x)A else B` vs `if(!x)B else A`,
`-1` vs `0xffff`, reordered independent stores — all change the score but **not behaviour**.
Conversely a wrong *operand* (struct field offset, constant, global address) is a real bug
the mnemonic-only score **cannot see at all**. This project ships native tools to catch
those — prefer them over eyeballing:

- `make verify-values` / `make verify-values-stack-locals` — compares concrete immediate /
  constant values against the original.
- `make verify-global-access` — flags reads/writes of the wrong global.
- `make verify-calls` — checks the call-target multiset per function.
- `make verify-globals` / `make verify-vtables` — global layout and vtable checks.

Get a function compiling and *faithful* first (right fields, right calls), then chase %.

---

## 1. Workflow & function structure

- One `/* Function start: 0xADDR */` marker immediately before each function (the progress
  tools key off these).
- **Sort functions by address within each file** — MSVC emits functions in source order, so
  ordering also makes block-layout / wrong-class-ownership bugs visible.
- Callbacks used *before* their address-sorted definition need a **plain forward
  declaration**; `extern` is for cross-file decls only.
- `code-full/FUN_*.decompiled.txt` is a *hint*, not ground truth (CLAUDE.md). The
  disassembly and `code-full/strings.txt` are the only authorities.

---

## 2. Return types & leaked registers

- **16-bit returns:** a function that only sets `AX` (returns a `short`/bool) but is declared
  `int`/`unsigned int` emits an `and eax,0xffff0000`-style epilogue. Declaring it
  `short`/`unsigned short` gives a clean `mov ax,..; ret` / `xor ax,ax; ret`.
- **8-bit returns → `unsigned char`** for a function whose binary returns a byte
  (`mov al,..` with no mask).
- **Leaked-register junk** (`in_EAX`, `extraout_ECX/EDX`, `CONCAT22(...)`,
  `(uint)ptr & 0xffff0000`) is Ghidra modelling a register the function never meaningfully
  sets. Make the function `void` / `return 0;` where the decl or callback-cast allows;
  the comparator only sees `xor`/`mov`.
- **`CONCAT22(hi,lo)` / `CONCAT31(...)` are Ghidra pseudo-ops, NOT C++.** Rewrite as
  `(hi<<16)|(unsigned short)lo`, or just assign the low word when only it is used.
- **[MSVC4.20] You cannot "fall off the end" of a non-`void` function to drop an
  `xor eax,eax`.** MSVC 4.20 treats a missing return as a **hard error C2561** ("function
  must return a value") — not a warning. So when the original is a bare `RET`/`RET n` with no
  `xor eax,eax` (i.e. it's effectively `void`) but your function is typed `int`, the *only*
  way to match is to change the return type to `void`. For a **virtual** that is
  hierarchy-wide: the base declaration **and every override** must change together (C++ rejects
  override-by-return-type-only). This IS worth doing for a whole vtable slot: the
  `Handler::ShutDown(SC_MessageParser*)` family (34 decls, all `return 0;`) was `int` but the
  original is a bare `RET 0x4`; changing base + all 34 overrides to `void` took ~19 of them to
  100% (no caller used the result). Caveats: **(a)** MSVC 4.20 also errors **C2562** on
  `return <void-expr>;`, so a tail `return Base::ShutDown(msg);` must become a plain
  `Base::ShutDown(msg);` call; **(b)** the Makefile has no header dependencies, so after editing
  a shared `.h` you must `make clean` — otherwise stale callers keep the old `int` mangling and
  the link fails with unresolved-external.

---

## 3. Control flow — the highest-yield lever (validated here)

Confirmed on this project: `GetFileSize` 93→**100**, `Sound::FindFreeSampleHandle` 93→**100**.

- **`if`/`else` polarity.** `if (x!=0){A}else{B}` and `if (x==0){B}else{A}` are equal but lay
  out differently. Match the original's `test/jz` direction so the *then*-branch the original
  reaches by fall-through stays the fall-through. **`GetFileSize`:** the original `JNZ
  error; <success fall-through>` needed `if (_stat(...)==0){ return size; } return -1;`
  (success first) instead of `if (...!=0){ return -1; } return size;`.
- **Early-return-zero whose success path the binary lays out as fall-through** → flip it:
  `if(c!=0){ …; return 1; } return 0;` rather than `if(c==0) return 0; …; return 1;`.
- **Single shared epilogue / loop-with-break.** When several paths converge on one `ret`,
  structure the C++ so they do. **`FindFreeSampleHandle`:** the original loops with a
  `break` and then a **shared** post-loop test (`if (count==i) return 0; return arr[i];`),
  re-reading the count — *not* an early `return arr[i];` inside the loop. Match whichever
  the disasm shows (separate `xor ax,ax` per fail path = use direct returns; one merged
  `mov ax,[u]` = funnel through a single return).
- **Empty-then / else.** When the disasm reaches the body via a *positive* jump and skips via
  a trailing `jmp` (`cmp; j<cond> body; jmp end; body:…`), the body is the **else** of an
  empty then: `if (cond_to_skip) { } else { body; }`. (Diff first — applying it to the wrong
  `if` *lowers* the score.)
- **Comparison operand order flips `jg`↔`jl` etc.** `if (hi <= i)` loads `hi` first; the
  binary often loads the variable first — write `if (i >= hi)`. (Mnemonic-only, so this only
  matters when it changes the *branch kind* via flag reuse, not operand order per se.)
- **`switch` vs `if`/`else-if`.** Consecutive integer cases compile to a `sub;jz;dec;jz;…`
  chain — that's a **`switch`**, not a `cmp/je` cascade. A dispatcher whose disasm is `cmp X;
  ja; jz` (one cmp, two conditional jumps reusing flags) is also a `switch`. Match the case
  **body order** to the jump-table label→body map (often reverse of source order).
- **`goto` direction.** `if(X) goto a; goto b;` ≠ `if(!X) goto b; goto a;` — flip to match
  `jz`/`jnz`.

---

## 4. Loops

- **`while(1){ if(end) return; if(match) break; advance; }` emits a literal `mov ecx,1;
  test; je` at the top** the original doesn't. Make the termination the loop *condition*:
  `while (i<count){ if(match){…;return 1;} advance; } return 0;`.
- **Decrement *inside* the body ⇒ `while`, not `for`.**
- **Search loops whose `i==0` access is a constant address get iteration-0 *peeled* by a
  do/while.** A plain `for (i=0;i<N;i++)` suppresses the peel.
- **16-bit counters:** `i += 1` keeps the op 16-bit (`add ax,1`); `i = i + 1` can trigger
  `movsx; add` promotion. *(The bulk `do-while→for` / `+=` rewriters in `bin/` found **no**
  candidates in this codebase — the sources are already hand-written in these forms; see §11.)*

---

## 5. Calls & arguments

- **Nested calls beat temps.** `outer(inner(a,b), c)` emits two `add esp,N` cleanups; hoisting
  to `t = inner(...); outer(t,c)` defers to one big cleanup and breaks similarity. Inline call
  results into compares too: `if (local < GetFoo())`.
- **Tail calls.** `void f(){ stuff; g(); }` with `void g()` → MSVC emits `jmp g`, not
  `call g; ret`. (Don't *add* a wrapper to force this — forbidden.)
- **[thiscall] Class methods are `__thiscall` (`this` in ECX, callee `RET n`).** Never
  spell `__thiscall` and never change the convention (CLAUDE.md). A call site that is
  `PUSH…CALL…ADD ESP n` is `cdecl`; a callee ending `RET n` is `thiscall`/`stdcall`. Ghidra
  sometimes mislabels these and invents `in_ECX`/extra params — verify real arity at the call
  site (no `mov ecx,X` / extra `push` ⇒ the arg doesn't exist).
- **Hallucinated args** (`extraout_ECX`, `extraout_EDX`, `in_stack_*`) — drop them.
- **By-value struct params.** A call site doing `SUB ESP,N; … REP MOVSD; CALL; ADD ESP,N+args`
  passes a struct *by value*. A standalone `typedef struct { char data[N]; }` passed by value
  is **not** a forbidden substructure; `sizeof` must equal the exact bytes copied.

---

## 6. Globals & data types

- **Direct pointer cast beats struct-field reassembly.** Read a struct word as
  `*(unsigned int*)&g`, not `(g.hi<<16)|g.lo` → single `mov eax,[g]`.
- **Inline field access beats a cached pointer** when the disasm recomputes `&arr[i]` each
  access: write `arr[i].x` each time rather than `p=&arr[i]; p->x`.
- **Signed vs unsigned width drives `movsx` vs `movzx`/`and`.** Index/flag global as `short`
  → `movsx`; as `unsigned short` → `mov; and 0xffff` (or `movzx`). Pick the type matching the
  disasm's extension instruction. Drop a redundant explicit `& 0xffff` on an
  already-`unsigned short` value (MSVC widens once; the mask adds a *second* `and`).
- **`__int64` grouped copy.** **[MSVC4.20: use `__int64`]** (already used in `FilePosCache` /
  `SoundTracker`). When the disasm reads BOTH source dwords then writes BOTH
  (`mov edx,[s]; mov eax,[s+4]; mov [d],edx; mov [d+4],eax`), a whole RECT/POINT-pair copy,
  write `*(__int64*)dst = *(__int64*)src;` — naive `d[0]=s[0]; d[1]=s[1];` interleaves
  read/write/read/write and mismatches.
- **Two adjacent 16-bit fields set from one 32-bit value → one dword store:**
  `*(unsigned int*)&obj->x = p;` (the split `obj->x=(short)p; obj->y=(short)(p>>16);` emits
  `mov word; shr; mov word`).
- **Zero a short array as dwords:** `*(int*)(s+2)=0` → one `mov dword[],0`.
- **Reconstruct the original's collapsed stack locals (NOT a rule violation — they're real).**
  The decompiler folds `a=node; b=*(a+0xc); node=b;` into `node=*(node+0xc);`. If the disasm
  shows extra stack slots written/read, re-introduce those locals **in declaration order
  matching the slot offsets** (1st decl → lowest slot). These are the original's own locals.

---

## 7. Arithmetic idioms (write the high-level op, let MSVC regenerate the magic)

- **Signed division by constant → write `x / K`**, not the expanded magic; MSVC regenerates
  the `cdq/and/add/sar` or `__int64`-magic sequence. **[MSVC4.20: `__int64`, not `long
  long`.]** Caveat: the *exact* magic sequence differs between compiler versions — since the
  comparator is mnemonic-only the *shape* still usually matches, but verify per function
  rather than assuming.
- **A Ghidra `(longlong)` cast can be a FALSE artifact** — check the disasm: `__alldiv`/
  `__aulldiv` call = real 64-bit; `CDQ; IDIV` = plain 32-bit → drop the cast.
- **Mod-by-2^k → write the high-level `(int)x % N`**, not the decompiler's materialized-boolean
  expansion (`x & 0x80000007; …`), which emits a wasteful `sete`/`test`. The high-level form
  regenerates the branchless `and; jns; dec; or; inc; jne`.
- **`__ftol()` (no args) is the MSVC FPU→long helper, not a float param.** The value is
  `(int)((double)someInt * doubleConst)`; read the `FILD…FMUL [const]` before `CALL __ftol`
  and write that — mnemonic-only, so the actual index/constant don't matter.

---

## 8. Optimization level & pragmas — mostly N/A on this project

**[MSVC4.20 / this build]** The full game builds with a **single, uniform** flag set
(`Makefile`): `/Og /Oi /Ot /Oy /Ob1 /Gs /Gf /GX` (global opt, intrinsics, favour speed,
**frame-pointer omission**, inline marked functions, no stack probes, string pooling, EH).
There are **no per-function `#pragma optimize` directives anywhere in `src/`**, and the
build does not mix optimization levels.

- MSVC 4.20 *does* accept `#pragma optimize("", off)` / `("y", off)` / `#pragma
  function(memcpy)` (verified — they compile). But because the codebase is built uniformly,
  the "flip a wrongly-applied `optimize("",off)`" / "switch `""`→`"y"`" workflow from the
  source project **has nothing to operate on here** — that is why `tryopt.py` was *not*
  ported. Do **not** add pragmas speculatively; CLAUDE.md forbids changing compiler flags,
  and a function that is hard to match is far more often a register-allocation ceiling (§10)
  than a missing-pragma problem.
- A function with no EBP frame in the original (esp-relative, register vars) is normal `/Og
  /Oy` output — do not try to force a frame.

---

## 9. Inlined CRT (strlen / strcmp / memcpy)

Ghidra inlines these; the decompiled loops are valid C++ and compile back to the same thing —
**keep them verbatim**, just retype `undefined*`/`byte`/`bool`:

- **strlen:** `n=0xffffffff; do { …; n--; c=*p++; } while (c!='\0'); n=~n;`
- **memcpy:** the `for (i=n>>2) *(int*)d=*(int*)s;` + byte tail = `REP MOVSD` + tail; for a
  constant size `memcpy(d,s,K)` regenerates `rep movsd; movsw; movsb`. You may use `memcpy`/
  `strcpy`/`memset` only where the compiler inlines them to match (CLAUDE.md).
- **strcmp:** the 2-byte-unrolled byte-compare chain — keep verbatim, or call `strcmp`.

---

## 10. Things that *can't* be matched in clean C++ (skip or accept the score)

On this project these dominate the small sub-95 long tail — confirm with `ndiff.py` before
spending time:

- **Register-allocation ceilings.** The original keeps a value in a different callee-saved
  register, or one more/fewer of them (`push edi` vs `push esi`, an extra `xor edi,edi`).
  `ndiff` shows a same-mnemonic `insert`+`delete` pair or a `push`/`pop` count difference.
  Unforceable from C++.
- **Instruction scheduling (transpositions).** The same instruction appears on both sides at
  different positions (`cmp cl,bl` vs `cmp dl,cl`, a hoisted `push 0`, a `mov eax,6` moved a
  few slots). `ndiff` shows it as a delete here / insert there. Unforceable.
- **Argument-evaluation-order / `operator new` arg hoist.** The binary materialises an arg at
  the very top of the function (e.g. `PUSH 0x300` before the field stores in
  `Palette::Palette`); clean C++ pushes it next to the call. Reproducing it needs artificial
  ordering — accept the gap.
- **`sub esp,N` frame size / `add esp,N` cleanup count** off by one slot.
- **JMP thunks / shared-tail fragments / secondary entry points** (`make progress`
  auto-completes most — don't write wrappers, forbidden).
- **CRT routines & SEH-heavy startup** (`strcpy`, `GenerateRandom`, `WinMain`).
- **The whole graphics subsystem — `Graphics.cpp` / `VideoTable.cpp` / `Blit.cpp` /
  `ScaleBuffer.cpp` / `PaletteUtils.cpp` (~25 funcs at <55%) — is a hard codegen ceiling.**
  The originals use one-operand `MUL`/`XLAT`, no CSE (they re-read the DC global and re-`call`
  the import thunk each time), direct import-thunk calls, and no callee-saved-register use; our
  `/Og` build caches those in registers and uses two-operand `imul`. No clean-C++ form **and no
  `#pragma optimize` variant** reproduces it (tested `g`/`""`/`t`/`a`/`s` on `GetColorBitDepth`
  — all ≤50%). Accept the low scores; don't chase.
- **SEH-split `LBLParse` entries.** A `LBLParse` reported at `0xNNNN0` with ~0–10% similarity
  is usually the SEH-prologue funclet split from the real body (which lives at an adjacent
  address and often already scores high). It is a mapping artifact, not a source bug.

---

## 11. Tooling (ported from the source project, adapted for `binary-comp` + `src/*.cpp`)

`bin/cmp_lib.py` is the shared helper (paths, `build()`, `sim(name,addr)` via `binary-comp
compare --no-build`, report parsing). All scripts run from the repo root.

- **`bin/ndiff.py [--no-build] Name:0xADDR …`** — the normalized-mnemonic diff: only the
  divergent instruction blocks, raw asm both sides. **Working and indispensable here** —
  reuses `binary-comp`'s own disassembler so the stream is exactly what the score sees.
- **`bin/triage.py REPORT.txt [--min N --max M] [--no-build] [--sort pol|ratio|size]`** —
  categorizes sub-95 functions by diff shape: ratio, #replace/insert/delete blocks, changed
  insns, and **POL** = opposite-polarity branch pairs (a strong "if/else flip will help"
  signal). Build the report first: `make report > /tmp/report.txt`. **Working.** (Caveat:
  Ghidra hands out duplicate recovered names and a few names alias the same address — POL on
  huge `LBLParse` parsers is inflated by SequenceMatcher misalignment; trust it on small,
  high-ratio rows.)
- **`bin/trysweep.py REPORT.txt [inc|compound|cmp|swap|all] [File.cpp]`** — keep-if-improves
  identity-preserving rewrites (`x=x+1`→`x+=1`, `x=x op E`→`x op= E`, `-1<x`→`x>=0`,
  `CONST<x`→`x>CONST`). Reverts anything that doesn't strictly improve, so it can never lower
  a score.
- **`bin/tryforloop.py` / `bin/trymask.py` / `bin/tryspill.py REPORT.txt [File.cpp…]`** —
  keep-if-improves do-while→for / drop-redundant-`&0xffff` / dead-spill-inline.
- **⚠ On *this* codebase the bulk transformers found 0 wins / 0 candidates** (a full sweep
  with all of the above). The source project was raw decompiler output full of `x=x+1`,
  peeled do-while loops, masked `ushort` calls and dead spills; **this** project's `src/` is
  hand-written/cleaned, so those artifact-patterns occur almost only in already-≥95%
  functions. The scripts are kept (harmless, revert-on-no-gain) for any *newly* added
  raw-decompiled function, but don't expect them to carry the work — the real wins here are
  **manual structural fixes** (§3) found via `ndiff`/`triage`.
- **`tryopt.py` was deliberately not ported** — there are no `#pragma optimize` directives to
  flip (§8).
- The `binary-comp` map is **`TEACHER.map`** (full) — and it is for the *rebuilt* code only,
  not the original. `make report` finds all functions (the link line lists every `.obj`
  explicitly, so there is **no `/OPT:NOREF` dead-strip problem** to fix).

---

*Bottom line:* the comparator rewards the **right instruction sequence**, not exact operands.
The wins that actually land here come from **control-flow shape** (if-polarity, switch-vs-if,
shared-vs-split epilogues, loop-with-break) and **return-type width** — found one at a time
with `ndiff`/`triage`. Most small functions that remain below 95% are
register-allocation/scheduling ceilings (§10); verify before investing. Write the high-level
op (`x/1000`, `*(__int64*)d=*(__int64*)s`, `memcpy`) and let the compiler regenerate the magic.
