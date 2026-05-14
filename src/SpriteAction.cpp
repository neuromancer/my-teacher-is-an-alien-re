#include "SpriteAction.h"
#include "GameEngine.h"
#include "GameState.h"
#include "FileArchive.h"
#include "globals.h"
extern "C" void WriteToLog(const char* format, ...);
#include "string.h"
#include <string.h>
#include <stdlib.h>
#include <new.h>

/* Function start: 0x401120 */ /* No assembly extracted */
SlimeDim::~SlimeDim() {}

/* Function start: 0x444B70 */
void SpriteAction::Print(int param) {
    if (addressType == 2) {
        WriteToMessageLog("\t\t\tADDRESS\t\t%s  %s",
            g_StringTable_0046aa34->GetState(addressType),
            g_GameState_0046aa30->GetState(addressValue));
    } else {
        WriteToMessageLog("\t\t\tADDRESS\t\t%s  %d",
            g_StringTable_0046aa34->GetState(addressType), addressValue);
    }

    if (fromType == 2) {
        WriteToMessageLog("\t\t\tFROM\t\t%s  %s",
            g_StringTable_0046aa34->GetState(fromType),
            g_GameState_0046aa30->GetState(fromValue));
    } else {
        WriteToMessageLog("\t\t\tFROM\t\t%s  %d",
            g_StringTable_0046aa34->GetState(fromType), fromValue);
    }

    WriteToMessageLog("\t\t\tINSTRUCTION\t%s",
        g_StringState_0046aa38->GetState(instruction));

    WriteToMessageLog("\t\t\tMOUSE\t\t%d\t%d", mousePos.x, mousePos.y);

    if (button1 != 0) {
        WriteToMessageLog("\t\t\tBUTTON1\t\t%d", button1);
    }
    if (button2 != 0) {
        WriteToMessageLog("\t\t\tBUTTON2\t\t%d", button2);
    }
    if (lastKey != 0) {
        WriteToMessageLog("\t\t\tLASTKEY\t\t%d", lastKey);
    }
    if (time != 0) {
        WriteToMessageLog("\t\t\tTIME\t\t%lu", time);
    }
    if (extra1 != 0) {
        WriteToMessageLog("\t\t\tEXTRA1\t\t%lu", extra1);
    }
    if (extra2 != 0) {
        WriteToMessageLog("\t\t\tEXTRA2\t\t%lu", extra2);
    }
    WriteToMessageLog("\t\tEND\t\t//message");
}

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
        mousePos.x = other->mousePos.x;
        mousePos.y = other->mousePos.y;
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
    mousePos.x = 0;
    mousePos.y = 0;
    memset(this, 0, sizeof(SpriteAction));
    addressType = p1;
    addressValue = p2;
    fromType = p3;
    fromValue = p4;
    instruction = p5;
    extra1 = p6;
    extra2 = p7;
    childAction = (SpriteAction*)p8;
    mousePos.x = p9;
    mousePos.y = p10;
}

/* Function start: 0x444AF0 */
SpriteAction::~SpriteAction() {
    if (childAction != 0) {
        delete childAction;
        childAction = 0;
    }
}

/* Function start: 0x444D90 */
extern "C" void SendGameMessage(int a, int b, int c, int d, int e, int f, int g, int h, int i, int j) {
    SpriteAction action(a, b, c, d, e, f, g, h, i, j);
    EnqueueSpriteAction((void*)&action);
}

/* Function start: 0x444E20 */
void EnqueueSpriteAction(void* action) {
    g_GameEngine_0046a6ec->EnqueueAction((SpriteAction*)action);
    ((SpriteAction*)action)->childAction = 0;
}

extern void ShowError(const char* format, ...);

/* Function start: 0x444CD0 */
void SpriteAction::Serialize(void* param) {
    int headerLen = strlen("MESSAGE_INFO") + 1;
    FileArchive* ar = (FileArchive*)param;
    FILE* fp = ar->fp;

    if (ar->mode != 0) {
        fwrite("MESSAGE_INFO", headerLen, 1, fp);
        fwrite(this, 0x38, 1, fp);
        return;
    }

    g_Buffer_0046aa00[0] = 0;
    fread(g_Buffer_0046aa00, headerLen, 1, fp);
    if (strcmp(g_Buffer_0046aa00, "MESSAGE_INFO") != 0) {
        ShowError("SC_Message::Serialize() - Error Loading (Wrong ID '%s')", g_Buffer_0046aa00);
    }
    fread(this, 0x38, 1, fp);
}

/* Function start: 0x444E40 */
void EnqueueSpriteAction(SpriteAction* action) {
    g_GameEngine_0046a6ec->EnqueueAction(action);
    action->childAction = 0;
}
