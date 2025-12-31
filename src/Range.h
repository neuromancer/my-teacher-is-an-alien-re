#ifndef RANGE_H
#define RANGE_H

struct Range {
    int start;   // 0x00
    int end;     // 0x04
    Range();     // Constructor - generates thunk at 0x41d850
    ~Range() {}  // Destructor - inline, trivial
};

#endif // RANGE_H
