#ifndef VIEWPORT_H
#define VIEWPORT_H

class Viewport {
public:
    int x1, y1, x2, y2; // 0x00, 0x04, 0x08, 0x0c
    int width, height;  // 0x10, 0x14
    int centerX;        // 0x18
    int centerY;        // 0x1c
    int scrollX;        // 0x20
    int scrollY;        // 0x24
    int anchorOffsetY;  // 0x28

    Viewport();                       // 0x412B50
    void SetDimensions(int w, int h); // 0x412C30
    void SetAnchor(int x, int y);     // 0x412C50
    void SetDimensions2(int w, int h); // 0x412C90
    void SetCenter();                 // 0x412CB0
    void SetCenterX(int x);           // 0x412CE0
    void SetCenterY(int y);           // 0x412D00
};

#endif // VIEWPORT_H
