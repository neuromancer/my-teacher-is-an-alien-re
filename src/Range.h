#ifndef RANGE_H
#define RANGE_H

struct Range {
    int start;   // 0x00
    int end;     // 0x04
    inline Range() : start(0), end(0) {}
    ~Range() {}
};

struct IntPair {
    int start;   // 0x00
    int end;     // 0x04
    IntPair() : start(0), end(0) {}
};

#endif // RANGE_H
