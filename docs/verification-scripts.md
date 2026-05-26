# Verification Tools

Makefile-facing verification and comparison are handled by `binary-comp`, with
the local progress script kept in this repository.

Migrated verifiers:

- `binary-comp globals`
- `binary-comp calls`
- `binary-comp compare`
- `binary-comp report`
- `binary-comp values`
- `binary-comp vtables`
- `binary-comp exe`
- `binary-comp data`

Local scripts kept in this repository:

- `bin/showProgress.py`

All migrated verifiers read project-specific paths, address ranges, aliases,
type sizes, and known allowances from `config/binary-comp.json`. Make targets
pass `$(VERIFY_CONFIG)`, so another target config can be selected with:

```sh
make verify-globals VERIFY_CONFIG=config/other-game.json
make verify-calls VERIFY_CONFIG=config/other-game.json
make verify-values VERIFY_CONFIG=config/other-game.json
make verify-vtables VERIFY_CONFIG=config/other-game.json
```

Direct CLI examples:

```sh
binary-comp globals --config config/binary-comp.json --target full
binary-comp calls --config config/binary-comp.json --target full
binary-comp compare --config config/binary-comp.json --target full ClassName::MethodName code-full/FUN_XXXXXX.disassembled.txt
binary-comp report --config config/binary-comp.json --target full
binary-comp values --config config/binary-comp.json --target full --min-similarity 80
binary-comp vtables --config config/binary-comp.json --target full
binary-comp exe --config config/binary-comp.json --target demo --functions
binary-comp data --config config/binary-comp.json --target full
```

When adding a new known mismatch or binary-specific address, prefer updating
`config/binary-comp.json`. Tool code should stay limited to generic parsing,
normalization, and comparison logic.
