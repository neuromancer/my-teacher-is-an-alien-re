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
	src/Mouse.cpp \
	src/MouseControl.cpp \
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

TEACHER.EXE: $(OBJS)
	env LIB='$(MSVC_LIB);3rdparty\miles\lib\win32;3rdparty\smack\lib\win32' $(LINK) /nologo /MAP:TEACHER.map $^ mss32.lib smackw32.lib kernel32.lib user32.lib gdi32.lib winmm.lib advapi32.lib /OUT:$@

$(OUT_DIR)/%.obj $(OUT_DIR)/%.asm: src/%.cpp | $(WIBO)
	@mkdir -p $(OUT_DIR)
	@env INCLUDE='$(MSVC_INC)' $(CC) $(CFLAGS) $< /Fo$(OUT_DIR)/$*.obj /Fa$(OUT_DIR)/$*.asm > out/$*.stdout

clean:
	rm -f $(OUT_DIR)/*.obj $(OUT_DIR)/*.asm $(OUT_DIR)/*.map $(OUT_DIR)/*.stdout TEACHER.EXE TEACHER.map

sort:
	@python3 bin/sortByAddress.py

report:
	@python3 bin/compileAndReport.py

progress:
	@python3 bin/showProgress.py

globals:
	@python3 bin/compareGlobalData.py re/demo/CDDATA/TEACHER.ORI.EXE TEACHER.EXE

globals-verbose:
	@python3 bin/compareGlobalData.py -v re/demo/CDDATA/TEACHER.ORI.EXE TEACHER.EXE

globals-missing:
	@python3 bin/compareGlobalData.py --find-missing re/demo/CDDATA/TEACHER.ORI.EXE

.PHONY: all clean globals globals-verbose globals-missing
