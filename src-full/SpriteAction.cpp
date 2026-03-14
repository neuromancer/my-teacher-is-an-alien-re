#include "SpriteAction.h"
#include "GameEngine.h"
#include <string.h>
#include <stdlib.h>

extern "C" int DAT_0046a6ec;

/* Function start: 0x401120 */
SlimeDim::~SlimeDim() {}

/* Function start: 0x444920 */
SpriteAction* SpriteAction::CopyFrom(SpriteAction* other) {
    if (this != other) {
        SpriteAction* child;

        child = (SpriteAction*)field_34;
        if (child != 0) {
            child->~SpriteAction();
            free(child);
            field_34 = 0;
        }

        field_00 = other->field_00;
        field_04 = other->field_04;
        field_08 = other->field_08;
        field_0C = other->field_0C;
        field_10 = other->field_10;
        field_14 = other->field_14;
        field_18 = other->field_18;
        dim.field_0 = other->dim.field_0;
        dim.field_4 = other->dim.field_4;
        field_24 = other->field_24;
        field_28 = other->field_28;
        field_2C = other->field_2C;
        field_30 = other->field_30;

        if (other->field_34 != 0) {
            child = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            field_34 = (int)child;
            child->CopyFrom((SpriteAction*)other->field_34);
        }
    }

    return this;
}

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
        free(p);
        field_34 = 0;
    }
}

/* Function start: 0x444D90 */
extern "C" void FUN_00444d90(int a, int b, int c, int d, int e, int f, int g, int h, int i, int j) {
    SpriteAction action(a, b, c, d, e, f, g, h, i, j);
    if (DAT_0046a6ec != 0) {
        ((GameEngine*)DAT_0046a6ec)->EnqueueAction(&action);
        action.field_34 = 0;
    }
}

/* Function start: 0x444E20 */
extern "C" void FUN_00444e20(void* action) {
    if (DAT_0046a6ec != 0) {
        ((GameEngine*)DAT_0046a6ec)->EnqueueAction((SpriteAction*)action);
        ((SpriteAction*)action)->field_34 = 0;
    }
}

/* Function start: 0x444E40 */
void FUN_00444E40(SpriteAction* action) {
    if (DAT_0046a6ec != 0) {
        ((GameEngine*)DAT_0046a6ec)->EnqueueAction(action);
        action->field_34 = 0;
    }
}

// IconBar.obj: FUN_00444e40 with SpriteAction* param (different mangling from void* version)
void FUN_00444e40(SpriteAction* action) {
    if (DAT_0046a6ec != 0) {
        ((GameEngine*)DAT_0046a6ec)->EnqueueAction(action);
        action->field_34 = 0;
    }
}
