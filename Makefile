# Makefile for My Teacher is an Alien

# Compiler and flags
WIBO = ./wibo
CC = $(WIBO) compilers/msvc420/bin/CL.EXE
LINK = $(WIBO) compilers/msvc420/bin/LINK.EXE

# Environment variables for MSVC
# Note: Paths are relative to project root, formatted for Windows
MSVC_INC = compilers\msvc420\include;compilers\msvc420\mfc\include
MSVC_LIB = compilers\msvc420\lib;compilers\msvc420\mfc\lib

CFLAGS = /nologo /c /O2 /GX /I msvc420\\include /I 3rdparty\\miles\\include /I 3rdparty\\smack\\include
OUT_DIR = out

# SRCS ordered to match Original Binary (Demo) Globals layout
# Order determined by analyzing string positions in the demo .data section
SRCS_ORDERED = \
	src/Animation.cpp \
	src/FlagArray.cpp \
	src/GameState.cpp \
	src/GameWindow.cpp \
	src/MouseControl.cpp \
	src/MMPlayer.cpp \
	src/Parser.cpp \
	src/Sample.cpp \
	src/Sound.cpp \
	src/Sprite.cpp \
	src/VBuffer.cpp \
	src/main.cpp \
	src/Queue.cpp \
	src/CDData.cpp \
	src/Timer.cpp \
	src/SoundList.cpp \
	src/ZBuffer.cpp \
	src/ZBufferManager.cpp \
	src/GameLoop.cpp \
	src/globals.cpp \
	src/Hotspot.cpp \
	src/SCI_AfterSchoolMenu.cpp \
	src/SCI_Dialog.cpp \
	src/MMPlayer2.cpp \
	src/OptionMenu.cpp \
	src/SC_OnScreenMessage.cpp \
	src/Engine.cpp \
	src/EngineB.cpp \
	src/mCNavigator.cpp \
	src/string.cpp

SRCS = $(SRCS_ORDERED) $(filter-out $(SRCS_ORDERED), $(wildcard src/*.cpp))
OBJS = $(patsubst src/%.cpp,$(OUT_DIR)/%.obj,$(SRCS))
ASMS = $(patsubst src/%.cpp,$(OUT_DIR)/%.asm,$(SRCS))

# Full game build
OUT_DIR_FULL = out-full
SRCS_ORDERED_FULL = $(patsubst src/%,src-full/%,$(SRCS_ORDERED))
SRCS_FULL = $(SRCS_ORDERED_FULL) $(filter-out $(SRCS_ORDERED_FULL), $(wildcard src-full/*.cpp))
OBJS_FULL = $(patsubst src-full/%.cpp,$(OUT_DIR_FULL)/%.obj,$(SRCS_FULL))
ASMS_FULL = $(patsubst src-full/%.cpp,$(OUT_DIR_FULL)/%.asm,$(SRCS_FULL))

all: $(WIBO) $(OBJS) $(ASMS)

UNAME_S := $(shell uname -s)
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

TEACHER-DEMO.EXE: $(OBJS)
	env LIB='$(MSVC_LIB);3rdparty\miles\lib\win32;3rdparty\smack\lib\win32' $(LINK) /nologo /MAP:TEACHER-DEMO.map $^ mss32.lib smackw32.lib kernel32.lib user32.lib gdi32.lib winmm.lib advapi32.lib /OUT:$@

TEACHER-FULL.EXE: $(OBJS_FULL)
	env LIB='$(MSVC_LIB);3rdparty\miles\lib\win32;3rdparty\smack\lib\win32' $(LINK) /nologo /MAP:TEACHER-FULL.map $^ mss32.lib smackw32.lib kernel32.lib user32.lib gdi32.lib winmm.lib advapi32.lib /OUT:$@

$(OUT_DIR)/%.obj $(OUT_DIR)/%.asm: src/%.cpp | $(WIBO)
	@mkdir -p $(OUT_DIR)
	@env INCLUDE='$(MSVC_INC)' $(CC) $(CFLAGS) $< /Fo$(OUT_DIR)/$*.obj /Fa$(OUT_DIR)/$*.asm > out/$*.stdout

# Full game build rule
full: $(WIBO) $(OBJS_FULL) $(ASMS_FULL)

$(OUT_DIR_FULL)/%.obj $(OUT_DIR_FULL)/%.asm: src-full/%.cpp | $(WIBO)
	@mkdir -p $(OUT_DIR_FULL)
	@env INCLUDE='$(MSVC_INC)' $(CC) $(CFLAGS) $< /Fo$(OUT_DIR_FULL)/$*.obj /Fa$(OUT_DIR_FULL)/$*.asm > $(OUT_DIR_FULL)/$*.stdout

clean:
	rm -f $(OUT_DIR)/*.obj $(OUT_DIR)/*.asm $(OUT_DIR)/*.map $(OUT_DIR)/*.stdout TEACHER-DEMO.EXE TEACHER-DEMO.map

clean-full:
	rm -f $(OUT_DIR_FULL)/*.obj $(OUT_DIR_FULL)/*.asm $(OUT_DIR_FULL)/*.stdout TEACHER-FULL.EXE TEACHER-FULL.map

sort:
	@python3 bin/sortByAddress.py

report:
	@python3 bin/compileAndReport.py

progress:
	@python3 bin/showProgress.py

progress-full:
	@python3 bin/showProgress.py --full

report-full:
	@python3 bin/compileAndReport.py --full

globals:
	@python3 bin/compareGlobalData.py re/demo/CDDATA/TEACHER.ORI.EXE TEACHER-DEMO.EXE

globals-verbose:
	@python3 bin/compareGlobalData.py -v re/demo/CDDATA/TEACHER.ORI.EXE TEACHER-DEMO.EXE

globals-missing:
	@python3 bin/compareGlobalData.py --find-missing re/demo/CDDATA/TEACHER.ORI.EXE

.PHONY: all full clean clean-full globals globals-verbose globals-missing progress progress-full report report-full
