#include "EngineSubsystems.h"
#include "globals.h"
#include "Sprite.h"
#include "Memory.h"
#include "SoundList.h"
#include "Sample.h"
#include "InputManager.h"
#include "Engine.h"
#include "CDData.h"
#include "string.h"
#include <string.h>
#include <stdio.h>

extern CDData* g_CDData_0043697c;
extern int FUN_00421f90(void* cdData, char* param);

// ============================================================================
// Target implementation
// ============================================================================

/* Function start: 0x413DC0 */
Target::Target() : Sprite((char*)0), 
    animRange(), hitRange(), timeRange(), progressRange(),
    scoreIndex(0), weight(0),
    hitPoints(0), missPoints(0),
    combatBonus1(0), field_0x120(0),
    combatBonus2(0), field_0x128(0),
    animParam1(0), animParam2(0),
    hitX(0), hitY(0)
{
    memset((char*)this + 0xd8, 0, 0x80);

    Target::active = 0;
    Target::id = -1;
    Target::pendingAction = 0;
}

/* Function start: 0x413F10 */
Target::~Target()
{
    if (Target::animFilename != 0) {
        FreeMemory(Target::animFilename);
        Target::animFilename = 0;
    }

    if (Target::identifier != 0) {
        FreeMemory(Target::identifier);
        Target::identifier = 0;
    }

    if (Target::hotspotList != 0) {
        Target::hotspotList->~HotspotListData();
        FreeMemory(Target::hotspotList);
        Target::hotspotList = 0;
    }
}

extern "C" int __cdecl GetPixelAt(int x, int y);
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

    if (Target::active != 0) {
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
        Target::active = 0;
    }
}

/* Function start: 0x4142C0 */
int Target::CheckTimeInRange()
{
    int* timePtr;
    int x;
    int y;
    int result;

    timePtr = (int*)g_InputManager_00436968->pMouse;
    y = 0;
    if (timePtr != 0) {
        y = timePtr[1];
    }
    if (timePtr != 0) {
        x = *timePtr;
    } else {
        x = 0;
    }
    result = GetPixelAt(x, y);
    if (Target::timeRange.start <= result && result <= Target::timeRange.end) {
        return 1;
    }
    return 0;
}

/* Function start: 0x414310 */
int Target::CheckTimeInRangeParam(int* coords)
{
    int result;

    result = GetPixelAt(coords[0], coords[1]);
    if (Target::timeRange.start <= result && result <= Target::timeRange.end) {
        return 1;
    }
    return 0;
}

/* Function start: 0x414350 */
int Target::AdvanceHotspot()
{
    HotspotListData* list;
    HotspotNode* node;

    list = Target::hotspotList;
    if (list != 0) {
        if (Target::animation_data == 0) {
            if (list->currentId == 0) {
                goto advance;
            }
        } else if (Target::animation_data->smk->FrameNum == list->currentId) {
advance:
            node = list->head;
            if (node == 0) {
                list->currentId = -1;
            } else {
                list->head = node->next;
                list->currentId = node->id;
            }
            ((TargetList*)DAT_00435f0c)->currentTarget = this;
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

    if (Target::active != 1) {
        return;
    }
    newValue = Target::progressRange.start + delta;
    Target::progressRange.start = newValue;
    if (Target::progressRange.end == 0) {
        shouldTrigger = 0;
    } else if (newValue >= Target::progressRange.end) {
        shouldTrigger = 1;
    } else {
        shouldTrigger = 0;
    }
    if (shouldTrigger != 0) {
        Target::pendingAction = 3;
        return;
    }
    sample = Target::progressSound;
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

    if (Target::active == 0) {
        return 1;
    }
    state = Target::pendingAction;
    if (state == 1) {
        if (Target::animRange.end == Target::current_state) {
            *g_ScoreManager = *g_ScoreManager - Target::missPoints;
            Target::Deactivate();
            return 1;
        }
        Sprite::SetState2(Target::current_state + 1);
    } else if (state == 3) {
        Target::active = 3;
        Sprite::SetState2(Target::hitRange.start + Target::current_state);
        if ((Target::targetFlags & 1) != 0) {
            tmpX = Target::hitX;
            tmpY = Target::hitY;
            Target::loc_x = tmpX;
            Target::loc_y = tmpY;
        }
        sample = Target::stopSound;
        if (sample != 0) {
            sample->~Sample();
        }
        sample = Target::hitSound;
        if (sample != 0) {
            sample->Play(100, 1);
        }
        g_ScoreManager[3] = g_ScoreManager[3] + 1;
        *g_ScoreManager = *g_ScoreManager + Target::hitPoints;
        FUN_00416ba0(g_ScoreManager, Target::scoreIndex);
        g_CombatEngine->field_0xb4 = g_CombatEngine->field_0xb4 + Target::combatBonus1;
        g_CombatEngine->field_0xc4 = g_CombatEngine->field_0xc4 + Target::combatBonus2.val;
    }
    Target::pendingAction = 0;
    doResult = Sprite::Do(Target::loc_x, Target::loc_y, 1.0);
    if (doResult != 0) {
        if (Target::active == 3) {
            Target::Deactivate();
        } else {
            Target::pendingAction = Target::active;
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

            char* src = &g_CDData_0043697c->field_0xc0[0x85];
            unsigned int len = strlen(src) + 1;
            Target::animFilename = (char*)AllocateMemory(len);
            memcpy(Target::animFilename, src, len);
        }
    }
}

/* Function start: 0x414690 */
void Target::ParseSound(char* line)
{
    char buffer[128];
    int index;
    Sample* sound;

    sscanf(line + 3, "%s", buffer);
    index = line[1] - '0';
    sound = (Sample*)g_SoundList_00435f1c->Register(buffer);

    switch (index) {
    case 0:
        Target::stopSound = sound;
        break;
    case 1:
        Target::progressSound = sound;
        break;
    case 2:
        Target::hitSound = sound;
        break;
    case 3:
        Target::sound3 = sound;
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
        sscanf(line + 3, "%d %d", &Target::animParam1, &Target::animParam2);
    }
    else if (firstChar == 'B') {
        int result = sscanf(line + 3, "%d %d", &value1, &value2);
        if (result == 2) {
            Target::SetRange(DAT_004362cc, value1, value2);
            Target::animRange.end = DAT_004362cc;
            DAT_004362c8 = value2;
            DAT_004362cc++;
        }
    }
    else if (firstChar == 'C') {
        sscanf(line + 1, "%d", &Target::progressRange.end);
    }
    else if (firstChar == 'D') {
        sscanf(line + 3, "%d", &Target::hitPoints);
    }
    else if (firstChar == 'F') {
        // Do nothing
    }
    else if (firstChar == 'H') {
        int result = sscanf(line + 3, "%d", &value1);
        if (result == 1) {
            // Simplified hotspot handling - allocate list if needed
            if (Target::hotspotList == 0) {
                Target::hotspotList = new HotspotListData;
                if (Target::hotspotList != 0) {
                    memset(Target::hotspotList, 0, sizeof(HotspotListData));
                    Target::hotspotList->fields[5] = 10;
                    Target::hotspotList->currentId = -1;
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
                Target::identifier = (char*)AllocateMemory(len);
                memcpy(Target::identifier, buffer, len);
            }
        }
    }
    else if (firstChar == 'K') {
        int result = sscanf(line + 3, "%d %d", &value1, &value2);
        if (result == 2) {
            Target::SetRange(DAT_004362cc, value1, value2);
            if (Target::hitRange.start == 0) {
                Target::hitRange.start = DAT_004362cc;
            }
            Target::hitRange.end = DAT_004362cc;
            DAT_004362cc++;
        }
    }
    else if (firstChar == 'O') {
        sscanf(line + 3, "%d %d", &Target::hitX, &Target::hitY);
        Target::targetFlags |= 1;
    }
    else if (firstChar == 'P') {
        sscanf(line + 3, "%d %d", &Target::timeRange.start, &Target::timeRange.end);
    }
    else if (firstChar == 'S') {
        Target::ParseSound(line);
    }
    else if (firstChar == 'V') {
        sscanf(line + 3, "%d", &Target::scoreIndex);
    }
    else if (firstChar == 'W') {
        sscanf(line + 3, "%d", &Target::weight);
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
