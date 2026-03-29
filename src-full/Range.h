#ifndef RANGE_H
#define RANGE_H

#include <string.h>
#include "SpriteAction.h"

struct Range {
    SlimeDim dim;        // 0x00-0x07 — start/end frame pair (triggers SEH in destructor)
    int frameCounter;    // 0x08 — current frame count within cycle
    int repeatLimit;     // 0x0C — max repeats before cycle ends (0 = infinite)
    Range() { memset(this, 0, sizeof(Range)); }
    ~Range();    // 0x44C830
};

struct IntPair {
    int start;   // 0x00
    int end;     // 0x04
    IntPair() {
        memset(this, 0, sizeof(IntPair));
    }
};

#endif // RANGE_H
