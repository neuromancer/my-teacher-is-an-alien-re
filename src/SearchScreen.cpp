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
    int* pUp;
    int* pDown;

    // Zero upScroll rect (4 dwords at 0xC0)
    pUp = &upScroll.left;
    pUp[0] = 0;
    pUp[1] = 0;
    pUp[2] = 0;
    pUp[3] = 0;

    // Zero downScroll rect (4 dwords at 0xD0)
    pDown = &downScroll.left;
    pDown[0] = 0;
    pDown[1] = 0;
    pDown[2] = 0;
    pDown[3] = 0;

    // Zero 0x14 dwords (80 bytes) at 0xA0, overwriting timer
    memset(&timer, 0, 0x14 * sizeof(int));

    handlerId = 5;

    // Reset Timer
    timer.Reset();

    // Set fields
    scrollOffset = 0;
    selectedRow = 0;
    upScroll.left = 0;
    upScroll.top = 0;
    upScroll.right = 0x27f;
    downScroll.left = 0;
    downScroll.right = 0x27f;
    upScroll.bottom = 0x14;
    downScroll.top = 0x1cb;
    downScroll.bottom = 0x1df;
    textX = 10;
    textY = 0x1d;
    rowHeight = 0x1e;

    sprite = new Sprite("elements\\gamestat.smk");
    sprite->flags &= ~2;
    sprite->priority = 0;
}

/* Function start: 0x40FD70 */
SearchScreen::~SearchScreen() {
    if (sprite != 0) {
        delete sprite;
        sprite = 0;
    }
}

/* Function start: 0x40FE40 */
void SearchScreen::Init(SC_Message* msg) {
    SearchScreen::CopyCommandData(msg);
    if (msg != 0) {
        moduleParam = msg->data;
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
    GlyphRect localRect;
    Sprite* spr;
    char* stateStr;
    int y;
    GameState* pGameState;
    int rowIdx;

    if (timer.Update() > 10000) {
        SC_Message_Send(3, handlerId, handlerId, moduleParam, 0x14, 0, 0, 0, 0, 0);
    }

    if (handlerId != param2) {
        return;
    }
    spr = sprite;
    if (spr != 0) {
        spr->Do(spr->loc_x, spr->loc_y, 1.0);
    }
    for (idx = scrollOffset; idx < scrollOffset + 0xe; idx++) {
        stateStr = g_GameState_00436998->GetState(idx);
        if (stateStr != 0) {
            stateStr = g_GameState_00436998->GetState(idx);
            sprintf(g_Buffer_00436960, "%s", stateStr);
        } else {
            sprintf(g_Buffer_00436960, "\"NOLABEL\"");
        }
        y = (idx - scrollOffset + 1) * rowHeight + textY;
        g_ZBufferManager_0043698c->ShowSubtitle(g_Buffer_00436960, textX, y, 10000, 8);

        pGameState = g_GameState_00436998;
        if (idx > 0 && pGameState->maxStates <= idx) {
            ShowError("GameState Error  #%d", 1);
        }
        sprintf(g_Buffer_00436960, "%d", pGameState->stateValues[idx]);
        g_ZBufferManager_0043698c->ShowSubtitle(g_Buffer_00436960, textX + 0x15e, y, 10000, 8);

        rowIdx = idx - scrollOffset;
        localRect.left = 0;
        localRect.right = 0x27f;
        localRect.top = rowIdx * rowHeight + textY;
        localRect.bottom = (rowIdx + 1) * rowHeight + textY;

        if (selectedRow != idx) {
        } else {
            g_ZBufferManager_0043698c->DrawRect(0, localRect.top, 0x27f, localRect.bottom, 0x28, 8, 1);
        }
    }
    g_Mouse_00436978->DrawCursor();
}

/* Function start: 0x4100E0 */
int SearchScreen::AddMessage(SC_Message* msg) {
    int stateIdx;
    GameState* pGameState;
    int inRect;
    int clickX;
    int rowIdx;

    SearchScreen::WriteMessageAddress(msg);

    if (msg->lastKey != 0) {
        switch (msg->lastKey) {
        case 0x44:
            stateIdx = selectedRow;
            pGameState = g_GameState_00436998;
            if (stateIdx > 0 && pGameState->maxStates <= stateIdx) {
                ShowError("GameState Error  #%d", 1);
            }
            pGameState->stateValues[stateIdx]--;
            return 1;
        case 0x49:
            stateIdx = selectedRow;
            pGameState = g_GameState_00436998;
            if (stateIdx > 0 && pGameState->maxStates <= stateIdx) {
                ShowError("GameState Error  #%d", 1);
            }
            pGameState->stateValues[stateIdx]++;
            return 1;
        default:
            return 1;
        }
    }
    if (msg->mouseX <= 1) {
        if (msg->mouseY > 1) {
            msg->priority = 1;
        }
        return 1;
    }
    clickX = msg->clickPos.x;
    if (upScroll.left > clickX || upScroll.right < clickX ||
        upScroll.top > msg->clickPos.y || upScroll.bottom < msg->clickPos.y) {
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
    if (downScroll.left > clickX || downScroll.right < clickX ||
        downScroll.top > msg->clickPos.y || downScroll.bottom < msg->clickPos.y) {
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
    GameState* pGameState;
    int param1;

    if (msg->targetAddress != handlerId) {
        return 0;
    }

    timer.Reset();

    switch (msg->priority) {
    case 0:
        return 1;
    case 1:
        SC_Message_Send(savedCommand, savedMsgData, handlerId, moduleParam, 5, 0, 0, 0, 0, 0);
        return 1;
    case 0xd:
        stateIdx = msg->sourceAddress;
        pGameState = g_GameState_00436998;
        if (stateIdx > 0 && pGameState->maxStates <= stateIdx) {
            ShowError("GameState Error  #%d", 1);
        }
        if (stateIdx > 0 && pGameState->maxStates <= stateIdx) {
            ShowError("GameState Error  #%d", 1);
        }
        if (pGameState->stateValues[stateIdx] == 0) {
            if (stateIdx > 0 && pGameState->maxStates <= stateIdx) {
                ShowError("GameState Error  #%d", 1);
            }
            pGameState->stateValues[stateIdx] = 1;
            return 1;
        }
        if (stateIdx > 0 && pGameState->maxStates <= stateIdx) {
            ShowError("GameState Error  #%d", 1);
        }
        pGameState->stateValues[stateIdx] = 0;
        return 1;
    case 0xe:
        stateIdx = msg->sourceAddress;
        pGameState = g_GameState_00436998;
        if (stateIdx > 0 && pGameState->maxStates <= stateIdx) {
            ShowError("GameState Error  #%d", 1);
        }
        pGameState->stateValues[stateIdx] = 1;
        return 1;
    case 0xf:
        stateIdx = msg->sourceAddress;
        pGameState = g_GameState_00436998;
        if (stateIdx > 0 && pGameState->maxStates <= stateIdx) {
            ShowError("GameState Error  #%d", 1);
        }
        pGameState->stateValues[stateIdx] = 0;
        return 1;
    case 0x10:
        stateIdx = msg->sourceAddress;
        pGameState = g_GameState_00436998;
        if (stateIdx > 0 && pGameState->maxStates <= stateIdx) {
            ShowError("GameState Error  #%d", 1);
        }
        pGameState->stateValues[stateIdx]++;
        return 1;
    case 0x11:
        stateIdx = msg->sourceAddress;
        pGameState = g_GameState_00436998;
        if (stateIdx > 0 && pGameState->maxStates <= stateIdx) {
            ShowError("GameState Error  #%d", 1);
        }
        pGameState->stateValues[stateIdx]--;
        return 1;
    case 0x12:
        param1 = msg->param1;
        pGameState = g_GameState_00436998;
        stateIdx = msg->sourceAddress;
        if (stateIdx > 0 && pGameState->maxStates <= stateIdx) {
            ShowError("GameState Error  #%d", 1);
        }
        pGameState->stateValues[stateIdx] = param1;
        return 1;
    case 0x1b:
        SC_Message_Send(3, handlerId, handlerId, moduleParam, 0x14, 0, 0, 0, 0, 0);
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
    memset(&DAT_0043d130, 0, 16);
}

static void AtExitHandler() {}

/* Function start: 0x410630 */
extern "C" void RegisterAtExitHandler() {
    atexit(AtExitHandler);
}

/* Function start: 0x410640 */
// AtExitHandler is defined above as static - it's the atexit callback
