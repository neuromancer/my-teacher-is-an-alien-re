#ifndef GLYPHRECT_H
#define GLYPHRECT_H

struct GlyphRect {
    int left;    // 0x00 - left edge X coordinate
    int top;     // 0x04 - top edge Y coordinate
    int right;   // 0x08 - right edge X coordinate
    int bottom;  // 0x0C - bottom edge Y coordinate
    GlyphRect() {
        left = 0;
        top = 0;
        right = 0;
        bottom = 0;
    }
    GlyphRect(int l, int t, int r, int b) {
        left = l;
        top = t;
        right = r;
        bottom = b;
    }
    ~GlyphRect();  // Destructor - generates thunk at 0x401680
};

#endif // GLYPHRECT_H
