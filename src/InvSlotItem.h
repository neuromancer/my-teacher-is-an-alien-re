#ifndef RECT_H
#define RECT_H

#include <string.h>

// 16-byte rectangle struct, ctor 0x40D0C0, dtor 0x401130
struct Rect {
    int left;
    int top;
    int right;
    int bottom;
    Rect() { memset(this, 0, 0x10); }
    ~Rect();
};

// Keep old name as alias for compatibility
typedef Rect InvSlotItem;

#endif // RECT_H
