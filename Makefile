# Makefile for My Teacher is an Alien

# Compiler and flags
CC = cl
CFLAGS = /c /O2 /GX /I msvc420\\include
LDFLAGS =
OUT_DIR = out

# Source files
SRCS = $(wildcard src/*.cpp)
OBJS = $(patsubst src/%.cpp,$(OUT_DIR)/%.obj,$(SRCS))

# Default target
all: $(OBJS)

TEACHER.EXE: $(OBJS)
	wine cmd /c "compilers\\msvc420\\bin\\VCVARS32.BAT x86 && link $(OBJS) /OUT:$@" 2>/dev/null

# Rule to compile .cpp files to .obj files
$(OUT_DIR)/%.obj: src/%.cpp
	@mkdir -p $(OUT_DIR)
	wine cmd /c "compilers\\msvc420\\bin\\VCVARS32.BAT x86 && $(CC) $(CFLAGS) $< /Fo$@" 2>/dev/null

# Clean rule
clean:
	rm -f $(OUT_DIR)/*.obj
	rm -f $(OUT_DIR)/*.asm
	rm -f $(OUT_DIR)/*.map

.PHONY: all clean
