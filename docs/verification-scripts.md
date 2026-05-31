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

`verify-values-stack-locals` uses `--no-offsets` by default so the result stays
focused on stack-local constants and strings instead of compiler-dependent stack
frame layout. Set `STACK_LOCAL_VALUES_FLAGS=` to include stack displacement
diagnostics.

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
