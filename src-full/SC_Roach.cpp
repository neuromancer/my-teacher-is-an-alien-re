#include "SC_Roach.h"
#include <new.h>
#include "SC_Question.h"
#include "SpriteAction.h"
#include "Sprite.h"
#include "Palette.h"
#include "SoundList.h"
#include "GameState.h"
#include "Memory.h"
#include <string.h>

extern "C" extern GameState* DAT_0046aa30;
extern void* __fastcall FUN_00421880(void*, int);            // progress obj ctor
extern void __fastcall FUN_0044c740(void*);                  // Sprite dtor
extern void __fastcall FUN_004218b0(void*);                  // Timer dtor
extern char* DAT_0046aa00;
extern void* DAT_0046aa18;

extern "C" void SendGameMessage(int, int, int, int, int, int, int, int, int, int);

/* Function start: 0x418690 */
NavCrystal::NavCrystal(int id) {
    memset(&crystalId, 0, 0x114 * 4);
    crystalId = id;
    sprintf(DAT_0046aa00, "puz\\navcrystal\\crytls_%d.smk", id + 1);
    sprite = new Sprite(DAT_0046aa00);
    sprite->flags |= 0x40;
    sprite->priority = 1;
    sprite->ConfigStates(4);
    sprite->ConfigRange(0, 1, 1, 1);
    sprite->ConfigRange(1, 0xB, 0xB, 1);
    sprite->ConfigRange(2, 0x15, 0x15, 1);
    sprite->ConfigRange(3, 0x1F, 0x1F, 1);
}

/* Function start: 0x418C20 */
SC_Roach::SC_Roach()
{
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
        FUN_004218b0(progressObj);
        FreeMemory(progressObj);
        progressObj = 0;
    }

    if (circleSprite != 0) {
        FUN_0044c740(circleSprite);
        FreeMemory(circleSprite);
        circleSprite = 0;
    }

    if (barSprite != 0) {
        FUN_0044c740(barSprite);
        FreeMemory(barSprite);
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

    i = 0;
    int** crystalArr = (int**)&crystals[0];
    do {
        crystal = (NavCrystal*)*crystalArr;
        if (crystal != 0) {
            *(int*)crystal = 0x461320;
            spr = crystal->sprite;
            if (spr != 0) {
                FUN_0044c740(spr);
                FreeMemory(spr);
                crystal->sprite = 0;
            }
            FreeMemory(crystal);
            *crystalArr = 0;
        }
        crystalArr++;
        i++;
    } while (i < 8);

    currentPiece = 0;

    if (msg != 0) {
        SendGameMessage(0, 0, 0, 0, 0x1B, 0, 0, 0, 0, 0);
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
    SC_Combat::OnProcessEnd();

    int row = 0;
    int cellW = 0x2a;
    int cellH = 0x2a;
    int startX = 0xce;
    int startY = 0x40;
    int* gridPtr = (int*)((char*)this + 0x288);

    do {
        int col = 0;
        int* cell = gridPtr;
        do {
            cell[0] = col * cellW + startX;
            cell[1] = row * cellH + startY;
            cell[2] = cell[0] + cellW;
            cell[3] = cell[1] + cellH;
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

    // Allocate progress object (0x14 bytes)
    void* mem = operator new(0x14);
    void* obj = 0;
    if (mem != 0) {
        obj = FUN_00421880(mem, 0);
    }
    progressObj = obj;

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
        int idx, left, top, right, bottom;
        left = 0; top = 0; right = 0; bottom = 0;
        sscanf(line, " %s %d %d %d %d %d", label, &idx, &left, &top, &right, &bottom);
        sources[idx].pickupLeft = left;
        sources[idx].pickupTop = top;
        sources[idx].pickupRight = right;
        sources[idx].pickupBottom = bottom;
    } else if (strcmp(label, "SOURCE") == 0) {
        int idx;
        int left = 0, top = 0, right = 0, bottom = 0;
        int w = 0, h = 0;
        sscanf(line, " %s %d %d %d %d %d %d %d", label, &idx, &left, &top, &right, &bottom, &w, &h);
        sources[idx].hitboxLeft = left;
        sources[idx].hitboxTop = top;
        sources[idx].hitboxRight = right;
        sources[idx].hitboxBottom = bottom;
        sources[idx].field_0x20 = w;
        sources[idx].field_0x24 = h;
    } else if (strcmp(label, "PATTERN") == 0) {
        int idx;
        char p0[8], p1[8], p2[8], p3[8], p4[8], p5[8];
        sscanf(line, " %s %d %s %s %s %s %s %s", label, &idx, p0, p1, p2, p3, p4, p5);
        int* dest = (int*)((char*)this + idx * 0xC0 + 0x278);
        dest[0] = (p0[0] == '1') ? -1 : -2;
        dest[1] = (p0[1] == '1') ? -1 : -2;
        dest[2] = (p0[2] == '1') ? -1 : -2;
        dest[3] = (p0[3] == '1') ? -1 : -2;

        dest = (int*)((char*)this + idx * 0xC0 + 0x298);
        dest[0] = (p1[0] == '1') ? -1 : -2;
        dest[1] = (p1[1] == '1') ? -1 : -2;
        dest[2] = (p1[2] == '1') ? -1 : -2;
        dest[3] = (p1[3] == '1') ? -1 : -2;

        dest = (int*)((char*)this + idx * 0xC0 + 0x2B8);
        dest[0] = (p2[0] == '1') ? -1 : -2;
        dest[1] = (p2[1] == '1') ? -1 : -2;
        dest[2] = (p2[2] == '1') ? -1 : -2;
        dest[3] = (p2[3] == '1') ? -1 : -2;

        dest = (int*)((char*)this + idx * 0xC0 + 0x2D8);
        dest[0] = (p3[0] == '1') ? -1 : -2;
        dest[1] = (p3[1] == '1') ? -1 : -2;
        dest[2] = (p3[2] == '1') ? -1 : -2;
        dest[3] = (p3[3] == '1') ? -1 : -2;

        dest = (int*)((char*)this + idx * 0xC0 + 0x2F8);
        dest[0] = (p4[0] == '1') ? -1 : -2;
        dest[1] = (p4[1] == '1') ? -1 : -2;
        dest[2] = (p4[2] == '1') ? -1 : -2;
        dest[3] = (p4[3] == '1') ? -1 : -2;

        dest = (int*)((char*)this + idx * 0xC0 + 0x318);
        dest[0] = (p5[0] == '1') ? -1 : -2;
        dest[1] = (p5[1] == '1') ? -1 : -2;
        dest[2] = (p5[2] == '1') ? -1 : -2;
        if (p5[3] == '1') {
            dest[3] = -1;
        } else {
            dest[3] = -2;
        }
    } else if (strcmp(label, "CRYSTAL") == 0) {
        int idx;
        sscanf(line, " %s %d", label, &idx);
        // Destroy old crystal if it exists
        if (crystals[idx] != 0) {
            *(int*)crystals[idx] = 0x461320;  // set vtable
            Sprite* spr = *(Sprite**)((char*)crystals[idx] + 0x4d8);
            if (spr != 0) {
                FUN_0044c740(spr);
                FreeMemory(spr);
                *(int*)((char*)crystals[idx] + 0x4d8) = 0;
            }
            FreeMemory(crystals[idx]);
            crystals[idx] = 0;
        }
        // Allocate new crystal (0x4E0 bytes)
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
