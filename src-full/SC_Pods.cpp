#include "SC_Pods.h"
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
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "SlimeTable.h"
#include "globals.h"

extern "C" int FileExists(const char*);
extern "C" void SendGameMessage(int, int, int, int, int, int, int, int, int, int);
void __fastcall InitCombatScreen(void* self);


// FUN_004061e0 = RenderEntry::~RenderEntry — callers updated
#include "RenderEntry.h"
#include "PodsEngine.h"

extern "C" int g_CombatEngine_0046ae78;                      // active combat engine instance
extern int DAT_004734a4;

/* Function start: 0x4415E0 */
SC_Pods::SC_Pods() {
    memset(&handlerId, 0, 24);
    resultAction = 0;
    combatEngine = 0;
    handlerId = 0x44;
}

/* Function start: 0x4416A0 */
SC_Pods::~SC_Pods() {
    SC_Pods::ShutDown((SC_Message*)0);
}

/* Function start: 0x441700 */
void SC_Pods::Init(SC_Message* msg) {
    CopyCommandData(msg);
    moduleParam = ((int*)msg)[1];

    if (FileExists("CB_Pods") == 0) {
        ShowLoadingScreen();
    }

    ZBufferManager* zbm = g_ZBufferManager_0046aa24;
    if (zbm->m_state != 1) {
        zbm->m_state = 1;

        // Clear command queue
        int* list1 = (int*)zbm->m_queueA0;
        if (*list1 != 0) {
            list1[2] = *list1;
            while (*list1 != 0) {
                void* obj = ((LinkedList*)list1)->RemoveCurrent();
                if (obj != 0) {
                    free(obj);
                }
            }
        }

        // Clear draw entry queue
        int* list2 = (int*)zbm->m_queueA4;
        if (*list2 != 0) {
            list2[2] = *list2;
            while (*list2 != 0) {
                void* item = ((LinkedList*)list2)->RemoveCurrent();
                if (item != 0) {
                    ((ZBuffer*)item)->CleanUpVBuffer();
                    free(item);
                }
            }
        }

        // Clear sound queue
        int* list3 = (int*)zbm->m_queue9c;
        if (*list3 != 0) {
            list3[2] = *list3;
            while (*list3 != 0) {
                void* item = ((LinkedList*)list3)->RemoveCurrent();
                if (item != 0) {
                    ((RenderEntry*)item)->~RenderEntry();
                    free(item);
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
    DAT_0046bf28 = soundTable;
    soundTable->Allocate(5);

    ParseFile(this, "mis\\cb_Pods.mis", (char*)0);
    InitCombatScreen((void*)g_CombatEngine_0046ae78);

    // Create result action
    if (resultAction == 0) {
        SpriteAction* action = new SpriteAction(
            savedCommand, savedMsgData, handlerId, moduleParam, 4, 0, 0, 0, 0, 0);
        resultAction = (int)action;
    }
}

/* Function start: 0x4419E0 */
int SC_Pods::ShutDown(SC_Message* msg) {
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

    if (DAT_0046bf28 != 0) {
        DAT_0046bf28->~SlimeTable();
        FreeMemory(DAT_0046bf28);
        DAT_0046bf28 = 0;
    }

    if (msg != 0) {
        SendGameMessage(1, handlerId, handlerId, moduleParam, 0x18, 0, 0, 0, 0, 0);
    }

    return 0;
}

/* Function start: 0x441AE0 */
int SC_Pods::AddMessage(SC_Message* msg) {
    msg->command = handlerId;
    msg->priority = 0;
    msg->data = moduleParam;

    if (msg->mouseX == 0x1b && savedCommand == 0x2b) {
        ((SC_CombatBase*)g_CombatEngine_0046ae78)->combatFlags |= 4;
    }

    return 1;
}

/* Function start: 0x441B60 */
void SC_Pods::HandleResult() {
    int flags = ((SC_CombatBase*)g_CombatEngine_0046ae78)->combatFlags;

    if (savedCommand == 0x2b) {
        if (flags & 1) {
            // WIN in practice mode
            if (resultAction != 0) {
                ((SpriteAction*)resultAction)->~SpriteAction();
                FreeMemory((void*)resultAction);
                resultAction = 0;
            }

            SpriteAction* newAction = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            resultAction = (int)newAction;

            {
                SC_Message temp;
                temp.targetAddress = (int)newAction;
                ParseFile(&temp, "mis\\cb_Pods.mis", "[WIN_LBL_PR]");
            }
        } else if (flags & 2) {
            // LOSE in practice mode
            if (resultAction != 0) {
                ((SpriteAction*)resultAction)->~SpriteAction();
                FreeMemory((void*)resultAction);
                resultAction = 0;
            }

            SpriteAction* newAction = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            resultAction = (int)newAction;

            {
                SC_Message temp;
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
            ((SpriteAction*)resultAction)->addressValue = DAT_004734a4;
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

    if (resultAction != 0) {
        ((SpriteAction*)resultAction)->~SpriteAction();
        FreeMemory((void*)resultAction);
        resultAction = 0;
    }
}

/* Function start: 0x442090 */
int SC_Pods::LBLParse(char* line) {
    char label[32];
    int soundId = 0;

    label[0] = 0;
    sscanf(line, " %s ", label);

    if (strcmp(label, "DERIVED_ENGINE_INFO") == 0) {
        PodsEngine* eng = new PodsEngine();
        combatEngine = (int)eng;
        g_CombatEngine_0046ae78 = (int)eng;
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

extern void BlankScreen();

/* Function start: 0x42BE00 */
void __fastcall InitCombatScreen(void* self)
{
    SC_CombatBase* engine = (SC_CombatBase*)self;

    engine->combatFlags = 0;

    DAT_0046ae54->SetDimensions(
        *(int*)((char*)g_WeaponParser_0046ae4c + 0x98),
        *(int*)((char*)g_WeaponParser_0046ae4c + 0x9c));

    {
        Sprite* navSpr = *(Sprite**)((char*)g_Navigator_0046ae70 + 0xa0);
        Animation* anim = navSpr->animation_data;
        int fh = 0;
        if (anim != 0) {
            fh = anim->targetBuffer->height;
        }
        int fw = 0;
        if (anim != 0) {
            fw = anim->targetBuffer->width;
        }
        fh -= DAT_0046ae54->dim.b;
        fw -= DAT_0046ae54->dim.a;
        DAT_0046ae54->SetDimensions2(fw, fh);
    }
    DAT_0046ae54->SetCenter();
    DAT_0046ae54->SetAnchor(
        *(int*)((char*)g_WeaponParser_0046ae4c + 0x90),
        *(int*)((char*)g_WeaponParser_0046ae4c + 0x94));

    *(int*)((int)DAT_0046ae6c + 4) = 100;
    engine->spriteFrameCount = 1;
    engine->frameCount = 1;
    BlankScreen();

    {
        unsigned int palStart = *(unsigned int*)((char*)g_WeaponParser_0046ae4c + 0xa0);
        int palEnd = *(int*)((char*)g_WeaponParser_0046ae4c + 0xa4);
        DAT_0046ae64->SetPalette(palStart, palEnd - palStart + 1);
    }

    if (engine->backgroundSound != 0) {
        engine->backgroundSound->Play(100, 0);
    }
}

// FUN_00440860 = PodsEngine::PodsEngine — implemented in PodsEngine.cpp
