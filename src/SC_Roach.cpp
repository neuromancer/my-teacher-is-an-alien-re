#include "SC_Roach.h"
#include "stubs.h"
#include <new.h>
#include "SC_Question.h"
#include "SpriteAction.h"
#include "InvSlotItem.h"
#include "Sprite.h"
#include "Palette.h"
#include "SoundList.h"
#include "GameState.h"
#include "Memory.h"
#include <string.h>

#include "globals.h"
#include "Timer.h"
#include "MouseControl.h"
#include "InputManager.h"

extern "C" void SendGameMessage(int, int, int, int, int, int, int, int, int, int);

/* Function start: 0x418690 */
NavCrystal::NavCrystal(int id) {
    memset(&crystalId, 0, 0x114 * 4);
    crystalId = id;
    sprintf(g_Buffer_0046aa00, "puz_navcrystal\\crytls%d.smk", id + 1);
    sprite = new Sprite(g_Buffer_0046aa00);
    sprite->flags |= 0x40;
    sprite->priority = 1;
    sprite->ConfigStates(4);
    sprite->ConfigRange(0, 1, 1, 1);
    sprite->ConfigRange(1, 0xB, 0xB, 1);
    sprite->ConfigRange(2, 0x15, 0x15, 1);
    sprite->ConfigRange(3, 0x1F, 0x1F, 1);
}

/* Function start: 0x418850 */
int NavCrystal::LBLParse(char* line) {
    char label[32];
    label[0] = 0;
    sscanf(line, " %s", label);

    if (strcmp(label, "LOC") == 0) {
        int idx, x, y;
        sscanf(line, " %s %d %d %d", label, &idx, &x, &y);
        dimArray1[idx].x = x;
        dimArray1[idx].y = y;
        return 0;
    }
    if (strcmp(label, "PZLOC") == 0) {
        int idx, x, y;
        sscanf(line, " %s %d %d %d", label, &idx, &x, &y);
        dimArray2[idx].x = x;
        dimArray2[idx].y = y;
        return 0;
    }
    if (strcmp(label, "PATTERN") == 0) {
        int rot, base;
        char p0[8], p1[8], p2[8], p3[8];
        sscanf(line, " %s %d %d %s %s %s %s", label, &rot, &base, p0, p1, p2, p3);

        base = base * 4;
        if (p0[0] == '1') patternData[rot * 64 + base * 4 + 0] = 1;
        if (p0[1] == '1') patternData[rot * 64 + base * 4 + 1] = 1;
        if (p0[2] == '1') patternData[rot * 64 + base * 4 + 2] = 1;
        if (p0[3] == '1') patternData[rot * 64 + base * 4 + 3] = 1;
        int r = base + 1;
        if (p1[0] == '1') patternData[rot * 64 + r * 4 + 0] = 1;
        if (p1[1] == '1') patternData[rot * 64 + r * 4 + 1] = 1;
        if (p1[2] == '1') patternData[rot * 64 + r * 4 + 2] = 1;
        if (p1[3] == '1') patternData[rot * 64 + r * 4 + 3] = 1;
        r = base + 2;
        if (p2[0] == '1') patternData[rot * 64 + r * 4 + 0] = 1;
        if (p2[1] == '1') patternData[rot * 64 + r * 4 + 1] = 1;
        if (p2[2] == '1') patternData[rot * 64 + r * 4 + 2] = 1;
        if (p2[3] == '1') patternData[rot * 64 + r * 4 + 3] = 1;
        base = base + 3;
        if (p3[0] == '1') patternData[rot * 64 + base * 4 + 0] = 1;
        if (p3[1] == '1') patternData[rot * 64 + base * 4 + 1] = 1;
        if (p3[2] == '1') patternData[rot * 64 + base * 4 + 2] = 1;
        if (p3[3] == '1') patternData[rot * 64 + base * 4 + 3] = 1;
        return 0;
    }
    if (strcmp(label, "END") == 0) {
        return 1;
    }
    ReportUnknownLabel("Crystal_Piece");
    return 0;
}

CrystalSource::CrystalSource() { memset(this, 0, sizeof(CrystalSource)); }
GridCell::GridCell() { memset(this, 0, sizeof(GridCell)); }

/* Function start: 0x418C20 */
SC_Roach::SC_Roach()
{
    memset(sources, 0, 0x186 * 4);
    handlerId = 0x47;
}

/* Function start: 0x418F10 */
SC_Roach::~SC_Roach() {
    ShutDown(0);
}

/* Function start: 0x419010 */
void SC_Roach::ShutDown(SC_MessageParser* msg) {
    int i;
    Sprite* spr;
    NavCrystal* crystal;

    SC_Combat::ShutDown(msg);

    if (progressObj != 0) {
        delete (Timer*)progressObj;
        progressObj = 0;
    }

    if (circleSprite != 0) {
        delete (Sprite*)circleSprite;
        circleSprite = 0;
    }

    if (barSprite != 0) {
        delete (Sprite*)barSprite;
        barSprite = 0;
    }

    {
        i = 0;
        do {
            sources[i].crystalPtr = 0;
            i++;
        } while (i < 8);
    }

    for (i = 0; i < 8; i++) {
        crystal = crystals[i];
        if (crystal != 0) {
            crystal->~NavCrystal();
            FreeMemory(crystal);
            crystals[i] = 0;
        }
    }

    currentPiece = 0;

    if (msg != 0) {
        SendGameMessage(1, handlerId, handlerId, moduleParam, 0x18, 0, 0, 0, 0, 0);
    }

    return;
}

/* Function start: 0x419220 */
int SC_Roach::AddMessage(SC_MessageParser* msg) {
    SpriteAction* action = (SpriteAction*)msg;
    if (SC_Combat::AddMessage(msg) != 0) {
        return 1;
    }

    if (action->lastKey == 0x1B) {
        if (savedCommand == 0x2B) {
            statusPtr[2] = 1;
        }
    } else if (action->button1 > 1) {
        if (currentPiece != 0) {
            int result = TryPlacePiece((int*)action);
            if (result != 0) {
                bgSound->Play(7);
            } else {
                TryDropOnSource((int*)action);
            }
        } else {
            if (PickFromGrid((int*)action) == 0) {
                PickFromSource((int*)action);
            }
        }
    } else if (action->button2 > 1) {
        if (currentPiece != 0) {
            rotatePending = 1;
            bgSound->Play(8);
            NavCrystal* crys = currentPiece;
            crys->rotation = crys->rotation + 1;
            if (crys->rotation >= 4) {
                crys->rotation = 0;
            }
            Sprite* spr = crys->sprite;
            if (spr != 0) {
                spr->ResetAnimation(crys->rotation, 0);
            }
        }
    }

    int count = 0;
    CrystalSource* source = sources;
    int n = 8;
    do {
        if (source->crystalPtr != 0) {
            count++;
        }
        source++;
        n--;
    } while (n != 0);

    if (currentPiece == 0 && count == 0) {
        statusPtr[1] = 1;
    }

    return 1;
}

/* Function start: 0x4195D0 */
void SC_Roach::UpdateProgress()
{
    unsigned int elapsed = ((Timer*)progressObj)->Update();
    int seconds = elapsed / 1000;
    int phase = seconds / 24;
    int frame = (seconds / 2) % 24;

    if (rotateIndex != phase) {
        int snd;
        if (phase == 1 && rotatePending == 0) {
            snd = 10;
        } else if (phase == 3) {
            snd = 3;
        } else if (phase == 5) {
            snd = 4;
        } else if (phase == 8) {
            snd = 5;
        } else {
            goto skip_sound;
        }
        bgSound->Play(snd);
    }
skip_sound:
    rotateIndex = phase;
    if (phase == 10) {
        circleSprite->ResetAnimation(frame, 0);
        statusPtr[0] = 1;
        return;
    }
    if (phase < 10) {
        barSprite->ResetAnimation(phase, 0);
        circleSprite->ResetAnimation(frame, 0);
    }
}

/* Function start: 0x419690 */
void SC_Roach::RenderBoard()
{
    // Draw background sprite
    if (bgSprite != 0) {
        bgSprite->Do(bgSprite->loc.x, bgSprite->loc.y, 1.0);
    }

    // Draw held piece following cursor, or draw cursor if no piece held
    NavCrystal* crys = currentPiece;
    if (crys != 0) {
        if (crys->sprite != 0) {
            InputState* pm = g_InputManager_0046aa08->pMouse;
            int mouseY = 0;
            if (pm != 0) {
                mouseY = pm->y;
            }
            int mouseX = 0;
            if (pm != 0) {
                mouseX = pm->x;
            }
            crys->sprite->Do(
                mouseX - crys->dimArray1[crys->rotation].x,
                mouseY - crys->dimArray1[crys->rotation].y,
                1.0);
        }
    } else {
        g_Mouse_0046aa18->DrawCursor();
    }

    // Draw crystals in source slots
    int drawn[8];
    {
        int* p = drawn;
        int n = 8;
        do { *p = 0; p++; n--; } while (n != 0);
    }

    int i = 0;
    CrystalSource* src = &sources[0];
    do {
        if (src->crystalPtr != 0) {
            SlimeDim pos;
            pos.x = src->sourceX;
            pos.y = src->sourceY;
            NavCrystal* crys = src->crystalPtr;
            if (crys->sprite != 0) {
                crys->sprite->Do(pos.x, pos.y, 1.0);
            }
        }
        src++;
        i++;
    } while (i < 8);

    // Draw crystals placed on grid
    int cellCount = 0x24;
    int* cellPtr = (int*)grid;
    do {
        int col = 4;
        int* cell = cellPtr;
        do {
            int id = *cell;
            if (id >= 0 && id < 8 && drawn[id] == 0) {
                drawn[id] = 1;
                NavCrystal* crys = crystals[id];
                if (crys != 0 && crys->sprite != 0) {
                    crys->sprite->Do(
                        cellPtr[4] - crys->dimArray2[crys->rotation].x,
                        cellPtr[5] - crys->dimArray2[crys->rotation].y,
                        1.0);
                }
            }
            cell++;
            col--;
        } while (col != 0);
        cellPtr += 8;
        cellCount--;
    } while (cellCount != 0);

    // Draw progress sprites
    circleSprite->Do(circleSprite->loc.x, circleSprite->loc.y, 1.0);
    barSprite->Do(barSprite->loc.x, barSprite->loc.y, 1.0);
}

/* Function start: 0x4198B0 */
int SC_Roach::TryPlacePiece(int* msg)
{
    int found = -1;
    int* cellPtr = (int*)grid + 4;
    int idx = 0;
    int mouseX = msg[7] + 10;

    do {
        if (cellPtr[0] <= mouseX && cellPtr[2] >= mouseX) {
            int mouseY = msg[8] + 10;
            if (cellPtr[1] <= mouseY && cellPtr[3] >= mouseY) {
                found = idx;
                goto found_cell;
            }
        }
        cellPtr += 8;
        idx++;
    } while (idx < 0x24);

found_cell:
    if (found == -1) {
        return 0;
    }

    NavCrystal* crystal = currentPiece;
    int row = 0;
    int* patternBase = crystal->patternData + crystal->rotation * 0x40;

    do {
        int col = 0;
        int patternIdx = row % 4 + (row / 4) * 6 + found;
        int* patternPtr = (int*)grid + patternIdx * 8;
        int* cp = patternBase + row * 4;
        do {
            if (*cp != 0 && *patternPtr != -2) {
                return 0;
            }
            patternPtr++;
            cp++;
            col++;
        } while (col < 4);
        row++;
    } while (row < 0x10);

    int* pattern = patternBase;
    row = 0;
    do {
        int patternIdx = row % 4 + (row / 4) * 6 + found;
        int count = 4;
        int* dest = (int*)grid + patternIdx * 8;
        int zero = 0;
        do {
            NavCrystal* crys = currentPiece;
            if (*pattern != zero) {
                *dest = crys->crystalId;
            }
            dest++;
            pattern++;
            count--;
        } while (count != 0);
        row++;
    } while (row < 0x10);
    currentPiece = 0;
    return 1;
}

/* Function start: 0x419A10 */
int SC_Roach::TryDropOnSource(int* msg)
{
    NavCrystal* crystal = currentPiece;
    int id = crystal->crystalId;
    CrystalSource* src = &sources[id];
    int mouseX = msg[7];
    int hitTest = (src->hitboxLeft <= mouseX &&
        src->hitboxRight >= mouseX &&
        src->hitboxTop <= msg[8] &&
        src->hitboxBottom >= msg[8]);
    if (hitTest != 0) {
        crystal->rotation = 0;
        Sprite* spr = crystal->sprite;
        if (spr != 0) {
            spr->ResetAnimation(0, 0);
        }
        NavCrystal* crys2 = currentPiece;
        int id2 = crys2->crystalId;
        sources[id2].crystalPtr = crys2;
        currentPiece = 0;
        bgSound->Play(7);
        return 1;
    }
    bgSound->Play(9);
    return 0;
}

/* Function start: 0x419AE0 */
int SC_Roach::PickFromGrid(int* msg)
{
    int idx = 0;
    int* cellPtr = (int*)grid + 4;
    int mouseX = msg[7];

    int hit;
    do {
        if (cellPtr[0] <= mouseX && cellPtr[2] >= mouseX &&
            cellPtr[1] <= msg[8] && cellPtr[3] >= msg[8]) {
            hit = 1;
        } else {
            hit = 0;
        }
        if (hit != 0) {
            goto found;
        }
        cellPtr += 8;
        idx++;
    } while (idx < 0x24);

    return 0;

found:
    {
        int xofs = mouseX - ((int*)grid)[idx * 8 + 4];
        int yofs = msg[8] - ((int*)grid)[idx * 8 + 5];
        int orient;
        if (yofs < xofs) {
            orient = 0;
        } else {
            orient = 3;
        }

        int crystalId = ((int*)grid)[orient + idx * 8];
        if (crystalId < 0) {
            return 0;
        }

        currentPiece = crystals[crystalId];
    }

    int* gridPtr = (int*)grid;
    int count = 0x24;
    do {
        int* cell = gridPtr;
        int i = 4;
        do {
            if (currentPiece->crystalId == *cell) {
                *cell = -2;
            }
            cell++;
            i--;
        } while (i != 0);
        gridPtr += 8;
        count--;
    } while (count != 0);

    bgSound->Play(6);
    return 1;
}

/* Function start: 0x419BC0 */
int SC_Roach::PickFromSource(int* msg)
{
    int idx = 0;
    int mouseX = msg[7];
    CrystalSource* src = sources;

    do {
        int hit;
        if (src->pickupLeft <= mouseX && src->pickupRight >= mouseX &&
            src->pickupTop <= msg[8] && src->pickupBottom >= msg[8]) {
            hit = 1;
        } else {
            hit = 0;
        }
        if (hit != 0) {
            goto found;
        }
        src++;
        idx++;
    } while (idx < 8);

    return 0;

found:
    currentPiece = sources[idx].crystalPtr;
    sources[idx].crystalPtr = 0;
    bgSound->Play(6);
    return 1;
}

/* Function start: 0x419D70 */
void SC_Roach::OnProcessEnd()
{
    SC_Combat::OnProcessEnd();

    int row = 0;
    SlimeDim cellSize;
    cellSize.x = 0x2a;
    cellSize.y = 0x2a;
    SlimeDim start;
    start.x = 0xce;
    start.y = 0x40;
    int* gridPtr = (int*)grid + 4;

    do {
        int col = 0;
        int* cell = gridPtr;
        do {
            SlimeDim sd;
            sd.x = col * cellSize.x + start.x;
            sd.y = row * cellSize.y + start.y;
            cell[0] = sd.x;
            cell[1] = sd.y;
            cell[2] = cellSize.x + sd.x;
            cell[3] = cellSize.y + sd.y;
            cell += 8;
            col++;
        } while (col < 6);
        gridPtr += 0x30;
        row++;
    } while (row < 6);

    sources[0].crystalPtr = crystals[0];
    sources[1].crystalPtr = crystals[1];
    sources[2].crystalPtr = crystals[2];
    sources[3].crystalPtr = crystals[3];
    sources[4].crystalPtr = crystals[4];
    sources[5].crystalPtr = crystals[5];
    currentPiece = 0;
    sources[6].crystalPtr = crystals[6];
    sources[7].crystalPtr = crystals[7];

    progressObj = new Timer();

    SendGameMessage(5, field_0x114,
        handlerId, moduleParam,
        0x1b, 0, 0, 0, 0, 0);
}

/* Function start: 0x418FB0 */
void SC_Roach::Init(SC_MessageParser* msg) {
    memset(&sources, 0, 0x186 * sizeof(int));
    SC_Combat::Init(msg);
    strcpy(missionPath, "mis\\pz_crystal.mis");
    ParseFile(this, missionPath, (char*)0);
}

/* Function start: 0x419200 */
void SC_Roach::Update(int p1, int p2) {
    if (handlerId == p2) {
        SC_Combat::Update(p1, p2);
    }
}

/* Function start: 0x419350 */
int SC_Roach::Exit(SC_MessageParser* msg) {
    SpriteAction* action = (SpriteAction*)msg;
    if (action->addressType != handlerId) {
        return 0;
    }
    SC_Combat::Exit(msg);
    return 1;
}

/* Function start: 0x419380 */
void SC_Roach::ProcessLose() {
    if (statusPtr[1] != 0) {
        if (pendingAction != 0) {
            delete pendingAction;
            pendingAction = 0;
        }
        SpriteAction* newAction = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        pendingAction = newAction;
        SC_MessageParser temp;
        temp.targetAddress = (int)newAction;
        ParseFile(&temp, "mis\\pz_crystal.mis", "[WIN_MESSAGE]");
    } else if (statusPtr[0] != 0) {
        if (pendingAction != 0) {
            delete pendingAction;
            pendingAction = 0;
        }
        SpriteAction* newAction = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        pendingAction = newAction;
        SC_MessageParser temp;
        temp.targetAddress = (int)newAction;
        ParseFile(&temp, "mis\\pz_crystal.mis", "[LOSE_MESSAGE]");
    }

    if (savedCommand == 0x2B) {
        ((int*)pendingAction)[2] = 0x2B;
        ((int*)pendingAction)[3] = savedMsgData;
    }

    SC_Combat::ProcessLose();
}

/* Function start: 0x419C40 */
void SC_Roach::ProcessAction(int action, int* data) {
    switch (action) {
    case 0:
        if (*data == 1) {
            bgSound->Play(2);
            (*data)++;
            statusPtr[4] = 0;
        }
        if (bgSound->IsSamplePlaying(2) == 0) {
            ProcessLose();
            return;
        }
        break;
    case 1:
        if (*data == 1) {
            bgSound->Play(1);
            (*data)++;
            statusPtr[4] = 0;
            bgSprite->ResetAnimation(1, 0);
        }
        if (bgSound->IsSamplePlaying(1) == 0) {
            ProcessLose();
            return;
        }
        break;
    case 2:
        ProcessLose();
        return;
    case 3:
        *data = 0;
        ((Timer*)progressObj)->Reset();
        bgSound->Play(0);
        return;
    case 4:
        UpdateProgress();
        return;
    case 5:
        RenderBoard();
        break;
    default:
        ShowError("SC_CrystalPuzzle::Process_Action - invalid Action=%d, value=%d", action, *data);
        return;
    }
}

/* Function start: 0x419F50 */
int SC_Roach::LBLParse(char* line)
{
    char label[32];
    label[0] = 0;
    sscanf(line, "%s", label);

    if (strcmp(label, "CIRCLE_SPRITE") == 0) {
        circleSprite = new Sprite((char*)0);
        Parser::ProcessFile(circleSprite, this, (char*)0);
    } else if (strcmp(label, "BAR_SPRITE") == 0) {
        barSprite = new Sprite((char*)0);
        Parser::ProcessFile(barSprite, this, (char*)0);
    } else if (strcmp(label, "SOURCEPICKUP") == 0) {
        int idx;
        {
            Rect rc;
            sscanf(line, " %s %d %d %d %d %d", label, &idx, &rc.left, &rc.top, &rc.right, &rc.bottom);
            sources[idx].pickupLeft = rc.left;
            sources[idx].pickupTop = rc.top;
            sources[idx].pickupRight = rc.right;
            sources[idx].pickupBottom = rc.bottom;
        }
    } else if (strcmp(label, "SOURCE") == 0) {
        int idx;
        {
            SlimeDim wh;
            {
                Rect rc;
                sscanf(line, " %s %d %d %d %d %d %d %d", label, &idx, &rc.left, &rc.top, &rc.right, &rc.bottom, &wh.x, &wh.y);
                sources[idx].hitboxLeft = rc.left;
                sources[idx].hitboxTop = rc.top;
                sources[idx].hitboxRight = rc.right;
                sources[idx].hitboxBottom = rc.bottom;
            }
            sources[idx].sourceX = wh.x;
            sources[idx].sourceY = wh.y;
        }
    } else if (strcmp(label, "PATTERN") == 0) {
        int idx;
        char p0[8], p1[8], p2[8], p3[8], p4[8], p5[8];
        sscanf(line, " %s %d %s %s %s %s %s %s", label, &idx, p0, p1, p2, p3, p4, p5);

        int* dest = (int*)grid + idx * 48;
        if (p0[0] == '1') dest[0] = -1; else dest[0] = -2;
        if (p0[1] == '1') dest[1] = -1; else dest[1] = -2;
        if (p0[2] == '1') dest[2] = -1; else dest[2] = -2;
        if (p0[3] == '1') dest[3] = -1; else dest[3] = -2;

        dest = (int*)grid + idx * 48 + 8;
        if (p1[0] == '1') dest[0] = -1; else dest[0] = -2;
        if (p1[1] == '1') dest[1] = -1; else dest[1] = -2;
        if (p1[2] == '1') dest[2] = -1; else dest[2] = -2;
        if (p1[3] == '1') dest[3] = -1; else dest[3] = -2;

        dest = (int*)grid + idx * 48 + 16;
        if (p2[0] == '1') dest[0] = -1; else dest[0] = -2;
        if (p2[1] == '1') dest[1] = -1; else dest[1] = -2;
        if (p2[2] == '1') dest[2] = -1; else dest[2] = -2;
        if (p2[3] == '1') dest[3] = -1; else dest[3] = -2;

        dest = (int*)grid + idx * 48 + 24;
        if (p3[0] == '1') dest[0] = -1; else dest[0] = -2;
        if (p3[1] == '1') dest[1] = -1; else dest[1] = -2;
        if (p3[2] == '1') dest[2] = -1; else dest[2] = -2;
        if (p3[3] == '1') dest[3] = -1; else dest[3] = -2;

        dest = (int*)grid + idx * 48 + 32;
        if (p4[0] == '1') dest[0] = -1; else dest[0] = -2;
        if (p4[1] == '1') dest[1] = -1; else dest[1] = -2;
        if (p4[2] == '1') dest[2] = -1; else dest[2] = -2;
        if (p4[3] == '1') dest[3] = -1; else dest[3] = -2;

        dest = (int*)grid + idx * 48 + 40;
        if (p5[0] == '1') dest[0] = -1; else dest[0] = -2;
        if (p5[1] == '1') dest[1] = -1; else dest[1] = -2;
        if (p5[2] == '1') dest[2] = -1; else dest[2] = -2;
        if (p5[3] == '1') {
            dest[3] = -1;
        } else {
            dest[3] = -2;
        }
    } else if (strcmp(label, "CRYSTAL") == 0) {
        int idx;
        sscanf(line, " %s %d", label, &idx);
        NavCrystal* crystal = crystals[idx];
        if (crystal != 0) {
            crystal->~NavCrystal();
            FreeMemory(crystal);
            crystals[idx] = 0;
        }
        NavCrystal* obj = new NavCrystal(idx);
        crystals[idx] = obj;
        Parser::ProcessFile((Parser*)obj, this, (char*)0);
    } else if (strcmp(label, "END") == 0) {
        return 1;
    } else {
        SC_Combat::LBLParse(line);
    }

    return 0;
}
