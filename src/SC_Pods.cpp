#include "SC_Pods.h"
#include "stubs.h"
#include "SpriteAction.h"
#include "Memory.h"
#include "Engine.h"
#include "LinkedList.h"
#include "GameState.h"
#include "Viewport.h"
#include "Palette.h"
#include "SC_CombatBase.h"
#include "SC_Question.h"
#include "Sprite.h"
#include "Animation.h"
#include "Sample.h"
#include "main.h"
#include "ZBuffer.h"
#include "ZBufferManager.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

extern void ReportUnknownLabel(const char* name);

extern "C" void ShowError(const char* format, ...);

#include "SlimeTable.h"
#include "globals.h"

extern "C" int FileExists(const char*);
extern "C" void SendGameMessage(int, int, int, int, int, int, int, int, int, int);
void __fastcall InitCombatScreen(void* self);



// FUN_004061e0 = RenderEntry::~RenderEntry — callers updated
#include "RenderEntry.h"
#include "PodsEngine.h"

/* Function start: 0x4415E0 */
SC_Pods::SC_Pods() {
    int* p = &resultAction;
    p[0] = 0;
    p[1] = 0;
    handlerId = 0x44;
}

/* Function start: 0x4416A0 */
SC_Pods::~SC_Pods() {
    SC_Pods::ShutDown((SC_MessageParser*)0);
}

/* Function start: 0x441700 */
void SC_Pods::Init(SC_MessageParser* msg) {
    CopyCommandData(msg);
    moduleParam = ((SpriteAction*)msg)->addressValue;

    if (FileExists("CB_Pods") == 0) {
        ShowLoadingScreen();
    }

    ZBufferManager* zbm = g_ZBufferManager_0046aa24;
    if (zbm->m_state != 1) {
        zbm->m_state = 1;

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

        ZBQueue* list2 = zbm->m_queueA4;
        if (list2->head != 0) {
            list2->current = list2->head;
            while (list2->head != 0) {
                void* item = list2->Pop();
                if (item != 0) {
                    ((ZBuffer*)item)->CleanUpVBuffer();
                    operator delete(item);
                }
            }
        }

        ZBQueue* list3 = zbm->m_queue9c;
        if (list3->head != 0) {
            list3->current = list3->head;
            while (list3->head != 0) {
                void* item = list3->Pop();
                if (item != 0) {
                    ((RenderEntry*)item)->~RenderEntry();
                    operator delete(item);
                }
            }
        }

        zbm->m_palette = 0;
    }

    // Practice mode setup
    if (savedCommand == 0x2b) {
        int idx = g_GameState_0046aa30->FindState("OBJ011");
        if (idx < 0 || g_GameState_0046aa30->maxStates - 1 < idx) {
            ShowError("Invalid gamestate %d", idx);
        }
        g_GameState_0046aa30->stateValues[idx] = 1;

        int kidIdx = g_GameState_0046aa30->FindState("KID");
        if (kidIdx < 0 || g_GameState_0046aa30->maxStates - 1 < kidIdx) {
            ShowError("Invalid gamestate %d", kidIdx);
        }

        int kidVal = g_GameState_0046aa30->stateValues[kidIdx];
        int periodVal = (kidVal == 1) ? 0x0e : 0x10;

        int periodIdx = g_GameState_0046aa30->FindState("PERIOD");
        if (periodIdx < 0 || g_GameState_0046aa30->maxStates - 1 < periodIdx) {
            ShowError("Invalid gamestate %d", periodIdx);
        }
        g_GameState_0046aa30->stateValues[periodIdx] = periodVal;
    }

    // Create sound table
    SlimeTable* soundTable = new SlimeTable();
    g_SlimeTable_0046bf28 = soundTable;
    soundTable->Allocate(5);

    ParseFile(this, "mis\\cb_Pods.mis", (char*)0);
    InitCombatScreen(g_CombatEngine_0046ae78);

    // Create result action
    if (resultAction == 0) {
        SpriteAction* action = new SpriteAction(
            savedCommand, savedMsgData, handlerId, moduleParam, 4, 0, 0, 0, 0, 0);
        resultAction = (int)action;
    }
}

/* Function start: 0x4419E0 */
int SC_Pods::ShutDown(SC_MessageParser* msg) {
    if (combatEngine != 0) {
        ((Engine*)combatEngine)->StopAndCleanup();
        if (combatEngine != 0) {
            delete (Engine*)combatEngine;
            combatEngine = 0;
        }
        g_CombatEngine_0046ae78 = 0;
    }

    if (resultAction != 0) {
        delete (SpriteAction*)resultAction;
        resultAction = 0;
    }

    if (g_SlimeTable_0046bf28 != 0) {
        SlimeTable* st = g_SlimeTable_0046bf28;
        st->~SlimeTable();
        operator delete(st);
        g_SlimeTable_0046bf28 = 0;
    }

    if (msg != 0) {
        SendGameMessage(1, handlerId, handlerId, moduleParam, 0x18, 0, 0, 0, 0, 0);
    }

    return 0;
}

/* Function start: 0x441AA0 */
void SC_Pods::Update(int p1, int p2) {
    if (handlerId == p2) {
        int result = g_CombatEngine_0046ae78->StopAndCleanup();
        if (result || g_CombatEngine_0046ae78->combatFlags) {
            HandleResult();
        }
    }
}

/* Function start: 0x441AE0 */
int SC_Pods::AddMessage(SC_MessageParser* msg) {
    SpriteAction* action = (SpriteAction*)msg;
    action->fromType = handlerId;
    action->instruction = 0;
    action->fromValue = moduleParam;
    if (action->lastKey == 0x1b && savedCommand == 0x2b) {
        g_CombatEngine_0046ae78->combatFlags |= 4;
    }
    return 1;
}

/* Function start: 0x441B20 */
int SC_Pods::Exit(SC_MessageParser* msg) {
    SpriteAction* action = (SpriteAction*)msg;
    if (action->addressType != handlerId) {
        return 0;
    }
    switch (action->instruction) {
    case 0:
        break;
    case 0x17:
        ShowError("SCMI_INSERT");
        break;
    default:
        return 0;
    }
    return 1;
}

/* Function start: 0x441B60 */
void SC_Pods::HandleResult() {
    int flags = g_CombatEngine_0046ae78->combatFlags;

    if (savedCommand == 0x2b) {
        if (flags & 1) {
            // WIN in practice mode
            SpriteAction* ra = (SpriteAction*)resultAction;
            if (ra != 0) {
                ra->~SpriteAction();
                operator delete(ra);
                resultAction = 0;
            }

            SpriteAction* newAction = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            resultAction = (int)newAction;

            {
                SC_MessageParser temp;
                temp.targetAddress = (int)newAction;
                ParseFile(&temp, "mis\\cb_Pods.mis", "[WIN_LBL_PR]");
            }
        } else if (flags & 2) {
            // LOSE in practice mode
            SpriteAction* ra = (SpriteAction*)resultAction;
            if (ra != 0) {
                ra->~SpriteAction();
                operator delete(ra);
                resultAction = 0;
            }

            SpriteAction* newAction = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            resultAction = (int)newAction;

            {
                SC_MessageParser temp;
                temp.targetAddress = (int)newAction;
                ParseFile(&temp, "mis\\cb_Pods.mis", "[LOSE_LBL_PR]");
            }
        }
    } else {
        if (flags & 2) {
            // LOSE in normal mode
            GameState* gs = g_GameState_0046aa30;
            int periodIdx = gs->FindLabel("PERIOD");
            gs->ValidateIndex(periodIdx);

            if (gs->stateValues[periodIdx] == 2) {
                int periodStateIdx = g_PeriodStateIdx_0046cb90;
                GameState* gs2 = g_GameState_0046aa30;
                if (periodStateIdx < 0 || gs2->maxStates - 1 < periodStateIdx) {
                    ShowError("Invalid gamestate %d", periodStateIdx);
                }

                char buf[28];
                sprintf(buf, "%c_AWARE_MIKE", (char)g_PeriodCharTable_0046cb94[gs2->stateValues[periodStateIdx]]);

                gs = g_GameState_0046aa30;
                int idx = gs->FindLabel(buf);
                gs->ValidateIndex(idx);
                gs->stateValues[idx] = 1;

                GameState* gs3 = g_GameState_0046aa30;
                int periodStateIdx2 = g_PeriodStateIdx_0046cb90;
                if (periodStateIdx2 < 0 || gs3->maxStates - 1 < periodStateIdx2) {
                    ShowError("Invalid gamestate %d", periodStateIdx2);
                }

                sprintf(buf, "%c_SAW_MIKE_CAPTURED", (char)g_PeriodCharTable_0046cb94[gs3->stateValues[periodStateIdx2]]);

                gs3 = g_GameState_0046aa30;
                idx = gs3->FindLabel(buf);
                gs3->ValidateIndex(idx);
                gs3->stateValues[idx] = 1;
            }

            ((SpriteAction*)resultAction)->addressType = 0x2c;
            ((SpriteAction*)resultAction)->addressValue = 1;
            ((SpriteAction*)resultAction)->instruction = 4;
        } else if (flags & 1) {
            // WIN in normal mode
            GameState* gs = g_GameState_0046aa30;
            int periodIdx = gs->FindLabel("PERIOD");
            gs->ValidateIndex(periodIdx);
            int periodVal = gs->stateValues[periodIdx];

            if (periodVal == 7) {
                gs = g_GameState_0046aa30;
                int idx = gs->FindLabel("WENDY_ALIEN");
                gs->ValidateIndex(idx);
                gs->stateValues[idx] = 0;
            } else if (periodVal == 0xb) {
                gs = g_GameState_0046aa30;
                int idx = gs->FindLabel("JACK_ALIEN");
                gs->ValidateIndex(idx);
                gs->stateValues[idx] = 0;
            } else if (periodVal == 0xe) {
                gs = g_GameState_0046aa30;
                int idx = gs->FindLabel("STACY_ALIEN");
                gs->ValidateIndex(idx);
                gs->stateValues[idx] = 0;
            } else if (periodVal == 0x10) {
                gs = g_GameState_0046aa30;
                int idx = gs->FindLabel("ORVILLE_ALIEN");
                gs->ValidateIndex(idx);
                gs->stateValues[idx] = 0;
            }

            gs = g_GameState_0046aa30;
            int numActIdx = gs->FindLabel("NUM_ACTIONS");
            gs->ValidateIndex(numActIdx);
            gs->stateValues[numActIdx] += 0x1e;

            ((SpriteAction*)resultAction)->addressType = 3;
            ((SpriteAction*)resultAction)->addressValue = g_GameField_004734a4;
            ((SpriteAction*)resultAction)->fromType = savedCommand;
            ((SpriteAction*)resultAction)->fromValue = 1;
            ((SpriteAction*)resultAction)->instruction = 4;

            gs = g_GameState_0046aa30;
            int rescIdx = gs->FindLabel("COMBAT_POD_RESCUED");
            if (rescIdx < 0 || gs->maxStates - 1 < rescIdx) {
                ShowError("Invalid gamestate %d", rescIdx);
            }
            gs->stateValues[rescIdx] = 1;
        }

        {
            GameState* gs = g_GameState_0046aa30;
            int availIdx = gs->FindLabel("COMBAT_POD_AVAILABLE");
            gs->ValidateIndex(availIdx);
            gs->stateValues[availIdx] = 0;
        }
    }

    EnqueueSpriteAction((SpriteAction*)resultAction);

    {
        SpriteAction* ra = (SpriteAction*)resultAction;
        if (ra != 0) {
            ra->~SpriteAction();
            operator delete(ra);
            resultAction = 0;
        }
    }
}

/* Function start: 0x442090 */
/* Function start: 0x4420B3 */
int SC_Pods::LBLParse(char* line) {
    char label[32];
    int soundId = 0;

    label[0] = 0;
    sscanf(line, " %s ", label);

    if (strcmp(label, "DERIVED_ENGINE_INFO") == 0) {
        PodsEngine* eng = new PodsEngine();
        combatEngine = (int)eng;
        g_CombatEngine_0046ae78 = (SC_CombatBase*)eng;
        Parser::ProcessFile((Parser*)eng, this, (char*)0);
    } else if (strcmp(label, "BGSOUND") == 0) {
        sscanf(line, " %s %d ", label, &soundId);
        SendGameMessage(5, soundId, handlerId, moduleParam, 0x1b, 0, 0, 0, 0, 0);
    } else if (strcmp(label, "END") == 0) {
        return 1;
    } else {
        ReportUnknownLabel("SC_Pods");
    }

    return 0;
}

// FUN_00440860 = PodsEngine::PodsEngine — implemented in PodsEngine.cpp
