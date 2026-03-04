#include "SC_DemoScreen.h"
#include "Sprite.h"
#include "Palette.h"
#include "Memory.h"

extern "C" int FUN_00444d90(int, int, int, int, int, int, int, int, int, int);
extern void __cdecl FUN_00425d70(char* msg, ...);
extern void __fastcall FUN_00432da0(void* self);

extern void* DAT_0046aa24;
extern void* DAT_0046aa18;

/* Function start: 0x44E5C0 */
SC_DemoScreen::SC_DemoScreen() {
    field_A8 = 0;
    field_AC = 0;
    handlerId = 0x29;
    moduleParam = 1;

    Palette* pal = new Palette();
    field_A8 = (int)pal;
    pal->Load("elements\\Demoscrn.col");

    field_AC = (int)new Sprite("elements\\Demoscrn.smk");
    ((Sprite*)field_AC)->priority &= ~2;
    ((Sprite*)field_AC)->handle = 0;
    ((Sprite*)field_AC)->num_states = 0;
    ((Sprite*)field_AC)->field_0xb0 = 0;
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
        int* palSlot = (int*)((char*)DAT_0046aa24 + 0xa8);
        if (*palSlot != 0) {
            FUN_00425d70("ddouble palette");
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
    return FUN_00444d90(1, handlerId, handlerId, moduleParam, 0x18, 0, 0, 0, 0, 0);
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
        spr->Do(spr->num_states, spr->field_0xb0, 1.0);
        FUN_00432da0(DAT_0046aa18);
    }
}
