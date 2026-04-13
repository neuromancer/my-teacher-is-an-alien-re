#include "SC_WordSearch.h"
#include "ZBufferManager.h"
#include "SpriteAction.h"
#include "Sprite.h"
#include "Palette.h"
#include "Sample.h"
#include "SoundList.h"
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
#include "VBuffer.h"

/* Function start: 0x42E4B0 */
void __fastcall UpdateWordSearchCursor(int* self) {
    int state = self[0];
    if (state > 1) {
        ((Sprite*)self[6])->ResetAnimation(state - 2, 0);
        ((Sprite*)self[6])->Do(((Sprite*)self[6])->loc_x, ((Sprite*)self[6])->loc_y, 1.0);
    }

    InputState* pMouse = g_InputManager_0046aa08->pMouse;
    int mouseY = 0;
    if (pMouse != 0) {
        mouseY = pMouse->y;
    }
    int mouseX = 0;
    if (pMouse != 0) {
        mouseX = pMouse->x;
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
    ArrayConstruct(gameData + (0x4D4 - 0xD4), 0x10, 0x20, (void*)0x40d0c0, (void*)0x401130);

    memset(&palette, 0, 0x790);
    handlerId = 0x51;
    timer.Reset();

    int* entry = (int*)(gameData + (0x4D4 - 0xD4));
    int i;
    for (i = 0; i < 0x20; i++) {
        int idx = i;
        int sign;
        // abs(i) % 4 with sign preservation
        if (idx < 0) { sign = -1; idx = -idx; } else { sign = 1; }
        int mod = idx % 4;
        if (sign < 0) mod = -mod;
        int col = mod;
        int x = col + col * 12;
        x = x + x * 8 + 0x26;
        entry[0] = x;

        int row = i;
        int rowSign;
        if (row < 0) { rowSign = (row & 3); row = (row + rowSign) >> 2; } else { row = row >> 2; }
        int y = row * 16 + row + 0xf5;
        entry[1] = y;
        entry[2] = x + 0x74;
        entry[3] = y + 0x10;
        entry += 4;
    }

    int* data = (int*)(gameData + (0x6D4 - 0xD4));
    data[0] = 0x184;  // 0x6D4
    data[1] = 0xCC;   // 0x6D8
    data[2] = 0x1BC;  // 0x6DC
    data[3] = 0xDE;   // 0x6E0
    data[4] = 0x1C0;  // 0x6E4
    data[5] = 0xCC;   // 0x6E8
    data[6] = 0x1F8;  // 0x6EC
    data[7] = 0xDE;   // 0x6F0
    data[8] = 0x1A1;  // 0x6F4
    data[9] = 0xE1;   // 0x6F8
    data[10] = 0x1DA; // 0x6FC
    data[11] = 0xF3;  // 0x700
    data[12] = 0x107; // 0x704
    data[13] = 0x13F; // 0x708
    data[14] = 0x13F; // 0x70C
    data[15] = 0x151; // 0x710
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
int SC_WordSearch::ShutDown(SC_MessageParser* msg) {
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
    return 0;
}

/* Function start: 0x435F10 */
void SC_WordSearch::Update(int param1, int param2) {
    Sprite* spr;
    void* pvVar10;
    int* piVar1;
    int iVar5, iVar9;
    int iVar8;

    spr = *(Sprite**)((int)g_Mouse_0046aa18 + 0x94);
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

    if (*(int*)(gameData + 0x640) != 0) {
        pvVar10 = timerSprite;
        ((Sprite*)pvVar10)->Do(((Sprite*)pvVar10)->loc_x, ((Sprite*)pvVar10)->loc_y, 1.0);
        pvVar10 = scoreSprite;
        ((Sprite*)pvVar10)->Do(((Sprite*)pvVar10)->loc_x, ((Sprite*)pvVar10)->loc_y, 1.0);
        (g_Mouse_0046aa18)->DrawCursor();
        piVar1 = *(int**)((int)g_InputManager_0046aa08 + 0x1a0);
        iVar9 = 0;
        if (piVar1 != 0) {
            iVar9 = piVar1[1];
        }
        if (piVar1 != 0) {
            iVar5 = *piVar1;
        } else {
            iVar5 = 0;
        }
        if (iVar5 < *(int*)(gameData + 0x630)) return;
        if (*(int*)(gameData + 0x638) < iVar5) return;
        if (iVar9 < *(int*)(gameData + 0x634)) return;
        if (*(int*)(gameData + 0x63C) < iVar9) return;
        spr = *(Sprite**)((int)g_Mouse_0046aa18 + 0x94);
        if (spr != 0) {
            spr->ResetAnimation(0xf, 0);
        }
        g_ZBufferManager_0046aa24->DrawRect(
            *(int*)(gameData + 0x630), *(int*)(gameData + 0x634),
            *(int*)(gameData + 0x638), *(int*)(gameData + 0x63C),
            10000, 0xfc, 2);
        return;
    }

    pvVar10 = bgSprite;
    if (pvVar10 != 0) {
        ((Sprite*)pvVar10)->Do(((Sprite*)pvVar10)->loc_x, ((Sprite*)pvVar10)->loc_y, 1.0);
    } else {
        ((VBuffer*)g_BackBuffer_0046aa14)->ClearScreen(0);
    }

    int* piVar13 = (int*)(gameData + 0x400);
    char* pcVar12 = gameData;
    int local_4 = 0x20;
    int* piVar11 = piVar13;
    do {
        if (*pcVar12 != '\0') {
            iVar9 = 0;
            piVar1 = *(int**)((int)g_InputManager_0046aa08 + 0x1a0);
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

    int* puVar2 = *(int**)((int)g_InputManager_0046aa08 + 0x1a0);
    if (puVar2 == 0 || (uVar4 = *puVar2, (int)uVar4 < 0x22)) {
        spr = *(Sprite**)((int)g_Mouse_0046aa18 + 0x94);
        if (spr != 0) {
            spr->ResetAnimation(10, 0);
        }
    } else {
        if (0x23f < (int)uVar4) {
            spr = *(Sprite**)((int)g_Mouse_0046aa18 + 0x94);
            if (spr != 0) {
                spr->ResetAnimation(0xb, 0);
            }
            goto sprite_section;
        }
        {
        unsigned int uVar7 = 0;
        if (puVar2 != 0) {
            uVar7 = puVar2[1];
        }
        unsigned int uVar6 = ((int)(puVar2 == 0) - 1) & uVar4;

        if (*(int*)(gameData + 0x600) > (int)uVar6 ||
            *(int*)(gameData + 0x608) < (int)uVar6 ||
            *(int*)(gameData + 0x604) > (int)uVar7 ||
            *(int*)(gameData + 0x60C) < (int)uVar7) {

            uVar7 = 0;
            if (puVar2 != 0) {
                uVar7 = puVar2[1];
            }
            uVar6 = ((int)(puVar2 == 0) - 1) & uVar4;

            if (*(int*)(gameData + 0x610) > (int)uVar6 ||
                *(int*)(gameData + 0x618) < (int)uVar6 ||
                *(int*)(gameData + 0x614) > (int)uVar7 ||
                *(int*)(gameData + 0x61C) < (int)uVar7) {

                uVar7 = 0;
                if (puVar2 != 0) {
                    uVar7 = puVar2[1];
                }
                uVar4 = ((int)(puVar2 == 0) - 1) & uVar4;

                if (*(int*)(gameData + 0x620) > (int)uVar4 ||
                    *(int*)(gameData + 0x628) < (int)uVar4 ||
                    *(int*)(gameData + 0x624) > (int)uVar7 ||
                    *(int*)(gameData + 0x62C) < (int)uVar7) {
                    pcVar12 = gameData;
                    iVar9 = 0x20;
                    do {
                        if (*pcVar12 != '\0') {
                            iVar5 = 0;
                            piVar11 = *(int**)((int)g_InputManager_0046aa08 + 0x1a0);
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
                                *(Sprite**)((int)g_Mouse_0046aa18 + 0x94) != 0) {
                                (*(Sprite**)((int)g_Mouse_0046aa18 + 0x94))->ResetAnimation(0xf, 0);
                            }
                        }
                        piVar13 = piVar13 + 4;
                        pcVar12 = pcVar12 + 0x20;
                        iVar9 = iVar9 - 1;
                    } while (iVar9 != 0);
                } else {
                    spr = *(Sprite**)((int)g_Mouse_0046aa18 + 0x94);
                    if (spr != 0) {
                        spr->ResetAnimation(0xf, 0);
                    }
                    g_ZBufferManager_0046aa24->DrawRect(
                        *(int*)(gameData + 0x620), *(int*)(gameData + 0x624),
                        *(int*)(gameData + 0x628), *(int*)(gameData + 0x62C),
                        10000, 0xfc, 2);
                }
            } else {
                spr = *(Sprite**)((int)g_Mouse_0046aa18 + 0x94);
                if (spr != 0) {
                    spr->ResetAnimation(0xf, 0);
                }
                g_ZBufferManager_0046aa24->DrawRect(
                    *(int*)(gameData + 0x610), *(int*)(gameData + 0x614),
                    *(int*)(gameData + 0x618), *(int*)(gameData + 0x61C),
                    10000, 0xfc, 2);
            }
        } else {
            spr = *(Sprite**)((int)g_Mouse_0046aa18 + 0x94);
            if (spr != 0) {
                spr->ResetAnimation(0xf, 0);
            }
            g_ZBufferManager_0046aa24->DrawRect(
                *(int*)(gameData + 0x600), *(int*)(gameData + 0x604),
                *(int*)(gameData + 0x608), *(int*)(gameData + 0x60C),
                10000, 0xfc, 2);
        }
        }
    }

    sprite_section:
    pvVar10 = puzzleSprite1;
    if (((Sprite*)pvVar10)->Do(((Sprite*)pvVar10)->loc_x, ((Sprite*)pvVar10)->loc_y, 1.0) != 0) {
        if (puzzleSprite1->handle == 0) {
            puzzleSprite1->ResetAnimation(-1, 0);
            gameFlags = gameFlags | 1;
            SC_WordSearch::DispatchResult();
        }
    }
    pvVar10 = puzzleSprite2;
    if (((Sprite*)pvVar10)->Do(((Sprite*)pvVar10)->loc_x, ((Sprite*)pvVar10)->loc_y, 1.0) != 0) {
        if (puzzleSprite2->handle == 0) {
            SC_WordSearch::InitWordList();
            puzzleSprite2->ResetAnimation(-1, 0);
        }
    }
    g_ZBufferManager_0046aa24->ShowText((char*)(gameData + 0x644), 0x49, 0xdc, 10000, 0);
    (g_Mouse_0046aa18)->DrawCursor();
    pvVar10 = scoreSprite;
    ((Sprite*)pvVar10)->Do(((Sprite*)pvVar10)->loc_x, ((Sprite*)pvVar10)->loc_y, 1.0);
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
        memset(gameData, 0, 0x400);
        strcpy(gameData, "Encounter");
        strcpy(gameData + 0x20, "Eye");
        strcpy(gameData + 0x40, "Spaceship");
        strcpy(gameData + 0x60, "Wand");
        strcpy(gameData + 0x80, "Roswell");
        strcpy(gameData + 0xA0, "Two");
        strcpy(gameData + 0xC0, "Abduction");
        strcpy(gameData + 0xE0, "Believe");
        strcpy(gameData + 0x100, "Sighting");
        strcpy(gameData + 0x120, "Cover-up");
    } else if (gsVal == 1) {
        memset(gameData, 0, 0x400);
        strcpy(gameData, "Encounter");
        strcpy(gameData + 0x20, "Eye");
        strcpy(gameData + 0x40, "Spaceship");
        strcpy(gameData + 0x60, "Wand");
        strcpy(gameData + 0x80, "Area 51");
        strcpy(gameData + 0xA0, "Roswell");
        strcpy(gameData + 0xC0, "Information");
        strcpy(gameData + 0xE0, "Barca-Lounger");
        strcpy(gameData + 0x100, "Two");
        strcpy(gameData + 0x120, "UFO");
        strcpy(gameData + 0x140, "Grey-men");
        strcpy(gameData + 0x160, "Abduction");
        strcpy(gameData + 0x180, "Sighting");
        strcpy(gameData + 0x1A0, "Believe");
        strcpy(gameData + 0x1C0, "Cover-up");
    } else if (gsVal == 2) {
        memset(gameData, 0, 0x400);
        strcpy(gameData, "Encounter");
        strcpy(gameData + 0x20, "Eye");
        strcpy(gameData + 0x40, "Spaceship");
        strcpy(gameData + 0x60, "Area 51");
        strcpy(gameData + 0x80, "Wand");
        strcpy(gameData + 0xA0, "Roswell");
        strcpy(gameData + 0xC0, "Information");
        strcpy(gameData + 0xE0, "Barca-Lounger");
        strcpy(gameData + 0x100, "Two");
        strcpy(gameData + 0x120, "Cow");
        strcpy(gameData + 0x140, "UFO");
        strcpy(gameData + 0x160, "Grey-men");
        strcpy(gameData + 0x180, "Abduction");
        strcpy(gameData + 0x1A0, "Believe");
        strcpy(gameData + 0x1C0, "Bogie");
        strcpy(gameData + 0x1E0, "Sighting");
        strcpy(gameData + 0x200, "Conspiracy");
        strcpy(gameData + 0x220, "Probe");
        strcpy(gameData + 0x240, "Nuts");
        strcpy(gameData + 0x260, "Cover-up");
    }
    *(char*)(gameData + 0x644) = 0;
    *(int*)(gameData + 0x744) = 0;
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
int SC_WordSearch::AddMessage(SC_MessageParser*) { return 0; }
/* Function start: 0x4370F0 */
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
        Palette* pal = InitPalette((Palette*)AllocateMemory(8));
        palette = pal;
        pal->Load(buffer);
        if (palette != 0) {
            int* palSlot = (int*)((char*)g_ZBufferManager_0046aa24 + 0xa8);
            if (*palSlot != 0) {
                WriteToLog("ddouble palette");
            }
            *palSlot = (int)palette;
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
    } else if (strcmp(keyword, "FONT") == 0) {
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
        Parser::LBLParse("SC_Wahoo");
    }
    return 0;
}

extern void __fastcall InitCombatGrid(int);
// Engine::OnProcessEnd wrapper for cross-hierarchy __fastcall calls
#include "Engine.h"
void __fastcall Handler_OnProcessEnd(int thisPtr) {
    ((Engine*)thisPtr)->OnProcessEnd();
}

/* Function start: 0x42F800 */
void SC_WordSearch::OnProcessEnd() {
    int startX = 0x9B;
    Handler_OnProcessEnd((int)this);
    int* ptr = (int*)(gameData + 0x74);
    do {
        int cellX = 0x48;
        int* cell = ptr;
        do {
            cell[0] = startX;
            cell[1] = cellX;
            cell[2] = startX + 0x45;
            cell[3] = cellX + 0x32;
            ptr = cell + 7;
            cellX += 0x33;
            cell = ptr;
        } while (cellX < 0x17A);
        startX += 0x46;
    } while (startX < 0x23F);

    // Set initial grid cell states
    *(int*)(gameData + 0xA4) = 1;
    *(int*)(gameData + 0x1D8) = 1;
    *(int*)(gameData + 0x280) = 1;
    *(int*)(gameData + 0x328) = 1;
    *(int*)(gameData + 0x360) = 1;
    *(int*)(gameData + 0x1F4) = 1;
    *(int*)(gameData + 0x210) = 1;
    *(int*)(gameData + 0x2F0) = 1;

    // Set game parameters
    *(int*)(gameData + 0x54) = 10;
    *(int*)(gameData + 0x58) = 0x14;
    *(int*)(gameData + 0x5C) = 0x5A;
    *(int*)(gameData + 0x60) = 0xDC;
    *(int*)(gameData + 0x474) = 6;
    *(int*)(gameData + 0x428) = 1;
    *(int*)(gameData + 0x478) = 0x197;
    *(int*)(gameData + 0x47C) = 0x5F;
    *(int*)(gameData + 0x480) = 0x1D6;

    InitCombatGrid((int)this);

    if (*(int*)(gameData + 0x40) != 0) {
        SendGameMessage(5, *(int*)(gameData + 0x40), handlerId, moduleParam, 0x1B, 0, 0, 0, 0, 0);
    }
}

extern void __fastcall UpdateWordSearchCursor(int*);
#include "SoundList.h"

/* Function start: 0x42EFC0 */
void SC_WordSearch::Render() {
    Sprite* bgSpr = (Sprite*)*(int*)(gameData + 0x34);
    bgSpr->Do(bgSpr->loc_x, bgSpr->loc_y, 1.0);

    if (*(int*)(gameData + 0x50) != 0 && g_Mouse_0046aa18->m_sprite != 0) {
        g_Mouse_0046aa18->m_sprite->ResetAnimation(0, 0);
    }
    UpdateWordSearchCursor((int*)(gameData + 0x4C));
    if (*(int*)(gameData + 0x50) != 0 && g_Mouse_0046aa18->m_sprite != 0) {
        g_Mouse_0046aa18->m_sprite->ResetAnimation(0xC, 0);
    }

    int mouseY = 0;
    int mouseX = 0;
    InputState* pMouse = g_InputManager_0046aa08->pMouse;
    if (pMouse != 0) {
        mouseY = pMouse->y;
        mouseX = pMouse->x;
    }

    // Check exit button hover
    if (*(int*)(gameData + 0x474) <= mouseX && mouseX <= *(int*)(gameData + 0x47C) &&
        *(int*)(gameData + 0x478) <= mouseY && mouseY <= *(int*)(gameData + 0x480) &&
        g_Mouse_0046aa18->m_sprite != 0) {
        g_Mouse_0046aa18->m_sprite->ResetAnimation(0x13, 0);
    }

    int local_8 = 0x82;
    int local_4 = 0;
    int* gridCell = (int*)(gameData + 0x68);
    Sprite** spriteSlot = (Sprite**)(gameData + 0x458);

    do {
        int cellY = 0x2F;
        int* cell = gridCell;
        Sprite** slot = spriteSlot;
        do {
            pMouse = g_InputManager_0046aa08->pMouse;
            int mx = 0, my = 0;
            if (pMouse != 0) { my = pMouse->y; mx = pMouse->x; }

            if (cell[3] <= mx && mx <= cell[5] && cell[4] <= my && my <= cell[6]) {
                Sprite* mouseSpr = g_Mouse_0046aa18->m_sprite;
                if (cell[1] == 0) {
                    if (mouseSpr != 0) {
                        int anim = (*(int*)(gameData + 0x50) == 0) ? 0 : 0xC;
                        mouseSpr->ResetAnimation(anim, 0);
                    }
                } else {
                    if (mouseSpr != 0) {
                        int anim = (*(int*)(gameData + 0x50) == 0) ? 0 : 0xD;
                        mouseSpr->ResetAnimation(anim, 0);
                    }
                }
            }

            if (*cell != 0) {
                Sprite* spr = *slot;
                slot++;
                spr->ResetAnimation(*cell - 1, 0);
                spr->Do(local_8, cellY, 1.0);
                local_4++;
                int* statusPtr = (int*)((int)palette + 4);
                if (*statusPtr == 0 && cell[2] != 0 && *cell > 5) {
                    *statusPtr = 1;
                }
            }

            cell += 7;
            cellY += 0x33;
        } while (cellY < 0x161);

        local_8 += 0x46;
        if (local_8 > 0x225) {
            g_Mouse_0046aa18->DrawCursor();

            if (*(int*)(gameData + 0x4C) < 1) {
                int sndDone = ((SoundList*)*(void**)(gameData + 0x3C))->IsSamplePlaying(1);
                if (sndDone == 0) {
                    int* status = (int*)palette;
                    if (status[1] == 0) {
                        int count = *(int*)(gameData + 0x44) + 1;
                        *(int*)(gameData + 0x44) = count;
                        if (*(int*)(gameData + 0x48) == count) {
                            status[0] = 1;
                            return;
                        }
                        status[4] = 1;
                    }
                }
            }
            return;
        }
        gridCell += 0x2A;
        spriteSlot += 6;
    } while (1);
}

/* Function start: 0x42F220 */
void SC_WordSearch::PlaceWord(int param_1, int param_2) {
    int minRow = param_1 - 2;
    int maxRow = param_1 + 2;
    int maxCol = param_2 + 2;
    int minCol = param_2 - 2;
    int total = 0;

    if (minRow < 0) minRow = 0;
    if (minCol < 0) minCol = 0;
    if (maxRow > 5) maxRow = 5;
    if (maxCol > 5) maxCol = 5;

    int hasUpLeft = (param_1 - 1 >= 0);
    int hasDownLeft = (param_2 - 1 >= 0);
    int hasUpRight = (param_1 + 1 < 6);
    int hasDownRight = (param_2 + 1 < 6);

    // Scan right from current position
    if (param_2 < maxCol) {
        int* cell = (int*)(gameData + (param_2 + param_1 * 6) * 0x1C + 0x6C);
        int c = param_2;
        do {
            if (*cell != 0) maxCol = c;
            cell += 7;
            c++;
        } while (c < maxCol);
    }
    // Scan left
    if (minCol < param_2) {
        int* cell = (int*)(gameData + (param_2 + param_1 * 6) * 0x1C + 0x6C);
        int c = param_2;
        do {
            if (*cell != 0) minCol = c;
            cell -= 7;
            c--;
        } while (minCol < c);
    }
    // Scan down
    if (param_1 < maxRow) {
        int* cell = (int*)(gameData + (param_2 + param_1 * 6) * 0x1C + 0x6C);
        int r = param_1;
        do {
            if (*cell != 0) maxRow = r;
            cell += 0x2A;
            r++;
        } while (r < maxRow);
    }
    // Scan up
    if (minRow < param_1) {
        int* cell = (int*)(gameData + (param_2 + param_1 * 6) * 0x1C + 0x6C);
        int r = param_1;
        do {
            if (*cell != 0) minRow = r;
            cell -= 0x2A;
            r--;
        } while (minRow < r);
    }

    int idx = param_2 + param_1 * 6;
    int diagUL = *(int*)(gameData + idx * 0x1C - 0x58);
    int diagDR = *(int*)(gameData + idx * 0x1C + 0xF8);
    int diagUR = *(int*)(gameData + idx * 0x1C - 0x20);
    int diagDL = *(int*)(gameData + idx * 0x1C + 0x130);

    // Clear horizontal span
    if (minCol <= maxCol) {
        int count = maxCol - minCol + 1;
        int* cell = (int*)(gameData + (param_1 * 6 + minCol) * 0x1C + 0x68);
        do {
            total += *cell;
            *cell = 0;
            cell += 7;
            count--;
        } while (count != 0);
    }
    // Clear vertical span
    if (minRow <= maxRow) {
        int count = maxRow - minRow + 1;
        int* cell = (int*)(gameData + (param_2 + minRow * 6) * 0x1C + 0x68);
        do {
            total += *cell;
            *cell = 0;
            cell += 0x2A;
            count--;
        } while (count != 0);
    }
    // Clear diagonals
    if (diagUL == 0 && hasDownLeft && hasUpLeft) {
        int val = *(int*)(gameData + idx * 0x1C - 0x5C);
        if (val != 0) {
            *(int*)(gameData + idx * 0x1C - 0x5C) = 0;
            total += val;
        }
    }
    if (diagDR == 0 && hasUpRight && hasDownLeft) {
        int val = *(int*)(gameData + idx * 0x1C + 0xF4);
        if (val != 0) {
            *(int*)(gameData + idx * 0x1C + 0xF4) = 0;
            total += val;
        }
    }
    if (diagUR == 0 && hasDownRight && hasUpLeft) {
        int val = *(int*)(gameData + idx * 0x1C - 0x24);
        if (val != 0) {
            *(int*)(gameData + idx * 0x1C - 0x24) = 0;
            total += val;
        }
    }
    if (diagDL == 0 && hasDownRight && hasUpRight) {
        int val = *(int*)(gameData + idx * 0x1C + 0x12C);
        if (val != 0) {
            *(int*)(gameData + idx * 0x1C + 0x12C) = 0;
            total += val;
        }
    }

    int snd;
    if (total == 0) {
        snd = 0;
    } else {
        ((SoundList*)*(int*)(gameData + 0x3C))->Play(1);
        snd = 4 - *(int*)(gameData + 0x4C);
        if (snd < 0 || snd > 2) goto skip_sound;
        snd = 9 - *(int*)(gameData + 0x4C);
    }
    ((SoundList*)*(int*)(gameData + 0x3C))->Play(snd);
skip_sound:
    *(int*)(gameData + idx * 0x1C + 0x68) = total;
    if (*(int*)(gameData + 0x4C) == 1) {
        *(int*)(gameData + 0x4C) = 0;
    }
    *(int*)(gameData + 0x50) = 0;
}
