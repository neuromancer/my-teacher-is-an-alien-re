#ifndef RECT_H
#define RECT_H

// 16-byte rectangle struct, ctor 0x40D0C0, dtor 0x401130
struct Rect {
    int left;
    int top;
    int right;
    int bottom;
    Rect() { left = 0; top = 0; right = 0; bottom = 0; }
    ~Rect();
};

// Keep old name as alias for compatibility
typedef Rect InvSlotItem;

#endif // RECT_H
