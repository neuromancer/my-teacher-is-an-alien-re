#include "SC_ExtBridge.h"
#include "SC_CombatBase.h"
#include "SC_Question.h"
#include "SpriteAction.h"
#include "InvSlotItem.h"
#include "EngineA.h"
#include "Memory.h"
#include "LinkedList.h"
#include "Palette.h"
#include "mCNavigator.h"
#include "ZBufferManager.h"
#include "DrawEntry.h"
#include "VBuffer.h"
#include "RenderEntry.h"
#include "SoundCommand.h"
#include "Animation.h"
#include <string.h>
#include <stdlib.h>

// FUN_0040b760 = VBuffer sdtor — callers updated
// FUN_00404b80 = LinkedList::GetCurrentData — callers updated
// FUN_00404d70 = ListNode sdtor — callers updated to use delete
#include "globals.h"

#include "string.h"
#include "InputManager.h"
#include "MouseControl.h"
/* Function start: 0x4399E0 */
SC_ExtBridge::SC_ExtBridge() {
    memset(&actionMsg, 0, 0x18);
    handlerId = 0x41;
}

/* Function start: 0x439AC0 */
SC_ExtBridge::~SC_ExtBridge() {
    SpriteAction* am = actionMsg;
    if (am != 0) {
        am->~SpriteAction();
        FreeMemory(am);
        actionMsg = 0;
    }

    Palette* pal = palette;
    if (pal != 0) {
        pal->~Palette();
        FreeMemory(pal);
        palette = 0;
    }

    if (engine != 0) {
        delete engine;
        engine = 0;
    }
}

/* Function start: 0x439BB0 */
void SC_ExtBridge::Init(SC_MessageParser* msg) {
    SpriteAction* action = (SpriteAction*)msg;

    CopyCommandData(msg);

    if (msg != 0) {
        moduleParam = action->addressValue;
    }

    if (engine == 0) {
        engine = new EngineA();
        g_CombatEngine_0046ae78 = engine;
        ParseFile(this, "mis\\cb_ducts.mis", (char*)0);
    }

    g_CombatEngine_0046ae78 = engine;
    engine->SetupViewport();

    if (msg != 0 && action->extra1 == 1) {
        g_Navigator_0046ae70->SetNavParams(action->mousePos.x, action->mousePos.y);
    }

    SetVideoRes(dim.x, dim.y);

    ZBufferManager* zbm = g_ZBufferManager_0046aa24;
    if (zbm->m_state != 2) {
        zbm->m_state = 2;

        // First list cleanup (offset 0xA0)
        ZBQueue* list1 = zbm->m_queueA0;
        if (list1->head != 0) {
            list1->current = list1->head;
            while (list1->head != 0) {
                void* obj = list1->Pop();
                if (obj != 0) {
                    delete (SoundCommand*)obj;
                }
            }
        }

        // Second list cleanup (offset 0xA4)
        ZBQueue* list2 = zbm->m_queueA4;
        if (list2->head != 0) {
            list2->current = list2->head;
            while (list2->head != 0) {
                DrawEntry* item = (DrawEntry*)list2->Pop();
                if (item != 0) {
                    if (item->m_videoBuffer != 0) {
                        delete item->m_videoBuffer;
                        item->m_videoBuffer = 0;
                    }
                    if (item->m_childObject != 0) {
                        delete (Animation*)item->m_childObject;
                        item->m_childObject = 0;
                    }
                    operator delete(item);
                }
            }
        }

        // Third list cleanup (offset 0x9C)
        ZBQueue* list3 = zbm->m_queue9c;
        if (list3->head != 0) {
            list3->current = list3->head;
            while (list3->head != 0) {
                void* data;
                if (list3->current == 0) {
                    data = 0;
                } else {
                    if (list3->head == list3->current) {
                        list3->head = list3->current->next;
                    }
                    if (list3->tail == list3->current) {
                        list3->tail = list3->current->prev;
                    }
                    if (list3->current->prev != 0) {
                        list3->current->prev->next = list3->current->next;
                    }
                    if (list3->current->next != 0) {
                        list3->current->next->prev = list3->current->prev;
                    }
                    data = list3->GetCurrentData();
                    if (list3->current != 0) {
                        delete list3->current;
                        list3->current = 0;
                    }
                    list3->current = list3->head;
                }
                if (data != 0) {
                    ((RenderEntry*)data)->RenderEntry::~RenderEntry();
                    operator delete(data);
                }
            }
        }

        zbm->m_palette = 0;
    }

    // Create SpriteAction
    if (actionMsg == 0) {
        actionMsg = new SpriteAction(
            savedCommand, savedMsgData, handlerId, moduleParam, 4, 0, 0, 0, 0, 0);
    }

    // Palette handling
    if (palette != 0) {
        int* palSlot = (int*)&g_ZBufferManager_0046aa24->m_palette;
        if (*palSlot != 0) {
            WriteToMessageLog("ddouble palette");
        }
        *palSlot = (int)palette;
    }

    // Send message
    SendGameMessage(5, bgSoundId, handlerId, moduleParam, 0x1b, 0, 0, 0, 0, 0);
}

#include "GameEngine.h"
/* Function start: 0x439F30 */
void SC_ExtBridge::ShutDown(SC_MessageParser* msg)
{
    SpriteAction* action = new SpriteAction(5, 0x458, handlerId, moduleParam, 0x1b, 0, 0, 0, 0, 0);
    g_GameEngine_0046a6ec->ProcessMessage((SC_MessageParser*)action);
    if (action != 0) {
        delete action;
    }

    if (g_CombatEngine_0046ae78 != 0) {
        if (g_Navigator_0046ae70 != 0 && g_Navigator_0046ae70->sprite != 0) {
            g_Navigator_0046ae70->sprite->StopAnimationSound();
        }
        SC_CombatBase* eng = g_CombatEngine_0046ae78;
        eng->RenderState();
        g_CombatEngine_0046ae78 = 0;
    }
    return;
}

#include "mCNavigator.h"

/* Function start: 0x43A030 */
void SC_ExtBridge::Update(int p1, int p2)
{
    if (handlerId != p2) {
        return;
    }
    if (g_Navigator_0046ae70->Update() != 0) {
        return;
    }
    Sprite* spr = g_Mouse_0046aa18->m_sprite;
    if (spr == 0) {
        return;
    }
    if (spr->handle == -1) {
        return;
    }
    InputState* pMouse = g_InputManager_0046aa08->pMouse;
    int mouseVal;
    if (pMouse != 0) {
        mouseVal = pMouse->x * 3;
    } else {
        mouseVal = 0;
    }
    int idx = mouseVal / dim.x;
    if (idx >= 0) {
        if (idx > 2) {
            idx = 2;
        }
    } else {
        idx = 0;
    }
    int anim = g_DodgeAnimStates_0046bcd0[idx];
    if (spr != 0) {
        spr->ResetAnimation(anim, 0);
    }
    g_Mouse_0046aa18->DrawCursor();
}

/* Function start: 0x43A0C0 */
int SC_ExtBridge::AddMessage(SC_MessageParser* msg)
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
        {
            int mp = moduleParam;
            int hid = handlerId;
            SpriteAction local(
                hid, mp, hid, mp, 4, 1, 0, 0,
                g_Navigator_0046ae70->startingNode, g_Navigator_0046ae70->bearing);
            g_PendingAction_00472d58.CopyFrom(&local);
        }
        SendGameMessage(0x2d, 1, handlerId, moduleParam, 4, 0, 0, 0, 0, 0);
    }

    return 1;
}

/* Function start: 0x43A1D0 */
int SC_ExtBridge::Exit(SC_MessageParser* msg)
{
    SpriteAction* action = (SpriteAction*)msg;
    if (handlerId != action->addressType) {
        return 0;
    }
    int cmd = action->instruction;
    if (cmd != 0) {
        if (cmd != 0x17) {
            return 0;
        }
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
            delete actionMsg;
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
        palette->LoadFile(name);
    } else if (strcmp(label, "NAVIGATION") == 0) {
        Parser::ProcessFile((Parser*)g_Navigator_0046ae70, this, (char*)0);
    } else if (strcmp(label, "SET_WORKBUFF") == 0) {
        sscanf(line, " %s %d %d", label, &dim.x, &dim.y);
    } else if (strcmp(label, "BG_SOUND") == 0) {
        sscanf(line, " %s %d", label, &bgSoundId);
    } else if (strcmp(label, "END") == 0) {
        return 1;
    } else {
        Parser::ReportUnknownLabel("SC_DuctNav");
    }

    return 0;
}
