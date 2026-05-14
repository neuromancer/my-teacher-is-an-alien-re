#include "SCI_SearchScreen.h"
#include "stubs.h"
#include "SpriteAction.h"
#include "Parser.h"
#include "SC_Question.h"
#include "EngineC.h"
#include "Memory.h"
#include "ZBufferManager.h"
#include "RenderEntry.h"
#include "SoundCommand.h"
#include "LinkedList.h"
#include "VBuffer.h"
#include <new.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

extern "C" int FileExists(const char* filename);
extern "C" void ShowLoadingScreen();

extern void ParseSpriteAction(void*, void*);
extern void EnqueueSpriteAction(void*);

/* Function start: 0x40B420 */
SCI_SearchScreen::SCI_SearchScreen()
{
    handlerId = 0x48;
}

/* Function start: 0x40B4B0 */
SCI_SearchScreen::~SCI_SearchScreen()
{
    ShutDown(0);
}

#include "globals.h"
#include "SC_CombatBase.h"
extern "C" void SendGameMessage(int, int, int, int, int, int, int, int, int, int);

/* Function start: 0x40B510 */
void SCI_SearchScreen::Init(SC_MessageParser* msg) {
    if (FileExists("CB_Gauntlet") == 0) {
        ShowLoadingScreen();
    }

    SC_Combat::Init(msg);

    ZBufferManager* zb = g_ZBufferManager_0046aa24;
    if (zb->m_state != 1) {
        zb->m_state = 1;

        ZBQueue* q1 = zb->m_queueA0;
        if (q1->head != 0) {
            q1->current = q1->head;
            while (q1->head != 0) {
                SoundCommand* cmd = (SoundCommand*)q1->Pop();
                if (cmd != 0) {
                    *(int*)cmd = 0x461030;
                    FreeMemory(cmd);
                }
            }
        }

        ZBQueue* q2 = zb->m_queueA4;
        if (q2->head != 0) {
            q2->current = q2->head;
            while (q2->head != 0) {
                int* entry = (int*)q2->Pop();
                if (entry != 0) {
                    if (entry[1] != 0) {
                        delete (void*)entry[1];
                        entry[1] = 0;
                    }
                    if (entry[2] != 0) {
                        (**(void(***)(int))entry[2])(1);
                        entry[2] = 0;
                    }
                    FreeMemory(entry);
                }
            }
        }

        ZBQueue* q3 = zb->m_queue9c;
        if (q3->head != 0) {
            q3->current = q3->head;
            while (q3->head != 0) {
                RenderEntry* entry = (RenderEntry*)q3->RemoveCurrent();
                if (entry != 0) {
                    *(int*)entry = 0x46102c;
                    FreeMemory(entry);
                }
            }
        }

        zb->m_palette = 0;
    }

    strcpy((char*)(combatParams + 5), "mis\\cb_Gauntlet.mis");
    ParseFile(this, (char*)(combatParams + 5), (char*)0);
}

/* Function start: 0x40B780 */
int SCI_SearchScreen::ShutDown(SC_MessageParser* msg) {
    if (g_CombatEngine_0046ae78 != 0) {
        delete g_CombatEngine_0046ae78;
        g_CombatEngine_0046ae78 = 0;
    }
    return SC_Combat::ShutDown(msg);
}

/* Function start: 0x40B7C0 */
void SCI_SearchScreen::Update(int p1, int p2) {
    if (handlerId == p2) {
        SC_Combat::Update(p1, p2);
    }
}

/* Function start: 0x40B7E0 */
int SCI_SearchScreen::AddMessage(SC_MessageParser* msg) {
    int ret = SC_Combat::AddMessage(msg);
    if (ret != 0) {
        return 1;
    }
    SpriteAction* action = (SpriteAction*)msg;
    if (action->lastKey == 0x1B && savedCommand == 0x2B) {
        statusPtr[2] = 1;
    }
    return 1;
}

/* Function start: 0x40B830 */
int SCI_SearchScreen::Exit(SC_MessageParser* msg) {
    SpriteAction* action = (SpriteAction*)msg;
    if (action->addressType != handlerId) {
        return 0;
    }
    SC_Combat::Exit(msg);
    return 1;
}

/* Function start: 0x40B860 */
void SCI_SearchScreen::ProcessLose() {
    if (statusPtr[2] == 0) {
        if (pendingAction != 0) {
            delete pendingAction;
            pendingAction = 0;
        }
        SpriteAction* newAction = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        pendingAction = newAction;

        SC_MessageParser temp;
        temp.targetAddress = (int)newAction;
        if (statusPtr[1] != 0) {
            ParseFile(&temp, "mis\\cb_Gauntlet.mis", "[WIN_MESSAGE]");
        } else if (statusPtr[0] != 0) {
            ParseFile(&temp, "mis\\cb_Gauntlet.mis", "[LOSE_MESSAGE]");
        }

        if (savedCommand == 0x2B) {
            ((int*)pendingAction)[2] = 0x2B;
            ((int*)pendingAction)[3] = savedMsgData;
        }
    }

    SC_Combat::ProcessLose();
}

extern void UpdateCombatEngine();

/* Function start: 0x40BA10 */
void SCI_SearchScreen::ProcessAction(int action, int* data) {
    switch (action) {
    case 0:
        ProcessLose();
        return;
    case 1:
        ProcessLose();
        return;
    case 2:
        ProcessLose();
        return;
    case 3:
        *data = 0;
        SendGameMessage(5, field_0x114, handlerId, moduleParam, 0x1b, 0, 0, 0, 0, 0);
        return;
    case 4:
        UpdateCombatEngine();
        return;
    default:
        ShowError("SC_Gauntlet::Process_Action - invalid Action=%d, value=%d", action, *data);
        return;
    }
}

/* Function start: 0x40BAB0 */
void SCI_SearchScreen::OnProcessStart() {
}

/* Function start: 0x40BAC0 */
void SCI_SearchScreen::OnProcessEnd() {
}

/* Function start: 0x40BAD0 */
int SCI_SearchScreen::LBLParse(char* line) {
    char token[32];
    token[0] = 0;
    sscanf(line, " %s", token);

    if (strcmp(token, "DERIVED_ENGINE_INFO") == 0) {
        EngineC* obj = new EngineC();
        g_CombatEngine_0046ae78 = obj;
        Parser::ProcessFile((Parser*)obj, this, (char*)0);
        return 0;
    }
    if (strcmp(token, "END") == 0) {
        return 1;
    }
    SC_Combat::LBLParse(line);
    return 0;
}

/* Function start: 0x40EFB0 */
extern "C" void ClearGameStateSave() {
    memset(&g_FanField1_00472be0, 0, 16);
}

static int g_SearchScreenRects[36];
static int g_DefaultSearchCoords[12];

/* Function start: 0x44E960 */
void InitSearchScreenRects()
{
    g_SearchScreenRects[1] = 0x147;
    g_SearchScreenRects[2] = 0x205;
    g_SearchScreenRects[3] = 0x16D;
    g_SearchScreenRects[4] = 0x209;
    g_SearchScreenRects[5] = 0x147;
    g_SearchScreenRects[7] = 0x16D;
    g_SearchScreenRects[9] = 0x147;
    g_SearchScreenRects[0] = 0x1DE;
    g_SearchScreenRects[6] = 0x230;
    g_SearchScreenRects[11] = 0x16D;
    g_SearchScreenRects[13] = 0x11C;
    g_SearchScreenRects[14] = 0x205;
    g_SearchScreenRects[16] = 0x209;
    g_SearchScreenRects[15] = 0x143;
    g_SearchScreenRects[17] = 0x11C;
    g_SearchScreenRects[8] = 0x234;
    g_SearchScreenRects[10] = 0x25B;
    g_SearchScreenRects[19] = 0x143;
    g_SearchScreenRects[21] = 0x11C;
    g_SearchScreenRects[12] = 0x1DE;
    g_SearchScreenRects[18] = 0x230;
    g_SearchScreenRects[23] = 0x143;
    g_SearchScreenRects[25] = 0xF1;
    g_SearchScreenRects[26] = 0x205;
    g_SearchScreenRects[28] = 0x209;
    g_SearchScreenRects[27] = 0x118;
    g_SearchScreenRects[29] = 0xF1;
    g_SearchScreenRects[20] = 0x234;
    g_SearchScreenRects[22] = 0x25B;
    g_SearchScreenRects[31] = 0x118;
    g_SearchScreenRects[33] = 0xF1;
    g_SearchScreenRects[24] = 0x1DE;
    g_SearchScreenRects[35] = 0x118;
    g_SearchScreenRects[30] = 0x230;
    g_SearchScreenRects[32] = 0x234;
    g_SearchScreenRects[34] = 0x25B;
}

/* Function start: 0x44EAE0 */
void InitDefaultSearchCoords()
{
    g_DefaultSearchCoords[1] = 0xA1;
    g_DefaultSearchCoords[3] = 0x137;
    g_DefaultSearchCoords[0] = 0x14;
    g_DefaultSearchCoords[2] = 0x46;
    g_DefaultSearchCoords[4] = 0xB2;
    g_DefaultSearchCoords[5] = 0xAC;
    g_DefaultSearchCoords[6] = 0xEF;
    g_DefaultSearchCoords[9] = 0xA1;
    g_DefaultSearchCoords[11] = 0x137;
    g_DefaultSearchCoords[7] = 0x118;
    g_DefaultSearchCoords[8] = 0x15D;
    g_DefaultSearchCoords[10] = 0x18F;
}

static void AtExitHandler() {}

/* Function start: 0x44EB60 */
extern "C" void RegisterAtExitHandler() {
    atexit(AtExitHandler);
}
