#include "SC_Pods.h"
#include "SpriteAction.h"
#include "Memory.h"
#include "Engine.h"
#include "LinkedList.h"
#include "GameState.h"
#include "main.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>



// FUN_00413e10 = ParseFile in Parser.h
// FUN_00413e70 = Parser::ProcessFile in Parser.cpp
extern "C" int FileExists(const char*);
extern "C" void SendGameMessage(int, int, int, int, int, int, int, int, int, int);
// FUN_0042be00 = InitCombatScreen - implemented below
void __fastcall InitCombatScreen(void* self);

extern void __fastcall FUN_00425490(void*);
extern void* __fastcall FUN_00425480(void*);
#include "SlimeTable.h"
extern void* __fastcall FUN_00403620(void*);
extern void __fastcall FUN_00401c80(void*);
extern void __fastcall FUN_004061e0(void*);
extern void* __fastcall FUN_00440860(void*);

// FUN_00412a50 = Parser::LBLParse in Parser.h

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

    CopyCommandData((SC_Message*)msg);
    moduleParam = pmsg[1];

    if (FileExists("CB_Pods") == 0) {
        ShowLoadingScreen();
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
                void* obj = ((LinkedList*)list1)->RemoveCurrent();
                if (obj != 0) {
                    *(int*)obj = 0x461030;
                    free(obj);
                }
            }
        }

        // Second list cleanup (offset 0xA4)
        int* list2 = *(int**)(iVar2 + 0xa4);
        if (*list2 != 0) {
            list2[2] = *list2;
            while (*list2 != 0) {
                void* item = ((LinkedList*)list2)->RemoveCurrent();
                if (item != 0) {
                    FUN_00401c80(item);
                    free(item);
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
                    free(item);
                }
            }
        }

        *(int*)(iVar2 + 0xa8) = 0;
    }

    if (savedCommand == 0x2b) {
        void* gs = DAT_0046aa30;
        int idx = ((GameState*)gs)->FindLabel("OBJ011");
        if (idx < 0 || *(int*)((int)gs + 0x98) - 1 < idx) {
            ShowError("Invalid gamestate %d", idx);
        }
        *(int*)(*(int*)((int)gs + 0x90) + idx * 4) = 1;

        gs = DAT_0046aa30;
        int idx2 = ((GameState*)gs)->FindLabel("KID");
        if (idx2 < 0 || *(int*)((int)gs + 0x98) - 1 < idx2) {
            ShowError("Invalid gamestate %d", idx2);
        }

        int kidVal = *(int*)(*(int*)((int)gs + 0x90) + idx2 * 4);
        void* gs2 = DAT_0046aa30;

        int periodVal;
        if (kidVal == 1) {
            periodVal = 0x0e;
        } else {
            periodVal = 0x10;
        }

        int idx3 = ((GameState*)gs2)->FindLabel("PERIOD");
        if (idx3 < 0 || *(int*)((int)gs2 + 0x98) - 1 < idx3) {
            ShowError("Invalid gamestate %d", idx3);
        }
        *(int*)(*(int*)((int)gs2 + 0x90) + idx3 * 4) = periodVal;
    }

    // Create palette object
    void* pal = malloc(0xc);
    void* palObj = 0;
    if (pal != 0) {
        palObj = FUN_00425480(pal);
    }
    DAT_0046bf28 = palObj;
    ((SlimeTable*)palObj)->Allocate(5);

    ParseFile(this, "mis\\cb_Pods.mis", (char*)0);
    InitCombatScreen((void*)DAT_0046ae78);

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
        ((Engine*)field_A8[1])->StopAndCleanup();
        if (field_A8[1] != 0) {
            delete (Engine*)field_A8[1];
            field_A8[1] = 0;
        }
        DAT_0046ae78 = 0;
    }

    void* spr = (void*)field_A8[0];
    if (spr != 0) {
        ((SpriteAction*)spr)->~SpriteAction();
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
        SendGameMessage(1, handlerId, handlerId, moduleParam, 0x18, 0, 0, 0, 0, 0);
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
        void* mem = malloc(0x118);
        void* eng = 0;
        if (mem != 0) {
            eng = FUN_00440860(mem);
        }
        field_A8[1] = (int)eng;
        DAT_0046ae78 = (int)eng;
        Parser::ProcessFile((Parser*)eng, this, (char*)0);
    } else if (strcmp(label, "BGSOUND") == 0) {
        sscanf(line, " %s %d ", label, &soundId);
        SendGameMessage(5, soundId, handlerId, moduleParam, 0x1b, 0, 0, 0, 0, 0);
    } else if (strcmp(label, "END") == 0) {
        return 1;
    } else {
        Parser::LBLParse("SC_Pods");
    }

    return 0;
}

extern "C" {
    extern void* DAT_0046ae4c;
    extern void* DAT_0046ae54;
    extern void* DAT_0046ae64;
    extern void* DAT_0046ae6c;
    extern void* DAT_0046ae70;
}
extern void BlankScreen();

class CombatViewport {
public:
    int x; int y;
    void SetClip(int, int);     // 0x445590
    void SetOffset(int, int);   // 0x4455F0
    void Refresh();             // 0x445610
    void SetSize(int, int);     // 0x4455B0
};

class CombatWeapon {
public:
    void SetPriority(unsigned int, int); // 0x0041de20
};

class CombatSprite2 {
public:
    void SetVolume(int, int);   // 0x425100
};

/* Function start: 0x42BE00 */
void __fastcall InitCombatScreen(void* self)
{
    int param_1 = (int)self;
    *(int*)(param_1 + 0xdc) = 0;

    ((CombatViewport*)DAT_0046ae54)->SetClip(
        *(int*)((int)DAT_0046ae4c + 0x98),
        *(int*)((int)DAT_0046ae4c + 0x9c));

    int iVar1 = *(int*)(*(int*)((int)DAT_0046ae70 + 0xa0) + 0xf0);
    int iVar3 = 0;
    if (iVar1 != 0) {
        iVar3 = *(int*)(*(int*)(iVar1 + 0x18) + 0x18);
    }
    int iVar2 = 0;
    if (iVar1 != 0) {
        iVar2 = *(int*)(*(int*)(iVar1 + 0x18) + 0x14);
    }

    ((CombatViewport*)DAT_0046ae54)->SetOffset(
        iVar2 - *(int*)DAT_0046ae54,
        iVar3 - *((int*)DAT_0046ae54 + 1));
    ((CombatViewport*)DAT_0046ae54)->Refresh();
    ((CombatViewport*)DAT_0046ae54)->SetSize(
        *(int*)((int)DAT_0046ae4c + 0x90),
        *(int*)((int)DAT_0046ae4c + 0x94));

    *(int*)((int)DAT_0046ae6c + 4) = 100;
    *(int*)(param_1 + 0xe8) = 1;
    *(int*)(param_1 + 0xe0) = 1;
    BlankScreen();

    ((CombatWeapon*)DAT_0046ae64)->SetPriority(
        *(unsigned int*)((int)DAT_0046ae4c + 0xa0),
        *(int*)((int)DAT_0046ae4c + 0xa4) - *(int*)((int)DAT_0046ae4c + 0xa0) + 1);

    if (*(void**)(param_1 + 0xec) != 0) {
        ((CombatSprite2*)*(void**)(param_1 + 0xec))->SetVolume(100, 0);
    }
}
