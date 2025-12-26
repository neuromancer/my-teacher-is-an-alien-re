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

SRCS = $(wildcard src/*.cpp)
OBJS = $(patsubst src/%.cpp,$(OUT_DIR)/%.obj,$(SRCS))
ASMS = $(patsubst src/%.cpp,$(OUT_DIR)/%.asm,$(SRCS))

all: $(WIBO) $(OBJS) $(ASMS)

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    WIBO_PRESET = release
else
    WIBO_PRESET = release-macos
endif

$(WIBO):
	cd wibo-src && cmake --preset $(WIBO_PRESET) && cmake --build --preset $(WIBO_PRESET)
	ln -sf wibo-src/build/release/wibo $@

TEACHER.EXE: $(OBJS)
	env LIB='$(MSVC_LIB)' $(LINK) /nologo $^ /OUT:$@

$(OUT_DIR)/%.obj $(OUT_DIR)/%.asm: src/%.cpp | $(WIBO)
	@mkdir -p $(OUT_DIR)
	@env INCLUDE='$(MSVC_INC)' $(CC) $(CFLAGS) $< /Fo$(OUT_DIR)/$*.obj /Fa$(OUT_DIR)/$*.asm > out/$*.stdout

clean:
	rm -f $(OUT_DIR)/*.obj $(OUT_DIR)/*.asm $(OUT_DIR)/*.map $(OUT_DIR)/*.stdout TEACHER.EXE

sort:
	@python3 bin/sortByAddress.py

report:
	@python3 bin/compileAndReport.py

progress:
	@python3 bin/showProgress.py

.PHONY: all clean
