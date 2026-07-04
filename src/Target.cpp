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
#include "ScoreDisplay.h"
#include "string.h"
#include <string.h>
#include <stdio.h>

#include "VideoTable.h"

// ============================================================================
// Target implementation
// ============================================================================

/* Function start: 0x442350 */ /* ~83% match */
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


/* Function start: 0x4425F0 */ /* ~81% match */
void Target::Spawn()
{
    Target::Activate();
    Target::animation_data->UpdatePalette(Target::timeRange.x,
        (Target::timeRange.y - Target::timeRange.x) + 1);
    if (Target::stopSound != 0) {
        Target::stopSound->Play(100, 1);
    }
    g_ScoreDisplay_0046ae6c->completionCount++;
}

/* Function start: 0x442640 */
void Target::Activate()
{
    if (Target::active == 0) {
        TargetList* targetList = g_TargetList_0046ae58;
        HashTable* hashTable = targetList->hashTable;
        if (hashTable != 0) {
            unsigned int id = (unsigned int)Target::id;
            int numBuckets = hashTable->numBuckets;
            int hash = (id >> 4) % (unsigned int)numBuckets;

            HashNode** buckets = hashTable->buckets;
            HashNode* node;
            if (buckets != 0) {
                node = buckets[hash];
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
                node->next = hashTable->buckets[hash];
                hashTable->buckets[hash] = node;
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
        Sprite::ResetAnimation(Target::animRange.x, 0);

        {
            SlimeDim temp = Target::animParam;
            loc.x = temp.x;
            loc.y = temp.y;
        }

        Target::pendingAction = 0;
    }
}

/* Function start: 0x4427C0 */
void Target::Deactivate()
{
    if (Target::active != 0) {
        TargetList* targetList = g_TargetList_0046ae58;
        HashTable* hashTable = targetList->hashTable;
        if (hashTable != 0 && hashTable->buckets != 0) {
            unsigned int id = (unsigned int)Target::id;
            int numBuckets = hashTable->numBuckets;
            int hash = (id >> 4) % (unsigned int)numBuckets;

            HashNode** pNext = &hashTable->buckets[hash];
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
    if (Target::timeRange.x <= result && result <= Target::timeRange.y) {
        return 1;
    }
    return 0;
}

/* Function start: 0x4428A0 */
int Target::CheckTimeInRangeParam(int* coords)
{
    int result;

    result = GetPixelAt(coords[0], coords[1]);
    if (Target::timeRange.x <= result && result <= Target::timeRange.y) {
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
            (g_TargetList_0046ae58)->currentTarget = this;
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
    Target::progressRange.start += delta;
    newValue = Target::progressRange.start;
    if (Target::progressRange.end != 0 && Target::progressRange.end <= newValue) {
        shouldTrigger = 1;
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

#include "ScoreDisplay.h"

/* Function start: 0x4429A0 */
int Target::Update()
{
    if (active == 0) {
        return 1;
    }

    switch (pendingAction) {
    case 1:
    if (handle == animRange.y) {
        *(int*)g_ScoreDisplay_0046ae6c -= hitMissPoints.end;
        Target::Deactivate();
        return 1;
    }
    Sprite::ResetAnimation(handle + 1, 0);
    break;

    case 3:
    active = pendingAction;
    Sprite::ResetAnimation(hitRange.x + handle, 0);
    if ((targetFlags & 1) != 0) {
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
        loc.x = mouseX - hitOffset.x;
        loc.y = mouseY - hitOffset.y;
    }
    if (stopSound != 0) {
        stopSound->Stop();
    }
    if (hitSound != 0) {
        hitSound->Play(100, 1);
    }
    g_ScoreDisplay_0046ae6c->missCount++;
    *(int*)g_ScoreDisplay_0046ae6c += hitMissPoints.start;
    g_ScoreDisplay_0046ae6c->AdjustScore(scoreWeight.start);
    g_CombatEngine_0046ae78->hotspotX += combatBonus.start;
    *(int*)&g_CombatEngine_0046ae78->field_0xCC += combatBonus2.val;
    break;
    }

    pendingAction = 0;
    if (Sprite::Do(loc.x, loc.y, 1.0) != 0) {
        if (active == 3) {
            Target::Deactivate();
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
        if (stricmp(buffer, "NONE") != 0) {
            ResolveAssetPath(buffer);

            Target::animFilename = new char[strlen(g_PathResolver_0046aa1c->cdPath + 0x8a) + 1];
            strcpy(Target::animFilename, g_PathResolver_0046aa1c->cdPath + 0x8a);
        }
    }
}

#include "string.h"

/* Function start: 0x442BD0 */
void Target::ParseSound(char* line)
{
    char buffer[128];

    sscanf(line + 3, "%s", buffer);
    int type = line[1];
    char* resolved = MakeAudioName(buffer);
    Sample* sound = (Sample*)g_SoundList_0046ae68->Register(resolved);

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

/* Function start: 0x442C70 */
void Target::OnProcessStart()
{
    char buffer[128];

    g_TargetBearingValue_0046bff0 = 0;
    g_TargetRangeCounter_0046bff4 = 0;

    sprintf(buffer, "FNAME %s", animFilename);
    Sprite::LBLParse(buffer);
    Sprite::ConfigStates(40);

    flags = flags | 0x40;
    flags = flags & ~2;
    flags = flags | 0x100;

    InitAnimation();

    TargetList* tl = g_TargetList_0046ae58;
    stopSound     = tl->defaultStopSound;
    tl = g_TargetList_0046ae58;
    progressSound = tl->defaultProgressSound;
    tl = g_TargetList_0046ae58;
    hitSound      = tl->defaultHitSound;
    tl = g_TargetList_0046ae58;
    sound3        = tl->defaultSound;
    progressRange.end = 1;
    progressRange.start = 0;
}

/* Function start: 0x442D50 */
void Target::OnProcessEnd()
{
    Sprite::OnProcessEnd();
    if (hotspotList == 0 && g_TargetBearingValue_0046bff0 != 0) {
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

        node->id = g_TargetBearingValue_0046bff0;
        if (lastNode != 0) {
            lastNode->next = node;
        } else {
            list->first = node;
        }
        list->last = node;
    }
}

/* Function start: 0x442E90 */
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
        sscanf(line + 3, "%d %d", &animParam.x, &animParam.y);
    }
    else if (firstChar == 'B') {
        int result = sscanf(line + 3, "%d %d", &value1, &value2);
        if (result == 2) {
            ConfigRange(g_TargetRangeCounter_0046bff4, value1, value2, 1);
            animRange.y = g_TargetRangeCounter_0046bff4;
            g_TargetBearingValue_0046bff0 = value2;
            g_TargetRangeCounter_0046bff4++;
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
            ConfigRange(g_TargetRangeCounter_0046bff4, value1, value2, 1);
            if (hitRange.x == 0) {
                hitRange.x = g_TargetRangeCounter_0046bff4;
            }
            hitRange.y = g_TargetRangeCounter_0046bff4;
            g_TargetRangeCounter_0046bff4++;
        }
    }
    else if (firstChar == 'O') {
        sscanf(line + 3, "%d %d", &Target::hitOffset.x, &Target::hitOffset.y);
        targetFlags |= 1;
    }
    else if (firstChar == 'P') {
        sscanf(line + 3, "%d %d", &timeRange.x, &timeRange.y);
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
        Parser::ReportUnknownLabel("Target");
    }

    return 0;
}
