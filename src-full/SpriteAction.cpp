#include "SpriteAction.h"
#include "GameEngine.h"
extern "C" void WriteToLog(const char* format, ...);
#include <string.h>
#include <stdlib.h>
#include <new.h>

extern "C" int g_GameEngine_0046a6ec;

/* Function start: 0x401120 */
SlimeDim::~SlimeDim() {}

/* Function start: 0x444920 */
SpriteAction* SpriteAction::CopyFrom(SpriteAction* other) {
    if (this != other) {
        SpriteAction* child;

        child = childAction;
        if (child != 0) {
            child->~SpriteAction();
            ::operator delete(child);
            childAction = 0;
        }

        addressType = other->addressType;
        addressValue = other->addressValue;
        fromType = other->fromType;
        fromValue = other->fromValue;
        instruction = other->instruction;
        extra1 = other->extra1;
        extra2 = other->extra2;
        mousePos.field_0 = other->mousePos.field_0;
        mousePos.field_4 = other->mousePos.field_4;
        button1 = other->button1;
        button2 = other->button2;
        lastKey = other->lastKey;
        time = other->time;

        if (other->childAction != 0) {
            child = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            childAction = child;
            child->CopyFrom(other->childAction);
        }
    }

    return this;
}

/* Function start: 0x444A40 */
SpriteAction::SpriteAction(int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9, int p10) {
    int i;
    int* pData;
    int* pMouse;

    pData = (int*)this;
    pMouse = pData + 7;
    pMouse[0] = 0;
    pMouse[1] = 0;
    for (i = 0xe; i != 0; i = i - 1) {
        *pData = 0;
        pData = pData + 1;
    }
    addressType = p1;
    addressValue = p2;
    fromType = p3;
    fromValue = p4;
    instruction = p5;
    extra1 = p6;
    extra2 = p7;
    childAction = (SpriteAction*)p8;
    pMouse[0] = p9;
    pMouse[1] = p10;
}

/* Function start: 0x444AF0 */
SpriteAction::~SpriteAction() {
    SpriteAction* p = childAction;
    if (p != 0) {
        p->~SpriteAction();
        ::operator delete(p);
        childAction = 0;
    }
}

/* Function start: 0x444D90 */
extern "C" void SendGameMessage(int a, int b, int c, int d, int e, int f, int g, int h, int i, int j) {
    SpriteAction action(a, b, c, d, e, f, g, h, i, j);
    if (g_GameEngine_0046a6ec != 0) {
        EnqueueSpriteAction((void*)&action);
    }
}

/* Function start: 0x444E20 */
void EnqueueSpriteAction(void* action) {
    ((GameEngine*)g_GameEngine_0046a6ec)->EnqueueAction((SpriteAction*)action);
    ((SpriteAction*)action)->childAction = 0;
}

/* Function start: 0x444E40 */
void EnqueueSpriteAction(SpriteAction* action) {
    ((GameEngine*)g_GameEngine_0046a6ec)->EnqueueAction(action);
    action->childAction = 0;
}
