# HINTS — improving assembly similarity

This is the repository guide for rebuilding *My Teacher Is an Alien* with Microsoft
Visual C++ 4.20 while matching the original `TEACHER.EXE`. It consolidates the useful
parts of the former `HINTS.md` and `HINTS.DE.md`, removes advice that only applied to
their source projects, and records patterns validated in this repository.

The governing rule is simple: preserve the program first, then shape clean C++ so the
original compiler emits the desired instruction sequence. The disassembly and
`code-full/strings.txt` are authoritative; decompiler output is evidence, not source code.
Follow `CLAUDE.md` throughout—especially its restrictions on assembly, dummy variables,
calling conventions, and invented data structures.

## Quick loop

```sh
make TEACHER.EXE -j12
python3 bin/ndiff.py --no-build Name:0xADDR
binary-comp compare --config config/binary-comp.json --target full \
  Name code-full/FUN_00ADDR.disassembled.txt --no-build
make report > /tmp/teacher-report.txt
make verify
```

`--no-build` is only safe after relinking `TEACHER.EXE`. Address-qualified names are best
when a recovered name occurs more than once. Exported function filenames are normally
zero-padded; locate the exact file instead of guessing its width.

## 1. Know what the score measures

`binary-comp` compares normalized mnemonic sequences with Levenshtein distance. It does
not compare operands, registers, immediates, addresses, symbol names, or operand widths.

- `mov eax,[x]`, `mov ecx,[y]`, and `mov word ptr [z],5` each contribute one `mov`.
- Alias spellings such as `je`/`jz`, `jne`/`jnz`, `jb`/`jc`, and `jg`/`jnle` normalize to
  the same mnemonic. Different conditions such as `jl` and `jle` do not.
- Padding and exported data directives such as `db`, `dw`, `dd`, and `npad` are skipped.
- Both streams are decoded from PE bytes. The Ghidra export supplies boundaries, not the
  mnemonic text used for scoring.

This makes the score an excellent code-shape signal and a poor correctness oracle. A
wrong field offset, constant, global, scale, or call target can still score 100%. Conversely,
two equivalent C++ expressions may compile to different mnemonics.

Use the native audits for facts the score cannot see:

- `make verify-values` checks immediate and constant values.
- `make verify-values-stack-locals` includes stack-local references.
- `make verify-global-access` checks global reads and writes.
- `make verify-calls` checks call-target multisets.
- `make verify-globals`, `make verify-globals-code`, and `make verify-vtables` check layout
  and metadata.

Correctness takes precedence over percentage. Do not keep a score increase that introduces
an operand, call, layout, or behavioral mismatch.

## 2. Use a measured workflow

### Establish a trustworthy baseline

1. Start from a successfully linked executable and save a full report.
2. Record the target row by address, not just by possibly duplicated recovered name.
3. Check the source marker and the exported function boundary before editing.
4. Inspect the raw disassembly, the rebuilt `out/<TranslationUnit>.asm`, and a normalized
   diff.
5. Make one structural hypothesis at a time.
6. Rebuild and compare that function immediately.
7. Keep the change only if it preserves behavior and improves the intended row.
8. Finish with a fresh full report and `make verify`.

Compare exact rows and counts, not only the rounded average. A local win can perturb another
function in the same translation unit through register allocation, scheduling, inlining, or
literal placement. The whole report is the regression gate.

The Makefile does not model every header dependency. After changing a shared header, run a
clean rebuild before trusting compiler or linker results. Otherwise stale objects can retain
old declarations, mangled names, layouts, or inline bodies.

### Treat function boundaries as data

- Keep one `/* Function start: 0xADDR */` marker immediately before the corresponding
  function unless the export proves a special case.
- Keep recovered functions in address order within a source file. MSVC emits functions in
  source order, so order affects layout and mapping.
- An extra, missing, or misplaced marker changes comparison boundaries and may make adjacent
  rows misleading.
- Some recovered names occur at multiple addresses, and a few functions legitimately have
  more than one report row. Inspect all rows before declaring a change exact.
- SEH funclets, secondary entries, jump thunks, and shared tails can look like tiny standalone
  functions even when no C++ body belongs there.

### Pick targets by diff shape

Instruction-count ratio is a useful first filter:

- A ratio far from 1 usually means a missing body, wrong construct, wrong boundary, or
  optimization-region mismatch.
- A ratio near 1 with a few changed instructions is often a branch, epilogue, type, or local
  lifetime problem.
- A ratio near 1 with balanced delete/insert blocks may be a block-order transposition or a
  register-allocation ceiling.

Functions around 88–95% with one clear structural difference often yield more progress than
spending hours on a 99% function whose only residual is one scheduled `mov`. Look for an
already-exact sibling method: repeated queue, message, parser, and handler code is a better
template than decompiler prose.

Common signatures:

| Diff shape | First hypothesis |
| --- | --- |
| one conditional jump differs | boundary form, signedness, polarity, or flag reuse |
| equal-sized delete/insert blocks | branch arms or independent statements are reversed |
| extra `jmp` or duplicate `ret` | early return versus shared epilogue |
| extra `test` after a call | cached result or boolean materialization |
| `movsx` versus `movzx`/`and` | signed or unsigned source type |
| different `sub esp,N` | local count, width, lifetime, or declaration order |
| missing/extra `call` | inline form, intrinsic choice, or wrong prototype |
| low instruction-count ratio | missing control-flow region, wrong marker, or pragma state |

### Probe the compiler when syntax is the unknown

MSVC 4.20 is the only reliable authority on how a spelling lowers. For a stubborn small
sequence, compile a minimal C++ probe with the exact Makefile flags and the same
`#pragma optimize` state. Include enough locals and surrounding control flow to reproduce
register pressure; a parameter-only toy can answer a different question.

Use probes to compare equivalent spellings, then apply only the form that remains natural in
the recovered function. This repository has a validated example in
`SCI_Inventory::ProcessInventory`: an integer-cast ternary mask regenerated the original
branchless `cmp`/`adc` sequence where a pointer comparison did not. Trust emitted assembly,
not intuition about an old optimizer.

## 3. Control flow is the highest-yield lever

### Branch polarity and fall-through

Equivalent conditions can place different blocks on the fall-through path:

```cpp
if (ok) {
    success();
} else {
    failure();
}
```

and

```cpp
if (!ok) {
    failure();
} else {
    success();
}
```

Match the original block order, not the decompiler's preferred wording. De Morgan forms can
also help: a positive success condition may reproduce a fall-through that a chain of negative
failure tests does not.

Validated examples in this tree include:

- `GetFileSize`: success-first fall-through reached 100%.
- `SC_Question::Update`, `SC_CrystalPuzzle::ResetPuzzle`, and several list insertion methods:
  swapping equivalent branch arms reached 100%.
- `VBuffer::ClipAndBlitRegion`: reversing an otherwise identical conditional arm selected the
  matching branch mnemonic.

An empty then with work in the `else` is sometimes the clearest representation of a binary
that jumps positively into the body and otherwise jumps over it. Apply this only when the
actual labels support it.

### Boundaries and flag reuse

Equivalent integer boundaries can lower differently. For an `int`, `x < 1` and `x <= 0`
have the same meaning but may select different compare/branch shapes; the latter made
`internal_ReadLine` exact here. By contrast, changing `x < 0` to `x <= 0` is not equivalent
and is a correctness bug.

Operand order can change the branch family or allow flags to be reused. `i >= hi` and
`hi <= i` are mathematically equal, but only one may match the surrounding loads and jump.
Always verify signedness before rewriting a comparison.

Use bitwise `&` in place of `&&` only when both operands are side-effect-free, both must be
evaluated, and the disassembly proves a non-short-circuit form. It is not a general scoring
trick.

### Shared versus split exits

The location and number of epilogues matter:

- Multiple direct returns often produce separate `xor`/`mov` result sequences.
- A loop `break`, `goto done`, or result variable can funnel paths through one epilogue.
- An early return can be correct when the reference has a distinct exit, but wrong when all
  paths share cleanup and one `ret`.

`Sound::FindFreeSampleHandle` became exact by breaking out of the loop and using the shared
post-loop result path. `SCI_IconBarModule::AddMessage` became exact by routing an early-success
case to the existing final return.

Use a named label only when it expresses an actual join in the binary. Do not add arbitrary
gotos merely to move instructions.

### Conditional values

A ternary, an explicit two-arm assignment, and a conditional overwrite are not interchangeable
to this compiler:

```cpp
x = 0;
if (p != 0) {
    x = p->field;
}
```

This can avoid the extra jump or merge generated by `x = p ? p->field : 0`. It improved
`EngineC::RenderBackground` from 98.20% to 99.55%. Check both shapes; the better one depends
on surrounding register pressure.

For branchless masks, the source type matters. A form such as an integer comparison followed
by `? 0 : -1` may lower to `cmp`/`adc`, while a pointer-typed comparison may force another
shape. Preserve the null-safety and the original behavior when using this idiom.

### Switches and dispatchers

- Consecutive cases may lower to a `sub`/`jz`/`dec` chain or a jump table rather than an
  `if`/`else-if` cascade.
- A single `cmp` followed by two conditional jumps that reuse flags can also indicate a
  small `switch`.
- Recover case-body order from jump-table destinations and label addresses, not numeric case
  order. Source order and emitted body order can differ.
- Default handling, range checks, and shared case tails are part of the shape.

## 4. Loops

Read the control-flow graph before choosing `for`, `while`, or `do while`:

- An initial jump to a bottom test suggests `while` or `for`.
- A body entered before the first test suggests `do while`.
- A top test with a back edge to the test suggests a pre-tested loop.
- `while (1)` with an internal `break` can be right when the reference has a literal
  unconditional back edge; otherwise it can add an unwanted materialized true/test.

Other useful distinctions:

- Put termination in the loop condition when the reference tests it there. A manual infinite
  loop can add `mov 1; test; je` on MSVC 4.20.
- A decrement in the body may match `while` better than a `for` iteration expression.
- A `do while` can peel iteration zero when the first array address becomes constant; a plain
  `for` can suppress that peel.
- `i += 1` may retain a 16-bit operation where `i = i + 1` promotes through `movsx`.
- `*p++` in a condition can combine the load and pointer advance differently from separate
  statements.
- Precomputing a row pointer or pitch matches only if the reference also hoists it; otherwise
  repeated indexed access may be closer.
- A `break` followed by a shared return differs from returning inside the loop.

The optimizer canonicalizes many loop spellings, so confirm with the emitted assembly rather
than applying mechanical rewrites. The repository's bulk loop transformer previously found no
general wins; most existing loops are already hand-shaped.

## 5. Returns, calls, and prototypes

### Return width

- A function whose binary only sets `AX` is often `short` or `unsigned short`, not `int`.
- A function that only sets `AL` is often `unsigned char`.
- Ghidra artifacts such as `in_EAX`, `extraout_ECX`, `CONCAT22`, or preserved high halves do
  not prove a wide semantic return.
- `CONCAT22(hi,lo)` and `CONCAT31(...)` are pseudo-operations, not C++. Reconstruct the value
  only if callers use it.

MSVC 4.20 rejects falling off the end of a non-`void` function. If the original is a bare
`ret` and callers do not consume a result, the declaration may genuinely need to be `void`.
For a virtual, change the base slot and every override consistently; C++ cannot overload on
return type. After such a header edit, clean-rebuild all callers.

### Call expression shape

- Nested calls can generate a different stack-cleanup schedule from named temporaries.
  Compare `outer(inner(a,b),c)` with a cached `inner` result.
- A final call from a compatible `void` wrapper may become a tail `jmp`.
- A `thiscall` member receives `this` in `ECX` and normally cleans its explicit arguments.
  Do not spell or change calling conventions to force a match.
- Validate suspected arguments at call sites. Decompilers often invent `in_ECX`,
  `extraout_EDX`, or stack parameters from live registers.
- A caller that reserves bytes, copies them with `rep movsd`, then calls may be passing a
  struct by value. Model the real complete value and exact size; do not invent substructures.

Forward-declare a same-file callback when address order places its definition later. Use
`extern` for a declaration whose definition truly lives in another translation unit.

## 6. Stack frames, locals, and types

### Start with the frame delta

Compare `sub esp,N` before chasing individual instructions:

- If the rebuilt frame is larger, look for a single-use local, unnecessary cached pointer,
  widened temporary, extended lifetime, or inhibited inlining.
- If it is smaller, the original may have had a meaningful cached value, intermediate result,
  aggregate, or source-level scope that the decompiler collapsed.
- Declaration order and nested scopes affect slot reuse. Introduce only locals supported by
  reads/writes in the original; dummy padding variables are forbidden.
- Floating-point expressions are especially sensitive to whether an intermediate is named or
  left in one expression.

Do not label every frame mismatch impossible. First account for real locals and lifetimes.
After those are faithful, a one-slot residual may be an allocator ceiling.

### Signedness and width

- Signed and unsigned loads choose among `movsx`, `movzx`, plain `mov`, and explicit `and`.
- Signed and unsigned division choose `idiv` versus `div` and different setup sequences.
- A redundant `& 0xffff` on an already-unsigned-short value can add another instruction.
- Apply casts at the use site when only one operation needs a different interpretation;
  changing a shared declaration can perturb every consumer.
- A decompiler's `& 0x1f` on the right operand of a shift is often x86's implicit shift-count
  masking. Keep it only if the reference has a separate `and`.

Retyping pointers is especially dangerous: pointer arithmetic scales by the pointed-to type.
Mnemonic similarity may stay unchanged while every effective address becomes wrong. After a
type change, run value and global-access verification even if the score is unchanged.

### Locals and memory access

- Direct field access can be closer than caching `&arr[i]` when the reference recomputes the
  address for each use.
- Conversely, restore a cached pointer when the reference keeps it across several accesses.
- When adjacent fields are deliberately read as one value, a proved direct cast/load can match
  better than reconstructing the value with shifts and ORs.
- Decompiler propagation may collapse `a=node; b=a->next; node=b;` into one expression even
  though the frame proves distinct locals. Restore evidenced locals in declaration order.
- A serialized cursor that advances through a buffer differs from repeated fixed-offset
  accesses even when both read the same bytes.
- Local aggregate initialization, member-by-member stores, a copy, and `memset` can have very
  different instruction sequences.

## 7. Expression and memory idioms

Write the high-level operation first and let the original compiler regenerate its lowering:

- Use `x / K` and `x % K`, not decompiler-expanded magic-number arithmetic.
- A real 64-bit division has helpers such as `__alldiv`/`__aulldiv`; `cdq; idiv` is ordinary
  signed 32-bit division. MSVC 4.20 uses `__int64`, not modern `long long` assumptions.
- `__ftol()` consumes the x87 value already on the FPU stack. Recover the expression that
  precedes the call rather than inventing a parameter.
- `x &= ~1` can lower differently from `x &= 0xfffffffeU`; match the reference's immediate
  construction only after preserving the intended type width.
- `x op= y` can choose a memory read-modify-write form where `x = x op y` chooses separate
  load and store instructions, or vice versa.

Useful grouped-memory forms, when proved by adjacent accesses:

- `*(__int64*)dst = *(__int64*)src` can reproduce two grouped dword loads followed by two
  stores. A member-by-member copy may interleave reads and writes instead.
- Two adjacent 16-bit fields sourced from one 32-bit value may be one dword store.
- Two adjacent shorts can sometimes be cleared with one dword store.

These casts are appropriate only when alignment, object layout, and the original accesses
support them. The operand verifiers remain the safety net.

## 8. Optimization state and intrinsics

The base build flags in `Makefile` are:

```text
/Og /Oi /Ot /Oy /Ob1 /QIfdiv /Gs /Gf /GX
```

They are part of the reconstruction and must not be changed casually. The source is not
uniformly `/Oy`: several translation units contain repository-owned
`#pragma optimize("y", off)` regions, sometimes followed by `("y", on)` and sometimes
continuing to the end of the file. The `y` setting controls frame-pointer omission; it does
not disable every optimization.

Audit the current state with:

```sh
rg -n '#pragma optimize' src
```

Existing regions occur in low-level graphics/video code and selected main, state, mouse,
blit, scale, and palette routines. Respect the state at the target's exact source position.
A function with an EBP frame while surrounding code omits one may be explained by this pragma,
not by its local variables.

Do not add, remove, or widen an optimization region speculatively. First prove a frame or
instruction-count mismatch, then test the smallest possible change and run the full report;
pragma state can affect every later function in that translation unit.

With `/Oi`, CRT operations may be intrinsic. `#pragma function(name)` can force a call and an
intrinsic declaration can permit inline lowering, but use either only when `make verify-calls`
and the reference disassembly prove the desired call shape. It is a targeted diagnostic lever,
not a blanket fix.

## 9. CRT lowering, copies, and strings

Decompiler-expanded CRT loops may represent compiler intrinsics:

- `strlen` often appears as a decrementing scan beginning at `0xffffffff`, followed by a
  complement.
- `memcpy` can become `rep movsd` plus word/byte tails. A constant-size call and a hand-written
  loop do not necessarily lower alike.
- `strcmp` may appear as an unrolled byte-compare chain.
- `memset` may become stores or a repeat instruction depending on size and alignment.

Either retain the recovered loop or use the high-level CRT call according to the reference.
Check `make verify-calls`: a mnemonic-perfect inline sequence with the wrong external call
behavior is not a valid win. String contents and paths must come from `code-full/strings.txt`,
not from mnemonic similarity.

## 10. Decompiler and export traps

- Jump-table case order is reconstructed from destinations, not from decompiler formatting.
- SEH setup, cleanup funclets, shared epilogues, and secondary entry points can split one
  source function across exported blocks.
- Ghidra may hallucinate calling conventions, parameters, wide returns, or `CONCAT` values
  from register liveness.
- A very low `LBLParse` row adjacent to a healthy parser body is often a mapped funclet rather
  than a missing C++ function.
- Duplicate recovered names make name-only comparisons ambiguous.
- A changed function marker can make both the target and its neighbor appear to regress.
- A source edit can be mnemonic-neutral yet corrupt offsets, constants, or call targets.

When the report and source appear contradictory, inspect addresses, mapping boundaries, raw PE
disassembly, and native verifier output before rewriting code.

## 11. Recognize diminishing returns

After semantic and structural explanations are exhausted, some residuals are poor targets:

- register allocation chooses another callee-saved register or spills one value;
- independent instructions are scheduled in a different order;
- argument evaluation is hoisted or delayed without a clean source-level control;
- one real stack slot cannot be reconciled without a forbidden dummy;
- linker thunks, shared tails, or SEH split the reported range;
- CRT/startup code depends on compiler-library internals;
- low-level graphics code uses instruction selections that normal C++ does not express.

A normalized diff often shows these as the same mnemonic deleted at one location and inserted
at another, a lone prologue/epilogue difference, or a register-save imbalance. Do not use
inline assembly, dead variables, fake branches, unions, invented calling conventions, or
compiler-flag changes to force the final point.

This is prioritization, not proof that a subsystem can never improve. Revisit a deferred target
when an exact sibling, a better type recovery, a corrected marker, or new compiler evidence
appears.

## 12. Repository helper tools

All commands run from the repository root:

- `bin/ndiff.py [--no-build] Name:0xADDR ...` prints only divergent normalized-mnemonic
  blocks with both raw instruction streams. It uses the comparator's own decoder.
- `bin/triage.py REPORT [--min N --max M] [--no-build] [--sort pol|ratio|size]` classifies
  rows by ratio and edit shape. Its `POL` value highlights likely opposite-polarity branches.
- `bin/trysweep.py REPORT [inc|compound|cmp|swap|all] [File.cpp]` tests reversible,
  behavior-preserving expression rewrites and keeps only strict improvements.
- `bin/tryforloop.py`, `bin/trymask.py`, and `bin/tryspill.py` probe loop, redundant-mask,
  and evidenced-spill forms.

The bulk transformers are most useful on newly recovered decompiler-like source. Earlier full
sweeps found few or no candidates in the already hand-shaped tree. Review every retained edit
for semantic equivalence; a script's score gate cannot validate operands or behavior.

`TEACHER.map` describes the rebuilt executable, not the original. The original boundaries and
addresses come from `code-full/` plus the comparison configuration.

## 13. A compact worksheet

For each candidate, write down:

1. name and original address;
2. baseline score and instruction-count ratio;
3. exact marker/export range;
4. first normalized difference;
5. call, global, value, and vtable evidence;
6. one hypothesis: control flow, loop, epilogue, type, local, pragma, or likely ceiling;
7. target score after the edit;
8. full-report delta and verifier result.

That small record prevents circular experiments and makes successful shapes reusable across
sibling functions.

The practical bottom line: match control-flow topology, meaningful types, local lifetimes, and
compiler context. Use the percentage to find structural differences, the disassembly to explain
them, and the native verifiers to keep the reconstructed program honest.
