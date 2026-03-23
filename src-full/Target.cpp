#include "Target.h"
#include "globals.h"
#include "Sprite.h"
#include "Memory.h"
#include "SoundList.h"
#include "Sample.h"
#include "InputManager.h"
#include "SC_CombatBase.h"
#include "CDData.h"
#include "CursorState.h"
#include "string.h"
#include <string.h>
#include <stdio.h>

extern CDData* g_CDData_0043697c;

// ============================================================================
// Target implementation
// ============================================================================

/* Function start: 0x443780 */
HotspotListData::~HotspotListData()
{
    HotspotNode* node = first;
    while (node != 0) {
        int i = 0;
        do {
        } while (i-- != 0);
        node = node->next;
    }

    count = 0;
    freeList = 0;
    last = 0;
    first = 0;

    void* pool = nodePool;
    while (pool != 0) {
        void* next = *(void**)pool;
        FreeMemory(pool);
        pool = next;
    }
    nodePool = 0;
}

/* Function start: 0x442350 */ /* ~80% match */
Target::Target() : Sprite(0)
{
    memset(&active, 0, 0x80);

    Target::active = 0;
    Target::id = -1;
    Target::pendingAction = 0;
}

/* Function start: 0x4424A0 */ /* ~90% match */
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
/* Function start: 0x40C600 */
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

/* Function start: 0x4425F0 */ /* ~81% match */
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

/* Function start: 0x442640 */ /* ~86% match */
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
        Sprite::ResetAnimation(Target::animRange.start, 0);

        {
            Range temp = *(Range*)&animParam;
            Target::loc_x = temp.start;
            Target::loc_y = temp.end;
        }

        Target::pendingAction = 0;
    }
}

/* Function start: 0x4427C0 */
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

/* Function start: 0x442850 */ /* ~84% match */
int Target::CheckTimeInRange()
{
    InputState* mouse;
    int x;
    int y;
    int result;

    mouse = g_InputManager_0046aa08->pMouse;
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

/* Function start: 0x4428A0 */ /* ~89% match */
int Target::CheckTimeInRangeParam(int* coords)
{
    int result;

    result = GetPixelAt(coords[0], coords[1]);
    if (Target::timeRange.start <= result && result <= Target::timeRange.end) {
        return 1;
    }
    return 0;
}

/* Function start: 0x4428E0 */ /* ~91% match */
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

/* Function start: 0x442940 */
void Target::UpdateProgress(int delta)
{
    int newValue;
    int shouldTrigger;

    if (Target::active != 1) {
        return;
    }
    newValue = Target::progressRange.start + delta;
    Target::progressRange.start = newValue;
    if (Target::progressRange.end != 0) {
        shouldTrigger = (Target::progressRange.end <= newValue) ? 1 : 0;
    } else {
        shouldTrigger = 0;
    }
    if (shouldTrigger != 0) {
        Target::pendingAction = 3;
        return;
    }
    if (Target::progressSound != 0) {
        Target::progressSound->Play(100, 1);
    }
}

extern int g_CombatEngine_0046ae78;
extern void FUN_004427c0(int);
extern void FUN_0040c600(void*, int);

/* Function start: 0x4429A0 */
int Target::Update()
{
    if (active == 0) {
        return 1;
    }

    if (pendingAction == 1) {
        if (handle == animRange.end) {
            *(int*)DAT_0046ae6c -= hitMissPoints.end;
            FUN_004427c0((int)this);
            return 1;
        }
        Sprite::ResetAnimation(handle + 1, 0);
    } else if (pendingAction == 3) {
        active = 3;
        Sprite::ResetAnimation(hitRange.start + handle, 0);
        if ((targetFlags & 1) != 0) {
            int mouseY = 0;
            int mouseX = 0;
            InputState* pMouse = g_InputManager_0046aa08->pMouse;
            if (pMouse != 0) {
                mouseY = pMouse->y;
                mouseX = pMouse->x;
            }
            loc_x = mouseX - hitOffset.start;
            loc_y = mouseY - hitOffset.end;
        }
        if (stopSound != 0) {
            stopSound->~Sample();
        }
        if (hitSound != 0) {
            hitSound->Play(100, 1);
        }
        ((int*)DAT_0046ae6c)[3]++;
        *(int*)DAT_0046ae6c += hitMissPoints.start;
        FUN_0040c600((void*)DAT_0046ae6c, scoreWeight.start);
        *(int*)(g_CombatEngine_0046ae78 + 0xBC) += combatBonus.start;
        *(int*)(g_CombatEngine_0046ae78 + 0xCC) += combatBonus2.val;
    }

    pendingAction = 0;
    int done = Sprite::Do(loc_x, loc_y, 1.0);
    if (done != 0) {
        if (active == 3) {
            FUN_004427c0((int)this);
            return 0;
        }
        pendingAction = active;
    }
    return 0;
}

/* Function start: 0x442B20 */ /* ~85% match */
void Target::Init(char* line)
{
    char buffer[64];

    if (sscanf(line, "%s", buffer) == 1) {
        if (stricmp(buffer, "INIT") != 0) {
            g_CDData_0043697c->ResolvePath(buffer);

            Target::animFilename = new char[strlen(g_CDData_0043697c->field_190 + 5) + 1];
            strcpy(Target::animFilename, g_CDData_0043697c->field_190 + 5);
        }
    }
}

#include "string.h"

/* Function start: 0x442BD0 */
void Target::ParseSound(char* line)
{
    char buffer[128];

    sscanf(line + 3, "%s", buffer);
    char type = line[1];
    char* resolved = MakeAudioName(buffer);
    Sample* sound = (Sample*)DAT_0046ae68->Register(resolved);

    switch (type) {
    case '0':
        Target::stopSound = sound;
        break;
    case '1':
        Target::progressSound = sound;
        break;
    case '2':
        Target::hitSound = sound;
        break;
    case '3':
        Target::sound3 = sound;
        break;
    default:
        ShowError("TargetScene::ParseSound() - Undefined sound type => %s", line);
        break;
    }
}

/* Function start: 0x442B20 */
void Target::OnProcessStart()
{
    char buffer[128];
    extern int DAT_004362cc;
    extern int g_TargetBearingValue_004362c8;

    g_TargetBearingValue_004362c8 = 0;
    DAT_004362cc = 0;
    
    sprintf(buffer, "FNAME %s", animFilename);
    Sprite::LBLParse(buffer);
    Sprite::ConfigStates(40);
    
    flags = flags | 0x40;
    flags = flags & ~2;
    flags = flags | 0x200;
    
    InitAnimation();
    
    TargetList* tl = g_TargetList_00435f0c;
    stopSound     = tl->defaultStopSound;
    tl = g_TargetList_00435f0c;
    progressSound = tl->defaultProgressSound;
    tl = g_TargetList_00435f0c;
    hitSound      = tl->defaultHitSound;
    tl = g_TargetList_00435f0c;
    sound3        = tl->defaultSound;
}

/* Function start: 0x442BD0 */
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

/* Function start: 0x442BD0 */
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
            ConfigRange(DAT_004362cc, value1, value2, 1);
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
            ConfigRange(DAT_004362cc, value1, value2, 1);
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
