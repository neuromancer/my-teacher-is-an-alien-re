#ifndef RANGE_H
#define RANGE_H

#include <string.h>
#include "SpriteAction.h"

struct Range {
    int start;   // 0x00
    int end;     // 0x04
    int field_8; // 0x08
    int field_C; // 0x0C
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
