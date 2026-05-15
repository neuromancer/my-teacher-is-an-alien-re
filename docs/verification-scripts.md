# Verification Scripts

The Makefile-facing verification/report scripts share project-specific data from
`config/verification.json`. Keep executable paths, source/export directories,
address ranges, type sizes, CRT address maps, vtable overrides, call aliases,
and call-count allowances in that config instead of embedding them in scripts.

Scripts that read the config:

- `bin/checkGlobals.py`
- `bin/checkCallTargets.py`
- `bin/verifyVtables.py`
- `bin/compileAndReport.py`
- `bin/checkValuesAll.py`

All of these scripts accept `--config PATH`. The default is
`config/verification.json`, or the `VERIFY_CONFIG` environment variable. Make
targets pass `$(VERIFY_CONFIG)`, so another project can run the same scripts
with:

```sh
make verify-globals VERIFY_CONFIG=config/other-game.json
make verify-calls VERIFY_CONFIG=config/other-game.json
make verify-vtables VERIFY_CONFIG=config/other-game.json
make report VERIFY_CONFIG=config/other-game.json
```

Most config values also have direct CLI overrides, for example:

```sh
python3 bin/checkGlobals.py --config config/verification.json --exe path/to/original.exe
python3 bin/checkCallTargets.py --config config/verification.json --src-dir src --code-dir code-full --out-dir out
python3 bin/verifyVtables.py --config config/verification.json --rdata-min 0x461000 --rdata-max 0x468000
```

When adding a new known mismatch or binary-specific address, prefer updating
`config/verification.json`. Script code should stay limited to generic parsing,
normalization, and comparison logic.
