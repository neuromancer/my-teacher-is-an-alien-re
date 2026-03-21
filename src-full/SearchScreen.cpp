#include "SearchScreen.h"
#include "globals.h"
#include "string.h"
#include <stdlib.h>
#include "SC_Question.h"
#include "GameState.h"
#include "Message.h"
#include "MouseControl.h"
#include "ZBufferManager.h"

/* Function start: 0x40FB80 */ /* DEMO ONLY - no full game match */
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

/* Function start: 0x431F30 */ /* ~94% match */
SearchScreen::~SearchScreen() {
    if (sprite != 0) {
        delete sprite;
        sprite = 0;
    }
}

/* Function start: 0x40FE40 */ /* DEMO ONLY - no full game match */
void SearchScreen::Init(SC_Message* msg) {
    SearchScreen::CopyCommandData(msg);
    if (msg != 0) {
        moduleParam = msg->data;
    }
}

/* Function start: 0x40FE70 */ /* DEMO ONLY - no full game match */
int SearchScreen::ShutDown(SC_Message* msg) {
    if (sprite != 0) {
        sprite->StopAnimationSound();
    }
    return 0;
}

/* Function start: 0x40FE90 */ /* DEMO ONLY - no full game match */
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
        stateStr = g_GameState_0046aa30->GetState(idx);
        if (stateStr != 0) {
            stateStr = g_GameState_0046aa30->GetState(idx);
            sprintf(g_Buffer_0046aa00, "%s", stateStr);
        } else {
            sprintf(g_Buffer_0046aa00, "\"NOLABEL\"");
        }
        y = (idx - scrollOffset + 1) * rowHeight + textY;
        g_ZBufferManager_0046aa24->ShowSubtitle(g_Buffer_0046aa00, textX, y, 10000, 8);

        pGameState = g_GameState_0046aa30;
        if (idx > 0 && pGameState->maxStates <= idx) {
            ShowError("GameState Error  #%d", 1);
        }
        sprintf(g_Buffer_0046aa00, "%d", pGameState->stateValues[idx]);
        g_ZBufferManager_0046aa24->ShowSubtitle(g_Buffer_0046aa00, textX + 0x15e, y, 10000, 8);

        rowIdx = idx - scrollOffset;
        localRect.left = 0;
        localRect.right = 0x27f;
        localRect.top = rowIdx * rowHeight + textY;
        localRect.bottom = (rowIdx + 1) * rowHeight + textY;

        if (selectedRow != idx) {
        } else {
            g_ZBufferManager_0046aa24->DrawRect(0, localRect.top, 0x27f, localRect.bottom, 0x28, 8, 1);
        }
    }
    g_Mouse_0046aa18->DrawCursor();
}

/* Function start: 0x4100E0 */ /* DEMO ONLY - no full game match */
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
            pGameState = g_GameState_0046aa30;
            if (stateIdx > 0 && pGameState->maxStates <= stateIdx) {
                ShowError("GameState Error  #%d", 1);
            }
            pGameState->stateValues[stateIdx]--;
            return 1;
        case 0x49:
            stateIdx = selectedRow;
            pGameState = g_GameState_0046aa30;
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
    clickX = msg->clickX;
    if (upScroll.left > clickX || upScroll.right < clickX ||
        upScroll.top > msg->clickY || upScroll.bottom < msg->clickY) {
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
        downScroll.top > msg->clickY || downScroll.bottom < msg->clickY) {
        inRect = 0;
    } else {
        inRect = 1;
    }
    if (inRect == 0) {
        rowIdx = (msg->clickY - textY) / rowHeight;
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
    if (scrollOffset + 0xe < g_GameState_0046aa30->maxStates - 1) {
        scrollOffset = scrollOffset + 1;
    }
    if (selectedRow < scrollOffset) {
        selectedRow = scrollOffset;
        return 1;
    }
    return 1;
}

/* Function start: 0x4102E0 */ /* DEMO ONLY - no full game match */
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
        pGameState = g_GameState_0046aa30;
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
        pGameState = g_GameState_0046aa30;
        if (stateIdx > 0 && pGameState->maxStates <= stateIdx) {
            ShowError("GameState Error  #%d", 1);
        }
        pGameState->stateValues[stateIdx] = 1;
        return 1;
    case 0xf:
        stateIdx = msg->sourceAddress;
        pGameState = g_GameState_0046aa30;
        if (stateIdx > 0 && pGameState->maxStates <= stateIdx) {
            ShowError("GameState Error  #%d", 1);
        }
        pGameState->stateValues[stateIdx] = 0;
        return 1;
    case 0x10:
        stateIdx = msg->sourceAddress;
        pGameState = g_GameState_0046aa30;
        if (stateIdx > 0 && pGameState->maxStates <= stateIdx) {
            ShowError("GameState Error  #%d", 1);
        }
        pGameState->stateValues[stateIdx]++;
        return 1;
    case 0x11:
        stateIdx = msg->sourceAddress;
        pGameState = g_GameState_0046aa30;
        if (stateIdx > 0 && pGameState->maxStates <= stateIdx) {
            ShowError("GameState Error  #%d", 1);
        }
        pGameState->stateValues[stateIdx]--;
        return 1;
    case 0x12:
        param1 = msg->param1;
        pGameState = g_GameState_0046aa30;
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
        { int mode = 1; g_GameState_0046aa30->Serialize(&mode); }
        return 1;
    case 0x1d:
        { int mode = 2; g_GameState_0046aa30->Serialize(&mode); }
        return 1;
    default:
        return 0;
    }
}

/* Function start: 0x40EFB0 */
extern "C" void ClearGameStateSave() {
    memset(&DAT_0043d130, 0, 16);
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
    g_DefaultSearchCoords[7] = 0xA1;
    g_DefaultSearchCoords[9] = 0x137;
    g_DefaultSearchCoords[8] = 0x118;
    g_DefaultSearchCoords[10] = 0x15D;
    g_DefaultSearchCoords[11] = 0x18F;
}

static void AtExitHandler() {}

/* Function start: 0x44EB60 */
extern "C" void RegisterAtExitHandler() {
    atexit(AtExitHandler);
}

/* Function start: 0x410640 */ /* DEMO ONLY - no full game match */
// AtExitHandler is defined above as static - it's the atexit callback
