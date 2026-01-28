#include "Viewport.h"

/* Function start: 0x412C30 */
void Viewport::SetDimensions(int w, int h) {
    if (w >= 0 && h >= 0) {
        Viewport::x1 = w;
        Viewport::y1 = h;
    }
}

/* Function start: 0x412C50 */
void Viewport::SetAnchor(int x, int y) {
    int iVar1;
    iVar1 = (y - Viewport::y1) + 1;
    if (iVar1 >= 0 && x >= 0 && x < 0x140 && y >= 0 && y < 0xc8) {
        Viewport::x2 = x;
        Viewport::y2 = y;
        Viewport::anchorOffsetY = iVar1;
    }
}

/* Function start: 0x412C90 */
void Viewport::SetDimensions2(int w, int h) {
    if (w >= 0 && h >= 0) {
        Viewport::width = w;
        Viewport::height = h;
    }
}

/* Function start: 0x412CB0 */
void Viewport::SetCenter() {
    Viewport::SetCenterX(Viewport::width / 2);
    Viewport::SetCenterY(Viewport::height / 2);
}

/* Function start: 0x412CE0 */
void Viewport::SetCenterX(int x) {
    if (x >= 0 && x <= Viewport::width) {
        Viewport::centerX = x;
        Viewport::scrollX = Viewport::x1 + x - 1;
    }
}

/* Function start: 0x412D00 */
void Viewport::SetCenterY(int y) {
    if (y >= 0 && y <= Viewport::height) {
        Viewport::centerY = y;
        Viewport::scrollY = Viewport::y1 + y - 1;
    }
}
