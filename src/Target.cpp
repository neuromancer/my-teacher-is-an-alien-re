#include "Target.h"
#include "globals.h"
#include "Sprite.h"
#include "Memory.h"
#include "SoundList.h"
#include "Sample.h"
#include "InputManager.h"
#include "Engine.h"
#include "CDData.h"
#include "CursorState.h"
#include "string.h"
#include <string.h>
#include <stdio.h>

extern CDData* g_CDData_0043697c;

// ============================================================================
// Target implementation
// ============================================================================

HotspotListData::~HotspotListData() {}

/* Function start: 0x413DC0 */
Target::Target() : Sprite((char*)0)
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
        delete Target::hotspotList;
        Target::hotspotList = 0;
    }
}

extern "C" int __cdecl GetPixelAt(int x, int y);

class ScoreManager {
public:
    int field_0;
    int score;    // field_4
    void AdjustScore(int value); // 0x416ba0
};

// ScoreManager::AdjustScore - adds value to field_4, clamps between 0 and 200
/* Function start: 0x416ba0 */
void ScoreManager::AdjustScore(int value) {
    int newVal = ScoreManager::score + value;
    ScoreManager::score = newVal;
    if (newVal < 0) {
        ScoreManager::score = 0;
        return;
    }
    if (newVal > 0xc8) {
        ScoreManager::score = 0xc8;
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
    g_ScoreManager_00435f20->completionCount++;
}

/* Function start: 0x4140B0 */
void Target::Activate()
{
    if (Target::active == 0) {
        TargetList* targetList = g_TargetList_00435f0c;
        HashTable* hashTable = targetList->hashTable;
        if (hashTable != 0) {
            unsigned int id = (unsigned int)Target::id;
            int numBuckets = hashTable->numBuckets;
            int hash = (id >> 4) % (unsigned int)numBuckets;

            void* buckets = hashTable->buckets;
            HashNode* node;
            if (buckets != 0) {
                node = ((HashNode**)buckets)[hash];
                while (node != 0) {
                    if (node->key == id) break;
                    node = node->next;
                }
            } else {
                node = 0;
            }

            if (node == 0) {
                if (buckets == 0) {
                    hashTable->AllocateBuckets(numBuckets, 1);
                }
                node = hashTable->AllocateNode();
                node->bucketIndex = hash;
                node->key = (unsigned int)Target::id;
                node->next = ((HashNode**)hashTable->buckets)[hash];
                ((HashNode**)hashTable->buckets)[hash] = node;
            }
            node->reserved = (int)this;
        }

        HotspotListData* list = Target::hotspotList;
        if (list != 0 && list->count != 0) {
            HotspotNode* first = list->first;
            list->processingHead = first;
            if (first != 0) {
                list->processingHead = first->next;
                list->currentId = first->id;
            } else {
                list->currentId = -1;
            }
        }

        Target::progressRange.start = 0;
        Target::active = 1;
        Sprite::SetState2(Target::animRange.start);

        {
            Range temp = *(Range*)&animParam;
            Target::loc_x = temp.start;
            Target::loc_y = temp.end;
        }

        Target::pendingAction = 0;
    }
}

/* Function start: 0x414230 */
void Target::Deactivate()
{
    if (Target::active != 0) {
        TargetList* targetList = g_TargetList_00435f0c;
        HashTable* hashTable = targetList->hashTable;
        if (hashTable != 0 && hashTable->buckets != 0) {
            unsigned int id = (unsigned int)Target::id;
            int numBuckets = hashTable->numBuckets;
            int hash = (id >> 4) % (unsigned int)numBuckets;

            HashNode** pNext = (HashNode**)&hashTable->buckets[hash];
            HashNode* node = *pNext;

            while (node != 0) {
                if (node->key == id) {
                    *pNext = node->next;

                    int i = 0;
                    while (!i--) ;
                    i = 0;
                    while (!i--) ;

                    node->next = hashTable->freeList;
                    hashTable->freeList = node;
                    hashTable->count--;
                    break;
                }
                pNext = &node->next;
                node = node->next;
            }
        }
        Sprite::FreeAnimation();
        Target::active = 0;
    }
}

/* Function start: 0x4142C0 */
int Target::CheckTimeInRange()
{
    InputState* mouse;
    int x;
    int y;
    int result;

    mouse = g_InputManager_00436968->pMouse;
    y = 0;
    if (mouse != 0) {
        y = mouse->y;
    }
    if (mouse != 0) {
        x = mouse->x;
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
            if (node != 0) {
                list->processingHead = node->next;
                list->currentId = node->id;
            } else {
                list->currentId = -1;
            }
            (g_TargetList_00435f0c)->currentTarget = this;
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
    if (Target::progressRange.end) {
        shouldTrigger = (newValue >= Target::progressRange.end) ? 1 : 0;
    } else {
        shouldTrigger = 0;
    }
    if (shouldTrigger) {
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

    switch (Target::pendingAction) {
    case 1:
        if (Target::current_state == Target::animRange.end) {
            g_ScoreManager_00435f20->score -= Target::hitMissPoints.end;
            Target::Deactivate();
            return 1;
        }
        Sprite::SetState2(Target::current_state + 1);
        break;
    case 3:
        Target::active = 3;
        Sprite::SetState2(Target::hitRange.start + Target::current_state);
        if ((Target::targetFlags & 1) != 0) {
            Range temp = *(Range*)&hitOffset;
            Target::loc_y = temp.end;
            Target::loc_x = temp.start;
        }
        if (Target::stopSound != 0) {
            Target::stopSound->~Sample();
        }
        if (Target::hitSound != 0) {
            Target::hitSound->Play(100, 1);
        }
        g_ScoreManager_00435f20->missCount++;
        g_ScoreManager_00435f20->score += Target::hitMissPoints.start;
        ((ScoreManager*)g_ScoreManager_00435f20)->AdjustScore(Target::scoreWeight.start);
        g_CombatEngine_00435eb0->m_combatBonus1 += Target::combatBonus.start;
        g_CombatEngine_00435eb0->m_combatBonus2 += Target::combatBonus2.val;
        break;
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

            Target::animFilename = new char[strlen(g_CDData_0043697c->pathBuffer + 0x85) + 1];
            strcpy(Target::animFilename, g_CDData_0043697c->pathBuffer + 0x85);
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

/* Function start: 0x414730 */
void Target::OnProcessStart()
{
    char buffer[128];
    extern int DAT_004362cc;
    extern int g_TargetBearingValue_004362c8;

    g_TargetBearingValue_004362c8 = 0;
    DAT_004362cc = 0;
    
    sprintf(buffer, "FNAME %s", animFilename);
    Sprite::LBLParse(buffer);
    Sprite::SetState(40);
    
    flags = flags | 0x40;
    flags = flags & ~2;
    flags = flags | 0x200;
    
    InitAnimation();
    
    TargetList* tl = g_TargetList_00435f0c;
    stopSound     = (Sample*)tl->field_0x1b4;
    tl = g_TargetList_00435f0c;
    progressSound = (Sample*)tl->field_0x1b8;
    tl = g_TargetList_00435f0c;
    hitSound      = (Sample*)tl->field_0x1bc;
    tl = g_TargetList_00435f0c;
    sound3        = (Sample*)tl->field_0x1c0;
}

/* Function start: 0x4147F0 */
void Target::OnProcessEnd()
{
    extern int g_TargetBearingValue_004362c8;
    if (hotspotList == 0 && g_TargetBearingValue_004362c8 != 0) {
        HotspotListData* list = new HotspotListData;
        hotspotList = list;
        
        HotspotNode* lastNode = list->last;
        if (list->freeList == 0) {
            char* mem = new char[list->growthRate * 12 + 4];
            *(void**)mem = list->nodePool;
            list->nodePool = mem;

            int n = list->growthRate;
            HotspotNode* curr = (HotspotNode*)(mem + (n * 12) - 8);
            n--;
            if (n >= 0) {
                do {
                    curr->next = list->freeList;
                    n--;
                    list->freeList = curr;
                    curr = (HotspotNode*)((char*)curr - 12);
                } while (n >= 0);
            }
        }

        HotspotNode* node = list->freeList;
        list->freeList = node->next;
        node->prev = lastNode;
        node->next = 0;
        list->count++;
        node->id = 0;

        int i = 0;
        do {} while (i--);

        node->id = g_TargetBearingValue_004362c8;
        if (lastNode != 0) {
            lastNode->next = node;
        } else {
            list->first = node;
        }
        list->last = node;
    }
}

extern int DAT_004362cc;
extern int g_TargetBearingValue_004362c8;

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
        sscanf(line + 3, "%d %d", &animParam.start, &animParam.end);
    }
    else if (firstChar == 'B') {
        int result = sscanf(line + 3, "%d %d", &value1, &value2);
        if (result == 2) {
            SetRange(DAT_004362cc, value1, value2);
            animRange.end = DAT_004362cc;
            g_TargetBearingValue_004362c8 = value2;
            DAT_004362cc++;
        }
    }
    else if (firstChar == 'C') {
        sscanf(line + 1, "%d", &progressRange.end);
    }
    else if (firstChar == 'D') {
        sscanf(line + 3, "%d", &hitMissPoints.start);
    }
    else if (firstChar == 'F') {
        // Do nothing
    }
    else if (firstChar == 'H') {
        int result = sscanf(line + 3, "%d", &value1);
        if (result == 1) {
            if (hotspotList == 0) {
                hotspotList = new HotspotListData;
            }
            HotspotListData* list = hotspotList;
            HotspotNode* lastNode = list->last;
            if (list->freeList == 0) {
                char* mem = new char[list->growthRate * 12 + 4];
                *(HotspotNode**)mem = (HotspotNode*)list->nodePool;
                list->nodePool = mem;
                int i = list->growthRate;
                HotspotNode* node = (HotspotNode*)(mem + i * 12 - 8);
                i--;
                do {
                    node->next = list->freeList;
                    list->freeList = node;
                    node = (HotspotNode*)((char*)node - 12);
                } while (--i >= 0);
            }
            HotspotNode* node = list->freeList;
            list->freeList = node->next;
            node->prev = lastNode;
            node->next = 0;
            list->count++;
            node->id = 0;

            int dummy = 0;
            do {} while (dummy--);

            node->id = value1;
            if (lastNode != 0) {
                lastNode->next = node;
                list->last = node;
            } else {
                list->first = node;
                list->last = node;
            }
        }
    }
    else if (firstChar == 'I') {
        int result = sscanf(line + 3, "%s", buffer);
        if (result == 1) {
            if (strlen(buffer) != 0) {
                char* id = new char[strlen(buffer) + 1];
                identifier = id;
                strcpy(identifier, buffer);
            }
        }
    }
    else if (firstChar == 'K') {
        int result = sscanf(line + 3, "%d %d", &value1, &value2);
        if (result == 2) {
            SetRange(DAT_004362cc, value1, value2);
            if (hitRange.start == 0) {
                hitRange.start = DAT_004362cc;
            }
            hitRange.end = DAT_004362cc;
            DAT_004362cc++;
        }
    }
    else if (firstChar == 'O') {
        sscanf(line + 3, "%d %d", &Target::hitOffset.start, &Target::hitOffset.end);
        targetFlags |= 1;
    }
    else if (firstChar == 'P') {
        sscanf(line + 3, "%d %d", &timeRange.start, &timeRange.end);
    }
    else if (firstChar == 'S') {
        ParseSound(line);
    }
    else if (firstChar == 'V') {
        sscanf(line + 3, "%d", &scoreWeight.start);
    }
    else if (firstChar == 'W') {
        sscanf(line + 3, "%d", &scoreWeight.end);
    }
    else if (firstChar == 'Z') {
        return 1;
    }
    else {
        Parser::LBLParse("Target");
    }

    return 0;
}
