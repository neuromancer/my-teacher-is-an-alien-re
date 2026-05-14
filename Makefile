# Makefile for My Teacher is an Alien

# Compiler and flags
WIBO = ./wibo
CC = $(WIBO) compilers/msvc420/bin/CL.EXE
LINK = $(WIBO) compilers/msvc420/bin/LINK.EXE

# Environment variables for MSVC
# Note: Paths are relative to project root, formatted for Windows
MSVC_INC = compilers\msvc420\include;compilers\msvc420\mfc\include
MSVC_LIB = compilers\msvc420\lib;compilers\msvc420\mfc\lib

CFLAGS = /nologo /c /Og /Oi /Ot /Oy /Ob1 /Gs /Gf /GX /I msvc420\\include /I 3rdparty\\miles\\include /I 3rdparty\\smack\\include
DEMO_DATA_URL = https://downloads.scummvm.org/frs/demos/hypno/teacher-windows-demo-en.zip
OUT_DIR = out

# Platform detection
UNAME_S := $(shell uname -s)
UNAME_M := $(shell uname -m)

# DREAMM setup - auto-downloaded to .dreamm/
DREAMM_DIR = .dreamm
DREAMM_VERSION = 4.0
DREAMM_BASE_URL = https://dreamm.aarongiles.com/releases

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

# Full game build (primary)
# SRCS_ORDERED derived from demo order, remapped to src/
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

# Demo build
OUT_DIR_DEMO = out-demo
SRCS_DEMO = $(SRCS_ORDERED_DEMO) $(filter-out $(SRCS_ORDERED_DEMO), $(wildcard src-demo/*.cpp))
OBJS_DEMO = $(patsubst src-demo/%.cpp,$(OUT_DIR_DEMO)/%.obj,$(SRCS_DEMO))
ASMS_DEMO = $(patsubst src-demo/%.cpp,$(OUT_DIR_DEMO)/%.asm,$(SRCS_DEMO))

all: $(WIBO) $(OBJS) $(ASMS)

demo: $(WIBO) $(OBJS_DEMO) $(ASMS_DEMO)

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

$(DREAMM_BIN):
	@mkdir -p $(DREAMM_DIR)
	@echo "Downloading DREAMM $(DREAMM_VERSION)..."
	@curl -L -o $(DREAMM_DIR)/$(DREAMM_ARCHIVE) $(DREAMM_BASE_URL)/$(DREAMM_ARCHIVE)
ifeq ($(UNAME_S),Darwin)
	@hdiutil attach $(DREAMM_DIR)/$(DREAMM_ARCHIVE) -mountpoint $(DREAMM_DIR)/mnt -nobrowse -quiet
	@cp -R $(DREAMM_DIR)/mnt/DREAMM.app $(DREAMM_DIR)/
	@hdiutil detach $(DREAMM_DIR)/mnt -quiet
	@xattr -dr com.apple.quarantine $(DREAMM_DIR)/DREAMM.app
else
	@tar xzf $(DREAMM_DIR)/$(DREAMM_ARCHIVE) -C $(DREAMM_DIR) --strip-components=1
endif
	@rm $(DREAMM_DIR)/$(DREAMM_ARCHIVE)

TEACHER.EXE: $(OBJS)
	env LIB='$(MSVC_LIB);3rdparty\miles\lib\win32;3rdparty\smack\lib\win32' $(LINK) /nologo /MAP:TEACHER.map $^ mss32.lib smackw32.lib kernel32.lib user32.lib gdi32.lib winmm.lib advapi32.lib /OUT:$@

TEACHER-DEMO.EXE: $(OBJS_DEMO)
	env LIB='$(MSVC_LIB);3rdparty\miles\lib\win32;3rdparty\smack\lib\win32' $(LINK) /nologo /MAP:TEACHER-DEMO.map $^ mss32.lib smackw32.lib kernel32.lib user32.lib gdi32.lib winmm.lib advapi32.lib /OUT:$@

$(OUT_DIR)/%.obj $(OUT_DIR)/%.asm: src/%.cpp | $(WIBO)
	@mkdir -p $(OUT_DIR)
	@env INCLUDE='$(MSVC_INC)' $(CC) $(CFLAGS) $< /Fo$(OUT_DIR)/$*.obj /Fa$(OUT_DIR)/$*.asm > $(OUT_DIR)/$*.stdout

# Demo build rule
$(OUT_DIR_DEMO)/%.obj $(OUT_DIR_DEMO)/%.asm: src-demo/%.cpp | $(WIBO)
	@mkdir -p $(OUT_DIR_DEMO)
	@env INCLUDE='$(MSVC_INC)' $(CC) $(CFLAGS) $< /Fo$(OUT_DIR_DEMO)/$*.obj /Fa$(OUT_DIR_DEMO)/$*.asm > $(OUT_DIR_DEMO)/$*.stdout

clean:
	rm -f $(OUT_DIR)/*.obj $(OUT_DIR)/*.asm $(OUT_DIR)/*.map $(OUT_DIR)/*.stdout TEACHER.EXE TEACHER.map

clean-demo:
	rm -f $(OUT_DIR_DEMO)/*.obj $(OUT_DIR_DEMO)/*.asm $(OUT_DIR_DEMO)/*.stdout TEACHER-DEMO.EXE TEACHER-DEMO.map

sort:
	@python3 bin/sortByAddress.py

report:
	@python3 bin/compileAndReport.py $(if $(FILTER),--filter $(FILTER))

progress:
	@python3 bin/showProgress.py

progress-demo:
	@python3 bin/showProgress.py --demo

report-demo:
	@python3 bin/compileAndReport.py --demo

globals:
	@python3 bin/compareGlobalData.py re/demo/CDDATA/TEACHER.ORI.EXE TEACHER-DEMO.EXE

globals-verbose:
	@python3 bin/compareGlobalData.py -v re/demo/CDDATA/TEACHER.ORI.EXE TEACHER-DEMO.EXE

globals-missing:
	@python3 bin/compareGlobalData.py --find-missing re/demo/CDDATA/TEACHER.ORI.EXE

compare:
	@python3 bin/compareExe.py re/demo/CDDATA/TEACHER.ORI.EXE TEACHER-DEMO.EXE

compare-functions:
	@python3 bin/compareExe.py re/demo/CDDATA/TEACHER.ORI.EXE TEACHER-DEMO.EXE --functions

verify-globals:
	@python3 bin/checkGlobals.py --fail-on-issues --fail-on-warnings

verify-calls:
	@python3 bin/checkCallTargets.py

data/demo/CDDATA:
	@echo "Downloading demo data..."
	@curl -L -o data/teacher-demo.zip $(DEMO_DATA_URL)
	@unzip -o data/teacher-demo.zip -d data/demo
	@rm -f data/teacher-demo.zip
	@mv data/demo/CDDATA/TEACHER.EXE data/demo/CDDATA/TEACHER.ORIGINAL.EXE

run-demo: TEACHER-DEMO.EXE | data/demo/CDDATA $(DREAMM_BIN)
	cp TEACHER-DEMO.EXE data/demo/CDDATA/TEACHER.EXE
	cd data/demo/CDDATA && $(DREAMM) -prop winres=640x480x16 -launch TEACHER.EXE

run-demo-original: | data/demo/CDDATA $(DREAMM_BIN)
	cd data/demo/CDDATA && $(DREAMM) -prop winres=640x480x16 -launch TEACHER.ORIGINAL.EXE

run: TEACHER.EXE | $(DREAMM_BIN)
	@test -f data/full/teacher.iso || (echo "Error: data/full/teacher.iso not found. Place the game ISO there first." && exit 1)
	@mkdir -p data/full/hd
	cp TEACHER.EXE data/full/TEACHER.EXE
	cd data/full && $(DREAMM) -mount rw:C=hd -prop winres=640x480x16 -mount d=teacher.iso -launch TEACHER.EXE

run-original: | data/full/CDDATA $(DREAMM_BIN)
	@test -f data/full/teacher.iso || (echo "Error: data/full/teacher.iso not found. Place the game ISO there first." && exit 1)
	cd data/full && $(DREAMM) -mount rw:C=hd -prop winres=640x480x16 -mount d=teacher.iso -launch TEACHER.ORI.EXE


debug: TEACHER.EXE | $(DREAMM_BIN)
	@test -f data/full/teacher.iso || (echo "Error: data/full/teacher.iso not found. Place the game ISO there first." && exit 1)
	@mkdir -p data/full/hd
	cp TEACHER.EXE data/full/TEACHER.EXE
	cd data/full && $(DREAMM) -mount rw:C=hd -prop winres=640x480x16 -mount d=teacher.iso -debug -launch TEACHER.EXE



.PHONY: all demo clean clean-demo globals globals-verbose globals-missing progress progress-demo report report-demo compare compare-functions verify-globals verify-calls run-demo run-demo-original run
