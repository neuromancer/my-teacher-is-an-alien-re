#ifndef RANGE_H
#define RANGE_H

#include <string.h>

struct Range {
    int start;   // 0x00
    int end;     // 0x04
    inline Range() { memset(this, 0, sizeof(Range)); }
    ~Range() {}
};

struct IntPair {
    int start;   // 0x00
    int end;     // 0x04
    IntPair() {
        memset(this, 0, sizeof(IntPair));
    }
};

#endif // RANGE_H
