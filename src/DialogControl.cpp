#include "DialogControl.h"
#include "Sprite.h"
#include "MouseControl.h"
#include "Memory.h"
#include <string.h>

/* Function start: 0x409230 */
DialogControl::DialogControl()
{
    int* ptr;
    int i;

    field_13c = 0;
    glyphRect.left = 0;
    glyphRect.top = 0;
    glyphRect.right = 0;
    glyphRect.bottom = 0;

    ptr = (int*)&sprite;
    for (i = 0x2e; i != 0; i--) {
        *ptr = 0;
        ptr++;
    }
    field_118 = 1;
    field_11c = 1;
    field_120 = -1;
    field_124 = -1;
}

/* Function start: 0x4092E0 */
DialogControl::~DialogControl()
{
    Sprite* spr;
    MouseControl* mc;

    spr = sprite;
    if (spr != 0) {
        delete spr;
        sprite = 0;
    }

    mc = mouseArea1;
    if (mc != 0) {
        delete mc;
        mouseArea1 = 0;
    }

    mc = mouseArea2;
    if (mc != 0) {
        delete mc;
        mouseArea2 = 0;
    }

    mc = mouseArea3;
    if (mc != 0) {
        delete mc;
        mouseArea3 = 0;
    }
}
