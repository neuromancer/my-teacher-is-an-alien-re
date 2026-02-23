#include "Viewport.h"

ViewportPair::~ViewportPair()
{
}

/* Function start: 0x412B50 */ /* DEMO ONLY - no full game match */
Viewport::Viewport()
{
    Viewport::scrollX = 0;
    Viewport::scrollY = 0;
    Viewport::anchorOffsetY = 0;
    Viewport::SetDimensions(0x140, 0xc8);
    Viewport::SetDimensions2(0, 0);
    Viewport::SetCenter();
    Viewport::SetAnchor(0, Viewport::dim.b - 1);
}

/* Function start: 0x445590 */
void Viewport::SetDimensions(int w, int h) {
    if (w >= 0 && h >= 0) {
        Viewport::dim.a = w;
        Viewport::dim.b = h;
    }
}

/* Function start: 0x412C50 */ /* DEMO ONLY - no full game match */
void Viewport::SetAnchor(int x, int y) {
    int iVar1;
    iVar1 = (y - Viewport::dim.b) + 1;
    if (iVar1 >= 0 && x >= 0 && x < 0x140 && y >= 0 && y < 0xc8) {
        Viewport::anchor.a = x;
        Viewport::anchor.b = y;
        Viewport::anchorOffsetY = iVar1;
    }
}

/* Function start: 0x4455F0 */
void Viewport::SetDimensions2(int w, int h) {
    if (w >= 0 && h >= 0) {
        Viewport::size.a = w;
        Viewport::size.b = h;
    }
}

/* Function start: 0x445610 */
void Viewport::SetCenter() {
    Viewport::SetCenterX(Viewport::size.a / 2);
    Viewport::SetCenterY(Viewport::size.b / 2);
}

/* Function start: 0x445640 */
void Viewport::SetCenterX(int x) {
    if (x >= 0 && x <= Viewport::size.a) {
        Viewport::center.a = x;
        Viewport::scrollX = Viewport::dim.a + x - 1;
    }
}

/* Function start: 0x445660 */
void Viewport::SetCenterY(int y) {
    if (y >= 0 && y <= Viewport::size.b) {
        Viewport::center.b = y;
        Viewport::scrollY = Viewport::dim.b + y - 1;
    }
}
