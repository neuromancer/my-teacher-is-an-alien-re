# Verification Tools

Makefile-facing verification and comparison are handled by `binary-comp`, with
the local progress script kept in this repository.

Migrated verifiers:

- `binary-comp globals`
- `binary-comp calls`
- `binary-comp compare`
- `binary-comp report`
- `binary-comp seh`
- `binary-comp values`
- `binary-comp vtables`
- `binary-comp exe`
- `binary-comp data`

`binary-comp seh` compares C++ exception-handling structure (the MSVC `FuncInfo`
unwind map) between original and rebuilt, which mnemonic similarity cannot see.
For one function it lists each unwind state (what object is destroyed —
`this+0xNN` member, `stack@`, `ptr@`, or `arg@` — and the destructor target) and
warns on differences; with `--report` it scans the whole project and prints only
functions whose EH structure differs. Typical findings: `rebuilt destructs EXTRA
member object at this+0xNN` (a member wrongly typed with a destructor, e.g.
`SlimeDim`/`Rect` where the original used a dtor-less type), `rebuilt has NO C++
EH frame` (a small member destructor must be inline in a header, or a guard
object/try is missing), and `MISSING a member destructor`.

```sh
binary-comp seh --config config/binary-comp.json --target full ClassName::Method code-full/FUN_XXXXXX.disassembled.txt
binary-comp seh --config config/binary-comp.json --target full --report
binary-comp seh --config config/binary-comp.json --target full --report --filter SC_FireAlarm
```

Local scripts kept in this repository:

- `bin/showProgress.py`

All migrated verifiers read project-specific paths, address ranges, aliases,
type sizes, and known allowances from `config/binary-comp.json`. Make targets
pass `$(VERIFY_CONFIG)`, so another target config can be selected with:

```sh
make verify-globals VERIFY_CONFIG=config/other-game.json
make verify-calls VERIFY_CONFIG=config/other-game.json
make verify-global-access VERIFY_CONFIG=config/other-game.json
make verify-values VERIFY_CONFIG=config/other-game.json
make verify-values-stack-locals VERIFY_CONFIG=config/other-game.json
make verify-vtables VERIFY_CONFIG=config/other-game.json
```

`make verify` runs the full non-audit suite: globals, calls, global access,
values, stack-local values, and vtables.

`make seh` prints the C++ exception-handling structure report (`binary-comp seh
--report`) — the functions whose `FuncInfo` unwind structure differs from the
original. It is informational (always exits zero) like `make report`. Restrict
it with `FILTER`. Use the direct CLI with `--strict` when you need exact unwind
state, `toState`, and guard comparisons instead of the default low-noise
member/kind comparison:

```sh
make seh
make seh FILTER=SC_Gauntlet
binary-comp seh --config config/binary-comp.json --target full --report --strict
```

`verify-values-stack-locals` uses `--no-offsets` by default so the result stays
focused on stack-local constants and strings instead of compiler-dependent stack
frame layout. Set `STACK_LOCAL_VALUES_FLAGS=` to include stack displacement
diagnostics.

## Suppression policy

Call aliases must only normalize ABI spellings, exact wrapper functions, or
byte-identical COMDAT bodies. Do not alias routines merely because their current
call sites appear to have the same effect: call verification compares the
canonical target name, so such an alias can completely hide a wrong call.
Check both the full and demo targets before adding or removing a shared alias.

Call-count and global-access allowances must correspond to a currently observed
analyzer artifact. Remove an allowance when running the same check without it no
longer changes the report. Call-count allowances should include a `reason` in
`config/binary-comp.json` because they suppress an otherwise resolved mismatch.

The normal call check canonicalizes the project's allocation wrappers. Use the
strict-memory mode when auditing whether a call went through the original
wrapper rather than directly to its underlying allocator:

```sh
make verify-calls CALLS_FLAGS=--strict-memory
```

Calls and global-access checks are advisory by default because the reconstruction
still has known findings. Focused checks that are expected to be clean should be
made into real gates with `--fail-on-mismatches`:

```sh
make verify-calls FILTER=ClassName::Method CALLS_FLAGS=--fail-on-mismatches
make verify-global-access FILTER=ClassName::Method GLOBAL_ACCESS_FLAGS=--fail-on-mismatches
```

Direct CLI examples:

```sh
binary-comp globals --config config/binary-comp.json --target full
binary-comp calls --config config/binary-comp.json --target full
binary-comp global-access --config config/binary-comp.json --target full
binary-comp compare --config config/binary-comp.json --target full ClassName::MethodName code-full/FUN_XXXXXX.disassembled.txt
binary-comp report --config config/binary-comp.json --target full
binary-comp values --config config/binary-comp.json --target full --min-similarity 80
binary-comp values --config config/binary-comp.json --target full --min-similarity 90 --include-stack-locals --no-offsets
binary-comp vtables --config config/binary-comp.json --target full
binary-comp exe --config config/binary-comp.json --target demo --functions
binary-comp data --config config/binary-comp.json --target full
```

When adding a new known mismatch or binary-specific address, prefer updating
`config/binary-comp.json`. Tool code should stay limited to generic parsing,
normalization, and comparison logic.
