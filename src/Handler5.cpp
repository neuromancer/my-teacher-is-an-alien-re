#include "Handler5.h"
#include "globals.h"
#include "string.h"
#include "SC_Question.h"
#include "GameState.h"
#include "Message.h"
#include "Mouse.h"
#include "ZBufferManager.h"

/* Function start: 0x40FB80 */
Handler5::Handler5() {
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
Handler5::~Handler5() {
    if (sprite) {
        delete sprite;
        sprite = 0;
    }
}

// Stub virtual method implementations
/* Function start: 0x40FE40 */
void Handler5::Init(SC_Message* msg) {
    Handler::Init(msg);
}

/* Function start: 0x4100E0 */
int Handler5::HandleMessage(SC_Message* msg) {
    return 1;
}

/* Function start: 0x40FE70 */
void Handler5::Update(SC_Message* msg) {
    if (sprite != 0) {
        sprite->StopAnimationSound();
    }
}

/* Function start: 0x40FE90 */
void Handler5::Draw(int param1, int param2) {
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
                DAT_0043698c->ShowSubtitle(g_Buffer_00436960, field_E0, y, 10000, 0);
                
                if (idx > 0 && g_GameState_00436998->maxStates <= idx) {
                    ShowError("GameState Error  #%d", 1);
                }
                sprintf(g_Buffer_00436960, "%d", g_GameState_00436998->stateValues[idx]);
                DAT_0043698c->ShowSubtitle(g_Buffer_00436960, field_E0 + 0x15e, y, 10000, 0);
                
                int rowIdx = idx - field_B8;
                int top = rowIdx * field_E8 + field_E4;
                int bottom = (rowIdx + 1) * field_E8 + field_E4;
                
                if (field_BC == idx) {
                    DAT_0043698c->DrawRect(0, top, 0x27f, bottom, 0x28, 8, 1);
                }
                idx = idx + 1;
            } while (idx < field_B8 + 0xe);
        }
        g_Mouse_00436978->DrawCursor();
    }
}

/* Function start: 0x4102E0 */
int Handler5::Exit(SC_Message* msg) {
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


