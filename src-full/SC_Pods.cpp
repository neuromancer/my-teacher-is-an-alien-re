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


extern void __fastcall FUN_004061e0(void*);   // SoundEntry dtor
#include "PodsEngine.h"

extern int DAT_0046ae78;                      // active combat engine instance
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
                    FUN_004061e0(item);
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
    InitCombatScreen((void*)DAT_0046ae78);

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
        DAT_0046ae78 = 0;
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
        ((SC_CombatBase*)DAT_0046ae78)->combatFlags |= 4;
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
        PodsEngine* eng = new PodsEngine();
        combatEngine = (int)eng;
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

extern void BlankScreen();

/* Function start: 0x42BE00 */
void __fastcall InitCombatScreen(void* self)
{
    SC_CombatBase* engine = (SC_CombatBase*)self;
    Viewport* vp = DAT_0046ae54;
    int* wpData = (int*)DAT_0046ae4c;  // WeaponParser sprite data

    engine->combatFlags = 0;

    // wpData offsets: +0x90=x, +0x94=y, +0x98=width, +0x9c=height, +0xa0=palStart, +0xa4=palEnd
    vp->SetDimensions(*(int*)((char*)wpData + 0x98), *(int*)((char*)wpData + 0x9c));

    Sprite* navSprite = *(Sprite**)((char*)DAT_0046ae70 + 0xa0);
    Animation* anim = navSprite->animation_data;
    int frameHeight = 0;
    if (anim != 0) {
        frameHeight = anim->targetBuffer->height;
    }
    int frameWidth = 0;
    if (anim != 0) {
        frameWidth = anim->targetBuffer->width;
    }

    vp->SetDimensions2(frameWidth - vp->dim.a, frameHeight - vp->dim.b);
    vp->SetCenter();
    vp->SetAnchor(*(int*)((char*)wpData + 0x90), *(int*)((char*)wpData + 0x94));

    *(int*)((int)DAT_0046ae6c + 4) = 100;
    engine->spriteFrameCount = 1;
    engine->frameCount = 1;
    BlankScreen();

    DAT_0046ae64->SetPalette(
        *(unsigned int*)((char*)wpData + 0xa0),
        *(int*)((char*)wpData + 0xa4) - *(int*)((char*)wpData + 0xa0) + 1);

    if (engine->backgroundSound != 0) {
        engine->backgroundSound->Play(100, 0);
    }
}
