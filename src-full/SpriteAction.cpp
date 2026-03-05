#include "SpriteAction.h"
#include <string.h>
#include "Memory.h"

/* Function start: 0x444A40 */
SpriteAction::SpriteAction(int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9, int p10) {
    memset(&field_00, 0, 0x38);
    field_00 = p1;
    field_04 = p2;
    field_08 = p3;
    field_0C = p4;
    field_10 = p5;
    field_14 = p6;
    field_18 = p7;
    field_34 = p8;
    dim.field_0 = p9;
    dim.field_4 = p10;
}

/* Function start: 0x444AF0 */
SpriteAction::~SpriteAction() {
    SpriteAction* p = (SpriteAction*)field_34;
    if (p != 0) {
        p->~SpriteAction();
        FreeMemory(p);
        field_34 = 0;
    }
}
