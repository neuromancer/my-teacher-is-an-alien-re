#include "SC_Game.h"
#include "Message.h"
#include "string.h"
#include "ZBufferManager.h"
#include "GameState.h"
#include "GlyphRect.h"
#include <stdio.h>
#include <string.h>

extern "C" extern void* DAT_0046aa30;
extern char* DAT_0046aa00;
extern void* DAT_0046aa24;
extern void* DAT_0046aa18;
extern void __fastcall FUN_00432da0(void* self);

/* Function start: 0x431D20 */
SC_Game::SC_Game() {
    memset(&timer, 0, 0x14 * sizeof(int));

    handlerId = 2;

    timer.Reset();

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
    sprite->priority = 0;
    field_F4 = (int)DAT_0046aa30;
}

/* Function start: 0x431F30 */
SC_Game::~SC_Game() {
    if (sprite != 0) {
        delete sprite;
        sprite = 0;
    }
}

/* Function start: 0x432000 */
void SC_Game::Init(SC_Message* msg) {
    CopyCommandData(msg);
    if (msg != 0) {
        moduleParam = ((int*)msg)[1];
    }
}

/* Function start: 0x432040 */
void SC_Game::Update(int param1, int param2) {
    GlyphRect localRect;

    unsigned int elapsed = timer.Update();
    if (elapsed > 30000) {
        SC_Message_Send(1, handlerId, handlerId, moduleParam, 0x18, 0, 0, 0, 0, 0);
    }

    if (handlerId != param2) {
        return;
    }

    timer.Reset();

    if (sprite != 0) {
        sprite->Do(sprite->loc_x, sprite->loc_y, 1.0);
    }

    int row = scrollOffset;
    if (row < scrollOffset + 0xe) {
        do {
            int stateName = (int)((GameState*)field_F4)->GetState(row);
            if (stateName != 0) {
                stateName = (int)((GameState*)field_F4)->GetState(row);
                sprintf(DAT_0046aa00, "%s", stateName);
                ((ZBufferManager*)DAT_0046aa24)->ShowSubtitle(
                    DAT_0046aa00, textX,
                    (row - scrollOffset + 1) * rowHeight + textY,
                    10000, -1);

                int gs = field_F4;
                if (row < 0 || *(int*)(gs + 0x98) - 1 < row) {
                    ShowError("Invalid gamestate %d", row);
                }
                sprintf(DAT_0046aa00, "%d", *(int*)(*(int*)(gs + 0x90) + row * 4));
                ((ZBufferManager*)DAT_0046aa24)->ShowSubtitle(
                    DAT_0046aa00, textX + 0x15e,
                    (row - scrollOffset + 1) * rowHeight + textY,
                    10000, -1);
            } else {
                sprintf(DAT_0046aa00, "**NOLABEL**");
                ((ZBufferManager*)DAT_0046aa24)->ShowSubtitle(
                    DAT_0046aa00, textX,
                    (row - scrollOffset + 1) * rowHeight + textY,
                    10000, -1);
            }

            localRect.left = 0;
            localRect.right = 0x27f;
            int top = (row - scrollOffset) * rowHeight + textY;
            int bottom = (row - scrollOffset + 1) * rowHeight + textY;
            localRect.top = top;
            localRect.bottom = bottom;
            if (selectedRow == row) {
                ((ZBufferManager*)DAT_0046aa24)->DrawRect(
                    0, top, 0x27f, bottom, 0x28, 0xfb, 2);
            }

            row++;
        } while (row < scrollOffset + 0xe);
    }

    FUN_00432da0(DAT_0046aa18);
}

/* Function start: 0x4322D0 */
int SC_Game::AddMessage(SC_Message* msg) {
    int* msgData = (int*)msg;

    WriteMessageAddress(msg);
    timer.Reset();

    GameState* gs = (GameState*)field_F4;
    int maxStates = gs->maxStates;
    int maxVisible = maxStates - 0xe;

    if (msgData[0xb] != 0) {
        switch (msgData[0xb]) {
        case 0x1b:
            msgData[4] = 1;
            return 1;

        case 0x21:
            scrollOffset -= 0xe;
            if (scrollOffset < 0) {
                scrollOffset = 0;
            }
            if (selectedRow < scrollOffset) {
                selectedRow = scrollOffset;
                return 1;
            }
            if (scrollOffset + 0xd < selectedRow) {
                selectedRow = scrollOffset + 0xd;
                return 1;
            }
            break;

        case 0x22:
            scrollOffset += 0xe;
            if (maxVisible < scrollOffset) {
                scrollOffset = maxVisible;
            }
            if (selectedRow < scrollOffset) {
                selectedRow = scrollOffset;
                return 1;
            }
            if (scrollOffset + 0xd < selectedRow) {
                selectedRow = scrollOffset + 0xd;
                return 1;
            }
            break;

        case 0x23:
            if (maxVisible == scrollOffset) {
                selectedRow = scrollOffset + 0xd;
            }
            scrollOffset = maxVisible;
            if (selectedRow < maxVisible) {
                selectedRow = maxVisible;
                return 1;
            }
            if (maxStates - 1 < selectedRow) {
                selectedRow = maxStates - 1;
                return 1;
            }
            break;

        case 0x24:
            if (scrollOffset == 0) {
                selectedRow = 0;
            }
            scrollOffset = 0;
            if (selectedRow < 0) {
                selectedRow = 0;
                return 1;
            }
            if (0xd < selectedRow) {
                selectedRow = 0xd;
                return 1;
            }
            break;

        case 0x25: {
            int sel = selectedRow;
            if (sel < 0 || maxStates - 1 < sel) {
                ShowError("Invalid gamestate %d", sel);
            }
            gs->stateValues[sel]--;
            return 1;
        }

        case 0x26:
            scrollOffset--;
            if (scrollOffset < 0) {
                scrollOffset = 0;
            }
            if (selectedRow < scrollOffset) {
                selectedRow = scrollOffset;
                return 1;
            }
            if (scrollOffset + 0xd < selectedRow) {
                selectedRow = scrollOffset + 0xd;
                return 1;
            }
            break;

        case 0x27: {
            int sel = selectedRow;
            if (sel < 0 || maxStates - 1 < sel) {
                ShowError("Invalid gamestate %d", sel);
            }
            gs->stateValues[sel]++;
            return 1;
        }

        case 0x28:
            scrollOffset++;
            if (maxVisible < scrollOffset) {
                scrollOffset = maxVisible;
            }
            if (selectedRow < scrollOffset) {
                selectedRow = scrollOffset;
                return 1;
            }
            if (scrollOffset + 0xd < selectedRow) {
                selectedRow = scrollOffset + 0xd;
                return 1;
            }
            break;
        }
    } else {
        if (msgData[9] > 1) {
            int clickX = msgData[7];
            int clickY = msgData[8];
            int inUpScroll;
            if (upScroll.left > clickX || upScroll.right < clickX ||
                upScroll.top > clickY || upScroll.bottom < clickY) {
                inUpScroll = 0;
            } else {
                inUpScroll = 1;
            }
            if (inUpScroll) {
                if (0 < scrollOffset) {
                    scrollOffset--;
                }
                int check = scrollOffset + 0xd;
                if (check < selectedRow) {
                    selectedRow = check;
                    return 1;
                }
            } else {
                int inDownScroll;
                if (downScroll.left > clickX || downScroll.right < clickX ||
                    downScroll.top > clickY || downScroll.bottom < clickY) {
                    inDownScroll = 0;
                } else {
                    inDownScroll = 1;
                }
                if (!inDownScroll) {
                    selectedRow = (clickY - textY) / rowHeight;
                    if (selectedRow > 0xd) {
                        selectedRow = 0xd;
                    }
                    if (selectedRow < 0) {
                        selectedRow = 0;
                    }
                    selectedRow = selectedRow + scrollOffset;
                    return 1;
                }
                if (scrollOffset + 0xe < maxStates - 1) {
                    scrollOffset++;
                }
                if (selectedRow < scrollOffset) {
                    selectedRow = scrollOffset;
                    return 1;
                }
            }
        } else {
            if (1 < msgData[10]) {
                msgData[4] = 1;
            }
        }
    }

    return 1;
}
