# Clang warning POC results

Command:

```sh
tools/analyze_clang.sh all
```

Result on macOS/Clang:

- `checked=133 ok=133 failed=0`
- Full generated log: `analysis/out/clang-all.log` (ignored by git)
- The warning POC itself uses compatibility headers in `analysis/include/` and syntax-only compilation.

## Warning volume

Total warnings in the current run after reconstruction fixes and analysis-only format annotations: 2281.

Most common classes:

- 383 `-Wint-to-pointer-cast`
- 375 `-Wc++11-compat-deprecated-writable-strings`
- 338 `-Wnontrivial-memcall`
- 255 `-Wcast-align`
- 225 `-Wpointer-to-int-cast`
- 211 `-Wdelete-non-abstract-non-virtual-dtor`
- 130 `-Wunused-parameter`
- 89 `-Wdeprecated-declarations`
- 49 `-Wvoid-pointer-to-int-cast`
- 46 `-Wformat`
- 44 `-Wint-to-void-pointer-cast`
- 25 `-Wshorten-64-to-32`
- 15 `-Wformat-security`
- 13 `-Wformat-insufficient-args`
- 2 `-Wdelete-incomplete`
- 2 uninitialized-flow warnings
- 1 dynamic-class memset

Most of the pointer/integer and format-width warnings are expected from 32-bit Windows code being inspected by a 64-bit macOS compiler. They are still useful for a future 64-bit port, but many are not necessarily original game bugs.

## Fixed reconstruction findings

- `src/SCI_SearchScreen.cpp:230`: changed the cross-object `memset` into the four explicit global stores shown by `0x40EFB0`.
- `src/SC_SelectHotSpot.cpp:714`: changed the `HANDLE` parse target from pointer-to-member syntax to the current object's `moduleParam`.
- `src/SC_Game.cpp:93`: changed `stateName` from `int` to `char*`, matching the pointer returned by `GameState::GetState`.
- `src/Parser.cpp:585-614`: changed `SET_GAMESTATE` and `SET_MOUSE` integer parse storage from reused pointer locals to typed `int` locals.
- `src/Parser.cpp:748-785`: changed variable substitution to keep the game-state value as an `int` and initialize the suffix pointer.
- `src/Palette.cpp:55,109`: changed the palette buffer allocation from `new char[0x300]` to scalar `operator new(0x300)`, matching the scalar delete path in `0x41DC10` and allocation in `0x41DC40`.
- `src/Queue.cpp:221-310` and `src/TimedEvent.h:47-48`: removed unused `SC_MessageParser` `TimedEventPool::Pop` overloads. The real pop path at `0x42D1A0` operates on embedded `SpriteAction` nodes and calls `SpriteAction::CopyFrom` (`0x444920`); the parser-message overloads were reconstruction artifacts.
- `src/FlagArray.cpp:112`: changed the null-file path in `FlagArray::Seek` from `WriteToMessageLog` to terminating `ShowError`, matching the call in `0x420990`.
- `src/Graphics.cpp:328,341` and `src/main.cpp:107,110`: removed source-only self-assignments. `0x4534F4` stores the WinG `GetProcAddress` and `WinGRecommendDIBFormat` results directly, and `0x4236F0` stores the allocated `GameState` pointers directly.
- `src/GameState.cpp:239-241`, `src/Parser.cpp:732-777`, `src/Queue.cpp:224-228`, and `src/VideoTable.cpp:43-303`: fixed source-only dead-store artifacts while preserving the original operations: GameState bucket zeroing still follows the `0x4338C7` REP STOSD count, Parser's RETURN path still builds the same `fpos_t` for `fsetpos` at `0x4135D7`, `TimedEventPool::PopSafe` keeps the original exception-state flag write from `0x42CAD0` as a volatile local, and VideoTable no longer has duplicate zero stores or unused byte-offset locals where the source uses typed array indexing.
- `src/mCNavigator.cpp:222-223`: changed the duplicate-node `ShowError` argument from `&parser->nodeName` to `parser->nodeName`. The dumped disassembly for `0x44BB60` pushes the node-name buffer address and handle; the prior source used the same address but the wrong variadic type (`char (*)[36]`) for `%s`.
- `src/CombatSprite.cpp:101,106,189,194,261`, `src/ObjectPool.cpp:58,62`, `src/SoundList.cpp:19-51,89`, `src/StringTable.cpp:181`, `src/ZBufferManager.cpp:980`, and `src/SC_Pods.cpp:12`: changed source-only allocation/free representations to scalar `operator new`/`operator delete`, or completed the `SoundCommand` type, where dumped functions call scalar helpers (`0x454500`/`0x454400`) after any explicit destructor/reset work.

## Added analysis layers

- `analysis/include/analysis_prelude.h`: added analysis-only `printf`/`noreturn` attributes for `ShowError`, `ShowMessageYesNo`, `FormatStringVA`, `WriteToLog`, `WriteToMessageLog`, `ResolveAssetPath`, and `CDData_ResolvePath`.
- `tools/analyze_static.sh`: added a Clang Static Analyzer pass using the same Win32 shim headers and `#pragma data_seg` filtering as `tools/analyze_clang.sh`.
- `ShowMessage` is intentionally not annotated yet because `src/GameWindow.cpp` has an incompatible local prototype.

## Format findings triaged from annotations

- `src/Parser.cpp:82`: `Parser::ReportUnknownLabel` passes `lineNumber` to a `%s` slot. Assembly at `0x412A50` pushes name, line number, and filename, so this is an original binary bug. Behavior preserved and commented.
- `src/SCI_IconBarModule.cpp:726,753,775,829,866`: several `ShowError("repeat use of hotspot %d")` calls pass no hotspot id. Decompiler output for `0x4022B0` shows the same missing extra argument at each equivalent branch. Behavior preserved and commented.
- `src/SC_Question.cpp:352,390,428,466,504,542,712`: parse-error `ShowError` calls contain `%s` but pass no line/token argument. Decompiler output for `0x414DA0` shows the same single-argument calls. Behavior preserved and commented.
- `src/SC_Wahoo.cpp:564`: `ShowError("Invalid gamestate %d")` passes no index. Decompiler output for `0x438660` shows the same single-argument call. Behavior preserved and commented.
- Nonliteral format warnings for `ResolveAssetPath`, `FormatStringVA`, and `ShowError(param_3)` are useful hardening/portability warnings, but many may be expected because the original helper is a formatting path resolver.

## Static analyzer results

Command:

```sh
tools/analyze_static.sh all
```

Result:

- `checked=133 ok=133 failed=0`
- Full generated log: `analysis/out/static-all.log` (ignored by git)
- Total analyzer warnings: 135

Checker categories:

- 37 `core.NullDereference`
- 35 `cplusplus.NewDeleteLeaks`
- 16 `core.CallAndMessage`
- 15 `core.UndefinedBinaryOperatorResult`
- 7 `core.FixedAddressDereference`
- 7 `core.DivideZero`
- 6 `core.uninitialized.Assign`
- 5 `core.NullPointerArithm`
- 2 `deadcode.DeadStores`
- 2 `cplusplus.NewDelete`
- 1 each: `core.uninitialized.UndefReturn`, `core.uninitialized.ArraySubscript`, `core.NonNullParamChecker`

Static analyzer findings worth triage first:

- `src/SCI_IconBarModule.cpp:752`: confirms the original `DLG_HOTSPOT` uninitialized array subscript path.
- `src/main.cpp:463`: confirms the original `WinMain` uninitialized return.
- `src/SC_OnScreenMessage.cpp:118,122,276,279,512`: reports use-after-release, null-object, and fixed-address paths in message-list iteration. Assembly confirms the same null-current/fixed-address branches and the same delete/reset/advance shape. The ownership warnings are demoted after review; the null-current/fixed-address paths remain original bugs.
- `src/EngineB.cpp:193`: the refreshed Ghidra export now includes `FUN_450F70`. The original branch reaches `LAB_0045102E` when `m_progress.end` is zero, then divides by that same value. Behavior is intentionally preserved and commented.

## Original bugs noted in source

- `src/Parser.cpp:82`: `ReportUnknownLabel` passes `lineNumber` to a `%s` format slot. Assembly at `0x412A50` pushes the line number in that argument position. Behavior is intentionally preserved and commented.
- `src/SCI_IconBarModule.cpp:752`: `DLG_HOTSPOT` uses `hotspotIdx` without initializing it in that branch. The original disassembly at `0x4022B0` shows the same previous-stack-slot use. Behavior is intentionally preserved and commented.
- `src/SCI_IconBarModule.cpp:726,753,775,829,866`: repeat-hotspot errors omit the `%d` argument. The original decompiler output for `0x4022B0` shows the same missing argument. Behavior is intentionally preserved and commented.
- `src/SC_Question.cpp:352,390,428,466,504,542,712`: parse-error messages omit the `%s` argument. The original decompiler output for `0x414DA0` shows the same missing argument. Behavior is intentionally preserved and commented.
- `src/SC_Wahoo.cpp:564`: the `SET_GAMESTATE` parse error omits the `%d` gamestate index. The original decompiler output for `0x438660` shows the same missing argument. Behavior is intentionally preserved and commented.
- `src/T_MenuHotspot.cpp:235`: `SWITCHFOCUS` passes an extra `sscanf` output pointer for a format with only four conversions, leaving the final integer at its initialized value. The original disassembly at `0x421560` pushes the same extra argument. Behavior is intentionally preserved and commented.
- `src/SC_OnScreenMessage.cpp:122,276`: null-current list handling can call `SoundUpdate` with `ECX=0` and can read fixed address `0x24`. The original decompiler output for `0x448610` and `0x448830` shows these same paths. Behavior is intentionally preserved and commented.
- `src/EngineC.cpp:70,108`: progress counters with zero limits fall through to division. The original decompiler output for `0x40BDC0` shows the same checks and divides. Behavior is intentionally preserved and commented.
- `src/SC_DodgeOrville.cpp:315,436`: zero `hitCount.y` and zero `throwState.y` fall through to division/modulo. The original decompiler output for `0x429110` and `0x4294A0` shows the same paths. Behavior is intentionally preserved and commented.
- `src/SC_Fan.cpp:621`: zero `dim_168.y` falls through to division. The original decompiler output for `0x410060` shows the same path. Behavior is intentionally preserved and commented.
- `src/SC_Slime.cpp:231`: zero `maxHits` falls through to division. The original decompiler output for `0x40DF30` shows the same path. Behavior is intentionally preserved and commented.
- `src/Parser.cpp:846`: `SubstituteVars` reaches the final `strncat` with `pos == 0`, so the count is based on null minus `src`. The original disassembly at `0x4140F0` does the same `SUB EAX, ESI` after `strchr` returns null. Behavior is intentionally preserved and commented.
- `src/ZBufferManager.cpp:853,970`: removed queue data can be null before it is used as a `GlyphRect` pointer. The original decompiler output for `0x4047C0` and `0x404B90` shows the same null-before-use paths. Behavior is intentionally preserved and commented.
- `src/GameLoopHelper.cpp:141`: `RemoveAnimation` dereferences the matched node even when no handle was found. The original disassembly at `0x41A919` sets the candidate to null after a failed search and immediately enters the removal path. Behavior is intentionally preserved and commented.
- `src/HotspotAction.cpp:212`: condition-list iteration can leave condition data null and then read `addressType`. The original disassembly at `0x41B7DB` dereferences the same null data pointer before the type check. Behavior is intentionally preserved and commented.
- `src/GameEngine.cpp:632`: handler-list iteration compares against fixed address `0x90` when the current handler node is null. The original disassembly at `0x4318F8` performs the same fixed-address read. Behavior is intentionally preserved and commented.
- `src/Handler31.cpp:375`, `src/MMPlayer.cpp:58,83,224`, `src/SCI_PracticeRoom.cpp:210,311`, `src/SC_SelectHotSpot.cpp:534,580,625`, `src/SC_Timer.cpp:78`, and `src/ZBufferManager.cpp:863`: list-iteration code substitutes a null object pointer when `current` is null, then calls through it. The original decompiler output for the corresponding functions shows the same ECX=0 call pattern. Behavior is intentionally preserved and commented.
- `src/MMPlayer.cpp:199`: `Draw` can dereference a null current sprite before calling `Do`. The original disassembly at `0x443E76` reads sprite fields with `ECX=0` if the current list node is null. Behavior is intentionally preserved and commented.
- `src/MouseControl.cpp:157`, `src/PodsEngine.cpp:162`, `src/SCI_Schedule.cpp:346`, and `src/SC_Slime.cpp:271`: singleton/sprite pointers can be null before unconditional method calls. The original decompiler output shows the same missing checks. Behavior is intentionally preserved and commented.
- `src/EngineB.cpp:269`, `src/EngineC.cpp:228`, `src/PodsEngine.cpp:244`, `src/SC_FireAlarm.cpp:644`, `src/SC_Slime.cpp:335`, and `src/SC_Wahoo.cpp:351`: `g_InputManager_0046aa08` is checked before `Refresh`, but later dereferenced again to read `pMouse` without rechecking. The original decompiler output shows the same `DAT_0046aa08 + 0x1a0` reads after only the earlier guard. Behavior is intentionally preserved and commented.
- `src/EngineB.cpp:193`: zero `m_progress.end` falls through to the partial-meter path and is used as the divisor. The refreshed original disassembly at `0x450F70` shows `TEST ECX,ECX`, branch to `LAB_0045102E`, and `IDIV ECX`. Behavior is intentionally preserved and commented.
- `src/StringTable.cpp:334`, `src/TargetList.cpp:136`, `src/GameState.cpp:339`, and `src/RockThrower.cpp:106`: hash-iterator scans can leave the current node as the sentinel or null, then dereference it without another check. The original disassembly at `0x44C4D2`, `0x4436C4`, `0x433A89`, and `0x427AA0` shows the same unchecked dereferences. Behavior is intentionally preserved and commented.
- `src/SCI_SchoolMenu.cpp:526`: the disabled-OK else path can run with `okayButton == 0` and then writes `ok->state`. The original disassembly at `0x41EC58` loads `okayButton` and stores through it without a null check on that path. Behavior is intentionally preserved and commented.
- `src/SC_DodgeOrville.cpp:311`: the full hit-meter path can leave the fill VBuffer null and then read its clip fields. The original disassembly at `0x4292CD` reads `0x24` and `0x2c` from that pointer before the draw call. Behavior is intentionally preserved and commented.
- `src/main.cpp:463`: `WinMain` returns uninitialized `uStack_14`. The original disassembly at `0x420620` returns an unwritten stack local. Behavior is intentionally preserved and commented.
- `src/T_MenuHotspot.cpp:24`: `T_MenuButton` zeroes `this` in a class derived from `Parser`, wiping the vptr. The original disassembly at `0x421A50` stores the vptr and then executes `REP STOSD` over `this`. Behavior is intentionally preserved and commented.
- `src/SC_CrystalPuzzle.cpp:553,557`: `Update` checks `&buttons1` and `&paths1` instead of the pointer values, making the display calls effectively unconditional for normal `this` values. The original disassembly at `0x44FE40` compares `this` against `-0xC4` and `-0xB8`, matching the address-of-member checks. Behavior is intentionally preserved and commented.

## Likely portability/noise bucket

- The many `int`/pointer cast warnings are mostly expected because the reconstructed source stores pointers in `int` fields.
- `%lu`/`%ld` format warnings in files such as `SC_MessageParser.cpp`, `SoundTracker.cpp`, `StringTable.cpp`, and `SC_Cinematic.cpp` are serious for LP64/macOS, but mostly not bugs for the original Win32 ABI where `long`, `int`, and pointers were 32-bit or pointer-sized by convention.
- Writable string literal warnings are C++11 compatibility noise for this codebase, not useful for this immediate triage.
- Warnings about deleting classes with virtual functions but non-virtual destructors need separate review. A representative `SoundCommand` queue-drain check in `GameEngine::HandleSystemMessage`, `0x431290`, pops a command, writes the base `SoundCommand` vtable (`0x461030`), and calls scalar delete. That matches the reconstructed non-virtual base delete shape and is not a reason to add virtual destructors, because doing so would change vtable layout. Treat this bucket as portability/source-UB debt unless a specific destructor path lacks the dumped reset/delete sequence.
- The two remaining `-Wdelete-incomplete` warnings are both in `AssetList::Cleanup`; no dumped body has been identified for that reconstructed helper yet, so this pass leaves them untouched.

## Next investigation targets

These are the remaining warning/result buckets worth investigating before another broad warning sweep:

- `src/AssetList.cpp:72,116`: the only remaining `-Wdelete-incomplete` warnings. `AssetList.h` still has a stale `0x404230` comment, but `code-full/FUN_404230` is `ZBufferManager::DrawText`, not `AssetList::Cleanup`. Find the real dumped body or prove this helper is reconstruction-only before changing `delete data` to scalar `operator delete` or a typed destructor path.
- `src/EngineB.cpp:193`: resolved by the refreshed `code-full/FUN_450F70` export. The divide-by-zero is original binary behavior and is now commented in source.
- `ShowMessage` format coverage: `ShowMessage` is not annotated yet because `src/GameWindow.cpp` declares a local `void ShowMessage(const char *)` while `src/string.h` declares the real variadic helper. Resolve this in the analysis layer first, then rerun format warnings to see whether additional missing-argument bugs appear.
- Nonliteral format warnings at `src/AnimatedAsset.cpp:61`, `src/CDData.cpp:83,475`, `src/EngineInfoParser.cpp:100`, `src/GameWindow.cpp:157`, `src/Palette.cpp:75`, `src/SC_Fan.cpp:765`, `src/SC_Question.cpp:126`, `src/SC_SelectHotSpot.cpp:276,289,302`, `src/SC_Wahoo.cpp:527`, `src/Sample.cpp:44`, `src/Sprite.cpp:104,326`, and `src/Target.cpp:327`. Most likely flow through path/error formatting helpers, but these are the remaining format warnings that could hide real variadic argument bugs.
- `-Wdelete-non-abstract-non-virtual-dtor`: treat as a class-by-class audit, not a request to add virtual destructors. The largest remaining classes are `Sprite` (100 warnings), `T_MenuButton` (28), `SC_Question` (19), `SoundCommand` (18), `T_MenuHotspot` (11), and `MMPlayer` (11). Keep the current layout unless a dumped delete path proves a different destructor/reset/free sequence.
- `-Wnontrivial-memcall`: review only unusual cases first. Repeated POD-like reset helpers dominate the bucket (`GlyphRect`, `SpriteAction`, `Range`, `FileArchive`), while `T_MenuButton` already has a confirmed original dynamic-class memset. Look for cross-object/global-region memsets like the fixed `SCI_SearchScreen` case, not for every constructor-style zero fill.
- Static analyzer ownership/custom-pool buckets are mostly demoted. Reopen only outliers where the trace does not end in an insertion/registration/ownership-transfer call, or where a custom pool allocation lacks the standard grow-then-pop pattern.
- 64-bit portability warnings (`int`/pointer casts, `cast-align`, `shorten-64-to-32`, `%ld`/`%lu` width mismatches, and `char` subscripts in `string.cpp`) should be tracked separately from original-bug triage. They matter for a 64-bit/macOS port, but they are usually not evidence of an original Win32 bug.

## Assembly triage

This pass checked the high-signal warnings against the function disassembly under `code-full/`. It confirms binary behavior, not original source text.

### Confirmed in the original binary

- `SCI_IconBarModule::LBLParse`, `0x4022B0`: the `DLG_HOTSPOT` branch parses the hotspot id into stack slot `EBP-0x10`, but the repeat-use check and `sprintf("DIALOG %d", ...)` use `EBP-0x14`, which is not initialized in that branch. This looks like an original bug.
- `Parser::ReportUnknownLabel`, `0x412A50`: the call pushes `name`, `lineNumber`, and `filename` for a format whose second conversion is `%s`. This looks like an original error-reporting bug.
- `SCI_IconBarModule::LBLParse`, `0x4022B0`: repeat-hotspot `ShowError("repeat use of hotspot %d")` calls push only the format string. This looks like an original error-reporting bug.
- `SC_Question::LBLParse`, `0x414DA0`: several parse-error calls push only the format string even though it contains `%s`. This looks like an original error-reporting bug.
- `SC_Wahoo::LBLParse`, `0x438660`: the `SET_GAMESTATE` invalid-index call pushes only `s_Invalid_gamestate__d`. This looks like an original error-reporting bug.
- `T_MenuHotspot::LBLParse`, `0x421560`: the `SWITCHFOCUS` branch calls `sscanf` with an extra destination pointer not consumed by the format string, then passes that still-initialized local into the constructed `SpriteAction`. This looks like an original parse bug.
- `SC_OnScreenMessage::Update`, `0x448610`: the loop can call `SoundEntry::SoundUpdate` with `ECX=0` when the current list node is null, and uses the same remove/delete/reset/advance sequence as the reconstruction.
- `SC_OnScreenMessage::Exit`, `0x448830`: the case `0x18` loop reads `_DAT_00000024` when the current list node is null. `FindOrCreateSoundEntry`, `0x448C60`, also reads fixed addresses `0x24` and `0x2c` for null-current fields. These look like original null-current/fixed-address bugs.
- `EngineC::RenderBackground`, `0x40BDC0`: the completion checks use `(limit == 0) || current < limit` as the not-complete path, then divide by that same limit. This looks like an original divide-by-zero bug in two counters.
- `EngineB::PostRender`, `0x450F70`: the partial-meter branch is entered when `m_progress.end` is zero or `m_progress.start < m_progress.end`; the zero case then divides by `m_progress.end`. This looks like an original divide-by-zero bug.
- `SC_DodgeOrville::Update`, `0x429110`: the hit meter uses the same zero-denominator fallthrough before dividing by `hitCount.y`. This looks like an original divide-by-zero bug.
- `SC_DodgeOrville::ThrowBomb`, `0x4294A0`: zero `throwState.y` avoids the limit branch and then is used as the `%` divisor. This looks like an original divide-by-zero bug.
- `SC_Fan::Update`, `0x410060`: zero `dim_168.y` follows the not-complete path and is used as the meter divisor. This looks like an original divide-by-zero bug.
- `SC_Slime::AnimateSprite`, `0x40DF30`: zero `maxHits` follows the not-complete path and is used as the meter divisor. This looks like an original divide-by-zero bug.
- `Parser::SubstituteVars`, `0x4140F0`: when no `%` remains, `strchr` returns null and the final `strncat` count is computed as `0 - src`. This looks like an original string-copy bug.
- `ZBufferManager::Update`, `0x4047C0`: after popping from `m_queue9c`, a null `local_18` can still be incremented and passed to queued commands if `m_queueA0` has work. This looks like an original null-pointer arithmetic bug.
- `ZBufferManager::PostRender`, `0x404B90`: removed render data is dereferenced before the null check. This looks like an original null dereference bug.
- `GameLoopHelper::RemoveAnimation`, `0x41A8D0`: a failed search leaves the node candidate null, then the removal path starts at `0x41A919` and dereferences it. This looks like an original missing-found-check bug.
- `HotspotAction::CheckConditions`, `0x41B7DB`: a null current condition node leads to `data = 0`, then the type check dereferences it. This looks like an original list-iteration bug.
- `GameEngine::GetHandler`, `0x4318F8`: a null current handler node falls into a comparison against `_DAT_00000090`. This looks like an original fixed-address bug.
- Repeated list-iteration null calls are original in `Handler31::Update` (`0x417500`), `MMPlayer::StopAll` (`0x443AB0`), `MMPlayer::Init` (`0x443B20`), `MMPlayer::ResetAnimations` (`0x443ED0`), `SCI_PracticeRoom::Update` (`0x42AD80`), `SCI_PracticeRoom::Exit` (`0x42B030`), `SC_SelectHotSpot::Draw` (`0x406240`), `SC_SelectHotSpot::Update` (`0x4062C0`), `SC_SelectHotSpot::Exit` (`0x4063E0`), `SC_Timer::Update` (`0x422EE0`), and `ZBufferManager::Update` (`0x4047C0`).
- `MMPlayer::Draw`, `0x443E76`: the draw loop can read sprite coordinates from `ECX=0` if the current list node is null. This is separate from the other MMPlayer list-call warnings because the field reads happen before the method call.
- Missing singleton/sprite checks are original in `MouseControl::DrawCursor` (`0x432DA0`), `PodsEngine::Update` (`0x440A50`), `SCI_Schedule::Update` (`0x435520`), and `SC_Slime::UpdateArmSprites` (`0x40E070`).
- Repeated input-manager null dereferences are original in `EngineB::OnProcessEnd` (`0x451230`), `EngineC::OnProcessEnd` (`0x40C200`), `PodsEngine::OnProcessEnd` (`0x441000`), `SC_FireAlarm::ResetGame` (`0x408530`), `SC_Slime::ResetGame` (`0x40E0D0`), and `SC_Wahoo::ResetGame` (`0x438470`).
- Repeated hash-iterator dereferences are original in `StringTable::TestStrings` (`0x44C4D2`), `TargetList::UpdateTargets` (`0x4436C4`), `GameState::GetStateValue` (`0x433A89`), and `RockThrower::CheckTargetCollision` (`0x427AA0`). Each function uses a `-1` sentinel to start bucket iteration, scans for a non-null bucket, and then dereferences the current node without rechecking whether the scan actually found one.
- `SCI_SchoolMenu::Update`, `0x41EC58`: the condition guards the OK-button active path, but the else path also writes through `okayButton` without checking it. This looks like an original null-button bug.
- `SC_DodgeOrville::Update`, `0x4292CD`: the full-meter draw path reads `clip_x2` and `clip_y2` from the fill VBuffer even if the animation data or target buffer is null. This looks like an original null-VBuffer bug.
- `WinMain`, `0x420620`: after normal shutdown, the function returns `MOV EAX, [ESP+0x8]` from its local stack area. There is no write to that local in the function, so the normal exit code is uninitialized in the original binary.
- `T_MenuButton::T_MenuButton`, `0x421A50`: the constructor stores the vtable pointer and then immediately executes `REP STOSD` from `this` for `0x2a` dwords, wiping the vptr. This is original binary behavior. It is source-level C++ UB if represented as `memset(this, ...)`, though it may have been benign if those objects are never used through virtual dispatch.
- `SC_CrystalPuzzle::Update`, `0x44FE40`: the display guards for `buttons1` and `paths1` are address-of-member checks. The dumped disassembly compares `ESI` (`this`) to `0xffffff3c` and `0xffffff48` before calling `DisplayButtons` and `DisplayMap`, so these checks are original and normally always true.

### Reconstruction/source representation issues

- `ClearGameStateSave`, `0x40EFB0`: assembly performs four explicit dword stores at `0x472be0`, `0x472be4`, `0x472be8`, and `0x472bec`. The prior reconstructed `memset(&g_FanField1_00472be0, 0, 16)` was unsafe C++ because the globals are separate declarations. Fixed.
- `SC_SelectHotSpot::LBLParse`, `0x4065E0`: the `HANDLE` branch pushes `this + 0x94` to `sscanf`, i.e. the address of `moduleParam`. The prior reconstructed `&SC_SelectHotSpot::moduleParam` was a source bug, not an original binary bug. Fixed.
- `SC_Game::Update`, `0x432040`: assembly passes the result of `GameState::GetState` to `%s`; the prior source stored it in an `int`, creating a 64-bit warning. Fixed.
- `Parser::HandleToken` variable substitution, `0x413120`: assembly passes a 32-bit game-state value to `%d`; the prior source had pointer-typed temporaries that made this invalid on LP64. Fixed for the warning-producing cases.
- `Palette::Palette`, `0x41DC40`: assembly calls scalar allocation helper `0x454500` for 0x300 bytes, and `Palette::~Palette`, `0x41DC10`, uses scalar delete helper `0x454400`. The reconstructed `new char[0x300]` was a source-only mismatch. Fixed.
- `TimedEventPool::Pop`, `0x42D1A0`: assembly copies a 0x38-byte embedded `SpriteAction` from the head node using `SpriteAction::CopyFrom` (`0x444920`), destroys that embedded `SpriteAction`, returns the node to the free list, zeroes the destination action, then copies into it. The prior unused `SC_MessageParser` overload copied parser fields from a 0xc8-byte imagined node and had no matching original function. Removed.
- `FlagArray::Seek`, `0x420990`: assembly calls terminating `ShowError("FlagArray::Seek")` if `fp` is null before the `fseek` call. The prior source logged the message and continued with a null stream. Fixed.
- `Graphics` WinG initialization, `0x4534F4`, and main initialization, `0x4236F0`: the original stores resolved function pointers, the recommended DIB format result, and allocated `GameState` pointers directly into their globals. The reconstructed duplicate self-assignments were source-only artifacts. Fixed.
- Dead-store cleanup: `GameState::LBLParse` (`0x433710`) now consumes the derived bucket count in the zeroing call, Parser RETURN and `V_>>` substitution (`0x413120`) no longer keep unused reconstructed temporaries, `TimedEventPool::PopSafe` (`0x42CAB0`) keeps the original exception-state write as volatile, and `VideoTable` (`0x4528EF`, `0x452FAA`, `0x4531D8`) no longer carries unused byte-offset locals or duplicate zero stores when the source uses array indexing. Fixed.
- `mCNavigator::LBLParse`, `0x44BB60`: the duplicate-node error pushes the node-name buffer address and node handle after the format string. The prior source used `&parser->nodeName`, which produces the same address but the wrong variadic type for `%s` on modern compilers. Fixed to `parser->nodeName`.
- Scalar allocation/free representation: `FreePointerArray` (`0x41A6F0`), `SpriteHashTable::AllocateBuckets` (`0x44C580`), `SpriteHashTable::Resize` (`0x4097F0`), `ObjectPool::AllocateBuckets` (`0x44C580`), `SoundList::SoundList`/destructor/`Register` (`0x425170`, `0x425200`, `0x4252F0`), `StringTable::Load` (`0x44C100`), `ZBufferManager::UpdateScreen` (`0x404B90`), and `SC_Pods::Init` (`0x441700`) use scalar allocation/free helper calls in the dump. The prior source used `new[]`/`delete[]`, `delete void*`, or an incomplete `SoundCommand` delete in several of those paths. Fixed without changing the original explicit destructor/reset operations.

### Demoted after assembly check

- `Parser::HandleToken_IF`, `0x412D00`: the `gsValue` warning is only reachable after invalid input. The assembly calls `ShowError` before use, and `ShowError` at `0x425C50` eventually calls exit.
- `Parser::FindKey`, `0x412AD0`: the uninitialized `pos[1]` warning is analyzer imprecision around the 64-bit `SoundTracker::Lookup` return. `SoundTracker::Lookup` at `0x412130` returns both `EAX` and `EDX` on cache hit and returns `EDX:EAX = -1` on miss; `FindKey` stores both dwords before comparing them.
- `Parser::HandleToken` malformed `SET_GAMESTATE`, `SET_MOUSE`, and `V_>>` paths: these contain bad-looking uninitialized paths in the C++ warning output, but valid dispatch reaches them only for matching token strings, and malformed cases call terminating `ShowError`.
- Repeated `SC_Question::LBLParse` queue-type checks, `0x414DA0`: the static analyzer reports uninitialized reads of `queue[3]`, but the original allocation path writes all four queue header dwords, including `local_14[3] = 0`, before the type checks. The reconstructed `Queue()` also delegates to `LinkedList()`, which initializes `type = 0`. Treat these as analyzer imprecision for now.
- Repeated `Hotspot::LBLParse` queue-type checks, `0x445A30`: the static analyzer reports uninitialized reads after `new LinkedList()`, but the original allocation path writes all four queue header dwords before `piVar3[2] = *piVar3`. This is the same analyzer imprecision as the `SC_Question` queue warnings.
- `GameEngine::QueueAction`, `0x431A90`, and `SC_CombatBase::QueueAction`, `0x42C150`: null-pointer arithmetic warnings point at pool block math after allocation. The original code assumes the allocation helper succeeds and performs the same pointer arithmetic before using the free list.
- `CombatSprite::ParseSpriteData`, `0x409500`: the warning follows paths where the current sprite table or custom pool is invalid. The original function also assumes `DAT_004686f8` and the pool free list are valid once the sprite-data parser is entered.
- `SCI_Inventory::Serialize`, `0x43F9B0`: the uninitialized assignment warning is another custom-pool allocation/free-list assumption; the original code initializes the pool header and then assumes the free list is populated.
- Custom pool/free-list null-dereference warnings in `CDData`, `CombatSprite`, `FilePosCache`, `GameLoopHelper`, `GameState`, `HashTable`, `mCNavigator`, `ObjectPool`, `Parser`, `SCI_Inventory`, `SC_DuctNav`, `SoundTracker`, and `TargetList` generally match the original allocator pattern: grow the pool if the free list is empty, then pop from the free list without checking allocation failure or a malformed pool. These are useful hardening notes but not reconstruction bugs by themselves.
- `ZBufferManager::ProcessRenderQueues`, `0x4047C0`: remaining queue-head null warnings for `m_queueA0`, `m_queue9c`, and `m_queueA4` match the original member-invariant assumption that constructor-created queues are non-null. Treat as hardening/invariant noise unless we explicitly start a non-preserving robustness pass.
- `SC_OnScreenMessage::Update` (`0x448610`) and `SC_OnScreenMessage::AddMessage` case `0x18` (`0x448830`): the analyzer's `cplusplus.NewDelete` findings follow manual remove-current code that unlinks the node, captures its data, frees the node, and resets `current` before advancing. The assembly has the same helper shape (`0x449050` for one remove-current path), so these are not reconstruction bugs in the current pass.
- `cplusplus.NewDeleteLeaks` warnings in `Handler31`, `IconBar`, `Hotspot`, `HotspotAction`, `SCI_Inventory`, `SC_PRHotSpot`, `SC_Question`, `SC_Timer`, `T_MenuHotspot`, `ZBufferManager`, and similar parser/list builders mostly follow ownership transfer into slots, queues, or action lists. A representative check in `IconBar::IconBar`, `0x42D460`, allocates a `SpriteAction` and passes it to `IconBarEntry::RegisterSlot`, matching the source. `T_MenuHotspot::LBLParse`, `0x421080`, `Handler31::Exit`, `0x417880`, `SCI_Inventory::LBLParse`, `0x43FDD0`, `SC_PRHotSpot::LBLParse`, `0x42A0C0`, and `ZBufferManager` queue insertion at `0x403CD0`/`0x4044E0` have the same sorted-queue insertion fall-through as the source if `current` becomes null before reaching `tail`; that depends on a malformed list invariant, so it is not a reconstruction bug. Treat this bucket as ownership-model noise unless a warning lacks an insertion/registration call.
- Remaining `deadcode.DeadStores` warnings are currently deliberate. `HotspotAction::CheckConditions`, `0x41B7A8`, initializes the condition result register to 1 before the queue loop; this only looks dead because analysis treats `ShowError` as no-return. `SC_Fan::Update`, `0x4101DF`, stores state 3 into the object after the timeout check, matching the original transition even though this helper does not read the field again.
