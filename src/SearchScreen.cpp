#include "SearchScreen.h"
#include "globals.h"
#include "string.h"
#include <stdlib.h>
#include "SC_Question.h"
#include "GameState.h"
#include "Message.h"
#include "Mouse.h"
#include "ZBufferManager.h"

/* Function start: 0x40FB80 */
SearchScreen::SearchScreen() {
    // Handler base class zeros handlerId through field_9C via memset
    // Timer constructor called implicitly

    // Zero 8 dwords at 0xC0-0xDF
    field_C0 = 0;
    field_C4 = 0;
    field_C8 = 0;
    field_CC = 0;
    field_D0 = 0;
    field_D4 = 0;
    field_D8 = 0;
    field_DC = 0;

    // Zero 0x14 dwords (80 bytes) at 0xA0, overwriting timer
    memset(&timer, 0, 0x14 * sizeof(int));

    handlerId = 5;

    // Reset Timer
    timer.Reset();

    // Set rectangle fields
    field_B8 = 0;
    field_BC = 0;
    field_C0 = 0;
    field_C4 = 0;
    field_C8 = 0x27f;
    field_D0 = 0;
    field_D8 = 0x27f;
    field_CC = 0x14;
    field_D4 = 0x1cb;
    field_DC = 0x1df;
    field_E0 = 10;
    field_E4 = 0x1d;
    field_E8 = 0x1e;

    sprite = new Sprite("elements\\gamestat.smk");
    sprite->flags &= ~2;
    sprite->priority = 0;
}

/* Function start: 0x40FD70 */
SearchScreen::~SearchScreen() {
    if (sprite) {
        delete sprite;
        sprite = 0;
    }
}

/* Function start: 0x40FE40 */
void SearchScreen::Init(SC_Message* msg) {
    SearchScreen::CopyCommandData(msg);
    if (msg != 0) {
        field_8C = msg->data;
    }
}

/* Function start: 0x40FE70 */
int SearchScreen::ShutDown(SC_Message* msg) {
    if (sprite != 0) {
        sprite->StopAnimationSound();
    }
    return 0;
}

/* Function start: 0x40FE90 */
void SearchScreen::Update(int param1, int param2) {
    int idx;
    int local_1c[4];
    Sprite* spr;
    char* stateStr;
    int y;

    local_1c[0] = 0;
    local_1c[1] = 0;
    local_1c[2] = 0;
    local_1c[3] = 0;

    if (timer.Update() > 10000) {
        SC_Message_Send(3, handlerId, handlerId, field_8C, 0x14, 0, 0, 0, 0, 0);
    }

    if (handlerId == param2) {
        spr = sprite;
        if (spr != 0) {
            spr->Do(spr->loc_x, spr->loc_y, 1.0);
        }
        idx = field_B8;
        if (idx < field_B8 + 0xe) {
            do {
                stateStr = g_GameState_00436998->GetState(idx);
                if (stateStr == 0) {
                    sprintf(g_Buffer_00436960, "\"NOLABEL\"");
                } else {
                    stateStr = g_GameState_00436998->GetState(idx);
                    sprintf(g_Buffer_00436960, "%s", stateStr);
                }
                y = ((idx - field_B8) + 1) * field_E8 + field_E4;
                g_ZBufferManager_0043698c->ShowSubtitle(g_Buffer_00436960, field_E0, y, 10000, 8);

                if (idx > 0 && g_GameState_00436998->maxStates <= idx) {
                    ShowError("GameState Error  #%d", 1);
                }
                sprintf(g_Buffer_00436960, "%d", g_GameState_00436998->stateValues[idx]);
                g_ZBufferManager_0043698c->ShowSubtitle(g_Buffer_00436960, field_E0 + 0x15e, y, 10000, 8);

                int rowIdx = idx - field_B8;
                int top = rowIdx * field_E8 + field_E4;
                int bottom = (rowIdx + 1) * field_E8 + field_E4;

                if (field_BC == idx) {
                    g_ZBufferManager_0043698c->DrawRect(0, top, 0x27f, bottom, 0x28, 8, 1);
                }
                idx = idx + 1;
            } while (idx < field_B8 + 0xe);
        }
        g_Mouse_00436978->DrawCursor();
    }
}

/* Function start: 0x4100E0 */
int SearchScreen::AddMessage(SC_Message* msg) {
    int stateIdx;
    GameState* pGameState;
    int inRect;
    int clickX;
    int rowIdx;

    SearchScreen::WriteMessageAddress(msg);

    if (msg->field_b4 != 0) {
        if (msg->field_b4 == 0x44) {
            stateIdx = field_BC;
            pGameState = g_GameState_00436998;
            if (stateIdx > 0 && pGameState->maxStates <= stateIdx) {
                ShowError("GameState Error  #%d", 1);
            }
            pGameState->stateValues[stateIdx]--;
            return 1;
        }
        if (msg->field_b4 != 0x49) {
            return 1;
        }
        stateIdx = field_BC;
        pGameState = g_GameState_00436998;
        if (stateIdx > 0 && pGameState->maxStates <= stateIdx) {
            ShowError("GameState Error  #%d", 1);
        }
        pGameState->stateValues[stateIdx]++;
        return 1;
    }
    if (msg->mouseX <= 1) {
        if (msg->mouseY > 1) {
            msg->priority = 1;
        }
        return 1;
    }
    clickX = msg->clickX;
    if (field_C0 > clickX || field_C8 < clickX ||
        field_C4 > msg->clickY || field_CC < msg->clickY) {
        inRect = 0;
    } else {
        inRect = 1;
    }
    if (inRect != 0) {
        if (field_B8 > 0) {
            field_B8 = field_B8 - 1;
        }
        if (field_BC > field_B8 + 0xd) {
            field_BC = field_B8 + 0xd;
            return 1;
        }
        return 1;
    }
    if (field_D0 > clickX || field_D8 < clickX ||
        field_D4 > msg->clickY || field_DC < msg->clickY) {
        inRect = 0;
    } else {
        inRect = 1;
    }
    if (inRect == 0) {
        rowIdx = (msg->clickY - field_E4) / field_E8;
        field_BC = rowIdx;
        if (rowIdx > 0xd) {
            field_BC = 0xd;
        }
        if (field_BC < 0) {
            field_BC = 0;
        }
        field_BC = field_BC + field_B8;
        return 1;
    }
    if (field_B8 + 0xe < g_GameState_00436998->maxStates - 1) {
        field_B8 = field_B8 + 1;
    }
    if (field_BC < field_B8) {
        field_BC = field_B8;
        return 1;
    }
    return 1;
}

/* Function start: 0x4102E0 */
int SearchScreen::Exit(SC_Message* msg) {
    int stateIdx;

    if (msg->targetAddress != handlerId) {
        return 0;
    }

    timer.Reset();

    switch (msg->priority) {
    case 0:
        return 1;
    case 1:
        SC_Message_Send(field_90, field_94, handlerId, field_8C, 5, 0, 0, 0, 0, 0);
        return 1;
    case 0xd:  // Toggle
        stateIdx = msg->sourceAddress;
        if (stateIdx > 0 && g_GameState_00436998->maxStates <= stateIdx) {
            ShowError("GameState Error  #%d", 1);
        }
        if (stateIdx > 0 && g_GameState_00436998->maxStates <= stateIdx) {
            ShowError("GameState Error  #%d", 1);
        }
        if (g_GameState_00436998->stateValues[stateIdx] == 0) {
            if (stateIdx > 0 && g_GameState_00436998->maxStates <= stateIdx) {
                ShowError("GameState Error  #%d", 1);
            }
            g_GameState_00436998->stateValues[stateIdx] = 1;
            return 1;
        }
        if (stateIdx > 0 && g_GameState_00436998->maxStates <= stateIdx) {
            ShowError("GameState Error  #%d", 1);
        }
        g_GameState_00436998->stateValues[stateIdx] = 0;
        return 1;
    case 0xe:  // Set to 1
        stateIdx = msg->sourceAddress;
        if (stateIdx > 0 && g_GameState_00436998->maxStates <= stateIdx) {
            ShowError("GameState Error  #%d", 1);
        }
        g_GameState_00436998->stateValues[stateIdx] = 1;
        return 1;
    case 0xf:  // Set to 0
        stateIdx = msg->sourceAddress;
        if (stateIdx > 0 && g_GameState_00436998->maxStates <= stateIdx) {
            ShowError("GameState Error  #%d", 1);
        }
        g_GameState_00436998->stateValues[stateIdx] = 0;
        return 1;
    case 0x10:  // Increment
        stateIdx = msg->sourceAddress;
        if (stateIdx > 0 && g_GameState_00436998->maxStates <= stateIdx) {
            ShowError("GameState Error  #%d", 1);
        }
        g_GameState_00436998->stateValues[stateIdx]++;
        return 1;
    case 0x11:  // Decrement
        stateIdx = msg->sourceAddress;
        if (stateIdx > 0 && g_GameState_00436998->maxStates <= stateIdx) {
            ShowError("GameState Error  #%d", 1);
        }
        g_GameState_00436998->stateValues[stateIdx]--;
        return 1;
    case 0x12:  // Set to value
        stateIdx = msg->sourceAddress;
        if (stateIdx > 0 && g_GameState_00436998->maxStates <= stateIdx) {
            ShowError("GameState Error  #%d", 1);
        }
        g_GameState_00436998->stateValues[stateIdx] = msg->param1;
        return 1;
    case 0x1b:  // Exit
        SC_Message_Send(3, handlerId, handlerId, field_8C, 0x14, 0, 0, 0, 0, 0);
        return 1;
    case 0x1c:
        g_GameState_00436998->Serialize(1);
        return 1;
    case 0x1d:
        g_GameState_00436998->Serialize(2);
        return 1;
    default:
        return 0;
    }
}

/* Function start: 0x410610 */
extern "C" void ClearGameStateSave() {
    int* p = &DAT_0043d130;
    p[0] = 0;
    p[1] = 0;
    p[2] = 0;
    p[3] = 0;
}

static void AtExitHandler() {}

/* Function start: 0x410630 */
extern "C" void RegisterAtExitHandler() {
    atexit(AtExitHandler);
}

/* Function start: 0x410640 */
// AtExitHandler is defined above as static - it's the atexit callback
