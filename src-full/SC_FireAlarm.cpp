#include "SC_FireAlarm.h"
#include "Memory.h"
#include "Engine.h"
#include <string.h>

extern "C" void* FUN_00454500(int);
extern "C" void FUN_00454400(void*);
extern "C" void FUN_00413e10(void*, char*, char*, ...);
extern "C" int FUN_00425fa0(char*);
extern "C" void FUN_004265a0();
extern "C" void FUN_00444d90(int, int, int, int, int, int, int, int, int, int);
extern void __fastcall FUN_00444af0(void*);
extern void __fastcall FUN_00425490(void*);
extern void* __fastcall FUN_00425480(void*);
extern void __cdecl FUN_00425a90(int, int);
extern void __cdecl FUN_00425c50(char*, ...);
extern void __fastcall FUN_0041dc10(void*);
extern void __fastcall FUN_0044c740(void*);

extern int DAT_004685ac;

/* Function start: 0x407290 */
SC_FireAlarm::SC_FireAlarm() {
    memset(&field_A8, 0, 0xA8);
    handlerId = 0x40;
}

/* Function start: 0x407470 */
SC_FireAlarm::~SC_FireAlarm() {
    SC_FireAlarm::ShutDown((SC_Message*)0);
}

/* Function start: 0x407590 */
void SC_FireAlarm::Init(SC_Message* msg) {
    int* pmsg = (int*)msg;

    Handler::Init(msg);

    if (FUN_00425fa0("CB_FireAlarm") == 0) {
        FUN_004265a0();
    }

    dim_B4.field_0 = 0x140;
    dim_B4.field_4 = 0xF0;

    FUN_00425a90(0x140, 0xF0);

    moduleParam = pmsg[1];

    void* pal = (void*)FUN_00454500(0xC);
    void* palObj = 0;
    if (pal != 0) {
        palObj = FUN_00425480(pal);
    }
    field_148 = (int)palObj;

    FUN_00413e10(this, "mis\\CB_FireAlarm.mis", (char*)0);

    if (field_A8 == 0) {
        SpriteAction* sprite = new SpriteAction(
            savedCommand, savedMsgData, handlerId, moduleParam, 4, 0, 0, 0, 0, 0);
        field_A8 = (int)sprite;
    }
}

/* Function start: 0x4076E0 */
int SC_FireAlarm::ShutDown(SC_Message* msg) {
    void* p;

    p = (void*)field_C0;
    if (p != 0) {
        FUN_0041dc10(p);
        FreeMemory(p);
        field_C0 = 0;
    }

    p = (void*)field_C4;
    if (p != 0) {
        FUN_0044c740(p);
        FreeMemory(p);
        field_C4 = 0;
    }

    if (DAT_004685ac != 0) {
        int* obj = (int*)DAT_004685ac;
        int* vtbl = (int*)*obj;
        ((void (__fastcall*)(int*, int, int))vtbl[3])(obj, 0, 1);
        DAT_004685ac = 0;
    }

    p = (void*)field_BC;
    if (p != 0) {
        FUN_0044c740(p);
        FreeMemory(p);
        field_BC = 0;
    }

    p = (void*)field_F8;
    if (p != 0) {
        FUN_0044c740(p);
        FreeMemory(p);
        field_F8 = 0;
    }

    p = (void*)field_DC;
    if (p != 0) {
        FUN_0044c740(p);
        FreeMemory(p);
        field_DC = 0;
    }

    p = (void*)field_110;
    if (p != 0) {
        FUN_0044c740(p);
        FreeMemory(p);
        field_110 = 0;
    }

    p = (void*)field_10C;
    if (p != 0) {
        FUN_0044c740(p);
        FreeMemory(p);
        field_10C = 0;
    }

    p = (void*)field_C8;
    if (p != 0) {
        FUN_0044c740(p);
        FreeMemory(p);
        field_C8 = 0;
    }

    p = (void*)field_A8;
    if (p != 0) {
        FUN_00444af0(p);
        FreeMemory(p);
        field_A8 = 0;
    }

    p = (void*)field_148;
    if (p != 0) {
        FUN_00425490(p);
        FreeMemory(p);
        field_148 = 0;
    }

    if (msg != 0) {
        FUN_00444d90(1, handlerId, handlerId, moduleParam, 0x18, 0, 0, 0, 0, 0);
    }

    return 0;
}

/* Function start: 0x407930 */
int SC_FireAlarm::AddMessage(SC_Message* msg) {
    int* pmsg = (int*)msg;

    Handler::AddMessage(msg);

    if (pmsg[11] == 0x1b && savedCommand == 0x2b) {
        field_B0 = 4;
    }

    return 1;
}

/* Function start: 0x407970 */
int SC_FireAlarm::Exit(SC_Message* msg) {
    int* pmsg = (int*)msg;

    if (pmsg[0] != handlerId) {
        return 0;
    }

    switch (pmsg[4]) {
    case 0:
        break;
    case 7:
        FUN_00444d90(1, handlerId, handlerId, moduleParam, 0x18, 0, 0, 0, 0, 0);
        return 1;
    case 0x17:
        FUN_00425c50("SCMI_INSERT");
        break;
    default:
        return 0;
    }

    return 1;
}
