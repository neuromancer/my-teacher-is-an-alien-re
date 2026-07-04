#include "SC_Gauntlet.h"
#include "Sprite.h"
#include "Memory.h"
#include "globals.h"
#include "SpriteAction.h"
#include "MouseControl.h"
#include "SoundList.h"
#include "InputManager.h"
#include "Animation.h"
#include "IconBar.h"
#include "SC_Question.h"
#include <string.h>
#include <stdio.h>
#include "CDData.h"
#include "main.h"
#include "string.h"

extern void __fastcall UpdateWordSearchCursor(int*);
extern char* MakeAnimName(int);

GauntletEntry::GauntletEntry() {
    fields[3] = 0;
    fields[4] = 0;
    fields[5] = 0;
    fields[6] = 0;
    memset(this, 0, 7 * 4);
}

/* Function start: 0x42E540 */
SC_Gauntlet::SC_Gauntlet() {
    memset(moveState, 0, 0x112 * sizeof(int));
    handlerId = 0x50;
}

/* Function start: 0x42E720 */
GauntletBoard::~GauntletBoard() {
    if (fgSprite != 0) {
        delete fgSprite;
        fgSprite = 0;
    }
}

/* Function start: 0x42E7C0 */
SC_Gauntlet::~SC_Gauntlet() {
    ShutDown(0);
}

/* Function start: 0x42E870 */
void SC_Gauntlet::Init(SC_MessageParser* msg) {
    memset(moveState, 0, 0x112 * sizeof(int));
    SC_Combat::Init(msg);
    if (!FileExists("puz_roach")) {
        ShowLoadingScreen();
    }
    strcpy(missionPath, "mis\\pz_roach.mis");
    ParseFile(this, missionPath, (char*)0);
}

/* Function start: 0x42E8F0 */
void SC_Gauntlet::ShutDown(SC_MessageParser* msg) {
    Sprite* spr;
    int i;

    spr = exitSprite;
    if (spr != 0) {
        spr->~Sprite();
        FreeMemory(spr);
        exitSprite = 0;
    }

    spr = board.fgSprite;
    if (spr != 0) {
        spr->~Sprite();
        FreeMemory(spr);
        board.fgSprite = 0;
    }

    i = 6;
    Sprite** p = &cellSprites[0];
    do {
        spr = *p;
        if (spr != 0) {
            spr->~Sprite();
            FreeMemory(spr);
            *p = 0;
        }
        p++;
        i--;
    } while (i != 0);

    if (msg != 0) {
        SendGameMessage(1, handlerId, handlerId, moduleParam, 0x18, 0, 0, 0, 0, 0);
    }

    SC_Combat::ShutDown(msg);
    return;
}

/* Function start: 0x42E9B0 */
void SC_Gauntlet::Update(int param1, int param2) {
    if (handlerId == param2) {
        SC_Combat::Update(param1, param2);
    }
}

/* Function start: 0x42E9D0 */
int SC_Gauntlet::AddMessage(SC_MessageParser* msg) {
    SpriteAction* action = (SpriteAction*)msg;

    if (SC_Combat::AddMessage(msg) != 0) {
        return 1;
    }

    if (action->lastKey == 0x1B) {
        if (savedCommand == 0x2B) {
            statusPtr[2] = 1;
        }
    } else if (action->button1 > 1) {
        int* actionFlags = statusPtr + 5;
        if (*actionFlags != 0) {
            int row = 0;
            int* gep = &entries[0].fields[3];
            do {
                int col = 0;
                do {
                    if (action->mousePos.x >= gep[0] &&
                        action->mousePos.x <= gep[2] &&
                        action->mousePos.y >= gep[1] &&
                        action->mousePos.y <= gep[3] &&
                        gep[-2] == 0) {
                        Sprite* mouseSpr = g_Mouse_0046aa18->m_sprite;
                        if (mouseSpr != 0 && mouseSpr->handle == 0xC) {
                            ProcessGrid(row, col);
                        }
                    }
                    gep = gep + 7;  // next GauntletEntry (0x1C bytes = 7 ints)
                    col++;
                } while (col < 6);
                row++;
            } while (row < 6);

            if (action->mousePos.x >= board.boardBounds.left &&
                action->mousePos.x <= board.boardBounds.right &&
                action->mousePos.y >= board.boardBounds.top &&
                action->mousePos.y <= board.boardBounds.bottom &&
                board.crystalState[1] == 0) {
                Sprite* mouseSpr2 = g_Mouse_0046aa18->m_sprite;
                if (mouseSpr2 != 0) {
                    mouseSpr2->ResetAnimation(0xC, 0);
                }
                board.crystalState[1] = 1;
                if (board.crystalState[0] > 0) {
                    board.crystalState[0]--;
                }
                bgSound->Play(2);
            } else if (action->mousePos.x >= exitBounds.left &&
                       action->mousePos.x <= exitBounds.right &&
                       action->mousePos.y >= exitBounds.top &&
                       action->mousePos.y <= exitBounds.bottom) {
                statusPtr[6] = 1;
            }
        } else {
            *actionFlags = 1;
        }
    }

    return 1;
}

/* Function start: 0x42EB70 */ /* No assembly extracted */
int SC_Gauntlet::Exit(SC_MessageParser* msg) {
    return 0;
}

/* Function start: 0x42EBA0 */
void SC_Gauntlet::ProcessLose() {
    SpriteAction* ptr;

    if (savedCommand != 0x2B) {
        if (statusPtr[1] != 0) {
            ptr = pendingAction;
            if (ptr != 0) {
                delete ptr;
                pendingAction = 0;
            }
            {
                SpriteAction* newAction = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                pendingAction = newAction;
                SC_MessageParser temp;
                temp.targetAddress = (int)newAction;
                ParseFile(&temp, missionPath, "[WIN_LBL]");
            }
        } else {
            ptr = pendingAction;
            if (ptr != 0) {
                delete ptr;
                pendingAction = 0;
            }
            {
                SpriteAction* newAction = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                pendingAction = newAction;
                SC_MessageParser temp;
                temp.targetAddress = (int)newAction;
                ParseFile(&temp, missionPath, "[LOSE_LBL]");
            }
        }
    } else {
        if (statusPtr[1] != 0) {
            ptr = pendingAction;
            if (ptr != 0) {
                delete ptr;
                pendingAction = 0;
            }
            {
                SpriteAction* newAction = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                pendingAction = newAction;
                SC_MessageParser temp;
                temp.targetAddress = (int)newAction;
                ParseFile(&temp, missionPath, "[WIN_LBL_PR]");
            }
        } else if (statusPtr[0] != 0) {
            ptr = pendingAction;
            if (ptr != 0) {
                delete ptr;
                pendingAction = 0;
            }
            {
                SpriteAction* newAction = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                pendingAction = newAction;
                SC_MessageParser temp;
                temp.targetAddress = (int)newAction;
                ParseFile(&temp, missionPath, "[LOSE_LBL_PR]");
            }
        }
    }

    SC_Combat::ProcessLose();
}

/* Function start: 0x42EFC0 */
void SC_Gauntlet::RenderGrid() {
    int sprIdx;
    int cellY;

    bgSprite->Do(bgSprite->loc.x, bgSprite->loc.y, 1.0);

    if (board.crystalState[1] != 0) {
        if (g_Mouse_0046aa18->m_sprite != 0) {
            g_Mouse_0046aa18->m_sprite->ResetAnimation(0, 0);
        }
    }

    UpdateWordSearchCursor(board.crystalState);

    if (board.crystalState[1] != 0) {
        if (g_Mouse_0046aa18->m_sprite != 0) {
            g_Mouse_0046aa18->m_sprite->ResetAnimation(0xC, 0);
        }
    }

    /* Check exit button hover */
    {
        int mouseY;
        int mouseX;
        if (g_InputManager_0046aa08->pMouse != 0) {
            mouseY = g_InputManager_0046aa08->pMouse->y;
        } else {
            mouseY = 0;
        }
        if (g_InputManager_0046aa08->pMouse != 0) {
            mouseX = g_InputManager_0046aa08->pMouse->x;
        } else {
            mouseX = 0;
        }
        if (exitBounds.left <= mouseX && exitBounds.right >= mouseX &&
            exitBounds.top <= mouseY && exitBounds.bottom >= mouseY) {
            if (g_Mouse_0046aa18->m_sprite != 0) {
                g_Mouse_0046aa18->m_sprite->ResetAnimation(0x13, 0);
            }
        }
    }

    /* Render grid entries */
    sprIdx = 0;
    cellY = 0x82;
    GauntletEntry* ge = entries;
    do {
        int cellX = 0x2F;
        Sprite** sprPtr = &cellSprites[sprIdx];
        do {
            int mouseY;
            int mouseX;
            if (g_InputManager_0046aa08->pMouse != 0) {
                mouseY = g_InputManager_0046aa08->pMouse->y;
            } else {
                mouseY = 0;
            }
            if (g_InputManager_0046aa08->pMouse != 0) {
                mouseX = g_InputManager_0046aa08->pMouse->x;
            } else {
                mouseX = 0;
            }
            if (ge->fields[3] <= mouseX && ge->fields[5] >= mouseX &&
                ge->fields[4] <= mouseY && ge->fields[6] >= mouseY) {
                if (ge->fields[1] == 0) {
                    Sprite* mouseSpr = g_Mouse_0046aa18->m_sprite;
                    if (board.crystalState[1] == 0) {
                        if (mouseSpr != 0) {
                            mouseSpr->ResetAnimation(0, 0);
                        }
                    } else {
                        if (mouseSpr != 0) {
                            mouseSpr->ResetAnimation(0xC, 0);
                        }
                    }
                } else {
                    Sprite* mouseSpr = g_Mouse_0046aa18->m_sprite;
                    if (board.crystalState[1] == 0) {
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

            if (ge->fields[0] != 0) {
                (*sprPtr)->ResetAnimation(ge->fields[0] - 1, 0);
                (*sprPtr)->Do(cellY, cellX, 1.0);
                sprIdx++;
                sprPtr++;
                if (statusPtr[1] == 0 && ge->fields[2] != 0 && ge->fields[0] >= 6) {
                    statusPtr[1] = 1;
                }
            }

            ge++;
            cellX += 0x33;
        } while (cellX < 0x161);
        cellY += 0x46;
    } while (cellY < 0x226);

    g_Mouse_0046aa18->DrawCursor();

    if (board.crystalState[0] < 1) {
        if (bgSound->IsSamplePlaying(1) == 0) {
            if (statusPtr[1] == 0) {
                int val = moveState[0] + 1;
                moveState[0] = val;
                if (moveState[1] == val) {
                    statusPtr[0] = 1;
                    return;
                }
                statusPtr[4] = 1;
            }
        }
    }
}

/* Function start: 0x42F220 */
void SC_Gauntlet::ProcessGrid(int row, int col) {
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

    rowMin = row - 2;
    rowMax = row + 2;
    colMax = col + 2;
    colMin = col - 2;
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

    if (row - 1 < 0) {
        canDiagUL = 0;
        canDiagUR = 0;
    }
    if (col - 1 < 0) {
        canDiagUL = 0;
        canDiagDL = 0;
    }
    if (row + 1 > 5) {
        canDiagDR = 0;
        canDiagDL = 0;
    }
    if (col + 1 > 5) {
        canDiagUR = 0;
        canDiagDR = 0;
    }

    /* Scan right: find nearest obstacle from col toward colMax */
    {
        int i = col;
        if (i < colMax) {
            p = &entries[row * 6 + i].fields[1];
            do {
                if (*p != 0) {
                    colMax = i;
                }
                p += 7;
                i++;
            } while (i < colMax);
        }
    }

    /* Scan left: find nearest obstacle from col toward colMin */
    {
        int i = col;
        if (i > colMin) {
            p = &entries[row * 6 + i].fields[1];
            do {
                if (*p != 0) {
                    colMin = i;
                }
                p -= 7;
                i--;
            } while (i > colMin);
        }
    }

    /* Scan down: find nearest obstacle from row toward rowMax */
    {
        int i = row;
        if (i < rowMax) {
            p = &entries[i * 6 + col].fields[1];
            do {
                if (*p != 0) {
                    rowMax = i;
                }
                p += 42;
                i++;
            } while (i < rowMax);
        }
    }

    /* Scan up: find nearest obstacle from row toward rowMin */
    {
        int i = row;
        if (i > rowMin) {
            p = &entries[i * 6 + col].fields[1];
            do {
                if (*p != 0) {
                    rowMin = i;
                }
                p -= 42;
                i--;
            } while (i > rowMin);
        }
    }

    /* Check diagonal neighbors for obstacles */
    index = row * 6 + col;

    if (entries[index - 7].fields[1] != 0) {
        canDiagUL = 0;
    }
    if (entries[index + 5].fields[1] != 0) {
        canDiagDL = 0;
    }
    if (entries[index - 5].fields[1] != 0) {
        canDiagUR = 0;
    }
    if (entries[index + 7].fields[1] != 0) {
        canDiagDR = 0;
    }

    /* Collect horizontal (same row, colMin to colMax) */
    if (colMax >= colMin) {
        int count = colMax - colMin + 1;
        p = &entries[row * 6 + colMin].fields[0];
        do {
            total += *p;
            p += 7;
            *(p - 7) = 0;
            count--;
        } while (count != 0);
    }

    /* Collect vertical (same col, rowMin to rowMax) */
    if (rowMax >= rowMin) {
        int count = rowMax - rowMin + 1;
        p = &entries[rowMin * 6 + col].fields[0];
        do {
            total += *p;
            p += 42;
            *(p - 42) = 0;
            count--;
        } while (count != 0);
    }

    /* Collect diagonals: UL, DL, UR, DR order matches original */
    if (canDiagUL != 0) {
        int val = entries[index - 7].fields[0];
        if (val != 0) {
            entries[index - 7].fields[0] = 0;
            total += val;
        }
    }
    if (canDiagDL != 0) {
        int val = entries[index + 5].fields[0];
        if (val != 0) {
            entries[index + 5].fields[0] = 0;
            total += val;
        }
    }
    if (canDiagUR != 0) {
        int val = entries[index - 5].fields[0];
        if (val != 0) {
            entries[index - 5].fields[0] = 0;
            total += val;
        }
    }
    if (canDiagDR != 0) {
        int val = entries[index + 7].fields[0];
        if (val != 0) {
            entries[index + 7].fields[0] = 0;
            total += val;
        }
    }

    /* Sound after collecting (not before!) */

    /* Play sounds based on result */
    if (total != 0) {
        bgSound->Play(1);
        int sndIdx = 4 - board.crystalState[0];
        if (sndIdx >= 0 && sndIdx <= 2) {
            bgSound->Play(sndIdx + 5);
        }
    } else {
        bgSound->Play(0);
    }

    /* Store result and update state */
    entries[index].fields[0] = total;
    if (board.crystalState[0] == 1) {
        board.crystalState[0] = 0;
    }
    board.crystalState[1] = 0;
}

// Duplicate body; original calls are canonicalized to InitCombatGrid.
void SC_Gauntlet::ResetGrid() {
    int i;
    GauntletEntry* ge;

    ge = entries;
    i = 6;
    do {
        int j = 6;
        do {
            ge->fields[0] = 0;
            ge++;
            j--;
        } while (j != 0);
        i--;
    } while (i != 0);

    entries[5].fields[0] = 1;
    entries[10].fields[0] = 1;
    entries[8].fields[0] = 1;
    entries[20].fields[0] = 1;
    entries[26].fields[0] = 1;
    entries[31].fields[0] = 1;
    board.crystalState[0] = 5;
    board.crystalState[1] = 0;
    cellSprites[6] = 0;
}

/* Function start: 0x42F5E0 */
void SC_Gauntlet::ProcessAction(int action, int* data) {
    switch (action) {
    case 0:
        ProcessLose();
        break;
    case 1:
        if (data[0] == 1) {
            data[0] = 2;
            bgSound->Play(9);
        }
        if (bgSound->IsSamplePlaying(9) == 0) {
            ProcessLose();
        }
        break;
    case 2:
        ProcessLose();
        break;
    case 3:
        data[0] = 0;
        bgSound->Play(4);
        break;
    case 4: {
        char* name = MakeAnimName(0x13DB);
        char* path = (char*)FormatAssetPath(name);
        if (FileExists(path) != 0) {
            Animation anim;
            anim.Play(path, 0);
        }
        ResetGrid();
        if (moveState[0] != 0) {
            bgSound->Play(3);
            bgSound->Play(8);
        }
        data[0] = 0;
        break;
    }
    case 5:
        if (data[0] == 1) {
            data[0] = 2;
            statusPtr[6] = 0;
        }
        RenderGrid();
        break;
    case 6:
        if (data[0] == 1) {
            data[0] = 2;
            if (g_Mouse_0046aa18->m_sprite != 0) {
                g_Mouse_0046aa18->m_sprite->ResetAnimation(0x13, 0);
            }
            statusPtr[5] = 0;
        }
        exitSprite->Do(exitSprite->loc.x, exitSprite->loc.y, 1.0);
        g_Mouse_0046aa18->DrawCursor();
        break;
    default:
        ShowError("SC_Roach::Process_Action - invalid Action=%d, value=%d", action, data[0]);
        break;
    }
}

/* Function start: 0x42F800 */
void SC_Gauntlet::OnProcessEnd() {
    int iRow;
    int* p;
    int iCol;

    SC_Combat::OnProcessEnd();
    p = &entries[0].fields[3];
    iRow = 0x9B;
    do {
        iCol = 0x48;
        do {
            p[0] = iRow;
            p[1] = iCol;
            p += 7;
            p[-5] = iRow + 0x45;
            p[-4] = iCol + 0x32;
            iCol += 0x33;
        } while (iCol < 0x17A);
        iRow += 0x46;
    } while (iRow < 0x23F);

    entries[2].fields[1] = 1;
    entries[13].fields[1] = 1;
    entries[19].fields[1] = 1;
    entries[25].fields[1] = 1;
    entries[27].fields[1] = 1;
    entries[14].fields[1] = 1;
    entries[15].fields[1] = 1;
    entries[23].fields[1] = 1;

    board.boardBounds.left = 0xA;
    board.boardBounds.top = 0x14;
    board.boardBounds.right = 0x5A;
    board.boardBounds.bottom = 0xDC;
    exitBounds.left = 6;
    entries[34].fields[2] = 1;
    exitBounds.top = 0x197;
    exitBounds.right = 0x5F;
    exitBounds.bottom = 0x1D6;

    ResetGrid();

    if (field_0x114 != 0) {
        SendGameMessage(5, field_0x114, handlerId, moduleParam, 0x1b, 0, 0, 0, 0, 0);
    }
}

/* Function start: 0x42F910 */
int SC_Gauntlet::LBLParse(char* line) {
    char label[32];
    label[0] = 0;
    sscanf(line, " %s ", label);

    if (strcmp(label, "TREATS_SPRITE") == 0) {
        Sprite* spr = new Sprite((char*)0);
        board.fgSprite = spr;
        Parser::ProcessFile(spr, this, (char*)0);
    } else if (strcmp(label, "RULES_SPRITE") == 0) {
        Sprite* spr = new Sprite((char*)0);
        exitSprite = spr;
        Parser::ProcessFile(spr, this, (char*)0);
    } else if (strcmp(label, "MAX_TRIES") == 0) {
        sscanf(line, " %s %d ", label, &moveState[1]);
    } else if (strcmp(label, "ROACH_SPRITE") == 0) {
        int i;
        char name[32];
        i = 0;
        SaveFilePosition();
        do {
            RestoreFilePosition();
            sprintf(name, "puz_roach\\Roach%d.smk", i % 3 + 1);
            Sprite* spr = new Sprite(name);
            cellSprites[i] = spr;
            i++;
            Parser::ProcessFile(spr, this, (char*)0);
        } while (i < 6);
    } else if (strcmp(label, "END") == 0) {
        return 1;
    } else {
        SC_Combat::LBLParse(line);
    }

    return 0;
}
