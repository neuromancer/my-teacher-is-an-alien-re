#include "SearchScreen.h"
#include "globals.h"
#include "string.h"
#include <stdlib.h>
#include "SC_Question.h"
#include "GameState.h"
#include "Message.h"
#include "MouseControl.h"
#include "ZBufferManager.h"

/* Function start: 0x40FB80 */
SearchScreen::SearchScreen() {
    // Handler base class zeros handlerId through field_9C via memset
    // Timer constructor called implicitly

    // Zero 8 dwords at 0xC0-0xDF
    upScrollLeft = 0;
    upScrollTop = 0;
    upScrollRight = 0;
    upScrollBottom = 0;
    downScrollLeft = 0;
    downScrollTop = 0;
    downScrollRight = 0;
    downScrollBottom = 0;

    // Zero 0x14 dwords (80 bytes) at 0xA0, overwriting timer
    memset(&timer, 0, 0x14 * sizeof(int));

    handlerId = 5;

    // Reset Timer
    timer.Reset();

    // Set rectangle fields
    scrollOffset = 0;
    selectedRow = 0;
    upScrollLeft = 0;
    upScrollTop = 0;
    upScrollRight = 0x27f;
    downScrollLeft = 0;
    downScrollRight = 0x27f;
    upScrollBottom = 0x14;
    downScrollTop = 0x1cb;
    downScrollBottom = 0x1df;
    textX = 10;
    textY = 0x1d;
    rowHeight = 0x1e;

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
        idx = scrollOffset;
        if (idx < scrollOffset + 0xe) {
            do {
                stateStr = g_GameState_00436998->GetState(idx);
                if (stateStr == 0) {
                    sprintf(g_Buffer_00436960, "\"NOLABEL\"");
                } else {
                    stateStr = g_GameState_00436998->GetState(idx);
                    sprintf(g_Buffer_00436960, "%s", stateStr);
                }
                y = ((idx - scrollOffset) + 1) * rowHeight + textY;
                g_ZBufferManager_0043698c->ShowSubtitle(g_Buffer_00436960, textX, y, 10000, 8);

                if (idx > 0 && g_GameState_00436998->maxStates <= idx) {
                    ShowError("GameState Error  #%d", 1);
                }
                sprintf(g_Buffer_00436960, "%d", g_GameState_00436998->stateValues[idx]);
                g_ZBufferManager_0043698c->ShowSubtitle(g_Buffer_00436960, textX + 0x15e, y, 10000, 8);

                int rowIdx = idx - scrollOffset;
                int top = rowIdx * rowHeight + textY;
                int bottom = (rowIdx + 1) * rowHeight + textY;

                if (selectedRow == idx) {
                    g_ZBufferManager_0043698c->DrawRect(0, top, 0x27f, bottom, 0x28, 8, 1);
                }
                idx = idx + 1;
            } while (idx < scrollOffset + 0xe);
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
            stateIdx = selectedRow;
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
        stateIdx = selectedRow;
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
    clickX = msg->clickPos.x;
    if (upScrollLeft > clickX || upScrollRight < clickX ||
        upScrollTop > msg->clickPos.y || upScrollBottom < msg->clickPos.y) {
        inRect = 0;
    } else {
        inRect = 1;
    }
    if (inRect != 0) {
        if (scrollOffset > 0) {
            scrollOffset = scrollOffset - 1;
        }
        if (selectedRow > scrollOffset + 0xd) {
            selectedRow = scrollOffset + 0xd;
            return 1;
        }
        return 1;
    }
    if (downScrollLeft > clickX || downScrollRight < clickX ||
        downScrollTop > msg->clickPos.y || downScrollBottom < msg->clickPos.y) {
        inRect = 0;
    } else {
        inRect = 1;
    }
    if (inRect == 0) {
        rowIdx = (msg->clickPos.y - textY) / rowHeight;
        selectedRow = rowIdx;
        if (rowIdx > 0xd) {
            selectedRow = 0xd;
        }
        if (selectedRow < 0) {
            selectedRow = 0;
        }
        selectedRow = selectedRow + scrollOffset;
        return 1;
    }
    if (scrollOffset + 0xe < g_GameState_00436998->maxStates - 1) {
        scrollOffset = scrollOffset + 1;
    }
    if (selectedRow < scrollOffset) {
        selectedRow = scrollOffset;
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
