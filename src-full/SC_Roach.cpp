#include "SC_Roach.h"
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

extern "C" extern GameState* g_GameState_0046aa30;
#include "Timer.h"
extern char* g_Buffer_0046aa00;
extern void* g_Mouse_0046aa18;

extern "C" void SendGameMessage(int, int, int, int, int, int, int, int, int, int);

/* Function start: 0x418690 */
NavCrystal::NavCrystal(int id) {
    memset(&crystalId, 0, 0x114 * 4);
    crystalId = id;
    sprintf(g_Buffer_0046aa00, "puz\\navcrystal\\crytls_%d.smk", id + 1);
    sprite = new Sprite(g_Buffer_0046aa00);
    sprite->flags |= 0x40;
    sprite->priority = 1;
    sprite->ConfigStates(4);
    sprite->ConfigRange(0, 1, 1, 1);
    sprite->ConfigRange(1, 0xB, 0xB, 1);
    sprite->ConfigRange(2, 0x15, 0x15, 1);
    sprite->ConfigRange(3, 0x1F, 0x1F, 1);
}

CrystalSource::CrystalSource() { memset(this, 0, sizeof(CrystalSource)); }
GridCell::GridCell() { memset(this, 0, sizeof(GridCell)); }

/* Function start: 0x418C20 */
SC_Roach::SC_Roach()
{
    memset(&grid, 0, 0x186 * 4);
    handlerId = 0x47;
}

/* Function start: 0x418F10 */
SC_Roach::~SC_Roach() {
    ShutDown(0);
}

/* Function start: 0x419010 */
int SC_Roach::ShutDown(SC_Message* msg) {
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
        int* srcPtr = (int*)((char*)this + 0x140);
        i = 8;
        do {
            *srcPtr = 0;
            srcPtr = (int*)((char*)srcPtr + 0x2C);
            i--;
        } while (i != 0);
    }

    for (i = 0; i < 8; i++) {
        crystal = (NavCrystal*)crystals[i];
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

    return 0;
}

/* Function start: 0x419220 */
int SC_Roach::AddMessage(SC_Message* msg) {
    int* msgData;
    int i;
    int count;
    int* srcPtr;
    int result;

    msgData = (int*)msg;
    if (SC_Combat::AddMessage(msg) != 0) {
        return 1;
    }

    if (msgData[0xB] == 0x1B) {
        if (savedCommand == 0x2B) {
            *(int*)(*(int*)((char*)this + 0xA8) + 8) = 1;
        }
    } else if (msgData[9] >= 2) {
        if (currentPiece == 0) {
            result = PickFromGrid(msgData);
            if (result == 0) {
                PickFromSource(msgData);
            }
        } else {
            result = TryPlacePiece(msgData);
            if (result == 0) {
                result = TryDropOnSource(msgData);
                if (result == 0) {
                    ((Palette*)*(int*)((char*)this + 0x110))->PlaySound(8);
                }
            }
        }
    } else if (msgData[0xA] >= 2) {
        if (currentPiece != 0) {
            ((Palette*)*(int*)((char*)this + 0x110))->PlaySound(7);
            field_0x728 = 1;
            field_0x72C = (field_0x72C + 1) % 4;
            Sprite* spr = (Sprite*)*(int*)((char*)currentPiece + 0x4D8);
            if (spr != 0) {
                spr->ResetAnimation(field_0x72C, 0);
            }
        }
    }

    count = 0;
    srcPtr = (int*)((char*)this + 0x140);
    i = 8;
    do {
        if (*srcPtr != 0) {
            count++;
        }
        srcPtr = (int*)((char*)srcPtr + 0x2C);
        i--;
    } while (i != 0);

    if (currentPiece == 0 && count == 0) {
        *(int*)(*(int*)((char*)this + 0xA8) + 4) = 1;
    }

    return 1;
}

/* Function start: 0x4198B0 */
int SC_Roach::TryPlacePiece(int* msg)
{
    int* cellPtr = (int*)((char*)this + 0x288);
    int found = -1;
    int idx = 0;
    int mouseX = msg[7] + 10;

    do {
        if (cellPtr[0] <= mouseX && mouseX <= cellPtr[2]) {
            int mouseY = msg[8] + 10;
            if (cellPtr[1] <= mouseY && mouseY <= cellPtr[3]) {
                found = idx;
                break;
            }
        }
        cellPtr += 8;
        idx++;
        found = -1;
    } while (idx < 0x24);

    if (found == -1) {
        return 0;
    }

    int crystalId = *(int*)(*(int*)((char*)this + 0x6f8) + 0x94);
    int stride = crystalId * 0x100;
    int* checkPtr = (int*)(stride + 0x98 + *(int*)((char*)this + 0x6f8));
    unsigned int row = 0;

    do {
        int col = 0;
        unsigned int urow = row;
        unsigned int sign = (int)urow >> 31;
        int* patternPtr = (int*)(((((urow ^ sign) - sign) & 3 ^ sign) - sign +
                          ((int)(urow + (sign & 3)) >> 2) * 6 + found) * 0x20 +
                          0x278 + (int)this);
        int* cp = checkPtr;
        do {
            if (*cp != 0 && *patternPtr != -2) {
                return 0;
            }
            patternPtr++;
            cp++;
            col++;
        } while (col < 4);
        checkPtr += 4;
        row++;
        if ((int)row > 0xf) {
            int placeStride = stride;
            row = 0;
            do {
                sign = (int)row >> 31;
                int count = 4;
                int* dest = (int*)(((((row ^ sign) - sign) & 3 ^ sign) - sign +
                             ((int)(row + (sign & 3)) >> 2) * 6 + found) * 0x20 +
                             0x278 + (int)this);
                do {
                    if (*(int*)(*(int*)((char*)this + 0x6f8) + 0x98 + placeStride) != 0) {
                        *dest = *(int*)(*(int*)((char*)this + 0x6f8) + 0x90);
                    }
                    dest++;
                    placeStride += 4;
                    count--;
                } while (count != 0);
                row++;
            } while ((int)row < 0x10);
            *(int*)((char*)this + 0x6f8) = 0;
            return 1;
        }
    } while (1);
}

/* Function start: 0x419A10 */
int SC_Roach::TryDropOnSource(int* msg)
{
    int* crystal = *(int**)((char*)this + 0x6f8);
    int id = *(int*)(crystal + 0x90 / 4);
    int mouseX = msg[7];

    if (mouseX < *(int*)((char*)this + id * 0x2c + 0x118) ||
        *(int*)((char*)this + id * 0x2c + 0x120) < mouseX ||
        msg[8] < *(int*)((char*)this + id * 0x2c + 0x11c) ||
        *(int*)((char*)this + id * 0x2c + 0x124) < msg[8]) {
        bgSound->Play(9);
        return 0;
    }

    *(int*)(crystal + 0x94 / 4) = 0;
    if (*(void**)(crystal + 0x4d8 / 4) != 0) {
        ((Sprite*)*(void**)(crystal + 0x4d8 / 4))->ResetAnimation(0, 0);
    }
    *(int*)(*(int*)((char*)this + 0x6f8) + 0x90) * 0x2c;
    *(int*)((char*)this + *(int*)(*(int*)((char*)this + 0x6f8) + 0x90) * 0x2c + 0x140) =
        *(int*)((char*)this + 0x6f8);
    *(int*)((char*)this + 0x6f8) = 0;
    bgSound->Play(7);
    return 1;
}

/* Function start: 0x419AE0 */
int SC_Roach::PickFromGrid(int* msg)
{
    int idx = 0;
    int* cellPtr = (int*)((char*)this + 0x288);
    int mouseX = msg[7];

    while (1) {
        if (mouseX >= cellPtr[0] && cellPtr[2] >= mouseX &&
            msg[8] >= cellPtr[1] && cellPtr[3] >= msg[8]) {
            break;
        }
        cellPtr += 8;
        idx++;
        if (idx > 0x23) {
            return 0;
        }
    }

    int orient = 0;
    if (mouseX - *(int*)((char*)this + idx * 0x20 + 0x288) <=
        msg[8] - *(int*)((char*)this + idx * 0x20 + 0x28c)) {
        orient = 3;
    }

    int crystalId = *(int*)((char*)this + (orient + idx * 8) * 4 + 0x278);
    if (crystalId < 0) {
        return 0;
    }

    int* gridPtr = (int*)((char*)this + 0x278);
    *(int*)((char*)this + 0x6f8) = *(int*)((char*)this + crystalId * 4 + 0x6fc);
    int count = 0x24;
    do {
        int i = 4;
        int* cell = gridPtr;
        do {
            if (*(int*)(*(int*)((char*)this + 0x6f8) + 0x90) == *cell) {
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
    int* srcPtr = (int*)((char*)this + 0x128);
    int idx = 0;

    while (1) {
        if (msg[7] >= srcPtr[0] && srcPtr[2] >= msg[7] &&
            msg[8] >= srcPtr[1] && srcPtr[3] >= msg[8]) {
            break;
        }
        srcPtr += 0xb;
        idx++;
        if (idx > 7) {
            return 0;
        }
    }

    int* assignPtr = (int*)((char*)this + idx * 0x2c + 0x140);
    *(int*)((char*)this + 0x6f8) = *assignPtr;
    *assignPtr = 0;
    bgSound->Play(6);
    return 1;
}

/* Function start: 0x419D70 */
void SC_Roach::OnProcessEnd()
{
    int* cell;
    int row;
    int col;
    int cellH;
    int cellW;
    int* gridPtr;
    int startY;
    int startX;

    SC_Combat::OnProcessEnd();

    row = 0;
    cellW = 0x2a;
    cellH = 0x2a;
    startX = 0xce;
    startY = 0x40;
    gridPtr = (int*)((char*)this + 0x288);

    do {
        col = 0;
        cell = gridPtr;
        do {
            {
                SlimeDim sd;
                sd.field_0 = col * cellW + startX;
                sd.field_4 = row * cellH + startY;
                cell[0] = sd.field_0;
                cell[1] = sd.field_4;
                cell[2] = sd.field_0 + cellW;
                cell[3] = sd.field_4 + cellH;
            }
            cell += 8;
            col++;
        } while (col < 6);
        gridPtr += 0x30;
        row++;
    } while (row < 6);

    // Copy initial crystal assignments to source slots
    sources[0].crystalPtr = (int)crystals[0];
    sources[1].crystalPtr = (int)crystals[1];
    sources[2].crystalPtr = (int)crystals[2];
    sources[3].crystalPtr = (int)crystals[3];
    sources[4].crystalPtr = (int)crystals[4];
    sources[5].crystalPtr = (int)crystals[5];
    currentPiece = 0;
    sources[6].crystalPtr = (int)crystals[6];
    sources[7].crystalPtr = (int)crystals[7];

    progressObj = new Timer();

    SendGameMessage(5, *(int*)((char*)this + 0x114),
        *(int*)((char*)this + 0x90), *(int*)((char*)this + 0x94),
        0x1b, 0, 0, 0, 0, 0);
}

void SC_Roach::Init(SC_Message* msg) {}
void SC_Roach::Update(int p1, int p2) {}
int SC_Roach::Exit(SC_Message* msg) { return 0; }
void SC_Roach::ProcessLose() {}
void SC_Roach::ProcessAction(int action, int* data) {}

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
                sscanf(line, " %s %d %d %d %d %d %d %d", label, &idx, &rc.left, &rc.top, &rc.right, &rc.bottom, &wh.field_0, &wh.field_4);
                sources[idx].hitboxLeft = rc.left;
                sources[idx].hitboxTop = rc.top;
                sources[idx].hitboxRight = rc.right;
                sources[idx].hitboxBottom = rc.bottom;
            }
            sources[idx].field_0x20 = wh.field_0;
            sources[idx].field_0x24 = wh.field_4;
        }
    } else if (strcmp(label, "PATTERN") == 0) {
        int idx;
        char p0[8], p1[8], p2[8], p3[8], p4[8], p5[8];
        sscanf(line, " %s %d %s %s %s %s %s %s", label, &idx, p0, p1, p2, p3, p4, p5);

        int* dest = (int*)((char*)this + idx * 0xC0 + 0x278);
        if (p0[0] == '1') dest[0] = -1; else dest[0] = -2;
        if (p0[1] == '1') dest[1] = -1; else dest[1] = -2;
        if (p0[2] == '1') dest[2] = -1; else dest[2] = -2;
        if (p0[3] == '1') dest[3] = -1; else dest[3] = -2;

        dest = (int*)((char*)this + idx * 0xC0 + 0x298);
        if (p1[0] == '1') dest[0] = -1; else dest[0] = -2;
        if (p1[1] == '1') dest[1] = -1; else dest[1] = -2;
        if (p1[2] == '1') dest[2] = -1; else dest[2] = -2;
        if (p1[3] == '1') dest[3] = -1; else dest[3] = -2;

        dest = (int*)((char*)this + idx * 0xC0 + 0x2B8);
        if (p2[0] == '1') dest[0] = -1; else dest[0] = -2;
        if (p2[1] == '1') dest[1] = -1; else dest[1] = -2;
        if (p2[2] == '1') dest[2] = -1; else dest[2] = -2;
        if (p2[3] == '1') dest[3] = -1; else dest[3] = -2;

        dest = (int*)((char*)this + idx * 0xC0 + 0x2D8);
        if (p3[0] == '1') dest[0] = -1; else dest[0] = -2;
        if (p3[1] == '1') dest[1] = -1; else dest[1] = -2;
        if (p3[2] == '1') dest[2] = -1; else dest[2] = -2;
        if (p3[3] == '1') dest[3] = -1; else dest[3] = -2;

        dest = (int*)((char*)this + idx * 0xC0 + 0x2F8);
        if (p4[0] == '1') dest[0] = -1; else dest[0] = -2;
        if (p4[1] == '1') dest[1] = -1; else dest[1] = -2;
        if (p4[2] == '1') dest[2] = -1; else dest[2] = -2;
        if (p4[3] == '1') dest[3] = -1; else dest[3] = -2;

        dest = (int*)((char*)this + idx * 0xC0 + 0x318);
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
        NavCrystal* crystal = (NavCrystal*)crystals[idx];
        if (crystal != 0) {
            crystal->~NavCrystal();
            FreeMemory(crystal);
            crystals[idx] = 0;
        }
        void* mem = operator new(0x4E0);
        NavCrystal* obj = 0;
        if (mem != 0) {
            obj = new(mem) NavCrystal(idx);
        }
        crystals[idx] = (int*)obj;
        Parser::ProcessFile((Parser*)obj, this, (char*)0);
    } else if (strcmp(label, "END") == 0) {
        return 1;
    } else {
        SC_Combat::LBLParse(line);
    }

    return 0;
}
