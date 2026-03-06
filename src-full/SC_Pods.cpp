#include "SC_Pods.h"
#include "SpriteAction.h"
#include "Memory.h"
#include "Engine.h"
#include <string.h>
#include <stdio.h>

extern void __fastcall FUN_004309a0(void*, int, int);
extern "C" void* FUN_00454500(int);
extern "C" void FUN_00454400(void*);
extern "C" void FUN_00413e10(void*, char*, char*, ...);
extern void __cdecl FUN_00413e70(void*, int, char*);
extern "C" int FUN_00425fa0(char*);
extern "C" void FUN_004265a0();
extern "C" void FUN_00444d90(int, int, int, int, int, int, int, int, int, int);
extern void __fastcall FUN_0042bf00(void*);
extern void __fastcall FUN_0042be00(void*);
extern void __fastcall FUN_00444af0(void*);
extern void __fastcall FUN_00425490(void*);
extern void* __fastcall FUN_00425480(void*);
extern void __fastcall FUN_004254a0(void*, int, int);
extern void* __fastcall FUN_00403520(void*);
extern void* __fastcall FUN_004035a0(void*);
extern void* __fastcall FUN_00403620(void*);
extern void __fastcall FUN_00401c80(void*);
extern void __fastcall FUN_004061e0(void*);
extern int __fastcall FUN_00433ae0(void*, int, char*);
extern void __fastcall FUN_00409f20(void*, int);
extern void __cdecl FUN_00425c50(char*, ...);
extern void __cdecl FUN_00444e40(void*);
extern void* __fastcall FUN_00440860(void*);
extern int __cdecl FUN_00454510(char*, char*, ...);
extern int __cdecl FUN_00412a50(void*);

extern int DAT_0046ae78;
extern void* DAT_0046aa24;
extern "C" extern void* DAT_0046aa30;
extern void* DAT_0046bf28;
extern int DAT_004734a4;
extern int DAT_0046cb90;
extern char* DAT_0046cb94;

/* Function start: 0x4415E0 */
SC_Pods::SC_Pods() {
    memset(&handlerId, 0, 24);
    field_A8[0] = 0;
    field_A8[1] = 0;
    handlerId = 0x44;
}

/* Function start: 0x4416A0 */
SC_Pods::~SC_Pods() {
    SC_Pods::ShutDown((SC_Message*)0);
}

/* Function start: 0x441700 */
void SC_Pods::Init(SC_Message* msg) {
    int* pmsg = (int*)msg;

    FUN_004309a0(this, 0, (int)msg);
    moduleParam = pmsg[1];

    if (FUN_00425fa0("CB_Pods") == 0) {
        FUN_004265a0();
    }

    int iVar2 = (int)DAT_0046aa24;
    int* pState = (int*)(iVar2 + 0x98);
    if (*pState != 1) {
        *pState = 1;

        // First list cleanup (offset 0xA0)
        int* list1 = *(int**)(iVar2 + 0xa0);
        if (*list1 != 0) {
            list1[2] = *list1;
            while (*list1 != 0) {
                void* obj = FUN_00403520(list1);
                if (obj != 0) {
                    *(int*)obj = 0x461030;
                    FUN_00454400(obj);
                }
            }
        }

        // Second list cleanup (offset 0xA4)
        int* list2 = *(int**)(iVar2 + 0xa4);
        if (*list2 != 0) {
            list2[2] = *list2;
            while (*list2 != 0) {
                void* item = FUN_004035a0(list2);
                if (item != 0) {
                    FUN_00401c80(item);
                    FUN_00454400(item);
                }
            }
        }

        // Third list cleanup (offset 0x9C)
        int* list3 = *(int**)(iVar2 + 0x9c);
        if (*list3 != 0) {
            list3[2] = *list3;
            while (*list3 != 0) {
                void* item = FUN_00403620(list3);
                if (item != 0) {
                    FUN_004061e0(item);
                    FUN_00454400(item);
                }
            }
        }

        *(int*)(iVar2 + 0xa8) = 0;
    }

    if (savedCommand == 0x2b) {
        void* gs = DAT_0046aa30;
        int idx = FUN_00433ae0(gs, 0, "OBJ011");
        if (idx < 0 || *(int*)((int)gs + 0x98) - 1 < idx) {
            FUN_00425c50("Invalid gamestate %d", idx);
        }
        *(int*)(*(int*)((int)gs + 0x90) + idx * 4) = 1;

        gs = DAT_0046aa30;
        int idx2 = FUN_00433ae0(gs, 0, "KID");
        if (idx2 < 0 || *(int*)((int)gs + 0x98) - 1 < idx2) {
            FUN_00425c50("Invalid gamestate %d", idx2);
        }

        int kidVal = *(int*)(*(int*)((int)gs + 0x90) + idx2 * 4);
        void* gs2 = DAT_0046aa30;

        int periodVal;
        if (kidVal == 1) {
            periodVal = 0x0e;
        } else {
            periodVal = 0x10;
        }

        int idx3 = FUN_00433ae0(gs2, 0, "PERIOD");
        if (idx3 < 0 || *(int*)((int)gs2 + 0x98) - 1 < idx3) {
            FUN_00425c50("Invalid gamestate %d", idx3);
        }
        *(int*)(*(int*)((int)gs2 + 0x90) + idx3 * 4) = periodVal;
    }

    // Create palette object
    void* pal = (void*)FUN_00454500(0xc);
    void* palObj = 0;
    if (pal != 0) {
        palObj = FUN_00425480(pal);
    }
    DAT_0046bf28 = palObj;
    FUN_004254a0(palObj, 0, 5);

    FUN_00413e10(this, "mis\\cb_Pods.mis", (char*)0);
    FUN_0042be00((void*)DAT_0046ae78);

    // Create SpriteAction
    if (field_A8[0] == 0) {
        SpriteAction* sprite = new SpriteAction(
            savedCommand, savedMsgData, handlerId, moduleParam, 4, 0, 0, 0, 0, 0);
        field_A8[0] = (int)sprite;
    }
}

/* Function start: 0x4419E0 */
int SC_Pods::ShutDown(SC_Message* msg) {
    if (field_A8[1] != 0) {
        FUN_0042bf00((void*)field_A8[1]);
        if (field_A8[1] != 0) {
            delete (Engine*)field_A8[1];
            field_A8[1] = 0;
        }
        DAT_0046ae78 = 0;
    }

    void* spr = (void*)field_A8[0];
    if (spr != 0) {
        FUN_00444af0(spr);
        FreeMemory(spr);
        field_A8[0] = 0;
    }

    if (DAT_0046bf28 != 0) {
        void* pal = DAT_0046bf28;
        FUN_00425490(pal);
        FreeMemory(pal);
        DAT_0046bf28 = 0;
    }

    if (msg != 0) {
        FUN_00444d90(1, handlerId, handlerId, moduleParam, 0x18, 0, 0, 0, 0, 0);
    }

    return 0;
}

/* Function start: 0x441AE0 */
int SC_Pods::AddMessage(SC_Message* msg) {
    int* pmsg = (int*)msg;

    pmsg[2] = handlerId;
    pmsg[4] = 0;
    pmsg[3] = moduleParam;

    if (pmsg[11] == 0x1b && savedCommand == 0x2b) {
        *(int*)(DAT_0046ae78 + 0xdc) |= 4;
    }

    return 1;
}

/* Function start: 0x442090 */
int SC_Pods::LBLParse(char* line) {
    char label[32];
    int soundId = 0;

    label[0] = 0;
    sscanf(line, " %s ", label);

    if (strcmp(label, "DERIVED_ENGINE_INFO") == 0) {
        void* mem = FUN_00454500(0x118);
        void* eng = 0;
        if (mem != 0) {
            eng = FUN_00440860(mem);
        }
        field_A8[1] = (int)eng;
        DAT_0046ae78 = (int)eng;
        FUN_00413e70(eng, (int)this, (char*)0);
    } else if (strcmp(label, "BGSOUND") == 0) {
        sscanf(line, " %s %d ", label, &soundId);
        FUN_00444d90(5, soundId, handlerId, moduleParam, 0x1b, 0, 0, 0, 0, 0);
    } else if (strcmp(label, "END") == 0) {
        return 1;
    } else {
        FUN_00412a50(this);
    }

    return 0;
}
