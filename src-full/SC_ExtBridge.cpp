#include "SC_ExtBridge.h"
#include "SpriteAction.h"
#include "InvSlotItem.h"
#include "EngineA.h"
#include "Engine.h"
#include "Memory.h"
#include "LinkedList.h"
#include "Palette.h"
#include <string.h>
#include <stdlib.h>

// FUN_00413e10 = ParseFile in Parser.h
extern "C" void SetVideoRes(int, int);
// FUN_0044bac0 = mCNavigator::SetNavParams in mCNavigator.h
#include "mCNavigator.h"
extern void __fastcall FUN_0040b760(void*, int, int);
extern void* __fastcall FUN_00404b80(void*);
extern void __fastcall FUN_00404d70(void*, int, int);
extern "C" void WriteToLog(const char* format, ...);
extern "C" void SendGameMessage(int, int, int, int, int, int, int, int, int, int);
extern int DAT_0046ae78;
extern int DAT_0046ae70;
class ZBufferManager;
extern ZBufferManager* DAT_0046aa24;

/* Function start: 0x4399E0 */
SC_ExtBridge::SC_ExtBridge() {
    memset(&field_A8, 0, 0x18);
    handlerId = 0x41;
}

/* Function start: 0x439AC0 */
SC_ExtBridge::~SC_ExtBridge() {
    SpriteAction* p1 = (SpriteAction*)field_A8;
    if (p1 != 0) {
        p1->~SpriteAction();
        FreeMemory(p1);
        field_A8 = 0;
    }

    void* p2 = (void*)field_B0;
    if (p2 != 0) {
        ((Palette*)p2)->~Palette();
        FreeMemory(p2);
        field_B0 = 0;
    }

    Parser* p3 = (Parser*)field_AC;
    if (p3 != 0) {
        delete p3;
        field_AC = 0;
    }
}

/* Function start: 0x439BB0 */
void SC_ExtBridge::Init(SC_Message* msg) {
    int* pmsg = (int*)msg;

    CopyCommandData((SC_Message*)msg);

    if (msg != 0) {
        moduleParam = pmsg[1];
    }

    if (field_AC == 0) {
        EngineA* eng = new EngineA();
        field_AC = (int)eng;
        DAT_0046ae78 = (int)eng;
        ParseFile(this, "mis\\cb_ducts.mis", (char*)0);
    }

    Engine* pAC = (Engine*)field_AC;
    DAT_0046ae78 = (int)pAC;
    pAC->CopyToGlobals();

    if (msg != 0 && pmsg[5] == 1) {
        ((mCNavigator*)DAT_0046ae70)->SetNavParams(pmsg[7], pmsg[8]);
    }

    SetVideoRes(dim.field_0, dim.field_4);

    int iVar2 = (int)DAT_0046aa24;
    int* pState = (int*)(iVar2 + 0x98);
    if (*pState != 2) {
        *pState = 2;

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
                int* item = (int*)((LinkedList*)list2)->RemoveCurrent();
                if (item != 0) {
                    if (*(int*)((int)item + 4) != 0) {
                        FUN_0040b760((void*)*(int*)((int)item + 4), 0, 1);
                        *(int*)((int)item + 4) = 0;
                    }
                    if (*(int*)((int)item + 8) != 0) {
                        void* sub = (void*)*(int*)((int)item + 8);
                        (*(void (__fastcall **)(void*, int, int))(*(int*)sub))(sub, 0, 1);
                        *(int*)((int)item + 8) = 0;
                    }
                    free(item);
                }
            }
        }

        // Third list cleanup (offset 0x9C)
        int* list3 = *(int**)(iVar2 + 0x9c);
        if (*list3 != 0) {
            list3[2] = *list3;
            while (*list3 != 0) {
                int* current = (int*)list3[2];
                void* data;
                if (current == 0) {
                    data = 0;
                } else {
                    if (*list3 == (int)current) {
                        *list3 = current[1];
                    }
                    if (list3[1] == (int)current) {
                        list3[1] = *current;
                    }
                    if (*current != 0) {
                        ((int*)*current)[1] = current[1];
                    }
                    if (current[1] != 0) {
                        *(int*)current[1] = *current;
                    }
                    data = FUN_00404b80(list3);
                    if ((void*)list3[2] != 0) {
                        FUN_00404d70((void*)list3[2], 0, 1);
                        list3[2] = 0;
                    }
                    list3[2] = *list3;
                }
                if (data != 0) {
                    *(int*)data = 0x46102c;
                    ((InvSlotItem*)((int)data + 4))->~InvSlotItem();
                    FreeMemory(data);
                }
            }
        }

        *(int*)(iVar2 + 0xa8) = 0;
    }

    // Create SpriteAction
    if (field_A8 == 0) {
        SpriteAction* sprite = new SpriteAction(
            savedCommand, savedMsgData, handlerId, moduleParam, 4, 0, 0, 0, 0, 0);
        field_A8 = (int)sprite;
    }

    // Palette handling
    int edi = field_B0;
    if (edi != 0) {
        int* palSlot = (int*)((int)DAT_0046aa24 + 0xa8);
        if (*palSlot != 0) {
            WriteToLog("ddouble palette");
        }
        *palSlot = edi;
    }

    // Send message
    SendGameMessage(5, field_BC, handlerId, moduleParam, 0x1b, 0, 0, 0, 0, 0);
}
