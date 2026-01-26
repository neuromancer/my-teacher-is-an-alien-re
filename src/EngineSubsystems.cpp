#include "EngineSubsystems.h"
#include "globals.h"
#include "Sprite.h"
#include "Memory.h"
#include "SoundList.h"
#include "Sample.h"
#include "InputManager.h"
#include "string.h"
#include <string.h>
#include <stdio.h>

extern CDData* g_CDData_0043697c;
extern int FUN_00421f90(void* cdData, char* param);
extern void ShowError(const char* message, ...);

// ============================================================================
// Target implementation
// ============================================================================

/* Function start: 0x413DC0 */
Target::Target() : Sprite((char*)0)
{
    Target::field_0xec = 0;
    Target::field_0xf0 = 0;
    Target::field_0xf4 = 0;
    Target::field_0xf8 = 0;
    Target::field_0xfc = 0;
    Target::field_0x100 = 0;
    Target::field_0x104 = 0;
    Target::field_0x108 = 0;
    Target::field_0x10c = 0;
    Target::field_0x110 = 0;
    Target::field_0x114 = 0;
    Target::field_0x118 = 0;
    Target::field_0x11c = 0;
    Target::field_0x120 = 0;
    Target::field_0x124 = 0;
    Target::field_0x128 = 0;
    Target::field_0x140 = 0;
    Target::field_0x144 = 0;
    Target::field_0x148 = 0;
    Target::field_0x14c = 0;

    memset((char*)this + 0xd8, 0, 0x80);

    Target::field_0xd8 = 0;
    Target::id = -1;
    Target::field_0x150 = 0;
}

/* Function start: 0x413F10 */
Target::~Target()
{
    if (Target::field_0xe0 != 0) {
        FreeMemory(Target::field_0xe0);
        Target::field_0xe0 = 0;
    }

    if (Target::field_0xe4 != 0) {
        FreeMemory(Target::field_0xe4);
        Target::field_0xe4 = 0;
    }

    if (Target::field_0x12c != 0) {
        FreeMemory(Target::field_0x12c);
        Target::field_0x12c = 0;
    }
}

extern int FUN_00422a46(int x, int y);
extern void FUN_00416ba0(int* scoreManager, int value);

/* Function start: 0x414230 */
void Target::Deactivate()
{
    HashTable* hashTable;
    int** bucketPtr;
    int* node;
    int* prevNode;
    unsigned int hash;
    int i;

    if (Target::field_0xd8 != 0) {
        hashTable = ((TargetList*)DAT_00435f0c)->hashTable;
        if (hashTable != 0 && hashTable->buckets != 0) {
            hash = ((unsigned int)Target::id >> 4) % (unsigned int)hashTable->numBuckets;
            bucketPtr = (int**)(hashTable->buckets + hash);
            node = *bucketPtr;
            if (node != 0) {
                while (node[2] != Target::id) {
                    prevNode = node;
                    node = (int*)*node;
                    if (node == 0) {
                        goto cleanup;
                    }
                    bucketPtr = (int**)prevNode;
                }
                *bucketPtr = (int*)*node;
                i = 0;
                do {
                    i--;
                } while (i != 0);
                i = 0;
                do {
                    i--;
                } while (i != 0);
                *node = (int)hashTable->freeList;
                hashTable->freeList = (HashNode*)node;
                hashTable->count--;
            }
        }
cleanup:
        Sprite::FreeAnimation();
        Target::field_0xd8 = 0;
    }
}

/* Function start: 0x4142C0 */
int Target::CheckTimeInRange()
{
    int* timePtr;
    int x;
    int y;
    int result;

    timePtr = *(int**)(((char*)g_InputManager_00436968) + 0x1a0);
    y = 0;
    if (timePtr != 0) {
        y = timePtr[1];
    }
    if (timePtr != 0) {
        x = *timePtr;
    } else {
        x = 0;
    }
    result = FUN_00422a46(x, y);
    if (Target::field_0xfc <= result && result <= Target::field_0x100) {
        return 1;
    }
    return 0;
}

/* Function start: 0x414310 */
int Target::CheckTimeInRangeParam(int* coords)
{
    int result;

    result = FUN_00422a46(coords[0], coords[1]);
    if (Target::field_0xfc <= result && result <= Target::field_0x100) {
        return 1;
    }
    return 0;
}

/* Function start: 0x414350 */
int Target::AdvanceHotspot()
{
    int* listData;
    int* hotspotIdPtr;
    int* nodePtr;

    listData = (int*)Target::field_0x12c;
    if (listData != 0) {
        hotspotIdPtr = (int*)((char*)listData + 0x1c);
        if (Target::animation_data == 0) {
            if (*hotspotIdPtr == 0) {
                goto advance;
            }
        } else if (*(int*)(*(int*)((char*)Target::animation_data + 0xc) + 0x374) == *hotspotIdPtr) {
advance:
            nodePtr = (int*)*(int*)((char*)listData + 0x18);
            if (nodePtr == 0) {
                *hotspotIdPtr = -1;
            } else {
                *(int*)((char*)listData + 0x18) = *nodePtr;
                *hotspotIdPtr = nodePtr[2];
            }
            *(Target**)((char*)DAT_00435f0c + 0x1a4) = this;
            return 1;
        }
    }
    return 0;
}

/* Function start: 0x4143B0 */
void Target::UpdateProgress(int delta)
{
    int newValue;
    int shouldTrigger;
    Sample* sample;

    if (Target::field_0xd8 != 1) {
        return;
    }
    newValue = Target::field_0x104 + delta;
    Target::field_0x104 = newValue;
    if (Target::field_0x108 == 0) {
        shouldTrigger = 0;
    } else if (newValue >= Target::field_0x108) {
        shouldTrigger = 1;
    } else {
        shouldTrigger = 0;
    }
    if (shouldTrigger != 0) {
        Target::field_0x150 = 3;
        return;
    }
    sample = (Sample*)Target::field_0x134;
    if (sample != 0) {
        sample->Play(100, 1);
    }
}

/* Function start: 0x414410 */
int Target::Update()
{
    int state;
    int tmpX;
    int tmpY;
    Sample* sample;
    int doResult;

    if (Target::field_0xd8 == 0) {
        return 1;
    }
    state = Target::field_0x150;
    if (state == 1) {
        if (Target::field_0xf0 == Target::current_state) {
            *g_ScoreManager = *g_ScoreManager - Target::field_0x118;
            Target::Deactivate();
            return 1;
        }
        Sprite::SetState2(Target::current_state + 1);
    } else if (state == 3) {
        Target::field_0xd8 = 3;
        Sprite::SetState2(Target::field_0xf4 + Target::current_state);
        if ((Target::field_0xdc & 1) != 0) {
            tmpX = Target::field_0x148;
            tmpY = Target::field_0x14c;
            Target::loc_x = tmpX;
            Target::loc_y = tmpY;
        }
        sample = (Sample*)Target::field_0x130;
        if (sample != 0) {
            sample->~Sample();
        }
        sample = (Sample*)Target::field_0x138;
        if (sample != 0) {
            sample->Play(100, 1);
        }
        g_ScoreManager[3] = g_ScoreManager[3] + 1;
        *g_ScoreManager = *g_ScoreManager + Target::field_0x114;
        FUN_00416ba0(g_ScoreManager, Target::field_0x10c);
        *(int*)((char*)g_CombatEngine + 0xb4) = *(int*)((char*)g_CombatEngine + 0xb4) + Target::field_0x11c;
        *(int*)((char*)g_CombatEngine + 0xc4) = *(int*)((char*)g_CombatEngine + 0xc4) + Target::field_0x124;
    }
    Target::field_0x150 = 0;
    doResult = Sprite::Do(Target::loc_x, Target::loc_y, 1.0);
    if (doResult != 0) {
        if (Target::field_0xd8 == 3) {
            Target::Deactivate();
        } else {
            Target::field_0x150 = Target::field_0xd8;
        }
    }
    return 0;
}

/* Function start: 0x4145E0 */
void Target::Init(char* line)
{
    char buffer[64];

    int result = sscanf(line, "%s", buffer);
    if (result == 1) {
        if (stricmp(buffer, "INIT") != 0) {
            FUN_00421f90(g_CDData_0043697c, buffer);

            char* src = ((char*)g_CDData_0043697c) + 0x145;
            unsigned int len = strlen(src) + 1;
            Target::field_0xe0 = (char*)AllocateMemory(len);
            memcpy(Target::field_0xe0, src, len);
        }
    }
}

/* Function start: 0x414690 */
void Target::ParseSound(char* line)
{
    char buffer[128];
    int index;
    int sound;

    sscanf(line + 3, "%s", buffer);
    index = line[1] - '0';
    sound = (int)g_SoundList_00435f1c->Register(buffer);

    switch (index) {
    case 0:
        Target::field_0x130 = (void*)sound;
        break;
    case 1:
        Target::field_0x134 = sound;
        break;
    case 2:
        Target::field_0x138 = sound;
        break;
    case 3:
        Target::field_0x13c = sound;
        break;
    default:
        ShowError("TargetScene::ParseSound() - Undefined sound type => %s", line);
        break;
    }
}

extern int DAT_004362cc;
extern int DAT_004362c8;

/* Function start: 0x414930 */
int Target::LBLParse(char* line)
{
    char label[64];
    char firstChar;
    int value1;
    int value2;
    char buffer[128];

    firstChar = 0;
    value1 = 0;
    value2 = 0;
    label[0] = 0;
    buffer[0] = 0;

    sscanf(line, "%s", label);
    sscanf(label, "%c", &firstChar);

    if (firstChar == 'A') {
        sscanf(line + 3, "%d %d", &Target::field_0x140, &Target::field_0x144);
    }
    else if (firstChar == 'B') {
        int result = sscanf(line + 3, "%d %d", &value1, &value2);
        if (result == 2) {
            Target::SetRange(DAT_004362cc, value1, value2);
            Target::field_0xf0 = DAT_004362cc;
            DAT_004362c8 = value2;
            DAT_004362cc++;
        }
    }
    else if (firstChar == 'C') {
        sscanf(line + 1, "%d", &Target::field_0x108);
    }
    else if (firstChar == 'D') {
        sscanf(line + 3, "%d", &Target::field_0x114);
    }
    else if (firstChar == 'F') {
        // Do nothing
    }
    else if (firstChar == 'H') {
        int result = sscanf(line + 3, "%d", &value1);
        if (result == 1) {
            // Simplified hotspot handling - allocate list if needed
            if (Target::field_0x12c == 0) {
                Target::field_0x12c = AllocateMemory(0x20);
                if (Target::field_0x12c != 0) {
                    memset(Target::field_0x12c, 0, 0x20);
                    ((int*)Target::field_0x12c)[5] = 10;
                    ((int*)Target::field_0x12c)[7] = -1;
                }
            }
            // Add hotspot value to list (simplified)
        }
    }
    else if (firstChar == 'I') {
        int result = sscanf(line + 3, "%s", buffer);
        if (result == 1) {
            unsigned int len = strlen(buffer);
            if (len > 0) {
                len = strlen(buffer) + 1;
                Target::field_0xe4 = (char*)AllocateMemory(len);
                memcpy(Target::field_0xe4, buffer, len);
            }
        }
    }
    else if (firstChar == 'K') {
        int result = sscanf(line + 3, "%d %d", &value1, &value2);
        if (result == 2) {
            Target::SetRange(DAT_004362cc, value1, value2);
            if (Target::field_0xf4 == 0) {
                Target::field_0xf4 = DAT_004362cc;
            }
            Target::field_0xf8 = DAT_004362cc;
            DAT_004362cc++;
        }
    }
    else if (firstChar == 'O') {
        sscanf(line + 3, "%d %d", &Target::field_0x148, &Target::field_0x14c);
        Target::field_0xdc |= 1;
    }
    else if (firstChar == 'P') {
        sscanf(line + 3, "%d %d", &Target::field_0xfc, &Target::field_0x100);
    }
    else if (firstChar == 'S') {
        Target::ParseSound(line);
    }
    else if (firstChar == 'V') {
        sscanf(line + 3, "%d", &Target::field_0x10c);
    }
    else if (firstChar == 'W') {
        sscanf(line + 3, "%d", &Target::field_0x110);
    }
    else if (firstChar == 'Z') {
        return 1;
    }
    else {
        Parser::LBLParse("Target");
    }

    return 0;
}

// ============================================================================
// Viewport implementation (Stubs for linking)
// ============================================================================

/* Function start: 0x412C30 */
void Viewport::SetDimensions(int w, int h) {
    if (w >= 0 && h >= 0) {
        Viewport::x1 = w;
        Viewport::y1 = h;
    }
}

/* Function start: 0x412C50 */
void Viewport::SetAnchor(int x, int y) {
    int iVar1;
    iVar1 = (y - Viewport::y1) + 1;
    if (iVar1 >= 0 && x >= 0 && x < 0x140 && y >= 0 && y < 0xc8) {
        Viewport::x2 = x;
        Viewport::y2 = y;
        Viewport::anchorOffsetY = iVar1;
    }
}

/* Function start: 0x412C90 */
void Viewport::SetDimensions2(int w, int h) {
    if (w >= 0 && h >= 0) {
        Viewport::width = w;
        Viewport::height = h;
    }
}

/* Function start: 0x412CB0 */
void Viewport::SetCenter() {
    Viewport::SetCenterX(Viewport::width / 2);
    Viewport::SetCenterY(Viewport::height / 2);
}

/* Function start: 0x412CE0 */
void Viewport::SetCenterX(int x) {
    if (x >= 0 && x <= Viewport::width) {
        Viewport::centerX = x;
        Viewport::scrollX = Viewport::x1 + x - 1;
    }
}

/* Function start: 0x412D00 */
void Viewport::SetCenterY(int y) {
    if (y >= 0 && y <= Viewport::height) {
        Viewport::centerY = y;
        Viewport::scrollY = Viewport::y1 + y - 1;
    }
}
