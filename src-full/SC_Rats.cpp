#include "SC_Rats.h"
#include "SC_CombatBase.h"
#include "SC_Question.h"
#include "ZBufferManager.h"
#include "SpriteAction.h"
#include "InvSlotItem.h"
#include "Memory.h"
#include "Sample.h"
#include "SoundList.h"
#include "GameState.h"
#include "Engine.h"
#include "LinkedList.h"
#include "DrawEntry.h"
#include "VBuffer.h"
#include "SoundCommand.h"
#include "RenderEntry.h"
#include "mss.h"
#include "main.h"
#include <string.h>
#include <stdio.h>
#include <new.h>

// Globals for SC_Rats state machine
extern "C" {
    int g_State4Phase = 0;       // g_State4Phase - State4Handler sub-phase
    int g_RatsField_00473e00 = 0; // g_RatsField_00473e00 - unused/reserved
    int g_State0Phase = 0;       // g_State0Phase - State0Handler sub-phase
    int g_RatsState = 0;         // g_RatsState - main state machine state (0-4)
    int g_State3Phase = 0;       // g_State3Phase - State3Handler sub-phase
    int g_State2Phase = 0;       // g_State2Phase - State2Handler sub-phase
    int g_CombatEngine_0046ae78 = 0;
}

// Extern functions
extern "C" int FileExists(const char*);
extern void __fastcall InitCombatScreen(void*);
extern "C" void SendGameMessage(int, int, int, int, int, int, int, int, int, int);
// FUN_00413e70 = Parser::ProcessFile in Parser.cpp
// FUN_00450b10 = EngineB::EngineB — callers updated to use placement new
#include "EngineB.h"
// FUN_0044bac0 = mCNavigator::SetNavParams
#include "mCNavigator.h"
// FUN_00412a50 is Parser::ReportUnknownLabel (thiscall, declared in Parser.h)

// IAT entries

// Scene data globals
class ZBufferManager;
extern ZBufferManager* g_ZBufferManager_0046aa24;
extern "C" GameState* g_GameState_0046aa30;

/* Function start: 0x451700 */
SC_Rats::SC_Rats() {
    int* p = (int*)&actionData;
    p[0] = 0;
    p[1] = 0;
    handlerId = 0x42;
}

/* Function start: 0x4517C0 */
SC_Rats::~SC_Rats() {
    ShutDown(0);
}

/* Function start: 0x451820 */
void SC_Rats::Init(SC_Message* msg) {
    int savedId = handlerId;
    int* p = (int*)&actionData;
    p[0] = 0;
    p[1] = 0;
    handlerId = savedId;
    g_State4Phase = 0;
    g_State3Phase = 0;
    g_State2Phase = 0;
    g_RatsField_00473e00 = 0;
    g_State0Phase = 0;
    g_RatsState = 0;
    CopyCommandData(msg);
    moduleParam = ((int*)msg)[1];
    if (!FileExists("CB_Rats")) {
        ShowLoadingScreen();
    }
    ZBufferManager* zbm = g_ZBufferManager_0046aa24;
    if (zbm->m_state != 1) {
        zbm->m_state = 1;
        ZBQueue* list1 = zbm->m_queueA0;
        if (list1->head != 0) {
            list1->current = list1->head;
            while (list1->head != 0) {
                void* item = list1->Pop();
                if (item != 0) {
                    *(int*)item = 0x461030;
                    FreeMemory(item);
                }
            }
        }
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
                        SoundCommand* sub = item->m_childObject;
                        (*(void (__fastcall **)(void*, int, int))(*(int*)sub))(sub, 0, 1);
                        item->m_childObject = 0;
                    }
                    FreeMemory(item);
                }
            }
        }
        ZBQueue* list3 = zbm->m_queue9c;
        if (list3->head != 0) {
            list3->current = list3->head;
            while (list3->head != 0) {
                void* data = list3->RemoveCurrent();
                if (data != 0) {
                    delete (RenderEntry*)data;
                }
            }
        }
        zbm->m_palette = 0;
    }
    ParseFile(this, "mis\\cb_rats.mis", (char*)0);
    InitCombatScreen((void*)g_CombatEngine_0046ae78);
    if (p[0] == 0) {
        SpriteAction* sprite = new SpriteAction(
            savedCommand, savedMsgData, handlerId, moduleParam, 4, 0, 0, 0, 0, 0);
        p[0] = (int)sprite;
        sprite->extra1 = ((int*)msg)[5];
        sprite->mousePos.x = ((int*)msg)[7];
        sprite->mousePos.y = ((int*)msg)[8];
    }
}

/* Function start: 0x451B30 */
int SC_Rats::ShutDown(SC_Message* msg) {
    if (combatEngine != 0) {
        ((Engine*)combatEngine)->StopAndCleanup();
        if (combatEngine != 0) {
            delete (Handler*)combatEngine;
            combatEngine = 0;
        }
        g_CombatEngine_0046ae78 = 0;
    }
    if (actionData != 0) {
        delete (SpriteAction*)actionData;
        actionData = 0;
    }
    if (msg != 0) {
        SendGameMessage(1, handlerId, handlerId, moduleParam, 0x18, 0, 0, 0, 0, 0);
    }
    return 0;
}

/* Function start: 0x451BC0 */
void SC_Rats::Update(int param1, int param2) {
    if (handlerId != param2) {
        return;
    }
    switch (g_RatsState) {
        case 0:
            State0Handler();
            break;
        case 1:
            State1Handler();
            break;
        case 2:
            State2Handler();
            break;
        case 3:
            State3Handler();
            break;
        case 4:
            State4Handler();
            break;
    }
}

/* Function start: 0x451C20 */
int SC_Rats::AddMessage(SC_Message* msg) {
    ((int*)msg)[2] = handlerId;
    ((int*)msg)[3] = moduleParam;
    ((int*)msg)[4] = 0;
    if (((int*)msg)[11] == 0x1B) {
        g_RatsState = 4;
    }
    return 1;
}

/* Function start: 0x451C60 */
int SC_Rats::Exit(SC_Message* msg) {
    if (handlerId != ((int*)msg)[0]) {
        return 0;
    }
    int cmd = ((int*)msg)[4];
    if (cmd != 0) {
        if (cmd != 0x17) {
            return 0;
        }
        ShowError("SCMI_INSERT");
    }
    return 1;
}

/* Function start: 0x451CA0 */
void SC_Rats::ProcessState() {
    int* sceneData;
    int idx;

    if (savedCommand == 0x2B) {
        if (g_RatsState == 2) {
            if (actionData != 0) {
                delete (SpriteAction*)actionData;
                actionData = 0;
            }
            actionData = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            SC_Message temp;
            ParseFile(&temp, "mis\\cb_rats.mis", "_WIN_LBL_PR_");
        } else if (g_RatsState == 3) {
            if (actionData != 0) {
                delete (SpriteAction*)actionData;
                actionData = 0;
            }
            actionData = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            SC_Message temp;
            ParseFile(&temp, "mis\\cb_rats.mis", "_LOSE_LBL_PR_");
        }
    } else {
        if (g_RatsState == 4) {
            int* spriteData = (int*)actionData;
            spriteData[8] = 2 - spriteData[8];
            GameState* gs = g_GameState_0046aa30;
            idx = ((GameState*)gs)->FindLabel("NUM_ACTIONS");
            if (idx < 0 || gs->maxStates - 1 < idx) {
                ShowError("Invalid gamestate %d", idx);
            }
            gs->stateValues[idx] += 0x14;
        } else if (g_RatsState == 3) {
            if (actionData != 0) {
                delete (SpriteAction*)actionData;
                actionData = 0;
            }
            actionData = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            SC_Message temp;
            ParseFile(&temp, "mis\\cb_rats.mis", "_LOSE_LBL_");
        } else if (g_RatsState == 2) {
            GameState* gs = g_GameState_0046aa30;
            idx = ((GameState*)gs)->FindLabel("RAT_COMBAT_AVAILABLE");
            if (idx < 0 || gs->maxStates - 1 < idx) {
                ShowError("Invalid gamestate %d", idx);
            }
            gs->stateValues[idx] = 0;
            gs = g_GameState_0046aa30;
            idx = ((GameState*)gs)->FindLabel("NUM_ACTIONS");
            if (idx < 0 || gs->maxStates - 1 < idx) {
                ShowError("Invalid gamestate %d", idx);
            }
            gs->stateValues[idx] += 0x1E;
        }
    }
    EnqueueSpriteAction((void*)actionData);
    if (actionData != 0) {
        delete (SpriteAction*)actionData;
        actionData = 0;
    }
}

/* Function start: 0x4520E0 */
void SC_Rats::State0Handler() {
    int engineObj = g_CombatEngine_0046ae78;
    Sample* snd = (Sample*)*(int*)(engineObj + 0x118);

    State1Handler();

    if (g_State0Phase == 0) {
        g_State0Phase++;
        if (*(int*)(engineObj + 0x100) != 0) {
            ((SoundList*)*(int*)(engineObj + 0x100))->StopAll();
        }
        SendGameMessage(5, 0x3E9, handlerId, moduleParam, 0x1B, 0, 0, 0, 0, 0);
        if (snd != 0) {
            snd->Play(100, 1);
            goto done;
        }
    } else {
        if (g_State0Phase != 1) goto done;
        if (snd != 0 && snd->m_sample != 0 && snd->m_size == *(int*)((char*)snd->m_sample + 0xC)) {
            if (AIL_sample_status(snd->m_sample) == 4) goto done;
        }
    }
    g_State0Phase++;

done:
    if (g_State0Phase == 2) {
        g_State0Phase = 3;
        g_RatsState = 1;
        g_Navigator_0046ae70->SetNavParams(1, 0);
    }
}

/* Function start: 0x4521B0 */
void SC_Rats::State1Handler() {
    ((SC_CombatBase*)g_CombatEngine_0046ae78)->StopAndCleanup();
}

/* Function start: 0x4521C0 */
void SC_Rats::State2Handler() {
    int engineObj = g_CombatEngine_0046ae78;
    Sample* snd = (Sample*)*(int*)(engineObj + 0x110);

    if (g_State2Phase == 0) {
        g_State2Phase = 1;
        if (*(int*)(engineObj + 0x100) != 0) {
            ((SoundList*)*(int*)(engineObj + 0x100))->StopAll();
        }
        if (snd != 0) {
            snd->Play(100, 1);
            goto done;
        }
    } else {
        if (g_State2Phase != 1) goto done;
        if (snd != 0 && snd->m_sample != 0 && snd->m_size == *(int*)((char*)snd->m_sample + 0xC)) {
            if (AIL_sample_status(snd->m_sample) == 4) goto done;
        }
    }
    g_State2Phase++;

done:
    if (g_State2Phase == 2) {
        g_State2Phase = 3;
        ProcessState();
    }
    State1Handler();
}

/* Function start: 0x452250 */
void SC_Rats::State3Handler() {
    int engineObj = g_CombatEngine_0046ae78;
    Sample* snd = (Sample*)*(int*)(engineObj + 0x10C);

    if (g_State3Phase == 0) {
        g_State3Phase = 1;
        if (*(int*)(engineObj + 0x100) != 0) {
            ((SoundList*)*(int*)(engineObj + 0x100))->StopAll();
        }
        if (snd != 0) {
            snd->Play(100, 1);
            goto done;
        }
    } else {
        if (g_State3Phase != 1) goto done;
        if (snd != 0 && snd->m_sample != 0 && snd->m_size == *(int*)((char*)snd->m_sample + 0xC)) {
            if (AIL_sample_status(snd->m_sample) == 4) goto done;
        }
    }
    g_State3Phase++;

done:
    if (g_State3Phase == 2) {
        g_State3Phase = 3;
        ProcessState();
    }
    State1Handler();
}

/* Function start: 0x4522E0 */
void SC_Rats::State4Handler() {
    int engineObj = g_CombatEngine_0046ae78;
    Sample* snd = (Sample*)*(int*)(engineObj + 0x114);

    if (g_State4Phase == 0) {
        g_State4Phase = 1;
        if (*(int*)(engineObj + 0x100) != 0) {
            ((SoundList*)*(int*)(engineObj + 0x100))->StopAll();
        }
        if (snd != 0) {
            snd->Play(100, 1);
            goto done;
        }
    } else {
        if (g_State4Phase != 1) goto done;
        if (snd != 0 && snd->m_sample != 0 && snd->m_size == *(int*)((char*)snd->m_sample + 0xC)) {
            if (AIL_sample_status(snd->m_sample) == 4) goto done;
        }
    }
    g_State4Phase++;

done:
    if (g_State4Phase == 2) {
        g_State4Phase = 3;
        ProcessState();
    }
    State1Handler();
}

/* Function start: 0x452370 */
int SC_Rats::LBLParse(char* param) {
    char buf[32];
    buf[0] = 0;
    sscanf(param, "%s", buf);
    if (strcmp(buf, "DERIVED_ENGINE_INFO") == 0) {
        EngineB* obj = new EngineB();
        combatEngine = (int)obj;
        g_CombatEngine_0046ae78 = (int)obj;
        Parser::ProcessFile((Parser*)obj, this, (char*)0);
        return 0;
    }
    if (strcmp(buf, "CB") == 0) {
        return 1;
    }
    ReportUnknownLabel("SC_Rats");
    return 0;
}
