#ifndef VIEWPORT_H
#define VIEWPORT_H

struct ViewportPair {
    int a, b;
    ViewportPair() { a = 0; b = 0; }
    ~ViewportPair();
};

class Viewport {
public:
    ViewportPair dim;       // 0x00 (a=width, b=height)
    ViewportPair anchor;    // 0x08 (a=anchorX, b=anchorY)
    ViewportPair size;      // 0x10 (a=sizeW, b=sizeH)
    ViewportPair center;    // 0x18 (a=centerX, b=centerY)
    int scrollX;            // 0x20
    int scrollY;            // 0x24
    int anchorOffsetY;      // 0x28

    Viewport();                       // 0x4454F0
    void SetDimensions(int w, int h); // 0x445590
    void SetAnchor(int x, int y);     // 0x4455B0
    void SetDimensions2(int w, int h); // 0x4455F0
    void SetCenter();                 // 0x445610
    void SetCenterX(int x);           // 0x445640
    void SetCenterY(int y);           // 0x445660
};

#endif // VIEWPORT_H
