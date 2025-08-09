# Makefile for My Teacher is an Alien

# Compiler and flags
WINEPREFIX=$(PWD)/.wine
CC = cl
CFLAGS = /c /O2 /GX /I msvc420\\include
OUT_DIR = out

SRCS = $(wildcard src/*.cpp)
OBJS = $(patsubst src/%.cpp,$(OUT_DIR)/%.obj,$(SRCS))
ASMS = $(patsubst src/%.cpp,$(OUT_DIR)/%.asm,$(SRCS))

all: $(OBJS) $(ASMS)

TEACHER.EXE: $(OBJS)
	wine cmd /c "compilers\\msvc420\\bin\\VCVARS32.BAT x86 && link $^ /OUT:$@" 2>/dev/null

$(OUT_DIR)/%.obj $(OUT_DIR)/%.asm: src/%.cpp
	@mkdir -p $(OUT_DIR)
	@wine cmd /c "compilers\\msvc420\\bin\\VCVARS32.BAT x86 > out\\$*.stdout && $(CC) $(CFLAGS) $< /Fo$(OUT_DIR)/$*.obj /Fa$(OUT_DIR)/$*.asm" 2>/dev/null

clean:
	rm -f $(OUT_DIR)/*.obj $(OUT_DIR)/*.asm $(OUT_DIR)/*.map $(OUT_DIR)/*.stdout TEACHER.EXE

sort:
	@python3 bin/sortByAddress.py

report:
	@python3 bin/compileAndReport.py

progress:
	@python3 bin/showProgress.py

.PHONY: all clean
