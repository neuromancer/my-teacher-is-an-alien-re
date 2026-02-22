#include "Handler4.h"
#include <string.h>
#include <stdio.h>
#include "Memory.h"
#include "globals.h"
#include "SC_Question.h"
#include "MouseControl.h"
#include "string.h"
#include "InputManager.h"

// External functions - CDData_FormatPath at 0x4195c0
extern "C" char* __cdecl CDData_FormatPath(char*, ...);

/* Function start: 0x40E220 */
Handler4::Handler4() {
    memset(&palette, 0, 0xf0);

    rect1.left = 0;
    rect1.top = 0;
    handlerId = 4;
    needsUpdate = 1;
    rect1.right = 0x197;
    rect1.bottom = 0x1aa;
    rect2.left = 0x1d7;
    rect2.top = 0x3b;
    rect2.right = 0x25c;
    rect2.bottom = 0xbb;
    rect3.top = 0xf1;
    rect3.right = 0x25b;
    rect3.bottom = 0x16d;
    rect4.left = 0x73;
    rect4.top = 0x11d;
    rect4.right = 0x12f;
    rect4.bottom = 0x149;
    rect5.left = 0x32;
    rect3.left = 0x1de;
    rect5.top = 0x14a;
    rect5.right = 0x172;
    rect5.bottom = 0x192;

    palette = new Palette();
    palette->Load(CDData_FormatPath("puzzle1\\Puzztest.col"));

    buttons1 = new Sprite("puzzle1\\buttons1.smk");
    buttons1->loc_x = 0x1dc;
    buttons1->loc_y = 0x147;

    buttons2 = new Sprite("puzzle1\\buttons2.smk");
    buttons2->loc_x = 0x1dc;
    buttons2->loc_y = 0x11c;

    buttons3 = new Sprite("puzzle1\\buttons3.smk");
    buttons3->loc_x = 0x1dc;
    buttons3->loc_y = 0xf1;

    paths1 = new Sprite("puzzle1\\paths1.smk");
    paths1->loc_x = 0x1ee;
    paths1->loc_y = 0x8d;

    paths2 = new Sprite("puzzle1\\paths2.smk");
    paths2->loc_x = 0x1ee;
    paths2->loc_y = 0x70;

    paths3 = new Sprite("puzzle1\\paths3.smk");
    paths3->loc_x = 0x1ee;
    paths3->loc_y = 0x53;

    {
        int count = 3;
        do {
            (&buttons1)[3 - count]->flags &= ~2;
            (&buttons1)[3 - count]->priority = 10;
            (&buttons1)[3 - count]->SetState(3);
            (&buttons1)[3 - count]->SetRange(0, 1, 1);
            (&buttons1)[3 - count]->SetRange(1, 2, 2);
            (&buttons1)[3 - count]->SetRange(2, 3, 3);

            (&paths1)[3 - count]->flags &= ~2;
            (&paths1)[3 - count]->priority = 10;
            (&paths1)[3 - count]->SetState(3);
            (&paths1)[3 - count]->SetRange(0, 1, 1);
            (&paths1)[3 - count]->SetRange(1, 2, 2);
            (&paths1)[3 - count]->SetRange(2, 3, 3);
            count--;
        } while (count != 0);
    }

    puzztest = new Sprite("puzzle1\\puzztest.smk");
    puzztest->flags &= ~2;
    puzztest->priority = 5;
    puzztest->loc_x = 0;
    puzztest->loc_y = 0;

    litdoors = new Sprite("puzzle1\\litdoors.smk");
    litdoors->flags &= ~2;
    litdoors->priority = 10;
    litdoors->flags |= 0x40;
    litdoors->SetState(3);
    litdoors->SetRange(0, 1, 1);
    litdoors->SetRange(1, 2, 2);
    litdoors->SetRange(2, 3, 3);

    lowfloor = new Sprite("puzzle1\\lowfloor.smk");
    lowfloor->loc_x = 0x1d;
    lowfloor->loc_y = 0x150;

    midfloor = new Sprite("puzzle1\\midfloor.smk");
    midfloor->loc_x = 0x2c;
    midfloor->loc_y = 0x130;

    topfloor = new Sprite("puzzle1\\topfloor.smk");
    topfloor->loc_x = 0x5a;
    topfloor->loc_y = 0x11c;

    {
        int count = 3;
        do {
            (&lowfloor)[3 - count]->flags &= ~2;
            (&lowfloor)[3 - count]->priority = 10;
            (&lowfloor)[3 - count]->flags |= 0x40;
            (&lowfloor)[3 - count]->SetState(4);
            (&lowfloor)[3 - count]->SetRange(0, 1, 4);
            (&lowfloor)[3 - count]->SetRange(1, 5, 8);
            (&lowfloor)[3 - count]->SetRange(2, 9, 12);
            (&lowfloor)[3 - count]->SetRange(3, 13, 16);
            count--;
        } while (count != 0);
    }

    for (int i = 0; i < 10; i++) {
        soundStates[i] = 1;
    }
}

/* Function start: 0x40E9A0 */
Handler4::~Handler4() {
    for (int i = 0; i < 3; i++) {
        if ((&lowfloor)[i]) {
            delete (&lowfloor)[i];
            (&lowfloor)[i] = 0;
        }
        if ((&paths1)[i]) {
            delete (&paths1)[i];
            (&paths1)[i] = 0;
        }
        if ((&buttons1)[i]) {
            delete (&buttons1)[i];
            (&buttons1)[i] = 0;
        }
    }

    if (litdoors) {
        delete litdoors;
        litdoors = 0;
    }

    if (puzztest) {
        delete puzztest;
        puzztest = 0;
    }

    if (palette) {
        delete palette;
        palette = 0;
    }
}

/* Function start: 0x40EB80 */
void Handler4::Init(SC_Message* msg) {
    WriteToMessageLog("\nENTER FORCEFIELD PUZZLE");
    InitIconBar(msg);

    Palette* pal = palette;
    if (pal != 0) {
        Palette** pPal = &g_ZBufferManager_0043698c->m_palette;
        if (*pPal != 0) {
            WriteToMessageLog("ddouble palette");
        }
        *pPal = pal;
    }

    if (soundStates[9] != 0) {
        sound2 = new Sample();
        sound2->Load("audio\\Snd0020.wav");
        sound2->Play(100, 0);
    }

    if (soundStates[4] != 0) {
        PlayPuzzleSound(4, 0);
    }
}

/* Function start: 0x40EC80 */
int Handler4::ShutDown(SC_Message* msg) {
    if (puzztest) puzztest->StopAnimationSound();
    if (litdoors) litdoors->StopAnimationSound();

    for (int i = 0; i < 3; i++) {
        (&paths1)[i]->StopAnimationSound();
        (&buttons1)[i]->StopAnimationSound();
        (&lowfloor)[i]->StopAnimationSound();
    }

    if (sound1 != 0) {
        sound1->Unload();
    }
    if (sound1 != 0) {
        delete sound1;
        sound1 = 0;
    }

    if (sound2 != 0) {
        sound2->Unload();
    }
    if (sound2 != 0) {
        delete sound2;
        sound2 = 0;
    }

    CleanupIconBar(msg);
    WriteToMessageLog("EXIT FORCEFIELD PUZZLE\n");
    return 0;
}

/* Function start: 0x40ED50 */
int Handler4::AddMessage(SC_Message* msg) {
    if (CheckButtonClick(msg)) {
        return 1;
    }

    if (msg->mouseX >= 2) {
        InputState* pMouse = g_InputManager_00436968->pMouse;
        int mouseY = 0;
        if (pMouse != 0) {
            mouseY = pMouse->y;
        }
        int mouseX;
        if (pMouse != 0) {
            mouseX = pMouse->x;
        } else {
            mouseX = 0;
        }

        OnClick(mouseX, mouseY);
        int* pFloorState = floorStates;
        GlyphRect* pRect = g_PuzzleRects2_0043d100;
        int i = 0;

        do {
            if (*pFloorState != 0) {
                InputState* pM = g_InputManager_00436968->pMouse;
                int curY = 0;
                if (pM != 0) {
                    curY = pM->y;
                }
                int curX;
                if (pM != 0) {
                    curX = pM->x;
                } else {
                    curX = 0;
                }

                if (pRect->left <= curX && curX <= pRect->right &&
                    pRect->top <= curY && curY <= pRect->bottom) {

                    if (pRect == g_PuzzleRects2_0043d100) {
                        ResetPuzzle();
                    }

                    msg->targetAddress = 8;
                    msg->sourceAddress = (i < 1) ? 2 : 3;

                    if (pRect == g_PuzzleRects2_0043d100) {
                        msg->command = 10;
                    } else {
                        msg->command = handlerId;
                    }

                    if (pRect == g_PuzzleRects2_0043d100) {
                        msg->data = 1;
                    } else {
                        msg->data = moduleParam;
                    }
                    msg->priority = 5;
                }
            }
            pRect++;
            pFloorState++;
            i++;
        } while ((unsigned int)pRect < (unsigned int)&g_PuzzleRects2_0043d100[3]);
    }

    return 1;
}

/* Function start: 0x40EEB0 */
int Handler4::Exit(SC_Message* msg) {
    return handlerId == msg->targetAddress;
}

/* Function start: 0x40EED0 */
void Handler4::Update(int param1, int param2) {
    if (handlerId != param2) {
        return;
    }

    if (sound1 != 0) {
        if (sound1->m_sample == 0 ||
            *(int*)((char*)sound1->m_sample + 0xc) != sound1->m_size ||
            AIL_sample_status(sound1->m_sample) != 4) {
            sound1->Unload();
            sound1 = 0;
        }
    }

    IconBar::Update(param1, param2);

    if (puzztest != 0) {
        puzztest->Do(puzztest->loc_x, puzztest->loc_y, 1.0);
    }

    DisplayButtons();
    DisplayMap();
    if (litdoors != 0) {
        DisplayLitDoors();
    }
    if (initialized != 0) {
        DisplayThisFloorRow();
    }

    g_Mouse_00436978->DrawCursor();
}

/* Function start: 0x40F310 */
void Handler4::ResetPuzzle() {
    int* pFs = floorStates;
    pFs[0] = 0;
    pFs[1] = 0;
    pFs[2] = 0;
    memset(buttonStates, 0, 9 * sizeof(int));
    puzzleSolved = 0;
    initialized = 0;

    Sprite** pFloor = &lowfloor;
    int i = 3;
    do {
        (*pFloor)->SetState2(3);
        pFloor++;
        i--;
    } while (i != 0);

    int* pSound = soundStates;
    int j = 10;
    do {
        *pSound = 1;
        pSound++;
        j--;
    } while (j != 0);
}

/* Function start: 0x40F370 */
void Handler4::PlayPuzzleSound(int index, int loop) {
    char filename[32];

    PlayButtonSound(-1);

    if (index < 0 || index > 9 || index == 9) {
        ShowError("%d is out of sound array range", index);
    }

    if (sound1 != 0) {
        sound1->Unload();
        delete sound1;
        sound1 = 0;
    }

    if (soundStates[index] != 0) {
        sprintf(filename, "audio\\snd%4.4d.wav", index + 11);
        soundStates[index] = loop;
        sound1 = new Sample();
        sound1->Load(filename);
        sound1->Play(100, 1);
    }
}

/* Function start: 0x40F490 */
void Handler4::DisplayButtons() {
    for (int i = 0; i < 9; i++) {
        if (buttonStates[i] != 0) {
            Sprite** ppBtn = &(&buttons1)[i / 3];
            (*ppBtn)->SetState2(i % 3);
            (*ppBtn)->Do((*ppBtn)->loc_x, (*ppBtn)->loc_y, 1.0);
        }
    }
}

/* Function start: 0x40F4F0 */
void Handler4::DisplayMap() {
    if (paths1 == 0 || paths2 == 0 || paths3 == 0) {
        ShowError("Error in SCIpuzz1.cpp - DisplayMap()");
    }
    for (int i = 0; i < 9; i++) {
        if (buttonStates[i] != 0) {
            Sprite** ppPath = &(&paths1)[i / 3];
            (*ppPath)->SetState2(i % 3);
            (*ppPath)->Do((*ppPath)->loc_x, (*ppPath)->loc_y, 1.0);
        }
    }
}

/* Function start: 0x40F580 */
void Handler4::DisplayLitDoors() {
    int i;
    for (i = 0; i < 3; i++) {
        if (floorStates[i] != 0) {
            litdoors->SetState2(i);
            int x = 0;
            if (i != 0) {
                x = ((unsigned int)(i - 1) < 1 ? 0xffffff5a : 0) + 0x138;
            }
            int y = ((unsigned int)(i - 1) < 1 ? 0xe : 0) + 0x60;
            litdoors->Do(x, y, 1.0);
        }
    }
}

/* Function start: 0x40F5F0 */
void Handler4::DisplayThisFloorRow() {
    int floorRow[3];
    int i;
    floorRow[0] = -1;
    floorRow[1] = -1;
    Sprite** pFloor = &lowfloor;
    floorRow[2] = -1;

    if (*pFloor == 0 || midfloor == 0 || topfloor == 0) return;

    for (i = 0; i < 9; i++) {
        if (buttonStates[i] != 0) {
            if (i >= 0 && i <= 2) {
                floorRow[0] = i;
            } else if (i >= 3 && i <= 5) {
                floorRow[1] = i % 3;
            } else if (i >= 6 && i <= 8) {
                floorRow[2] = i % 3;
            }
        }
    }
    if (needsUpdate != 0) {
        int* pRow = floorRow;
        Sprite** pSpr = pFloor;
        needsUpdate = 0;
        do {
            int state = *pRow;
            (*pSpr)->flags |= 0x20;
            if (state < -1 || state > 2) {
                ShowError("Error in SCIpuzz1.cpp - DisplayThisFloorRow:  Invalid case value");
            }
            if (state == -1) state = 3;
            (*pSpr)->SetState2(state);
            pRow++;
            pSpr++;
        } while (pRow < &floorRow[3]);
    }

    i = 3;
    do {
        (*pFloor)->Do((*pFloor)->loc_x, (*pFloor)->loc_y, 1.0);
        pFloor++;
        i--;
    } while (i != 0);
}

/* Function start: 0x40EFB0 */
void Handler4::OnClick(int x, int y) {
    MousePoint mp;
    mp.x = x;
    mp.y = y;

    if (initialized == 0) {
        initialized = 1;
        PlayPuzzleSound(0, 1);
    } else {
        int inRect3 = (rect3.left <= mp.x && mp.x <= rect3.right && rect3.top <= mp.y && mp.y <= rect3.bottom);
        if (inRect3) {
            int i = 0;
            GlyphRect* pRect = g_PuzzleRects1_0043d068;
            do {
                int hit = (pRect->left <= mp.x && mp.x <= pRect->right && pRect->top <= mp.y && mp.y <= pRect->bottom);
                if (hit) {
                    OnButtonClick(i);
                }
                pRect++;
                i++;
            } while ((unsigned int)pRect < (unsigned int)&g_PuzzleRects1_0043d068[9]);
            CheckSolution();
        } else {
            int inRect2 = (rect2.left <= mp.x && mp.x <= rect2.right && rect2.top <= mp.y && mp.y <= rect2.bottom);
            if (inRect2) {
                PlayPuzzleSound(3, 1);
            } else {
                int inRect1 = (rect1.left <= mp.x && mp.x <= rect1.right && rect1.top <= mp.y && mp.y <= rect1.bottom);
                if (inRect1) {
                    GlyphRect* pFlt = g_PuzzleRects2_0043d100;
                    do {
                        int fltHit = (pFlt->left <= mp.x && mp.x <= pFlt->right && pFlt->top <= mp.y && mp.y <= pFlt->bottom);
                        if (fltHit) {
                            PlayPuzzleSound(1, 1);
                        }
                        pFlt++;
                    } while ((unsigned int)pFlt < (unsigned int)&g_PuzzleRects2_0043d100[3]);

                    int inRect4 = (rect4.left <= mp.x && mp.x <= rect4.right && rect4.top <= mp.y && mp.y <= rect4.bottom);
                    if (!inRect4) {
                        int inRect5 = (rect5.left <= mp.x && mp.x <= rect5.right && rect5.top <= mp.y && mp.y <= rect5.bottom);
                        if (inRect5) {
                            PlayPuzzleSound(0, 1);
                        }
                    } else {
                        PlayPuzzleSound(0, 1);
                    }
                }
            }
        }
    }
}

/* Function start: 0x40F1C0 */
void Handler4::OnButtonClick(int buttonIndex) {
    int oldVal = buttonStates[buttonIndex];

    int rowStart = (buttonIndex / 3) * 3;
    PlayPuzzleSound(rowStart / 3 + 6, 1);

    int* pRow = &buttonStates[rowStart];
    pRow[0] = 0;
    pRow[1] = 0;
    pRow[2] = 0;

    if (oldVal == 0) {
        buttonStates[buttonIndex] = 1;
    }
    needsUpdate = 1;
}

/* Function start: 0x40F220 */
int Handler4::CheckSolution() {
    int* pFs = floorStates;
    SolutionEntry* pEntry = g_SolutionData_00435b88;

    int iVar7 = 0;
    pFs[0] = 0;
    pFs[1] = 0;
    pFs[2] = 0;

    while (1) {
        int local_24[9];
        int j = 9;
        int* pT = local_24;
        do {
            *pT = 0;
            pT++;
            j--;
        } while (j != 0);

        j = 3;
        int* pS = (int*)pEntry;
        do {
            local_24[*pS] = 1;
            pS++;
            j--;
        } while (j != 0);

        puzzleSolved = 1;
        int* pV = local_24;
        int* pP = buttonStates;
        do {
            if (*pP != *pV) {
                puzzleSolved = 0;
            }
            pV++;
            pP++;
        } while (pV < &local_24[9]);

        if (puzzleSolved != 0) break;

        pEntry++;
        iVar7++;
        if ((unsigned int)pEntry >= (unsigned int)&g_SolutionData_00435b88[9]) {
            return -1;
        }
    }

    int floorIdx;
    if (iVar7 >= 2) {
        floorIdx = 2;
        if (iVar7 < 5) floorIdx = 1;
    } else {
        floorIdx = 0;
    }

    floorStates[floorIdx] = 1;
    int soundIdx = (iVar7 < 2) ? 5 : 2;
    PlayPuzzleSound(soundIdx, 1);

    return g_SolutionData_00435b88[iVar7].result;
}
