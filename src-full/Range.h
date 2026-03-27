#ifndef RANGE_H
#define RANGE_H

#include <string.h>
#include "SpriteAction.h"

struct Range {
    SlimeDim dim;    // 0x00-0x07 — triggers SEH in destructor
    int field_8;     // 0x08
    int field_C;     // 0x0C
    Range() { memset(this, 0, sizeof(Range)); }
    ~Range();    // 0x44C830
};

// Accessor macros for Range dim fields (was .start/.end, now .dim.x/.dim.y)
#define RANGE_START(r) ((r).dim.x)
#define RANGE_END(r) ((r).dim.y)

struct IntPair {
    int start;   // 0x00
    int end;     // 0x04
    IntPair() {
        memset(this, 0, sizeof(IntPair));
    }
};

#endif // RANGE_H
