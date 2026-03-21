#include "SC_DemoScreen.h"
#include "Sprite.h"
#include "Palette.h"
#include "Memory.h"

extern "C" int SendGameMessage(int, int, int, int, int, int, int, int, int, int);
extern "C" void WriteToLog(const char* format, ...);
#include "MouseControl.h"

class ZBufferManager;
extern ZBufferManager* g_ZBufferManager_0046aa24;
class MouseControl;
extern MouseControl* g_Mouse_0046aa18;

/* Function start: 0x44E5C0 */
SC_DemoScreen::SC_DemoScreen() {
    field_A8 = 0;
    field_AC = 0;
    handlerId = 0x29;
    moduleParam = 1;

    field_A8 = new Palette();
    field_A8->Load("elements\\Demoscrn.col");

    field_AC = new Sprite("elements\\Demoscrn.smk");
    field_AC->priority &= ~2;
    field_AC->handle = 0;
    field_AC->loc_x = 0;
    field_AC->loc_y = 0;
}

/* Function start: 0x44E740 */
SC_DemoScreen::~SC_DemoScreen() {
    if (field_AC != 0) {
        delete (Sprite*)field_AC;
        field_AC = 0;
    }
    if (field_A8 != 0) {
        delete (Palette*)field_A8;
        field_A8 = 0;
    }
}

/* Function start: 0x44E7F0 */
void SC_DemoScreen::Init(SC_Message* msg) {
    CopyCommandData(msg);
    Palette* pal = (Palette*)field_A8;
    if (pal != 0) {
        int* palSlot = (int*)((char*)g_ZBufferManager_0046aa24 + 0xa8);
        if (*palSlot != 0) {
            WriteToLog("ddouble palette");
        }
        *palSlot = (int)pal;
    }
}

/* Function start: 0x44E830 */
int SC_DemoScreen::ShutDown(SC_Message* msg) {
    if (field_AC != 0) {
        delete (Sprite*)field_AC;
        field_AC = 0;
    }
    if (field_A8 != 0) {
        delete (Palette*)field_A8;
        field_A8 = 0;
    }
    return SendGameMessage(1, handlerId, handlerId, moduleParam, 0x18, 0, 0, 0, 0, 0);
}

/* Function start: 0x44E8B0 */
int SC_DemoScreen::AddMessage(SC_Message* msg) {
    int* pmsg = (int*)msg;
    WriteMessageAddress(msg);
    if (pmsg[9] >= 2) {
        pmsg[0] = 0x25;
        pmsg[1] = 1;
        pmsg[3] = 1;
        pmsg[2] = 0x29;
        pmsg[4] = 4;
    }
    return 1;
}

/* Function start: 0x44E8F0 */
int SC_DemoScreen::Exit(SC_Message* msg) {
    int* pmsg = (int*)msg;
    return (handlerId == pmsg[0]);
}

/* Function start: 0x44E910 */
void SC_DemoScreen::Update(int param1, int param2) {
    if (handlerId == param2) {
        Sprite* spr = (Sprite*)field_AC;
        spr->Do(spr->loc_x, spr->loc_y, 1.0);
        (g_Mouse_0046aa18)->DrawCursor();
    }
}
