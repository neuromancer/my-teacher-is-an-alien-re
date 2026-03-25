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
        cursor->ResetAnimation((inside == 0) ? 1 : 0, 0);
    }
}

/* Function start: 0x435800 */
SC_WordSearch::SC_WordSearch() {
}

SC_WordSearch::~SC_WordSearch() {
}

/* Function start: 0x435BE0 */
void SC_WordSearch::Init(SC_Message* msg) {
    CopyCommandData((SC_Message*)msg);
    if (msg != 0) {
        *(int*)((int)this + 0x94) = *(int*)((int)msg + 4);
        SC_WordSearch::FUN_4368F0();
    }
    ParseFile(this, DAT_0046bacc, (char*)0);
    if (*(int*)((int)this + 0x830) == 0) {
        *(void**)((int)this + 0x830) = new SpriteAction(
            *(int*)((int)this + 0x98),
            *(int*)((int)this + 0x9c),
            *(int*)((int)this + 0x90),
            *(int*)((int)this + 0x94),
            4, 0, 0, 0, 0, 0
        );
    }
    *(int*)((int)this + 0x81c) = 0;
    int iVar2 = g_PeriodStateIdx_0046cb90;
    void* pvVar4 = g_GameState_0046aa30;
    if (iVar2 < 0 || *(int*)((int)pvVar4 + 0x98) - 1 < iVar2) {
        ShowError("Invalid gamestate %d", iVar2);
    }
    ((Sprite*)*(void**)((int)this + 0x82c))->ResetAnimation(
        *(int*)(*(int*)((int)pvVar4 + 0x90) + iVar2 * 4), 0);
    ((Timer*)((int)this + 0xC0))->Reset();
    if (*(void**)((int)this + 0xB8) != 0) {
        ((Sample*)*(void**)((int)this + 0xB8))->Play(100, 1);
    }
    if (*(void**)((int)this + 0xBC) != 0) {
        ((Sample*)*(void**)((int)this + 0xBC))->Play(100, 1);
    }
}

/* Function start: 0x435D40 */
int SC_WordSearch::ShutDown(SC_Message* msg) {
    void* pVar;

    pVar = *(void**)((int)this + 0xAC);
    if (pVar != 0) {
        delete (Sprite*)pVar;
        *(void**)((int)this + 0xAC) = 0;
    }
    pVar = *(void**)((int)this + 0xA8);
    if (pVar != 0) {
        delete (Palette*)pVar;
        *(void**)((int)this + 0xA8) = 0;
    }
    pVar = *(void**)((int)this + 0xB8);
    if (pVar != 0) {
        ((Sample*)pVar)->Unload();
        free(pVar);
        *(void**)((int)this + 0xB8) = 0;
    }
    pVar = *(void**)((int)this + 0xB0);
    if (pVar != 0) {
        ((Sample*)pVar)->Unload();
        free(pVar);
        *(void**)((int)this + 0xB0) = 0;
    }
    pVar = *(void**)((int)this + 0xB4);
    if (pVar != 0) {
        ((Sample*)pVar)->Unload();
        free(pVar);
        *(void**)((int)this + 0xB4) = 0;
    }
    pVar = *(void**)((int)this + 0xBC);
    if (pVar != 0) {
        ((Sample*)pVar)->Unload();
        free(pVar);
        *(void**)((int)this + 0xBC) = 0;
    }
    pVar = *(void**)((int)this + 0x830);
    if (pVar != 0) {
        delete (SpriteAction*)pVar;
        *(void**)((int)this + 0x830) = 0;
    }
    pVar = *(void**)((int)this + 0x820);
    if (pVar != 0) {
        delete (Sprite*)pVar;
        *(void**)((int)this + 0x820) = 0;
    }
    pVar = *(void**)((int)this + 0x824);
    if (pVar != 0) {
        delete (Sprite*)pVar;
        *(void**)((int)this + 0x824) = 0;
    }
    pVar = *(void**)((int)this + 0x828);
    if (pVar != 0) {
        delete (Sprite*)pVar;
        *(void**)((int)this + 0x828) = 0;
    }
    pVar = *(void**)((int)this + 0x82C);
    if (pVar != 0) {
        delete (Sprite*)pVar;
        *(void**)((int)this + 0x82C) = 0;
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

    unsigned int uVar4 = ((Timer*)((int)this + 0xC0))->Update();
    if (uVar4 > 60000 && param2 != handlerId) {
        SendGameMessage(1, handlerId, handlerId, moduleParam, 0x18, 0, 0, 0, 0, 0);
    }
    if (handlerId != param2) {
        return;
    }

    if (*(int*)((int)this + 0x714) != 0) {
        pvVar10 = *(void**)((int)this + 0x828);
        ((Sprite*)pvVar10)->Do(*(int*)((int)pvVar10 + 0xac), *(int*)((int)pvVar10 + 0xb0), 1.0);
        pvVar10 = *(void**)((int)this + 0x82c);
        ((Sprite*)pvVar10)->Do(*(int*)((int)pvVar10 + 0xac), *(int*)((int)pvVar10 + 0xb0), 1.0);
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
        if (iVar5 < *(int*)((int)this + 0x704)) return;
        if (*(int*)((int)this + 0x70c) < iVar5) return;
        if (iVar9 < *(int*)((int)this + 0x708)) return;
        if (*(int*)((int)this + 0x710) < iVar9) return;
        spr = *(Sprite**)((int)g_Mouse_0046aa18 + 0x94);
        if (spr != 0) {
            spr->ResetAnimation(0xf, 0);
        }
        g_ZBufferManager_0046aa24->DrawRect(
            *(int*)((int)this + 0x704), *(int*)((int)this + 0x708),
            *(int*)((int)this + 0x70c), *(int*)((int)this + 0x710),
            10000, 0xfc, 2);
        return;
    }

    pvVar10 = *(void**)((int)this + 0xac);
    if (pvVar10 != 0) {
        ((Sprite*)pvVar10)->Do(*(int*)((int)pvVar10 + 0xac), *(int*)((int)pvVar10 + 0xb0), 1.0);
    } else {
        ((VBuffer*)g_BackBuffer_0046aa14)->ClearScreen(0);
    }

    int* piVar13 = (int*)((int)this + 0x4d4);
    char* pcVar12 = (char*)((int)this + 0xd4);
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

        if (*(int*)((int)this + 0x6d4) > (int)uVar6 ||
            *(int*)((int)this + 0x6dc) < (int)uVar6 ||
            *(int*)((int)this + 0x6d8) > (int)uVar7 ||
            *(int*)((int)this + 0x6e0) < (int)uVar7) {

            uVar7 = 0;
            if (puVar2 != 0) {
                uVar7 = puVar2[1];
            }
            uVar6 = ((int)(puVar2 == 0) - 1) & uVar4;

            if (*(int*)((int)this + 0x6e4) > (int)uVar6 ||
                *(int*)((int)this + 0x6ec) < (int)uVar6 ||
                *(int*)((int)this + 0x6e8) > (int)uVar7 ||
                *(int*)((int)this + 0x6f0) < (int)uVar7) {

                uVar7 = 0;
                if (puVar2 != 0) {
                    uVar7 = puVar2[1];
                }
                uVar4 = ((int)(puVar2 == 0) - 1) & uVar4;

                if (*(int*)((int)this + 0x6f4) > (int)uVar4 ||
                    *(int*)((int)this + 0x6fc) < (int)uVar4 ||
                    *(int*)((int)this + 0x6f8) > (int)uVar7 ||
                    *(int*)((int)this + 0x700) < (int)uVar7) {
                    pcVar12 = (char*)((int)this + 0xd4);
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
                        *(int*)((int)this + 0x6f4), *(int*)((int)this + 0x6f8),
                        *(int*)((int)this + 0x6fc), *(int*)((int)this + 0x700),
                        10000, 0xfc, 2);
                }
            } else {
                spr = *(Sprite**)((int)g_Mouse_0046aa18 + 0x94);
                if (spr != 0) {
                    spr->ResetAnimation(0xf, 0);
                }
                g_ZBufferManager_0046aa24->DrawRect(
                    *(int*)((int)this + 0x6e4), *(int*)((int)this + 0x6e8),
                    *(int*)((int)this + 0x6ec), *(int*)((int)this + 0x6f0),
                    10000, 0xfc, 2);
            }
        } else {
            spr = *(Sprite**)((int)g_Mouse_0046aa18 + 0x94);
            if (spr != 0) {
                spr->ResetAnimation(0xf, 0);
            }
            g_ZBufferManager_0046aa24->DrawRect(
                *(int*)((int)this + 0x6d4), *(int*)((int)this + 0x6d8),
                *(int*)((int)this + 0x6dc), *(int*)((int)this + 0x6e0),
                10000, 0xfc, 2);
        }
        }
    }

    sprite_section:
    pvVar10 = *(void**)((int)this + 0x820);
    if (((Sprite*)pvVar10)->Do(*(int*)((int)pvVar10 + 0xac), *(int*)((int)pvVar10 + 0xb0), 1.0) != 0) {
        if (*(int*)(*(int*)((int)this + 0x820) + 0x98) == 0) {
            ((Sprite*)*(void**)((int)this + 0x820))->ResetAnimation(-1, 0);
            *(int*)((int)this + 0x81c) = *(int*)((int)this + 0x81c) | 1;
            SC_WordSearch::FUN_436790();
        }
    }
    pvVar10 = *(void**)((int)this + 0x824);
    if (((Sprite*)pvVar10)->Do(*(int*)((int)pvVar10 + 0xac), *(int*)((int)pvVar10 + 0xb0), 1.0) != 0) {
        if (*(int*)(*(int*)((int)this + 0x824) + 0x98) == 0) {
            SC_WordSearch::FUN_4368F0();
            ((Sprite*)*(void**)((int)this + 0x824))->ResetAnimation(-1, 0);
        }
    }
    g_ZBufferManager_0046aa24->ShowText((char*)((int)this + 0x718), 0x49, 0xdc, 10000, 0);
    (g_Mouse_0046aa18)->DrawCursor();
    pvVar10 = *(void**)((int)this + 0x82c);
    ((Sprite*)pvVar10)->Do(*(int*)((int)pvVar10 + 0xac), *(int*)((int)pvVar10 + 0xb0), 1.0);
}

/* Function start: 0x436790 */
void SC_WordSearch::FUN_436790() {
    void* pvVar2;
    int uVar3;

    if (*(int*)((int)this + 0x98) != 0x2B) {
        pvVar2 = g_GameState_0046aa30;
        if (*(unsigned char*)((int)this + 0x81c) & 1) {
            uVar3 = ((GameState*)pvVar2)->FindLabel("PUZ_WAHOO_WON");
            if (uVar3 < 0 || *(int*)((int)pvVar2 + 0x98) - 1 < uVar3) {
                ShowError("Invalid gamestate %d", uVar3);
            }
            *(int*)(*(int*)((int)pvVar2 + 0x90) + uVar3 * 4) = 1;
            *(int*)(*(int*)((int)this + 0x830)) = 0x20;
            *(int*)(*(int*)((int)this + 0x830) + 4) = 1;
            *(int*)(*(int*)((int)this + 0x830) + 0x14) = 0x13;
            SendGameMessage(4, 0x1BA8, handlerId, moduleParam, 2, 0, 0, 0, 0, 0);
        } else {
            uVar3 = ((GameState*)pvVar2)->FindLabel("PERIOD");
            if (uVar3 < 0 || *(int*)((int)pvVar2 + 0x98) - 1 < uVar3) {
                ShowError("Invalid gamestate %d", uVar3);
            }
            if (*(int*)(*(int*)((int)pvVar2 + 0x90) + uVar3 * 4) % 5 == 0) {
                *(int*)(*(int*)((int)this + 0x830)) = 0x2C;
                *(int*)(*(int*)((int)this + 0x830) + 4) = 1;
            } else {
                *(int*)(*(int*)((int)this + 0x830)) = 0x20;
                *(int*)(*(int*)((int)this + 0x830) + 4) = 2;
                *(int*)(*(int*)((int)this + 0x830) + 0x14) = 8;
            }
        }
    }
    EnqueueSpriteAction(*(void**)((int)this + 0x830));
    void* pVar = *(void**)((int)this + 0x830);
    if (pVar != 0) {
        delete (SpriteAction*)pVar;
        *(void**)((int)this + 0x830) = 0;
    }
}

/* Function start: 0x4368F0 */
void SC_WordSearch::FUN_4368F0() {
    int iVar2 = g_PeriodStateIdx_0046cb90;
    void* pvVar3 = g_GameState_0046aa30;
    if (iVar2 < 0 || *(int*)((int)pvVar3 + 0x98) - 1 < iVar2) {
        ShowError("Invalid gamestate %d", iVar2);
    }
    int gsVal = *(int*)(*(int*)((int)pvVar3 + 0x90) + iVar2 * 4);
    if (gsVal == 0) {
        memset((char*)((int)this + 0xD4), 0, 0x400);
        strcpy((char*)((int)this + 0xD4), "Encounter");
        strcpy((char*)((int)this + 0xF4), "Eye");
        strcpy((char*)((int)this + 0x114), "Spaceship");
        strcpy((char*)((int)this + 0x134), "Wand");
        strcpy((char*)((int)this + 0x154), "Roswell");
        strcpy((char*)((int)this + 0x174), "Two");
        strcpy((char*)((int)this + 0x194), "Abduction");
        strcpy((char*)((int)this + 0x1B4), "Believe");
        strcpy((char*)((int)this + 0x1D4), "Sighting");
        strcpy((char*)((int)this + 0x1F4), "Cover-up");
    } else if (gsVal == 1) {
        memset((char*)((int)this + 0xD4), 0, 0x400);
        strcpy((char*)((int)this + 0xD4), "Encounter");
        strcpy((char*)((int)this + 0xF4), "Eye");
        strcpy((char*)((int)this + 0x114), "Spaceship");
        strcpy((char*)((int)this + 0x134), "Wand");
        strcpy((char*)((int)this + 0x154), "Area 51");
        strcpy((char*)((int)this + 0x174), "Roswell");
        strcpy((char*)((int)this + 0x194), "Information");
        strcpy((char*)((int)this + 0x1B4), "Barca-Lounger");
        strcpy((char*)((int)this + 0x1D4), "Two");
        strcpy((char*)((int)this + 0x1F4), "UFO");
        strcpy((char*)((int)this + 0x214), "Grey-men");
        strcpy((char*)((int)this + 0x234), "Abduction");
        strcpy((char*)((int)this + 0x254), "Sighting");
        strcpy((char*)((int)this + 0x274), "Believe");
        strcpy((char*)((int)this + 0x294), "Cover-up");
    } else if (gsVal == 2) {
        memset((char*)((int)this + 0xD4), 0, 0x400);
        strcpy((char*)((int)this + 0xD4), "Encounter");
        strcpy((char*)((int)this + 0xF4), "Eye");
        strcpy((char*)((int)this + 0x114), "Spaceship");
        strcpy((char*)((int)this + 0x134), "Area 51");
        strcpy((char*)((int)this + 0x154), "Wand");
        strcpy((char*)((int)this + 0x174), "Roswell");
        strcpy((char*)((int)this + 0x194), "Information");
        strcpy((char*)((int)this + 0x1B4), "Barca-Lounger");
        strcpy((char*)((int)this + 0x1D4), "Two");
        strcpy((char*)((int)this + 0x1F4), "Cow");
        strcpy((char*)((int)this + 0x214), "UFO");
        strcpy((char*)((int)this + 0x234), "Grey-men");
        strcpy((char*)((int)this + 0x254), "Abduction");
        strcpy((char*)((int)this + 0x274), "Believe");
        strcpy((char*)((int)this + 0x294), "Bogie");
        strcpy((char*)((int)this + 0x2B4), "Sighting");
        strcpy((char*)((int)this + 0x2D4), "Conspiracy");
        strcpy((char*)((int)this + 0x2F4), "Probe");
        strcpy((char*)((int)this + 0x314), "Nuts");
        strcpy((char*)((int)this + 0x334), "Cover-up");
    }
    *(char*)((int)this + 0x718) = 0;
    *(int*)((int)this + 0x818) = 0;
}

/* Function start: 0x437080 */
int SC_WordSearch::Exit(SC_Message* msg) {
    if (*(int*)msg != handlerId) {
        return 0;
    }
    ((Timer*)((int)this + 0xC0))->Reset();
    int iVar = *(int*)((int)msg + 0x10);
    if (iVar != 0) {
        if (iVar != 7) {
            return 0;
        }
        SendGameMessage(1, handlerId, handlerId, moduleParam, 0x18, 0, 0, 0, 0, 0);
    }
    return 1;
}

int SC_WordSearch::AddMessage(SC_Message*) { return 0; }
int SC_WordSearch::LBLParse(char*) { return 0; }

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
    int* ptr = (int*)((int)this + 0x148);
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
    *(int*)((int)this + 0x178) = 1;
    *(int*)((int)this + 0x2AC) = 1;
    *(int*)((int)this + 0x354) = 1;
    *(int*)((int)this + 0x3FC) = 1;
    *(int*)((int)this + 0x434) = 1;
    *(int*)((int)this + 0x2C8) = 1;
    *(int*)((int)this + 0x2E4) = 1;
    *(int*)((int)this + 0x3C4) = 1;

    // Set game parameters
    *(int*)((int)this + 0x128) = 10;
    *(int*)((int)this + 0x12C) = 0x14;
    *(int*)((int)this + 0x130) = 0x5A;
    *(int*)((int)this + 0x134) = 0xDC;
    *(int*)((int)this + 0x548) = 6;
    *(int*)((int)this + 0x4FC) = 1;
    *(int*)((int)this + 0x54C) = 0x197;
    *(int*)((int)this + 0x550) = 0x5F;
    *(int*)((int)this + 0x554) = 0x1D6;

    InitCombatGrid((int)this);

    if (*(int*)((int)this + 0x114) != 0) {
        SendGameMessage(5, *(int*)((int)this + 0x114), handlerId, moduleParam, 0x1B, 0, 0, 0, 0, 0);
    }
}

extern void __fastcall UpdateWordSearchCursor(int*);
#include "SoundList.h"

/* Function start: 0x42EFC0 */
void SC_WordSearch::Render() {
    Sprite* bgSpr = (Sprite*)*(int*)((int)this + 0x108);
    bgSpr->Do(bgSpr->loc_x, bgSpr->loc_y, 1.0);

    if (*(int*)((int)this + 0x124) != 0 && g_Mouse_0046aa18->m_sprite != 0) {
        g_Mouse_0046aa18->m_sprite->ResetAnimation(0, 0);
    }
    UpdateWordSearchCursor((int*)((int)this + 0x120));
    if (*(int*)((int)this + 0x124) != 0 && g_Mouse_0046aa18->m_sprite != 0) {
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
    if (*(int*)((int)this + 0x548) <= mouseX && mouseX <= *(int*)((int)this + 0x550) &&
        *(int*)((int)this + 0x54C) <= mouseY && mouseY <= *(int*)((int)this + 0x554) &&
        g_Mouse_0046aa18->m_sprite != 0) {
        g_Mouse_0046aa18->m_sprite->ResetAnimation(0x13, 0);
    }

    int local_8 = 0x82;
    int local_4 = 0;
    int* gridCell = (int*)((int)this + 0x13C);
    Sprite** spriteSlot = (Sprite**)((int)this + 0x52C);

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
                        int anim = (*(int*)((int)this + 0x124) == 0) ? 0 : 0xC;
                        mouseSpr->ResetAnimation(anim, 0);
                    }
                } else {
                    if (mouseSpr != 0) {
                        int anim = (*(int*)((int)this + 0x124) == 0) ? 0 : 0xD;
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
                int* statusPtr = (int*)(*(int*)((int)this + 0xA8) + 4);
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

            if (*(int*)((int)this + 0x120) < 1) {
                int sndDone = ((SoundList*)*(void**)((int)this + 0x110))->IsSamplePlaying(1);
                if (sndDone == 0) {
                    int* status = *(int**)((int)this + 0xA8);
                    if (status[1] == 0) {
                        int count = *(int*)((int)this + 0x118) + 1;
                        *(int*)((int)this + 0x118) = count;
                        if (*(int*)((int)this + 0x11C) == count) {
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
        int* cell = (int*)((int)this + (param_2 + param_1 * 6) * 0x1C + 0x140);
        int c = param_2;
        do {
            if (*cell != 0) maxCol = c;
            cell += 7;
            c++;
        } while (c < maxCol);
    }
    // Scan left
    if (minCol < param_2) {
        int* cell = (int*)((int)this + (param_2 + param_1 * 6) * 0x1C + 0x140);
        int c = param_2;
        do {
            if (*cell != 0) minCol = c;
            cell -= 7;
            c--;
        } while (minCol < c);
    }
    // Scan down
    if (param_1 < maxRow) {
        int* cell = (int*)((int)this + (param_2 + param_1 * 6) * 0x1C + 0x140);
        int r = param_1;
        do {
            if (*cell != 0) maxRow = r;
            cell += 0x2A;
            r++;
        } while (r < maxRow);
    }
    // Scan up
    if (minRow < param_1) {
        int* cell = (int*)((int)this + (param_2 + param_1 * 6) * 0x1C + 0x140);
        int r = param_1;
        do {
            if (*cell != 0) minRow = r;
            cell -= 0x2A;
            r--;
        } while (minRow < r);
    }

    int idx = param_2 + param_1 * 6;
    int diagUL = *(int*)((int)this + idx * 0x1C + 0x7C);
    int diagDR = *(int*)((int)this + idx * 0x1C + 0x1CC);
    int diagUR = *(int*)((int)this + idx * 0x1C + 0xB4);
    int diagDL = *(int*)((int)this + idx * 0x1C + 0x204);

    // Clear horizontal span
    if (minCol <= maxCol) {
        int count = maxCol - minCol + 1;
        int* cell = (int*)((int)this + (param_1 * 6 + minCol) * 0x1C + 0x13C);
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
        int* cell = (int*)((int)this + (param_2 + minRow * 6) * 0x1C + 0x13C);
        do {
            total += *cell;
            *cell = 0;
            cell += 0x2A;
            count--;
        } while (count != 0);
    }
    // Clear diagonals
    if (diagUL == 0 && hasDownLeft && hasUpLeft) {
        int val = *(int*)((int)this + idx * 0x1C + 0x78);
        if (val != 0) {
            *(int*)((int)this + idx * 0x1C + 0x78) = 0;
            total += val;
        }
    }
    if (diagDR == 0 && hasUpRight && hasDownLeft) {
        int val = *(int*)((int)this + idx * 0x1C + 0x1C8);
        if (val != 0) {
            *(int*)((int)this + idx * 0x1C + 0x1C8) = 0;
            total += val;
        }
    }
    if (diagUR == 0 && hasDownRight && hasUpLeft) {
        int val = *(int*)((int)this + idx * 0x1C + 0xB0);
        if (val != 0) {
            *(int*)((int)this + idx * 0x1C + 0xB0) = 0;
            total += val;
        }
    }
    if (diagDL == 0 && hasDownRight && hasUpRight) {
        int val = *(int*)((int)this + idx * 0x1C + 0x200);
        if (val != 0) {
            *(int*)((int)this + idx * 0x1C + 0x200) = 0;
            total += val;
        }
    }

    int snd;
    if (total == 0) {
        snd = 0;
    } else {
        ((SoundList*)*(int*)((int)this + 0x110))->Play(1);
        snd = 4 - *(int*)((int)this + 0x120);
        if (snd < 0 || snd > 2) goto skip_sound;
        snd = 9 - *(int*)((int)this + 0x120);
    }
    ((SoundList*)*(int*)((int)this + 0x110))->Play(snd);
skip_sound:
    *(int*)((int)this + idx * 0x1C + 0x13C) = total;
    if (*(int*)((int)this + 0x120) == 1) {
        *(int*)((int)this + 0x120) = 0;
    }
    *(int*)((int)this + 0x124) = 0;
}
