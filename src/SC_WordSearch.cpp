#include "SC_WordSearch.h"
#include "ZBufferManager.h"
#include "SpriteAction.h"
#include "Sprite.h"
#include "Palette.h"
#include "Sample.h"
#include "SoundList.h"
#include "SlimeTable.h"
#include "Timer.h"
#include "GameState.h"
#include "InputManager.h"
#include "MouseControl.h"
#include "globals.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

extern "C" void SendGameMessage(int, int, int, int, int, int, int, int, int, int);
// FUN_00413e10 = ParseFile in Parser.h



extern "C" void ShowError(const char* format, ...);
extern "C" char* MakeAudioName(char*);
extern Palette* __fastcall InitPalette(Palette*);
extern "C" void WriteToLog(const char* format, ...);

#include "Memory.h"
#include "AnimatedAsset.h"

#include "MouseControl.h"

struct WordSearchCell {
    int value;
    int selected;
    int markWin;
    int left;
    int top;
    int right;
    int bottom;
};

struct WordSearchRuntimeState {
    char pad_00[0x34];
    Sprite* bgSprite;
    int pad_38;
    SlimeTable* sound;
    int messageValue;
    int counter;
    int target;
    int cursorState;
    int modeFlag;
    int cursorLeft;
    int cursorTop;
    int cursorRight;
    int cursorBottom;
    Sprite* cursorSprite;
    WordSearchCell cells[36];
    Sprite* cellSprites[36];

    Rect* exitRect() { return (Rect*)&cellSprites[7]; }
};
#include "VBuffer.h"

/* Function start: 0x42E4B0 */
void __fastcall UpdateWordSearchCursor(int* self) {
    int state = self[0];
    if (state > 1) {
        ((Sprite*)self[6])->ResetAnimation(state - 2, 0);
        ((Sprite*)self[6])->Do(((Sprite*)self[6])->loc.x, ((Sprite*)self[6])->loc.y, 1.0);
    }

    int mouseY;
    if (g_InputManager_0046aa08->pMouse == 0) {
        mouseY = 0;
    } else {
        mouseY = g_InputManager_0046aa08->pMouse->y;
    }
    int mouseX;
    if (g_InputManager_0046aa08->pMouse != 0) {
        mouseX = g_InputManager_0046aa08->pMouse->x;
    } else {
        mouseX = 0;
    }

    int inside;
    if (self[2] <= mouseX && self[4] >= mouseX &&
        self[3] <= mouseY && self[5] >= mouseY) {
        inside = 1;
    } else {
        inside = 0;
    }

    Sprite* cursor = g_Mouse_0046aa18->m_sprite;
    if (cursor != 0) {
        cursor->ResetAnimation((inside != 0) ? 1 : 0, 0);
    }
}

// CRT array constructor helper (0x454AD0) — constructs 'count' objects of 'elemSize'
extern "C" void __cdecl ArrayConstruct(void* arr, int elemSize, int count, void* ctor, void* dtor) {}

/* Function start: 0x435800 */
SC_WordSearch::SC_WordSearch() {
    memset(&palette, 0, 0x790);
    handlerId = 0x51;
    timer.Reset();

    for (int i = 0; i < 0x20; i++) {
        int col = i % 4;
        int x = col + col * 12;
        x = x + x * 8 + 0x26;
        wordBounds[i].left = x;

        int row = i / 4;
        int y = row * 16 + row + 0xf5;
        wordBounds[i].top = y;
        wordBounds[i].right = x + 0x74;
        wordBounds[i].bottom = y + 0x10;
    }

    submitRect.top = 0xCC;
    submitRect.bottom = 0xDE;
    submitRect.left = 0x184;
    resetRect.top = 0xCC;
    resetRect.bottom = 0xDE;
    submitRect.right = 0x1BC;
    resetRect.left = 0x1C0;
    resetRect.right = 0x1F8;
    enterPlaceRect.left = 0x1A1;
    enterPlaceRect.top = 0xE1;
    enterPlaceRect.right = 0x1DA;
    enterPlaceRect.bottom = 0xF3;
    placeModeRect.left = 0x107;
    placeModeRect.bottom = 0x151;
    placeModeRect.top = 0x13F;
    placeModeRect.right = 0x13F;
}

SC_WordSearch::~SC_WordSearch() {
}

/* Function start: 0x435BE0 */
void SC_WordSearch::Init(SC_MessageParser* msg) {
    CopyCommandData((SC_MessageParser*)msg);
    if (msg != 0) {
        moduleParam = ((SpriteAction*)msg)->addressValue;
        SC_WordSearch::InitWordList();
    }
    ParseFile(this, g_MissionFilePath_0046bacc, (char*)0);
    if (resultAction == 0) {
        resultAction = new SpriteAction(
            savedCommand,
            savedMsgData,
            handlerId,
            moduleParam,
            4, 0, 0, 0, 0, 0
        );
    }
    gameFlags = 0;
    int iVar2 = g_PeriodStateIdx_0046cb90;
    void* pvVar4 = g_GameState_0046aa30;
    if (iVar2 < 0 || ((GameState*)pvVar4)->maxStates - 1 < iVar2) {
        ShowError("Invalid gamestate %d", iVar2);
    }
    scoreSprite->ResetAnimation(
        ((GameState*)pvVar4)->stateValues[iVar2], 0);
    timer.Reset();
    if (startSound != 0) {
        startSound->Play(100, 1);
    }
    if (introSound != 0) {
        introSound->Play(100, 1);
    }
}

/* Function start: 0x435D40 */
void SC_WordSearch::ShutDown(SC_MessageParser* msg) {
    if (bgSprite != 0) {
        delete bgSprite;
        bgSprite = 0;
    }
    if (palette != 0) {
        delete palette;
        palette = 0;
    }
    if (startSound != 0) {
        startSound->Unload();
        operator delete(startSound);
        startSound = 0;
    }
    if (correctSound != 0) {
        correctSound->Unload();
        operator delete(correctSound);
        correctSound = 0;
    }
    if (incorrectSound != 0) {
        incorrectSound->Unload();
        operator delete(incorrectSound);
        incorrectSound = 0;
    }
    if (introSound != 0) {
        introSound->Unload();
        operator delete(introSound);
        introSound = 0;
    }
    if (resultAction != 0) {
        delete resultAction;
        resultAction = 0;
    }
    if (puzzleSprite1 != 0) {
        delete puzzleSprite1;
        puzzleSprite1 = 0;
    }
    if (puzzleSprite2 != 0) {
        delete puzzleSprite2;
        puzzleSprite2 = 0;
    }
    if (timerSprite != 0) {
        delete timerSprite;
        timerSprite = 0;
    }
    if (scoreSprite != 0) {
        delete scoreSprite;
        scoreSprite = 0;
    }
    if ((int)msg != 0) {
        SendGameMessage(1, handlerId, handlerId, moduleParam, 0x18, 0, 0, 0, 0, 0);
    }
    return;
}

/* Function start: 0x435F10 */
void SC_WordSearch::Update(int param1, int param2) {
    Sprite* spr;
    void* pvVar10;
    int* piVar1;
    int iVar5, iVar9;
    int iVar8;

    spr = g_Mouse_0046aa18->m_sprite;
    if (spr != 0) {
        spr->ResetAnimation(0, 0);
    }

    unsigned int uVar4 = timer.Update();
    if (uVar4 > 60000 && param2 != handlerId) {
        SendGameMessage(1, handlerId, handlerId, moduleParam, 0x18, 0, 0, 0, 0, 0);
    }
    if (handlerId != param2) {
        return;
    }

    if (placementMode != 0) {
        pvVar10 = timerSprite;
        ((Sprite*)pvVar10)->Do(((Sprite*)pvVar10)->loc.x, ((Sprite*)pvVar10)->loc.y, 1.0);
        pvVar10 = scoreSprite;
        ((Sprite*)pvVar10)->Do(((Sprite*)pvVar10)->loc.x, ((Sprite*)pvVar10)->loc.y, 1.0);
        (g_Mouse_0046aa18)->DrawCursor();
        piVar1 = (int*)g_InputManager_0046aa08->pMouse;
        iVar9 = 0;
        if (piVar1 != 0) {
            iVar9 = piVar1[1];
        }
        if (piVar1 != 0) {
            iVar5 = *piVar1;
        } else {
            iVar5 = 0;
        }
        if (iVar5 < placeModeRect.left) return;
        if (placeModeRect.right < iVar5) return;
        if (iVar9 < placeModeRect.top) return;
        if (placeModeRect.bottom < iVar9) return;
        spr = g_Mouse_0046aa18->m_sprite;
        if (spr != 0) {
            spr->ResetAnimation(0xf, 0);
        }
        g_ZBufferManager_0046aa24->DrawRect(
            placeModeRect.left, placeModeRect.top,
            placeModeRect.right, placeModeRect.bottom,
            10000, 0xfc, 2);
        return;
    }

    pvVar10 = bgSprite;
    if (pvVar10 != 0) {
        ((Sprite*)pvVar10)->Do(((Sprite*)pvVar10)->loc.x, ((Sprite*)pvVar10)->loc.y, 1.0);
    } else {
        ((VBuffer*)g_BackBuffer_0046aa14)->ClearScreen(0);
    }

    int* piVar13 = (int*)((char*)wordBounds);
    char* pcVar12 = (char*)words;
    int local_4 = 0x20;
    int* piVar11 = piVar13;
    do {
        if (*pcVar12 != '\0') {
            iVar9 = 0;
            piVar1 = (int*)g_InputManager_0046aa08->pMouse;
            if (piVar1 != 0) {
                iVar9 = piVar1[1];
            }
            if (piVar1 != 0) {
                iVar5 = *piVar1;
            } else {
                iVar5 = 0;
            }
            if (*piVar11 > iVar5 || piVar11[2] < iVar5 || piVar11[1] > iVar9 ||
               (iVar5 = piVar11[3], iVar5 < iVar9)) {
                iVar8 = 0xFC;
                iVar5 = piVar11[3];
            } else {
                iVar8 = 0xFD;
            }
            g_ZBufferManager_0046aa24->ShowText(pcVar12, *piVar11 + 10, iVar5, 10000, iVar8);
        }
        piVar11 = piVar11 + 4;
        pcVar12 = pcVar12 + 0x20;
        local_4 = local_4 - 1;
    } while (local_4 != 0);

    int* puVar2 = (int*)g_InputManager_0046aa08->pMouse;
    if (puVar2 == 0 || (uVar4 = *puVar2, (int)uVar4 < 0x22)) {
        spr = g_Mouse_0046aa18->m_sprite;
        if (spr != 0) {
            spr->ResetAnimation(10, 0);
        }
    } else {
        if (0x23f < (int)uVar4) {
            spr = g_Mouse_0046aa18->m_sprite;
            if (spr != 0) {
                spr->ResetAnimation(0xb, 0);
            }
            goto sprite_section;
        }
        {
        unsigned int uVar7;
        if (puVar2 != 0) {
            uVar7 = puVar2[1];
        } else {
            uVar7 = 0;
        }
        unsigned int uVar6 = ((puVar2 == 0) - 1) & uVar4;

        if (submitRect.left > (int)uVar6 ||
            submitRect.right < (int)uVar6 ||
            submitRect.top > (int)uVar7 ||
            submitRect.bottom < (int)uVar7) {

            uVar7 = 0;
            if (puVar2 != 0) {
                uVar7 = puVar2[1];
            }
            uVar6 = ((puVar2 == 0) - 1) & uVar4;

            if (resetRect.left > (int)uVar6 ||
                resetRect.right < (int)uVar6 ||
                resetRect.top > (int)uVar7 ||
                resetRect.bottom < (int)uVar7) {

                uVar7 = 0;
                if (puVar2 != 0) {
                    uVar7 = puVar2[1];
                }
                uVar4 = ((puVar2 == 0) - 1) & uVar4;

                if (enterPlaceRect.left > (int)uVar4 ||
                    enterPlaceRect.right < (int)uVar4 ||
                    enterPlaceRect.top > (int)uVar7 ||
                    enterPlaceRect.bottom < (int)uVar7) {
                    pcVar12 = (char*)words;
                    iVar9 = 0x20;
                    do {
                        if (*pcVar12 != '\0') {
                            iVar5 = 0;
                            piVar11 = (int*)g_InputManager_0046aa08->pMouse;
                            if (piVar11 != 0) {
                                iVar5 = piVar11[1];
                            }
                            if (piVar11 != 0) {
                                iVar8 = *piVar11;
                            } else {
                                iVar8 = 0;
                            }
                            if (*piVar13 <= iVar8 && iVar8 <= piVar13[2] &&
                                piVar13[1] <= iVar5 &&
                                iVar5 <= piVar13[3] &&
                                g_Mouse_0046aa18->m_sprite != 0) {
                                (g_Mouse_0046aa18->m_sprite)->ResetAnimation(0xf, 0);
                            }
                        }
                        piVar13 = piVar13 + 4;
                        pcVar12 = pcVar12 + 0x20;
                        iVar9 = iVar9 - 1;
                    } while (iVar9 != 0);
                } else {
                    spr = g_Mouse_0046aa18->m_sprite;
                    if (spr != 0) {
                        spr->ResetAnimation(0xf, 0);
                    }
                    g_ZBufferManager_0046aa24->DrawRect(
                        enterPlaceRect.left, enterPlaceRect.top,
                        enterPlaceRect.right, enterPlaceRect.bottom,
                        10000, 0xfc, 2);
                }
            } else {
                spr = g_Mouse_0046aa18->m_sprite;
                if (spr != 0) {
                    spr->ResetAnimation(0xf, 0);
                }
                g_ZBufferManager_0046aa24->DrawRect(
                    resetRect.left, resetRect.top,
                    resetRect.right, resetRect.bottom,
                    10000, 0xfc, 2);
            }
        } else {
            spr = g_Mouse_0046aa18->m_sprite;
            if (spr != 0) {
                spr->ResetAnimation(0xf, 0);
            }
            g_ZBufferManager_0046aa24->DrawRect(
                submitRect.left, submitRect.top,
                submitRect.right, submitRect.bottom,
                10000, 0xfc, 2);
        }
        }
    }

    sprite_section:
    pvVar10 = puzzleSprite1;
    if (((Sprite*)pvVar10)->Do(((Sprite*)pvVar10)->loc.x, ((Sprite*)pvVar10)->loc.y, 1.0) != 0) {
        if (puzzleSprite1->handle == 0) {
            puzzleSprite1->ResetAnimation(-1, 0);
            gameFlags = gameFlags | 1;
            SC_WordSearch::DispatchResult();
        }
    }
    pvVar10 = puzzleSprite2;
    if (((Sprite*)pvVar10)->Do(((Sprite*)pvVar10)->loc.x, ((Sprite*)pvVar10)->loc.y, 1.0) != 0) {
        if (puzzleSprite2->handle == 0) {
            SC_WordSearch::InitWordList();
            puzzleSprite2->ResetAnimation(-1, 0);
        }
    }
    g_ZBufferManager_0046aa24->ShowText((char*)(wordBuffer), 0x49, 0xdc, 10000, 0);
    (g_Mouse_0046aa18)->DrawCursor();
    pvVar10 = scoreSprite;
    ((Sprite*)pvVar10)->Do(((Sprite*)pvVar10)->loc.x, ((Sprite*)pvVar10)->loc.y, 1.0);
}

/* Function start: 0x436790 */
void SC_WordSearch::DispatchResult() {
    void* pvVar2;
    int uVar3;

    if (savedCommand != 0x2B) {
        pvVar2 = g_GameState_0046aa30;
        if (*(unsigned char*)&gameFlags & 1) {
            uVar3 = ((GameState*)pvVar2)->FindLabel("PUZ_WAHOO_WON");
            if (uVar3 < 0 || ((GameState*)pvVar2)->maxStates - 1 < uVar3) {
                ShowError("Invalid gamestate %d", uVar3);
            }
            ((GameState*)pvVar2)->stateValues[uVar3] = 1;
            resultAction->addressType = 0x20;
            resultAction->addressValue = 1;
            resultAction->extra1 = 0x13;
            SendGameMessage(4, 0x1BA8, handlerId, moduleParam, 2, 0, 0, 0, 0, 0);
        } else {
            uVar3 = ((GameState*)pvVar2)->FindLabel("PERIOD");
            if (uVar3 < 0 || ((GameState*)pvVar2)->maxStates - 1 < uVar3) {
                ShowError("Invalid gamestate %d", uVar3);
            }
            if (((GameState*)pvVar2)->stateValues[uVar3] % 5 == 0) {
                resultAction->addressType = 0x2C;
                resultAction->addressValue = 1;
            } else {
                resultAction->addressType = 0x20;
                resultAction->addressValue = 2;
                resultAction->extra1 = 8;
            }
        }
    }
    EnqueueSpriteAction(resultAction);
    if (resultAction != 0) {
        delete resultAction;
        resultAction = 0;
    }
}

/* Function start: 0x4368F0 */
void SC_WordSearch::InitWordList() {
    int iVar2 = g_PeriodStateIdx_0046cb90;
    void* pvVar3 = g_GameState_0046aa30;
    if (iVar2 < 0 || ((GameState*)pvVar3)->maxStates - 1 < iVar2) {
        ShowError("Invalid gamestate %d", iVar2);
    }
    int gsVal = ((GameState*)pvVar3)->stateValues[iVar2];
    if (gsVal == 0) {
        memset(words, 0, sizeof(words));
        strcpy(words[0], "Encounter");
        strcpy(words[1], "Eye");
        strcpy(words[2], "Spaceship");
        strcpy(words[3], "Wand");
        strcpy(words[4], "Roswell");
        strcpy(words[5], "Two");
        strcpy(words[6], "Abduction");
        strcpy(words[7], "Believe");
        strcpy(words[8], "Sighting");
        strcpy(words[9], "Cover-up");
    } else if (gsVal == 1) {
        memset(words, 0, sizeof(words));
        strcpy(words[0], "Encounter");
        strcpy(words[1], "Eye");
        strcpy(words[2], "Spaceship");
        strcpy(words[3], "Wand");
        strcpy(words[4], "Area 51");
        strcpy(words[5], "Roswell");
        strcpy(words[6], "Information");
        strcpy(words[7], "Barca-Lounger");
        strcpy(words[8], "Two");
        strcpy(words[9], "UFO");
        strcpy(words[10], "Grey-men");
        strcpy(words[11], "Abduction");
        strcpy(words[12], "Sighting");
        strcpy(words[13], "Believe");
        strcpy(words[14], "Cover-up");
    } else if (gsVal == 2) {
        memset(words, 0, sizeof(words));
        strcpy(words[0], "Encounter");
        strcpy(words[1], "Eye");
        strcpy(words[2], "Spaceship");
        strcpy(words[3], "Area 51");
        strcpy(words[4], "Wand");
        strcpy(words[5], "Roswell");
        strcpy(words[6], "Information");
        strcpy(words[7], "Barca-Lounger");
        strcpy(words[8], "Two");
        strcpy(words[9], "Cow");
        strcpy(words[10], "UFO");
        strcpy(words[11], "Grey-men");
        strcpy(words[12], "Abduction");
        strcpy(words[13], "Believe");
        strcpy(words[14], "Bogie");
        strcpy(words[15], "Sighting");
        strcpy(words[16], "Conspiracy");
        strcpy(words[17], "Probe");
        strcpy(words[18], "Nuts");
        strcpy(words[19], "Cover-up");
    }
    wordBuffer[0] = 0;
    selectedCount = 0;
}

/* Function start: 0x437080 */
int SC_WordSearch::Exit(SC_MessageParser* msg) {
    if (((SpriteAction*)msg)->addressType != handlerId) {
        return 0;
    }
    timer.Reset();
    int iVar = ((SpriteAction*)msg)->instruction;
    if (iVar != 0) {
        if (iVar != 7) {
            return 0;
        }
        SendGameMessage(1, handlerId, handlerId, moduleParam, 0x18, 0, 0, 0, 0, 0);
    }
    return 1;
}

/* Function start: 0x436490 */
int SC_WordSearch::AddMessage(SC_MessageParser* msg) {
    SpriteAction* action = (SpriteAction*)msg;

    if (WriteMessageAddress(msg) != 0) {
        return 1;
    }

    if (action->lastKey == 0x1B && savedCommand == 0x2B) {
        gameFlags |= 4;
        DispatchResult();
    }

    if (action->button1 > 1) {
        if (placementMode != 0) {
            int mouseX = action->mousePos.x;
            if (placeModeRect.left <= mouseX && mouseX <= placeModeRect.right &&
                placeModeRect.top <= action->mousePos.y &&
                action->mousePos.y <= placeModeRect.bottom) {
                placementMode = 0;
            }
        } else {
            int mouseX = action->mousePos.x;
            if (mouseX < 0x22 || mouseX > 0x23f) {
                gameFlags |= 4;
                DispatchResult();
            } else if (mouseX >= submitRect.left && mouseX <= submitRect.right &&
                       action->mousePos.y >= submitRect.top && action->mousePos.y <= submitRect.bottom) {
                if (strcmp(wordBuffer, " Eye Wand Two Believe") == 0) {
                    puzzleSprite1->ResetAnimation(0, 0);
                } else if (selectedCount != 0) {
                    puzzleSprite2->ResetAnimation(0, 0);
                }
            } else if (mouseX >= resetRect.left && mouseX <= resetRect.right &&
                       action->mousePos.y >= resetRect.top && action->mousePos.y <= resetRect.bottom) {
                if (incorrectSound != 0) {
                    incorrectSound->Play(100, 1);
                }
                InitWordList();
            } else if (mouseX >= enterPlaceRect.left && mouseX <= enterPlaceRect.right &&
                       action->mousePos.y >= enterPlaceRect.top && action->mousePos.y <= enterPlaceRect.bottom) {
                placementMode = 1;
            } else if (selectedCount <= 3) {
                Rect* bounds = &wordBounds[0];
                char* word = &words[0][0];
                int count = 0x20;
                do {
                    if (word[0] != 0 && word != 0 &&
                        bounds->left <= action->mousePos.x && action->mousePos.x <= bounds->right &&
                        bounds->top <= action->mousePos.y && action->mousePos.y <= bounds->bottom) {
                        if (correctSound != 0) {
                            correctSound->Play(100, 1);
                        }
                        strcat(wordBuffer, " ");
                        strcat(wordBuffer, word);
                        *word = 0;
                        selectedCount++;
                    }
                    word += 0x20;
                    bounds++;
                    count--;
                } while (count != 0);
            }
        }
    }

    return 1;
}
/* Function start: 0x4370F0 */
/* Function start: 0x437120 */
int SC_WordSearch::LBLParse(char* line) {
    char keyword[32];
    char buffer[128];
    int val;

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
        Palette* pal = new Palette();
        palette = pal;
        pal->Load(buffer);
        if (palette != 0) {
            if (g_ZBufferManager_0046aa24->m_palette != 0) {
                WriteToLog("ddouble palette");
            }
            g_ZBufferManager_0046aa24->m_palette = palette;
        }
    } else if (strcmp(keyword, "BACKGROUND_SPRITE") == 0) {
        Sprite* spr = new Sprite(0);
        bgSprite = spr;
        Parser::ProcessFile(spr, this, 0);
    } else if (strcmp(keyword, "HINT_SPRITE") == 0) {
        Sprite* spr = new Sprite(0);
        timerSprite = spr;
        Parser::ProcessFile(spr, this, 0);
    } else if (strcmp(keyword, "KIDFACE_SPRITE") == 0) {
        Sprite* spr = new Sprite(0);
        scoreSprite = spr;
        Parser::ProcessFile(spr, this, 0);
    } else if (strcmp(keyword, "WEBVAL_SPRITE") == 0) {
        Sprite* spr = new Sprite(0);
        puzzleSprite1 = spr;
        Parser::ProcessFile(spr, this, 0);
    } else if (strcmp(keyword, "WEBINVAL_SPRITE") == 0) {
        Sprite* spr = new Sprite(0);
        puzzleSprite2 = spr;
        Parser::ProcessFile(spr, this, 0);
    } else if (strcmp(keyword, "TEXT") == 0) {
        sscanf(line, "%s %s", keyword, buffer);
        if (g_GlyphFont_0046aa28 != 0) {
            g_GlyphFont_0046aa28->LoadFont(buffer);
        }
    } else if (strcmp(keyword, "SOUND") == 0) {
        sscanf(line, "%s %d %s", keyword, &val, buffer);
        if (val == 0) {
            SendGameMessage(5, atoi(buffer), handlerId, moduleParam, 0x1b, 0, 0, 0, 0, 0);
        } else {
            char* path = MakeAudioName(buffer);
            switch (val) {
            case 1: {
                Sample* s = new Sample();
                correctSound = s;
                if (s != 0) {
                    s->Load(path);
                }
                break;
            }
            case 2: {
                Sample* s = new Sample();
                incorrectSound = s;
                if (s != 0) {
                    s->Load(path);
                }
                break;
            }
            case 3: {
                Sample* s = new Sample();
                startSound = s;
                if (s != 0) {
                    s->Load(path);
                }
                break;
            }
            case 4: {
                Sample* s = new Sample();
                introSound = s;
                if (s != 0) {
                    startSound->Load(path);
                }
                break;
            }
            }
        }
    } else if (strcmp(keyword, "END") == 0) {
        return 1;
    } else {
        Parser::ReportUnknownLabel("SC_Wahoo");
    }
    return 0;
}

extern void __fastcall UpdateWordSearchCursor(int*);
#include "SoundList.h"

// Dead duplicate of SC_Gauntlet::RenderGrid.
void SC_WordSearch::Render() {
    WordSearchRuntimeState* state = (WordSearchRuntimeState*)words;
    Sprite* bgSpr = state->bgSprite;
    bgSpr->Do(bgSpr->loc.x, bgSpr->loc.y, 1.0);

    if (state->modeFlag != 0 && g_Mouse_0046aa18->m_sprite != 0) {
        g_Mouse_0046aa18->m_sprite->ResetAnimation(0, 0);
    }
    UpdateWordSearchCursor(&state->cursorState);
    if (state->modeFlag != 0 && g_Mouse_0046aa18->m_sprite != 0) {
        g_Mouse_0046aa18->m_sprite->ResetAnimation(0xC, 0);
    }

    int local_4 = 0;
    int mouseY;
    int mouseX;
    int* mouse = (int*)g_InputManager_0046aa08->pMouse;
    if (mouse != 0) {
        mouseY = mouse[1];
    } else {
        mouseY = 0;
    }
    if (mouse != 0) {
        mouseX = mouse[0];
    } else {
        mouseX = 0;
    }

    // Check exit button hover (game-state exit-rect at +0x474)
    Rect* exitButton = state->exitRect();
    if (exitButton->left <= mouseX && mouseX <= exitButton->right &&
        exitButton->top <= mouseY && mouseY <= exitButton->bottom &&
        g_Mouse_0046aa18->m_sprite != 0) {
        g_Mouse_0046aa18->m_sprite->ResetAnimation(0x13, 0);
    }

    int local_8 = 0x82;
    WordSearchCell* cell = state->cells;

    do {
        int cellY = 0x2F;
        Sprite** slot = &state->cellSprites[local_4];
        do {
            int mx;
            int my;
            mouse = (int*)g_InputManager_0046aa08->pMouse;
            if (mouse != 0) {
                my = mouse[1];
            } else {
                my = 0;
            }
            if (mouse != 0) {
                mx = mouse[0];
            } else {
                mx = 0;
            }

            if (cell->left <= mx && mx <= cell->right && cell->top <= my && my <= cell->bottom) {
                Sprite* mouseSpr = g_Mouse_0046aa18->m_sprite;
                if (cell->selected == 0) {
                    if (state->modeFlag == 0) {
                        if (mouseSpr != 0) {
                            mouseSpr->ResetAnimation(0, 0);
                        }
                    } else {
                        if (mouseSpr != 0) {
                            mouseSpr->ResetAnimation(0xC, 0);
                        }
                    }
                } else {
                    if (state->modeFlag == 0) {
                        if (mouseSpr != 0) {
                            mouseSpr->ResetAnimation(0, 0);
                        }
                    } else {
                        if (mouseSpr != 0) {
                            mouseSpr->ResetAnimation(0xD, 0);
                        }
                    }
                }
            }

            if (cell->value != 0) {
                (*slot)->ResetAnimation(cell->value - 1, 0);
                slot++;
                (*(slot - 1))->Do(local_8, cellY, 1.0);
                local_4++;
                int* statusPtr = (int*)((int)palette + 4);
                if (*statusPtr == 0 && cell->markWin != 0 && cell->value > 5) {
                    *statusPtr = 1;
                }
            }

            cell++;
            cellY += 0x33;
        } while (cellY < 0x161);

            local_8 += 0x46;
        if (local_8 > 0x225) {
            g_Mouse_0046aa18->DrawCursor();

            if (state->cursorState < 1) {
                int sndDone = state->sound->IsSamplePlaying(1);
                if (sndDone == 0) {
                    int* status = (int*)palette;
                    if (status[1] == 0) {
                        int count = state->counter + 1;
                        state->counter = count;
                        if (state->target == count) {
                            status[0] = 1;
                            return;
                        }
                        status[4] = 1;
                    }
                }
            }
            return;
        }
    } while (1);
}

// Dead duplicate of SC_Gauntlet::ProcessGrid.
void SC_WordSearch::PlaceWord(int param_1, int param_2) {
    WordSearchRuntimeState* state = (WordSearchRuntimeState*)words;
    WordSearchCell* entries = state->cells;

    int colMin;
    int rowMin;
    int canDiagUL;
    int canDiagDL;
    int canDiagUR;
    int canDiagDR;
    int total;
    int rowMax;
    int colMax;
    int index;
    int* p;

    rowMin = param_1 - 2;
    rowMax = param_1 + 2;
    colMax = param_2 + 2;
    colMin = param_2 - 2;
    total = 0;
    canDiagUL = 1;
    canDiagDL = 1;
    canDiagUR = 1;
    canDiagDR = 1;

    if (rowMin < 0) {
        rowMin = 0;
    }
    if (colMin < 0) {
        colMin = 0;
    }
    if (rowMax > 5) {
        rowMax = 5;
    }
    if (colMax > 5) {
        colMax = 5;
    }

    if (param_1 - 1 < 0) {
        canDiagUL = 0;
        canDiagUR = 0;
    }
    if (param_2 - 1 < 0) {
        canDiagUL = 0;
        canDiagDL = 0;
    }
    if (param_1 + 1 > 5) {
        canDiagDR = 0;
        canDiagDL = 0;
    }
    if (param_2 + 1 > 5) {
        canDiagUR = 0;
        canDiagDR = 0;
    }

    {
        int i = param_2;
        if (i < colMax) {
            p = &entries[param_1 * 6 + i].selected;
            do {
                if (*p != 0) {
                    colMax = i;
                }
                p += 7;
                i++;
            } while (i < colMax);
        }
    }

    {
        int i = param_2;
        if (i > colMin) {
            p = &entries[param_1 * 6 + i].selected;
            do {
                if (*p != 0) {
                    colMin = i;
                }
                p -= 7;
                i--;
            } while (i > colMin);
        }
    }

    {
        int i = param_1;
        if (i < rowMax) {
            p = &entries[i * 6 + param_2].selected;
            do {
                if (*p != 0) {
                    rowMax = i;
                }
                p += 42;
                i++;
            } while (i < rowMax);
        }
    }

    {
        int i = param_1;
        if (i > rowMin) {
            p = &entries[i * 6 + param_2].selected;
            do {
                if (*p != 0) {
                    rowMin = i;
                }
                p -= 42;
                i--;
            } while (i > rowMin);
        }
    }

    index = param_1 * 6 + param_2;

    if (entries[index - 7].selected != 0) {
        canDiagUL = 0;
    }
    if (entries[index + 5].selected != 0) {
        canDiagDL = 0;
    }
    if (entries[index - 5].selected != 0) {
        canDiagUR = 0;
    }
    if (entries[index + 7].selected != 0) {
        canDiagDR = 0;
    }

    if (colMax >= colMin) {
        int count = colMax - colMin + 1;
        p = &entries[param_1 * 6 + colMin].value;
        do {
            total += *p;
            p += 7;
            *(p - 7) = 0;
            count--;
        } while (count != 0);
    }

    if (rowMax >= rowMin) {
        int count = rowMax - rowMin + 1;
        p = &entries[rowMin * 6 + param_2].value;
        do {
            total += *p;
            p += 42;
            *(p - 42) = 0;
            count--;
        } while (count != 0);
    }

    if (canDiagUL != 0) {
        int val = entries[index - 7].value;
        if (val != 0) {
            entries[index - 7].value = 0;
            total += val;
        }
    }
    if (canDiagDL != 0) {
        int val = entries[index + 5].value;
        if (val != 0) {
            entries[index + 5].value = 0;
            total += val;
        }
    }
    if (canDiagUR != 0) {
        int val = entries[index - 5].value;
        if (val != 0) {
            entries[index - 5].value = 0;
            total += val;
        }
    }
    if (canDiagDR != 0) {
        int val = entries[index + 7].value;
        if (val != 0) {
            entries[index + 7].value = 0;
            total += val;
        }
    }

    if (total != 0) {
        state->sound->Play(1);
        int sndIdx = 4 - state->cursorState;
        if (sndIdx >= 0 && sndIdx <= 2) {
            state->sound->Play(sndIdx + 5);
        }
    } else {
        state->sound->Play(0);
    }

    ((WordSearchCell*)&words[3][8])[index].value = total;
    if (state->cursorState == 1) {
        state->cursorState = 0;
    }
    state->modeFlag = 0;
}
