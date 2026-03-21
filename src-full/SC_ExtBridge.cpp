#include "SC_ExtBridge.h"
#include "SC_CombatBase.h"
#include "SC_Question.h"
#include "SpriteAction.h"
#include "InvSlotItem.h"
#include "EngineA.h"
#include "Engine.h"
#include "Memory.h"
#include "LinkedList.h"
#include "Palette.h"
#include "mCNavigator.h"
#include "ZBufferManager.h"
#include <string.h>
#include <stdlib.h>

extern "C" void SetVideoRes(int, int);
extern "C" void WriteToLog(const char* format, ...);
extern "C" void SendGameMessage(int, int, int, int, int, int, int, int, int, int);
// FUN_0040b760 = VBuffer sdtor — callers updated
// FUN_00404b80 = LinkedList::GetCurrentData — callers updated
// FUN_00404d70 = ListNode sdtor — callers updated to use delete
extern int DAT_0046ae78;
extern ZBufferManager* g_ZBufferManager_0046aa24;

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
    SpriteAction* action = (SpriteAction*)msg;

    CopyCommandData(msg);

    if (msg != 0) {
        moduleParam = action->addressValue;
    }

    if (engine == 0) {
        EngineA* eng = new EngineA();
        engine = (int)eng;
        DAT_0046ae78 = engine;
        ParseFile(this, "mis\\cb_ducts.mis", (char*)0);
    }

    DAT_0046ae78 = engine;
    ((Engine*)engine)->CopyToGlobals();

    if (msg != 0 && action->extra1 == 1) {
        DAT_0046ae70->SetNavParams(action->mousePos.field_0, action->mousePos.field_4);
    }

    SetVideoRes(dim.field_0, dim.field_4);

    int iVar2 = (int)g_ZBufferManager_0046aa24;
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
                        delete (VBuffer*)*(int*)((int)item + 4);
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
                    data = ((LinkedList*)list3)->GetCurrentData();
                    if ((void*)list3[2] != 0) {
                        delete (ListNode*)list3[2];
                        list3[2] = 0;
                    }
                    list3[2] = *list3;
                }
                if (data != 0) {
                    *(int*)data = 0x46102c;
                    ((Rect*)((int)data + 4))->~Rect();
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
        int* palSlot = (int*)((int)g_ZBufferManager_0046aa24 + 0xa8);
        if (*palSlot != 0) {
            WriteToLog("ddouble palette");
        }
        *palSlot = (int)palette;
    }

    // Send message
    SendGameMessage(5, bgSoundId, handlerId, moduleParam, 0x1b, 0, 0, 0, 0, 0);
}

#include "GameEngine.h"
extern "C" extern int DAT_0046a6ec;                       // GameEngine instance
extern SpriteAction DAT_00472d58;                          // global SpriteAction

/* Function start: 0x439F30 */
int SC_ExtBridge::ShutDown(SC_Message* msg)
{
    SpriteAction* action = new SpriteAction(5, 0x458, handlerId, moduleParam, 0x1b, 0, 0, 0, 0, 0);
    ((GameEngine*)DAT_0046a6ec)->ProcessMessage((SC_Message*)action);
    if (action != 0) {
        action->~SpriteAction();
        FreeMemory(action);
    }

    if (DAT_0046ae78 != 0) {
        if (DAT_0046ae70 != 0 && DAT_0046ae70->sprite != 0) {
            DAT_0046ae70->sprite->StopAnimationSound();
        }
        int* vtbl = *(int**)DAT_0046ae78;
        ((void (__fastcall*)(int*, int))vtbl[16])((int*)DAT_0046ae78, 0);
        DAT_0046ae78 = 0;
    }
    return 0;
}

#include "mCNavigator.h"
extern int DAT_0046bcd0[];
#include "MouseControl.h"
#include "InputManager.h"
extern MouseControl* g_Mouse_0046aa18;
extern InputManager* g_InputManager_0046aa08;

/* Function start: 0x43A030 */
void SC_ExtBridge::Update(int p1, int p2)
{
    Sprite* spr;
    int mouseVal;
    int idx;

    if (handlerId == p2) {
        if (DAT_0046ae70->Update() == 0) {
            spr = *(Sprite**)((char*)g_Mouse_0046aa18 + 0x94);
            if (spr != 0 && *(int*)((char*)spr + 0x98) != -1) {
                int* pMouse = *(int**)((char*)g_InputManager_0046aa08 + 0x1A0);
                if (pMouse == 0) {
                    mouseVal = 0;
                } else {
                    mouseVal = *pMouse * 3;
                }
                idx = mouseVal / *(int*)((char*)this + 0xB4);
                if (idx < 0) {
                    idx = 0;
                } else if (idx > 2) {
                    idx = 2;
                }
                if (spr != 0) {
                    spr->ResetAnimation(DAT_0046bcd0[idx], 0);
                }
                g_Mouse_0046aa18->DrawCursor();
            }
        }
    }
}

/* Function start: 0x43A0C0 */
int SC_ExtBridge::AddMessage(SC_Message* msg)
{
    SpriteAction* action = (SpriteAction*)msg;
    action->fromType = handlerId;
    action->instruction = 0;
    action->fromValue = moduleParam;

    if (savedCommand == 0x2b) {
        if (action->lastKey == 0x1b) {
            ProcessEscape();
        }
    } else if (action->lastKey == 0x77) {
        SpriteAction local(
            handlerId, moduleParam, handlerId, moduleParam, 4, 1, 0, 0,
            DAT_0046ae70->bearing, DAT_0046ae70->startingNode);
        DAT_00472d58.CopyFrom(&local);
        SendGameMessage(0x2d, 1, handlerId, moduleParam, 4, 0, 0, 0, 0, 0);
    }

    return 1;
}

/* Function start: 0x43A1D0 */
int SC_ExtBridge::Exit(SC_Message* msg)
{
    int* msgData = (int*)msg;
    if (msgData[0] != handlerId) {
        return 0;
    }
    int cmd = msgData[4];
    if (cmd == 0) {
        return 1;
    }
    if (cmd == 0x17) {
        ShowError("SCMI_INSERT");
    }
    return 1;
}

/* Function start: 0x43A210 */
void SC_ExtBridge::ProcessEscape()
{
    if (actionMsg != 0) {
        EnqueueSpriteAction(actionMsg);
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
        palette = new Palette();
        palette->Load(name);
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
