#include "Target.h"
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

// ============================================================================
// HotspotListData implementation
// ============================================================================

HotspotListData::HotspotListData() :
    first(0), last(0), count(0), freeList(0),
    nodePool(0), growthRate(10), processingHead(0), currentId(-1)
{}

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
    hitOffset()
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
        delete Target::animFilename;
        Target::animFilename = 0;
    }

    if (Target::identifier != 0) {
        delete Target::identifier;
        Target::identifier = 0;
    }

    if (Target::hotspotList != 0) {
        Target::hotspotList->~HotspotListData();
        delete Target::hotspotList;
        Target::hotspotList = 0;
    }
}

extern "C" int __cdecl GetPixelAt(int x, int y);

// ScoreManager::AdjustScore - adds value to field_4, clamps between 0 and 200
/* Function start: 0x416ba0 */
void FUN_00416ba0(int* scoreManager, int value) {
    int newVal = scoreManager[1] + value;
    scoreManager[1] = newVal;
    if (newVal < 0) {
        scoreManager[1] = 0;
        return;
    }
    if (newVal > 0xc8) {
        scoreManager[1] = 0xc8;
    }
}

/* Function start: 0x414060 */
void Target::Spawn()
{
    Target::Activate();
    Target::animation_data->SetPalette(Target::timeRange.start,
        (Target::timeRange.end - Target::timeRange.start) + 1);
    if (Target::stopSound != 0) {
        Target::stopSound->Play(100, 1);
    }
    g_ScoreManager[2]++;
}

/* Function start: 0x4140B0 */
void Target::Activate()
{
    // TODO: Complex function with SEH and hash table operations
    // For now, just set active state
    if (Target::active == 0) {
        Target::progressRange.start = 0;
        Target::active = 1;
    }
}

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
            node = list->processingHead;
            if (node == 0) {
                list->currentId = -1;
            } else {
                list->processingHead = node->next;
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
    if (Target::active == 0) {
        return 1;
    }

    int state = Target::pendingAction;
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
            Range temp = Target::hitOffset;
            Target::loc_y = temp.end;
            Target::loc_x = temp.start;
        }
        if (Target::stopSound != 0) {
            Target::stopSound->~Sample();
        }
        if (Target::hitSound != 0) {
            Target::hitSound->Play(100, 1);
        }
        g_ScoreManager[3]++;
        *g_ScoreManager += Target::hitPoints;
        FUN_00416ba0(g_ScoreManager, Target::scoreIndex);
        g_CombatEngine->field_0xb4 += Target::combatBonus1;
        g_CombatEngine->field_0xc4 += Target::combatBonus2.val;
    }

    int y = Target::loc_y;
    int x = Target::loc_x;
    Target::pendingAction = 0;
    if (Sprite::Do(x, y, 1.0)) {
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

    if (sscanf(line, "%s", buffer) == 1) {
        if (stricmp(buffer, "INIT") != 0) {
            g_CDData_0043697c->ResolvePath(buffer);

            Target::animFilename = (char*)AllocateMemory(strlen((char*)g_CDData_0043697c + 0x145) + 1);
            strcpy(Target::animFilename, (char*)g_CDData_0043697c + 0x145);
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
            HotspotListData* list = Target::hotspotList;
            if (list == 0) {
                list = new HotspotListData;
                Target::hotspotList = list;
            }
            if (list != 0) {
                HotspotNode* node = list->freeList;
                if (node == 0) {
                    node = (HotspotNode*)AllocateMemory(list->growthRate * 12 + 4);
                    *(HotspotNode**)node = (HotspotNode*)list->nodePool;
                    list->nodePool = node;
                    int n = list->growthRate - 1;
                    node = (HotspotNode*)((char*)node + (n + 1) * 12 - 8);
                    do {
                        node->next = list->freeList;
                        list->freeList = node;
                        node = (HotspotNode*)((char*)node - 12);
                        n--;
                    } while (n >= 0);
                    node = list->freeList;
                }
                list->freeList = node->next;
                node->prev = list->last;
                node->next = 0;
                list->count++;
                int temp = 0;
                do {
                    temp--;
                } while (temp != 0);
                node->id = value1;
                if (list->last != 0) {
                    list->last->next = node;
                    list->last = node;
                } else {
                    list->first = node;
                    list->last = node;
                }
            }
        }
    }
    else if (firstChar == 'I') {
        int result = sscanf(line + 3, "%s", buffer);
        if (result == 1) {
            if (strlen(buffer) != 0) {
                Target::identifier = (char*)AllocateMemory(strlen(buffer) + 1);
                strcpy(Target::identifier, buffer);
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
        sscanf(line + 3, "%d %d", &Target::hitOffset.start, &Target::hitOffset.end);
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
