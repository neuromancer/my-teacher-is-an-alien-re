#include "SC_Game.h"
#include "Message.h"
#include "SpriteAction.h"
#include "string.h"
#include "ZBufferManager.h"
#include "GameState.h"
#include "GlyphRect.h"
#include "Palette.h"
#include "Sprite.h"
#include "Sample.h"
#include "Memory.h"
#include "Parser.h"
#include <stdio.h>
#include <string.h>

extern "C" char* MakeAudioName(char*);
extern Palette* __fastcall InitPalette(Palette*);
#include "MouseControl.h"
#include "globals.h"

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
    gameState = g_GameState_0046aa30;
}

/* Function start: 0x431F30 */
SC_Game::~SC_Game() {
    if (sprite != 0) {
        delete sprite;
        sprite = 0;
    }
}

// 0x432000 = SC_Timer::Init (COMDAT)
void SC_Game::Init(SC_Message* msg) {
    CopyCommandData(msg);
    if (msg != 0) {
        moduleParam = ((SpriteAction*)msg)->addressValue;
    }
}

/* Function start: 0x432040 */
void SC_Game::Update(int param1, int param2) {
    GlyphRect localRect;

    unsigned int elapsed = timer.Update();
    if (elapsed > 30000) {
        SendGameMessage(1, handlerId, handlerId, moduleParam, 0x18, 0, 0, 0, 0, 0);
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
            int stateName = (int)gameState->GetState(row);
            if (stateName != 0) {
                stateName = (int)gameState->GetState(row);
                sprintf(g_Buffer_0046aa00, "%s", stateName);
                (g_ZBufferManager_0046aa24)->ShowSubtitle(
                    g_Buffer_0046aa00, textX,
                    (row - scrollOffset + 1) * rowHeight + textY,
                    10000, -1);

                GameState* gs = gameState;
                if (row < 0 || *(int*)(gs + 0x98) - 1 < row) {
                    ShowError("Invalid gamestate %d", row);
                }
                sprintf(g_Buffer_0046aa00, "%d", *(int*)(*(int*)(gs + 0x90) + row * 4));
                (g_ZBufferManager_0046aa24)->ShowSubtitle(
                    g_Buffer_0046aa00, textX + 0x15e,
                    (row - scrollOffset + 1) * rowHeight + textY,
                    10000, -1);
            } else {
                sprintf(g_Buffer_0046aa00, "**NOLABEL**");
                (g_ZBufferManager_0046aa24)->ShowSubtitle(
                    g_Buffer_0046aa00, textX,
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
                (g_ZBufferManager_0046aa24)->DrawRect(
                    0, top, 0x27f, bottom, 0x28, 0xfb, 2);
            }

            row++;
        } while (row < scrollOffset + 0xe);
    }

    (g_Mouse_0046aa18)->DrawCursor();
}

/* Function start: 0x4322D0 */
int SC_Game::AddMessage(SC_Message* msg) {
    SpriteAction* action = (SpriteAction*)msg;

    WriteMessageAddress(msg);
    timer.Reset();

    GameState* gs = (GameState*)gameState;
    int maxStates = gs->maxStates;
    int maxVisible = maxStates - 0xe;

    if (action->lastKey != 0) {
        switch (action->lastKey) {
        case 0x1b:
            action->instruction = 1;
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
        if (action->button1 > 1) {
            int clickX = action->mousePos.x;
            int clickY = action->mousePos.y;
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
            if (1 < action->button2) {
                action->instruction = 1;
            }
        }
    }

    return 1;
}

extern "C" void ShowError(const char*, ...);

/* Function start: 0x437120 */
int SC_Game::LBLParse(char* line) {
    char keyword[32];
    char buffer[128];
    int val;

    keyword[0] = 0;
    buffer[0] = 0;
    sscanf(line, "%s", keyword);

    if (strcmp(keyword, "SET_GAMESTATE") == 0) {
        sscanf(line, "%s %s %d", keyword, buffer, &val);
        GameState* gs = g_GameState_0046aa30;
        int idx = gs->FindState(buffer);
        if (idx < 0 || gs->maxStates - 1 < idx) {
            ShowError("Invalid gamestate %d", idx);
        }
        gs->stateValues[idx] = val;
    } else if (strcmp(keyword, "PALETTE") == 0) {
        sscanf(line, "%s %s", keyword, buffer);
        if (*(void**)((int)this + 0xBC) != 0) {
            ((Palette*)*(void**)((int)this + 0xBC))->~Palette();
            FreeMemory(*(void**)((int)this + 0xBC));
            *(void**)((int)this + 0xBC) = 0;
        }
        Palette* pal = InitPalette((Palette*)AllocateMemory(8));
        *(Palette**)((int)this + 0xBC) = pal;
        pal->Load(buffer);
    } else if (strcmp(keyword, "BACKGROUND_SPRITE") == 0) {
        Sprite* spr = new Sprite(0);
        *(Sprite**)((int)this + 0xC0) = spr;
        spr->flags &= ~2;
        Parser::ProcessFile(spr, this, 0);
    } else if (strcmp(keyword, "HINT_SPRITE") == 0) {
        Sprite* spr = new Sprite(0);
        *(Sprite**)((int)this + 0xC4) = spr;
        Parser::ProcessFile(spr, this, 0);
    } else if (strcmp(keyword, "KIDFACE_SPRITE") == 0) {
        Sprite* spr = new Sprite(0);
        *(Sprite**)((int)this + 0xC8) = spr;
        Parser::ProcessFile(spr, this, 0);
    } else if (strcmp(keyword, "WEBVAL_SPRITE") == 0) {
        Sprite* spr = new Sprite(0);
        *(Sprite**)((int)this + 0xCC) = spr;
        Parser::ProcessFile(spr, this, 0);
    } else if (strcmp(keyword, "WEBINVAL_SPRITE") == 0) {
        Sprite* spr = new Sprite(0);
        *(Sprite**)((int)this + 0xD0) = spr;
        Parser::ProcessFile(spr, this, 0);
    } else if (strcmp(keyword, "SOUND") == 0) {
        sscanf(line, "%s %d %s", keyword, &val, buffer);
        char* path = MakeAudioName(buffer);
        Sample* snd = new Sample();
        if (snd->Load(path) != 0) {
            snd->Unload();
            operator delete(snd);
            snd = 0;
        }
        *(Sample**)((int)this + 0xD4 + val * 4) = snd;
    } else if (strcmp(keyword, "END") == 0) {
        return 1;
    } else {
        Parser::LBLParse("SC_Game");
    }
    return 0;
}
