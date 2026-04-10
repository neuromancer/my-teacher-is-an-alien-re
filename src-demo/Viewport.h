#ifndef VIEWPORT_H
#define VIEWPORT_H

struct ViewportPair {
    int a, b;
    ViewportPair() { a = 0; b = 0; }
    ~ViewportPair();
};

class Viewport {
public:
    ViewportPair dim;       // 0x00 (a=x1, b=y1)
    ViewportPair anchor;    // 0x08 (a=x2, b=y2)
    ViewportPair size;      // 0x10 (a=width, b=height)
    ViewportPair center;    // 0x18 (a=centerX, b=centerY)
    int scrollX;            // 0x20
    int scrollY;            // 0x24
    int anchorOffsetY;      // 0x28

    Viewport();                       // 0x412B50
    void SetDimensions(int w, int h); // 0x412C30
    void SetAnchor(int x, int y);     // 0x412C50
    void SetDimensions2(int w, int h); // 0x412C90
    void SetCenter();                 // 0x412CB0
    void SetCenterX(int x);           // 0x412CE0
    void SetCenterY(int y);           // 0x412D00
};

#endif // VIEWPORT_H
