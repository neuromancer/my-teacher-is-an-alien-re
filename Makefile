# My Teacher is an Alien source reconstruction Makefile.
#
# This build intentionally uses the original Microsoft Visual C++ 4.20
# toolchain under wibo.  Matching that compiler, its flags, and the linker
# input order is part of the recovery process: binary-comp checks the rebuilt
# executable against the original at the instruction and data-layout level.
#
# Common entry points:
#   make                    # full-game executable
#   make TEACHER.EXE        # full-game executable
#   make TEACHER-DEMO.EXE   # demo executable
#   make run                # build and launch the full game in DREAMM
#   make run-demo           # build and launch the demo in DREAMM
#   make run-wine           # build and launch the full game in Wine
#   make verify             # run the primary recovery verification checklist

# ---------------------------------------------------------------------------
# Original toolchain
# ---------------------------------------------------------------------------

WIBO = ./wibo
CC = $(WIBO) compilers/msvc420/bin/CL.EXE
LINK = $(WIBO) compilers/msvc420/bin/LINK.EXE

# MSVC expects Windows-style include/library search paths.  The recipes pass
# these through the host shell to wibo, so command-line /I paths use doubled
# backslashes while env vars keep normal Windows separators.
MSVC_INC = compilers\msvc420\include;compilers\msvc420\mfc\include
MSVC_LIB = compilers\msvc420\lib;compilers\msvc420\mfc\lib

# Keep these flags synchronized with the recovered binary.  Changing optimizer
# or codegen flags will usually invalidate binary-comp comparisons.
CFLAGS = \
	/nologo \
	/c \
	/Og /Oi /Ot /Oy /Ob1 \
	/QIfdiv \
	/Gs /Gf /GX \
	/I msvc420\\include \
	/I 3rdparty\\miles\\include \
	/I 3rdparty\\smack\\include

LINKFLAGS = /nologo /SUBSYSTEM:WINDOWS /ENTRY:WinMainCRTStartup
GAME_LIBPATH = $(MSVC_LIB);3rdparty\miles\lib\win32;3rdparty\smack\lib\win32
GAME_LIBS = \
	mss32.lib \
	smackw32.lib \
	kernel32.lib \
	user32.lib \
	gdi32.lib \
	winmm.lib \
	advapi32.lib

# ---------------------------------------------------------------------------
# Project inputs and generated outputs
# ---------------------------------------------------------------------------

DEMO_DATA_URL = https://downloads.scummvm.org/frs/demos/hypno/teacher-windows-demo-en.zip
CODE_FULL_URL = https://github.com/neuromancer/my-teacher-is-an-alien-re/releases/download/0.0.0/code-full.tar.gz

OUT_DIR = out
OUT_DIR_DEMO = out-demo

# binary-comp is vendored during development.  Allow command-line overrides so
# contributors can point at an installed copy without editing this file.
VERIFY_CONFIG = config/binary-comp.json
BINARY_COMP ?= env PYTHONPATH=binary-comp/src python3 -m binary_comp.cli

VALUE_MIN_SIMILARITY ?= 80
STACK_LOCAL_VALUE_MIN_SIMILARITY ?= 90
STACK_LOCAL_VALUES_FLAGS ?= --no-offsets
VALUES_FLAGS ?=

# Address windows used by the globals audit helper.  The middle range is
# skipped because it belongs to MSVC CRT/library helper data, not game globals.
GLOBALS_MISSING_MIN_ADDRESS = 0x00468000
GLOBALS_MISSING_MAX_ADDRESS = 0x0047185b
GLOBALS_MISSING_TAIL_MIN_ADDRESS = 0x00472bd8
GLOBALS_MISSING_TAIL_MAX_ADDRESS = 0x0047508b

# ---------------------------------------------------------------------------
# Host platform, DREAMM runtime, and Wine runtime
# ---------------------------------------------------------------------------

UNAME_S := $(shell uname -s)
UNAME_M := $(shell uname -m)

# DREAMM is downloaded on demand into .dreamm/ so the repository does not need
# to vendor platform-specific runtime binaries.
DREAMM_DIR = .dreamm
DREAMM_VERSION = 4.0x21
DREAMM_BASE_URL = https://dreamm.aarongiles.com/releases/4.0x
DREAMM_PROPS = -prop winres=640x480x16

ifeq ($(UNAME_S),Darwin)
DREAMM_ARCHIVE = dreamm-$(DREAMM_VERSION)-macos.dmg
DREAMM_BIN = $(DREAMM_DIR)/DREAMM.app/Contents/MacOS/dreamm
else
ifeq ($(UNAME_M),aarch64)
DREAMM_ARCHIVE = dreamm-$(DREAMM_VERSION)-linux-arm64.tgz
else
DREAMM_ARCHIVE = dreamm-$(DREAMM_VERSION)-linux-x64.tgz
endif
DREAMM_BIN = $(DREAMM_DIR)/dreamm
endif
DREAMM = $(CURDIR)/$(DREAMM_BIN)
DREAMM_STAMP = $(DREAMM_DIR)/.$(DREAMM_ARCHIVE).stamp

# Wine runs from the installed full-game data directory.  The CD-ROM image
# should already be mounted by the desktop environment; Wine will expose it as
# a CD drive, and the game will perform its normal CD check.
WINE ?= wine
WINEPREFIX ?= $(CURDIR)/.wine-teacher
WINE_RUN_DIR = data/full

# ---------------------------------------------------------------------------
# Source order
# ---------------------------------------------------------------------------
#
# The object link order affects addresses and therefore binary comparison.
# SRCS_ORDERED_DEMO records the recovered demo order.  The full build maps that
# order from src-demo/ to src/ where filenames overlap, then appends any extra
# full-game sources so new files are still picked up automatically.
SRCS_ORDERED_DEMO = \
	src-demo/ZBuffer.cpp \
	src-demo/TimedEvent.cpp \
	src-demo/Handler13.cpp \
	src-demo/SC_Timer.cpp \
	src-demo/Queue.cpp \
	src-demo/IconBar.cpp \
	src-demo/Handler1.cpp \
	src-demo/Character.cpp \
	src-demo/AssetList.cpp \
	src-demo/Handler6.cpp \
	src-demo/SCI_AfterSchoolMenu.cpp \
	src-demo/Handler8.cpp \
	src-demo/SC_Question.cpp \
	src-demo/SCI_Dialog.cpp \
	src-demo/MMPlayer2.cpp \
	src-demo/Hotspot.cpp \
	src-demo/OptionMenu.cpp \
	src-demo/OnScreenMessage.cpp \
	src-demo/SC_OnScreenMessage.cpp \
	src-demo/SCI_SearchScreen.cpp \
	src-demo/SoundItem.cpp \
	src-demo/SC_Sound.cpp \
	src-demo/MessageList.cpp \
	src-demo/main.cpp \
	src-demo/GameLoop.cpp \
	src-demo/string.cpp \
	src-demo/Handler4.cpp \
	src-demo/Handler2.cpp \
	src-demo/SearchScreen.cpp \
	src-demo/SC_Combat1.cpp \
	src-demo/Engine.cpp \
	src-demo/EngineB.cpp \
	src-demo/EngineA.cpp \
	src-demo/Viewport.cpp \
	src-demo/mCNavigator.cpp \
	src-demo/mCNavNode.cpp \
	src-demo/ObjectPool.cpp \
	src-demo/Target.cpp \
	src-demo/TargetList.cpp \
	src-demo/CombatSprite.cpp \
	src-demo/GameOutcome.cpp \
	src-demo/Weapon.cpp \
	src-demo/WeaponDraw.cpp \
	src-demo/Projectile.cpp \
	src-demo/RockThrower.cpp \
	src-demo/CursorState.cpp \
	src-demo/EngineInfoParser.cpp \
	src-demo/ScriptHandler.cpp \
	src-demo/EventList.cpp \
	src-demo/Parser.cpp \
	src-demo/Timer.cpp \
	src-demo/TimeOut.cpp \
	src-demo/Graphics.cpp \
	src-demo/CDData.cpp \
	src-demo/Message.cpp \
	src-demo/VBuffer.cpp \
	src-demo/DrawScaledSprite.cpp \
	src-demo/ZBufferManager.cpp \
	src-demo/Sprite.cpp \
	src-demo/Sound.cpp \
	src-demo/Sample.cpp \
	src-demo/SoundList.cpp \
	src-demo/Palette.cpp \
	src-demo/MouseControl.cpp \
	src-demo/MMPlayer.cpp \
	src-demo/Animation.cpp \
	src-demo/FlagArray.cpp \
	src-demo/GameState.cpp \
	src-demo/StringTable.cpp \
	src-demo/HashTable.cpp \
	src-demo/AnimatedAsset.cpp \
	src-demo/InputManager.cpp \
	src-demo/GameWindow.cpp \
	src-demo/GameConfig.cpp \
	src-demo/VideoTable.cpp \
	src-demo/PaletteUtils.cpp \
	src-demo/StateFlag.cpp \
	src-demo/Blit.cpp \
	src-demo/ScaleBuffer.cpp \
	src-demo/Memory.cpp \
	src-demo/Array.cpp \
	src-demo/TimeUtils.cpp \
	src-demo/FileSystem.cpp \
	src-demo/globals.cpp

SRCS_ORDERED_ALL = $(patsubst src-demo/%,src/%,$(SRCS_ORDERED_DEMO))
SRCS_ORDERED = $(filter $(wildcard src/*.cpp), $(SRCS_ORDERED_ALL))
SRCS = $(SRCS_ORDERED) $(filter-out $(SRCS_ORDERED_ALL), $(wildcard src/*.cpp))
OBJS = $(patsubst src/%.cpp,$(OUT_DIR)/%.obj,$(SRCS))
ASMS = $(patsubst src/%.cpp,$(OUT_DIR)/%.asm,$(SRCS))

# The demo build keeps the known demo order exactly, then appends any local
# demo-only sources that are not in that list.
SRCS_DEMO = $(SRCS_ORDERED_DEMO) $(filter-out $(SRCS_ORDERED_DEMO), $(wildcard src-demo/*.cpp))
OBJS_DEMO = $(patsubst src-demo/%.cpp,$(OUT_DIR_DEMO)/%.obj,$(SRCS_DEMO))
ASMS_DEMO = $(patsubst src-demo/%.cpp,$(OUT_DIR_DEMO)/%.asm,$(SRCS_DEMO))

# ---------------------------------------------------------------------------
# Build targets and tool bootstrap
# ---------------------------------------------------------------------------

all: TEACHER.EXE

demo: $(WIBO) $(OBJS_DEMO) $(ASMS_DEMO)

build-full: TEACHER.EXE

build-demo: TEACHER-DEMO.EXE

ifeq ($(UNAME_S),Linux)
WIBO_PRESET = release64-clang
WIBO_BIN = wibo-src/build/release64-clang/wibo
else
WIBO_PRESET = release-macos
WIBO_BIN = wibo-src/build/release/wibo
endif

$(WIBO):
	cd wibo-src && cmake --preset $(WIBO_PRESET) && cmake --build --preset $(WIBO_PRESET)
	ln -sf $(WIBO_BIN) $@

# The MSVC420 submodule ships an msvcrt40.dll that doesn't work with wibo;
# overwrite it with the vendored copy in 3rdparty/ before invoking CL/LINK.
MSVCRT_DLL = compilers/msvc420/bin/msvcrt40.dll

$(MSVCRT_DLL): 3rdparty/msvcrt40.dll
	cp -f $< $@

$(DREAMM_BIN): force-dreamm
	@if [ ! -x "$(DREAMM_BIN)" ] || [ ! -f "$(DREAMM_STAMP)" ]; then $(MAKE) --no-print-directory -B "$(DREAMM_STAMP)"; fi

$(DREAMM_STAMP):
	@mkdir -p $(DREAMM_DIR)
	@echo "Downloading DREAMM $(DREAMM_VERSION)..."
	@curl -L -o $(DREAMM_DIR)/$(DREAMM_ARCHIVE) $(DREAMM_BASE_URL)/$(DREAMM_ARCHIVE)
ifeq ($(UNAME_S),Darwin)
	@rm -rf $(DREAMM_DIR)/mnt
	@hdiutil attach $(DREAMM_DIR)/$(DREAMM_ARCHIVE) -mountpoint $(DREAMM_DIR)/mnt -nobrowse -quiet
	@rm -rf $(DREAMM_DIR)/DREAMM.app
	@cp -R $(DREAMM_DIR)/mnt/DREAMM.app $(DREAMM_DIR)/
	@hdiutil detach $(DREAMM_DIR)/mnt -quiet
	@xattr -dr com.apple.quarantine $(DREAMM_DIR)/DREAMM.app
else
	@rm -rf $(DREAMM_DIR)/dreamm
	@tar xzf $(DREAMM_DIR)/$(DREAMM_ARCHIVE) -C $(DREAMM_DIR) --strip-components=1
endif
	@rm $(DREAMM_DIR)/$(DREAMM_ARCHIVE)
	@rm -f $(DREAMM_DIR)/.dreamm-*.stamp
	@touch "$(DREAMM_STAMP)"

TEACHER.EXE: $(OBJS) | $(MSVCRT_DLL)
	env LIB='$(GAME_LIBPATH)' $(LINK) $(LINKFLAGS) /MAP:TEACHER.map $^ $(GAME_LIBS) /OUT:$@

TEACHER-DEMO.EXE: $(OBJS_DEMO) | $(MSVCRT_DLL)
	env LIB='$(GAME_LIBPATH)' $(LINK) $(LINKFLAGS) /MAP:TEACHER-DEMO.map $^ $(GAME_LIBS) /OUT:$@

$(OUT_DIR)/%.obj $(OUT_DIR)/%.asm: src/%.cpp | $(WIBO) $(MSVCRT_DLL)
	@mkdir -p $(OUT_DIR)
	@env INCLUDE='$(MSVC_INC)' $(CC) $(CFLAGS) $< \
		/Fo$(OUT_DIR)/$*.obj \
		/Fa$(OUT_DIR)/$*.asm \
		> $(OUT_DIR)/$*.stdout

$(OUT_DIR_DEMO)/%.obj $(OUT_DIR_DEMO)/%.asm: src-demo/%.cpp | $(WIBO) $(MSVCRT_DLL)
	@mkdir -p $(OUT_DIR_DEMO)
	@env INCLUDE='$(MSVC_INC)' $(CC) $(CFLAGS) $< \
		/Fo$(OUT_DIR_DEMO)/$*.obj \
		/Fa$(OUT_DIR_DEMO)/$*.asm \
		> $(OUT_DIR_DEMO)/$*.stdout

# ---------------------------------------------------------------------------
# Recovery reports and broad binary-comp audits
# ---------------------------------------------------------------------------
#
# binary-comp command coverage:
#   calls         -> verify-calls
#   compare       -> manual per-function use; see HINTS.md examples
#   data          -> globals-data, globals-data-verbose, missing-data
#   exe           -> compare-full, compare-full-functions, compare-demo
#   global-access -> verify-global-access
#   globals       -> verify-globals, verify-globals-code, globals-missing
#   report        -> report, report-demo
#   seh           -> seh, seh-demo
#   values        -> verify-values, verify-values-stack-locals
#   vtables       -> verify-vtables

BC_FULL = --config $(VERIFY_CONFIG) --target full
BC_DEMO = --config $(VERIFY_CONFIG) --target demo

# Extra flags are intentionally overrideable from the command line:
#   make verify-calls CALLS_FLAGS=--all
#   make verify-global-access GLOBAL_ACCESS_FLAGS="--all --include-address-immediates"
CALLS_FLAGS ?=
GLOBAL_ACCESS_FLAGS ?=

# Clang analysis defaults to the whole reconstructed full-game source tree.
# Override for focused triage:
#   make analyze-clang ANALYZE_FILES="src/Parser.cpp src/SoundList.cpp"
ANALYZE_FILES ?= all

sort:
	@python3 bin/sortByAddress.py

progress:
	@python3 bin/showProgress.py

progress-demo:
	@python3 bin/showProgress.py --demo

report: TEACHER.EXE | code-full data/full/DATA
	@$(BINARY_COMP) report $(BC_FULL) $(if $(FILTER),--filter $(FILTER))

report-demo: TEACHER-DEMO.EXE | data/demo/CDDATA
	@$(BINARY_COMP) report $(BC_DEMO) $(if $(FILTER),--filter $(FILTER))

# C++ exception-handling (FuncInfo unwind) structure report.  Lists functions
# whose SEH structure differs from the original (wrong member dtor type, missing
# EH frame, member-count mismatches) that mnemonic similarity cannot see.
#   make seh                     -> scan all functions
#   make seh FILTER=SC_Gauntlet  -> restrict to matching names
# `seh --report` exits non-zero when differences exist (it doubles as a gate);
# `|| true` keeps `make seh` informational like `make report`.
seh: TEACHER.EXE | code-full data/full/DATA
	@$(BINARY_COMP) seh $(BC_FULL) --report $(if $(FILTER),--filter $(FILTER)) || true

seh-demo: TEACHER-DEMO.EXE | data/demo/CDDATA
	@$(BINARY_COMP) seh $(BC_DEMO) --report $(if $(FILTER),--filter $(FILTER)) || true

# Data comparison is a detailed global-by-global byte report.  It is kept
# separate from `verify` because this reconstruction still has known data/map
# deltas that are useful to inspect but not currently expected to be zero.
globals-data: TEACHER.EXE | data/full/DATA
	@$(BINARY_COMP) data $(BC_FULL)

globals-data-verbose: TEACHER.EXE | data/full/DATA
	@$(BINARY_COMP) data $(BC_FULL) --verbose

# Compatibility aliases used by older notes/scripts.
globals: globals-data

globals-verbose: globals-data-verbose

# Broad untracked-dword scan.  This intentionally reports many packed strings
# and tables today, so the actionable missing-global gate is `verify-globals-code`.
missing-data: | data/full/DATA
	@$(BINARY_COMP) data $(BC_FULL) \
		--find-missing \
		--min-address $(GLOBALS_MISSING_MIN_ADDRESS) \
		--max-address $(GLOBALS_MISSING_MAX_ADDRESS)
	@$(BINARY_COMP) data $(BC_FULL) \
		--find-missing \
		--min-address $(GLOBALS_MISSING_TAIL_MIN_ADDRESS) \
		--max-address $(GLOBALS_MISSING_TAIL_MAX_ADDRESS)

compare-full: TEACHER.EXE | data/full/DATA
	@$(BINARY_COMP) exe $(BC_FULL)

compare-full-functions: TEACHER.EXE | data/full/DATA
	@$(BINARY_COMP) exe $(BC_FULL) --functions

compare-demo: TEACHER-DEMO.EXE | data/demo/CDDATA
	@$(BINARY_COMP) exe $(BC_DEMO)

compare-demo-functions: TEACHER-DEMO.EXE | data/demo/CDDATA
	@$(BINARY_COMP) exe $(BC_DEMO) --functions

# Compatibility aliases: the original project workflow used demo comparison by
# default because the demo was the first target reconstructed.
compare: compare-demo

compare-functions: compare-demo-functions

# ---------------------------------------------------------------------------
# Verification
# ---------------------------------------------------------------------------
#
# `verify` runs the expected-zero gates first, then the diagnostic analyzers
# used during recovery.  The diagnostic analyzers stay in this sequence because
# their output is still part of the review checklist, even when the tool itself
# reports known non-zero findings without failing.

verify:
	@$(MAKE) verify-globals
	@$(MAKE) verify-globals-code
	@$(MAKE) verify-calls
	@$(MAKE) verify-global-access
	@$(MAKE) verify-values
	@$(MAKE) verify-values-stack-locals
	@$(MAKE) verify-vtables

verify-globals: TEACHER.EXE | code-full data/full/DATA
	@$(BINARY_COMP) globals $(BC_FULL) \
		--fail-on-issues \
		--fail-on-warnings

# Check nonzero globals exported by Ghidra but not covered by src/globals.cpp.
# The CRT/library helper range is intentionally excluded; it is compiler data,
# not recovered game state.
verify-globals-code: TEACHER.EXE | code-full data/full/DATA
	@$(BINARY_COMP) globals $(BC_FULL) \
		--include-code-globals \
		--fail-on-issues \
		--fail-on-warnings \
		--min-address $(GLOBALS_MISSING_MIN_ADDRESS) \
		--max-address $(GLOBALS_MISSING_MAX_ADDRESS)
	@$(BINARY_COMP) globals $(BC_FULL) \
		--include-code-globals \
		--fail-on-issues \
		--fail-on-warnings \
		--min-address $(GLOBALS_MISSING_TAIL_MIN_ADDRESS) \
		--max-address $(GLOBALS_MISSING_TAIL_MAX_ADDRESS)

globals-missing: verify-globals-code

audit-auto-complete-globals: TEACHER.EXE | code-full data/full/DATA
	@$(BINARY_COMP) globals $(BC_FULL) --show-auto-complete-reviewed

audit-rebuilt-global-layout: TEACHER.EXE | code-full data/full/DATA
	@$(BINARY_COMP) globals $(BC_FULL) --check-rebuilt-layout

verify-calls: | code-full data/full/DATA
	@$(BINARY_COMP) calls $(BC_FULL) $(CALLS_FLAGS) $(if $(FILTER),$(FILTER))

verify-global-access: | code-full data/full/DATA
	@$(BINARY_COMP) global-access $(BC_FULL) $(GLOBAL_ACCESS_FLAGS) $(if $(FILTER),$(FILTER))

verify-values: TEACHER.EXE | code-full data/full/DATA
	@$(BINARY_COMP) values $(BC_FULL) \
		--min-similarity $(VALUE_MIN_SIMILARITY) \
		$(VALUES_FLAGS)

verify-values-stack-locals: TEACHER.EXE | code-full data/full/DATA
	@$(BINARY_COMP) values $(BC_FULL) \
		--min-similarity $(STACK_LOCAL_VALUE_MIN_SIMILARITY) \
		--include-stack-locals \
		$(STACK_LOCAL_VALUES_FLAGS) \
		$(VALUES_FLAGS)

verify-vtables: TEACHER.EXE | code-full data/full/DATA
	@$(BINARY_COMP) vtables $(BC_FULL)

# ---------------------------------------------------------------------------
# Analysis-only Clang diagnostics
# ---------------------------------------------------------------------------

analyze:
	@$(MAKE) analyze-clang ANALYZE_FILES="$(ANALYZE_FILES)"
	@$(MAKE) analyze-static ANALYZE_FILES="$(ANALYZE_FILES)"

analyze-clang:
	@tools/analyze_clang.sh $(ANALYZE_FILES)

analyze-static:
	@tools/analyze_static.sh $(ANALYZE_FILES)

# ---------------------------------------------------------------------------
# Game data and Ghidra export downloads
# ---------------------------------------------------------------------------

data/demo/CDDATA:
	@echo "Downloading demo data..."
	@curl -L -o data/teacher-demo.zip $(DEMO_DATA_URL)
	@unzip -o data/teacher-demo.zip -d data/demo
	@rm -f data/teacher-demo.zip
	@mv data/demo/CDDATA/TEACHER.EXE data/demo/CDDATA/TEACHER.ORIGINAL.EXE

# code-full/ holds the Ghidra-exported disassembly used by binary-comp.
# Never overwrite an existing tree (users may have updated exports locally).
code-full:
	@test ! -d code-full || \
		(echo "code-full/ already present; refusing to overwrite. Remove it manually to re-download." >&2 && exit 1)
	@echo "Downloading code-full from $(CODE_FULL_URL)..."
	@curl -L -o code-full.tar.gz $(CODE_FULL_URL)
	@tar xzf code-full.tar.gz
	@rm -f code-full.tar.gz

data/full/teacher.iso:
	@echo "Error: data/full/teacher.iso not found. Place the retail CD-ROM image there first." >&2
	@exit 1

data/full/DATA: data/full/teacher.iso
	@command -v bsdtar >/dev/null 2>&1 || \
		(echo "Error: bsdtar required for ISO extraction (install libarchive)" >&2 && exit 1)
	@echo "Extracting game files from teacher.iso..."
	@bsdtar -xf data/full/teacher.iso -C data/full --strip-components=1 CDDATA
	@bsdtar -xf data/full/teacher.iso -C data/full TEACHER.ID 2>/dev/null || true
	@if [ -f data/full/TEACHER.ID ]; then mv data/full/TEACHER.ID data/full/teacher.id; fi
	@mv data/full/TEACHER.EXE data/full/TEACHER.ORI.EXE
	@touch data/full/DATA

# ---------------------------------------------------------------------------
# DREAMM launch targets
# ---------------------------------------------------------------------------

run-demo: TEACHER-DEMO.EXE | data/demo/CDDATA $(DREAMM_BIN)
	cp -f TEACHER-DEMO.EXE data/demo/CDDATA/TEACHER.EXE
	cd data/demo/CDDATA && $(DREAMM) $(DREAMM_PROPS) -launch TEACHER.EXE

run-demo-original: | data/demo/CDDATA $(DREAMM_BIN)
	cd data/demo/CDDATA && $(DREAMM) $(DREAMM_PROPS) -launch TEACHER.ORIGINAL.EXE

run: TEACHER.EXE | data/full/DATA $(DREAMM_BIN)
	@mkdir -p data/full/hd
	cp -f TEACHER.EXE data/full/TEACHER.EXE
	cd data/full && $(DREAMM) -mount rw:C=hd $(DREAMM_PROPS) -mount d=teacher.iso -launch TEACHER.EXE

run-original: | data/full/DATA $(DREAMM_BIN)
	cd data/full && $(DREAMM) -mount rw:C=hd $(DREAMM_PROPS) -mount d=teacher.iso -launch TEACHER.ORI.EXE

debug: TEACHER.EXE | data/full/DATA $(DREAMM_BIN)
	@mkdir -p data/full/hd
	cp -f TEACHER.EXE data/full/TEACHER.EXE
	cd data/full && $(DREAMM) -mount rw:C=hd $(DREAMM_PROPS) -mount d=teacher.iso -debug -launch TEACHER.EXE > debug.log

# ---------------------------------------------------------------------------
# Wine launch targets
# ---------------------------------------------------------------------------

wine-check:
	@command -v $(WINE) >/dev/null 2>&1 || \
		(echo "Error: wine not found. Install Wine or set WINE=/path/to/wine." >&2 && exit 1)
	@test -d "$(WINE_RUN_DIR)/DATA" || \
		(echo "Error: expected installed game data at $(WINE_RUN_DIR)/DATA." >&2 && exit 1)

wine-stage: TEACHER.EXE wine-check
	cp -f TEACHER.EXE "$(WINE_RUN_DIR)/TEACHER.EXE"

run-wine: wine-stage
	cd "$(WINE_RUN_DIR)" && WINEPREFIX="$(WINEPREFIX)" $(WINE) TEACHER.EXE

run-wine-original: wine-check
	@test -f "$(WINE_RUN_DIR)/TEACHER.ORI.EXE" || \
		(echo "Error: expected original executable at $(WINE_RUN_DIR)/TEACHER.ORI.EXE." >&2 && exit 1)
	cd "$(WINE_RUN_DIR)" && WINEPREFIX="$(WINEPREFIX)" $(WINE) TEACHER.ORI.EXE

# ---------------------------------------------------------------------------
# Cleanup and phony declarations
# ---------------------------------------------------------------------------

clean:
	rm -f $(OUT_DIR)/*.obj $(OUT_DIR)/*.asm $(OUT_DIR)/*.stdout TEACHER.EXE TEACHER.map

clean-demo:
	rm -f $(OUT_DIR_DEMO)/*.obj $(OUT_DIR_DEMO)/*.asm $(OUT_DIR_DEMO)/*.stdout TEACHER-DEMO.EXE TEACHER-DEMO.map

clean-wine:
	rm -f "$(WINE_RUN_DIR)/TEACHER.EXE"

.PHONY: \
	all \
	analyze \
	analyze-clang \
	analyze-static \
	audit-auto-complete-globals \
	audit-rebuilt-global-layout \
	build-demo \
	build-full \
	clean \
	clean-demo \
	clean-wine \
	compare \
	compare-demo \
	compare-demo-functions \
	compare-full \
	compare-full-functions \
	compare-functions \
	debug \
	demo \
	force-dreamm \
	globals \
	globals-data \
	globals-data-verbose \
	globals-missing \
	globals-verbose \
	missing-data \
	progress \
	progress-demo \
	report \
	report-demo \
	run \
	run-demo \
	run-demo-original \
	run-original \
	run-wine \
	run-wine-original \
	seh \
	seh-demo \
	sort \
	verify \
	verify-calls \
	verify-global-access \
	verify-globals \
	verify-globals-code \
	verify-values \
	verify-values-stack-locals \
	verify-vtables \
	wine-check \
	wine-stage
