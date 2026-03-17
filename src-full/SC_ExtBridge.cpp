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
class ZBufferManager;
extern ZBufferManager* DAT_0046aa24;

/* Function start: 0x4399E0 */
SC_ExtBridge::SC_ExtBridge() {
    memset(&actionMsg, 0, 0x18);
    handlerId = 0x41;
}

/* Function start: 0x439AC0 */
SC_ExtBridge::~SC_ExtBridge() {
    if (actionMsg != 0) {
        actionMsg->~SpriteAction();
        FreeMemory(actionMsg);
        actionMsg = 0;
    }

    if (palette != 0) {
        palette->~Palette();
        FreeMemory(palette);
        palette = 0;
    }

    if (engine != 0) {
        delete (Parser*)engine;
        engine = 0;
    }
}

/* Function start: 0x439BB0 */
void SC_ExtBridge::Init(SC_Message* msg) {
    int* pmsg = (int*)msg;

    CopyCommandData((SC_Message*)msg);

    if (msg != 0) {
        moduleParam = pmsg[1];
    }

    if (engine == 0) {
        EngineA* eng = new EngineA();
        engine = (int)eng;
        DAT_0046ae78 = engine;
        ParseFile(this, "mis\\cb_ducts.mis", (char*)0);
    }

    DAT_0046ae78 = engine;
    ((Engine*)engine)->CopyToGlobals();

    if (msg != 0 && pmsg[5] == 1) {
        DAT_0046ae70->SetNavParams(pmsg[7], pmsg[8]);
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
    if (actionMsg == 0) {
        actionMsg = new SpriteAction(
            savedCommand, savedMsgData, handlerId, moduleParam, 4, 0, 0, 0, 0, 0);
    }

    // Palette handling
    if (palette != 0) {
        int* palSlot = (int*)((int)DAT_0046aa24 + 0xa8);
        if (*palSlot != 0) {
            WriteToLog("ddouble palette");
        }
        *palSlot = (int)palette;
    }

    // Send message
    SendGameMessage(5, bgSoundId, handlerId, moduleParam, 0x1b, 0, 0, 0, 0, 0);
}

extern void __fastcall FUN_00431030(void*, int, int*);   // GameEngine::EnqueueAction
extern "C" extern int DAT_0046a6ec;                       // GameEngine instance
extern void __fastcall FUN_0044c9d0(void*);               // Sprite::Destroy
extern void __cdecl FUN_00444e40(void*);                  // SpriteAction::Dispatch
extern void* __fastcall FUN_0041dbf0(void*, int, char*);  // Palette ctor with name
extern void __fastcall FUN_0041dcc0(void*, int, char*);   // Palette::LoadFromFile
extern SpriteAction DAT_00472d58;                          // global SpriteAction

/* Function start: 0x439F30 */
int SC_ExtBridge::ShutDown(SC_Message* msg)
{
    SpriteAction* action = new SpriteAction(5, 0x458, handlerId, moduleParam, 0x1b, 0, 0, 0, 0, 0);
    FUN_00431030((void*)DAT_0046a6ec, 0, (int*)action);
    if (action != 0) {
        action->~SpriteAction();
        FreeMemory(action);
    }

    if (DAT_0046ae78 != 0) {
        mCNavigator* nav = DAT_0046ae70;
        if (nav != 0 && *(int*)((char*)nav + 0xa0) != 0) {
            FUN_0044c9d0(*(void**)((char*)nav + 0xa0));
        }
        int* vtbl = *(int**)DAT_0046ae78;
        ((void (__fastcall*)(int*, int))vtbl[16])((int*)DAT_0046ae78, 0);
        DAT_0046ae78 = 0;
    }
    return 0;
}

/* Function start: 0x43A030 */
void SC_ExtBridge::Update(int p1, int p2)
{
}

/* Function start: 0x43A0C0 */
int SC_ExtBridge::AddMessage(SC_Message* msg)
{
    int* pmsg = (int*)msg;
    pmsg[2] = handlerId;
    pmsg[4] = 0;
    pmsg[3] = moduleParam;

    if (savedCommand == 0x2b) {
        if (pmsg[11] == 0x1b) {
            ProcessEscape();
        }
    } else if (pmsg[11] == 0x77) {
        SpriteAction local(
            handlerId, moduleParam, handlerId, moduleParam, 4, 1, 0, 0,
            *(int*)((char*)DAT_0046ae70 + 0x94), *(int*)((char*)DAT_0046ae70 + 0x90));
        DAT_00472d58.CopyFrom(&local);
        SendGameMessage(0x2d, 1, handlerId, moduleParam, 4, 0, 0, 0, 0, 0);
    }

    return 1;
}

/* Function start: 0x43A1D0 */
int SC_ExtBridge::Exit(SC_Message* msg)
{
    return 0;
}

/* Function start: 0x43A210 */
void SC_ExtBridge::ProcessEscape()
{
    if (actionMsg != 0) {
        FUN_00444e40(actionMsg);
        if (actionMsg != 0) {
            actionMsg->~SpriteAction();
            FreeMemory(actionMsg);
            actionMsg = 0;
        }
        return;
    }
    SendGameMessage(savedCommand, savedMsgData, handlerId, moduleParam, 4, 0, 0, 0, 0, 0);
}

/* Function start: 0x43A290 */
int SC_ExtBridge::LBLParse(char* line)
{
    char label[32];
    char name[128];

    label[0] = 0;
    name[0] = 0;
    sscanf(line, "%s", label);

    if (strcmp(label, "PALETTE") == 0) {
        sscanf(line, "%s %s", label, name);
        void* mem = operator new(8);
        Palette* pal = 0;
        if (mem != 0) {
            pal = (Palette*)FUN_0041dbf0(mem, 0, 0);
        }
        palette = pal;
        FUN_0041dcc0(pal, 0, name);
    } else if (strcmp(label, "NAVIGATION") == 0) {
        Parser::ProcessFile((Parser*)DAT_0046ae70, this, (char*)0);
    } else if (strcmp(label, "SET_WORKBUFF") == 0) {
        sscanf(line, " %s %d %d", label, &dim.field_0, &dim.field_4);
    } else if (strcmp(label, "BG_SOUND") == 0) {
        sscanf(line, " %s %d", label, &bgSoundId);
    } else if (strcmp(label, "END") == 0) {
        return 1;
    } else {
        Parser::LBLParse("SC_ExtBridge");
    }

    return 0;
}
