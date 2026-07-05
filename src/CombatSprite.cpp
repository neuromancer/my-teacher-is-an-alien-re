#include "CombatSprite.h"
#include "stubs.h"
#include "Memory.h"
#include "string.h"
#include "EngineSubsystems.h"
#include "globals.h"
#include "Target.h"
#include "HashTable.h"
#include <stdio.h>
#include <string.h>

// Global variables
// g_CurrentSprite_004686f8 — defined in globals.cpp

/* Function start: 0x408EE0 */
void __cdecl FreeArrayItems(void** array, int count) {
    int remaining = count;
    count--;
    if (remaining == 0) {
        return;
    }
    do {
        if (*array != 0) {
            FreeMemory(*array);
            *array = 0;
        }
        array++;
        remaining = count;
        count--;
    } while (remaining != 0);
}

/* Function start: 0x408F20 */
void __cdecl CleanupSpriteHashArray(void** array, int count)
{
    if (count-- == 0) return;
    do {
        int* table = (int*)*array;
        if (table != 0) {
            int* node = (int*)*table;
            while (node != 0) {
                FreeArrayItems((void**)(node + 2), 1);
                node = (int*)*node;
            }
            table[2] = 0;
            table[3] = 0;
            table[1] = 0;
            *table = 0;
            int* pool = (int*)table[4];
            while (pool != 0) {
                int* next = (int*)*pool;
                FreeMemory(pool);
                pool = next;
            }
            table[4] = 0;
            FreeMemory(table);
            *array = 0;
        }
        array++;
    } while (count-- != 0);
}

/* Function start: 0x43E250 */
void SpriteHashTable::Clear() {
    int* node;
    int* poolBlock;
    int* nextPool;
    int delayCounter;

    node = (int*)buckets;
    while (node != 0) {
        delayCounter = 0;
        do {
            int temp = delayCounter;
            delayCounter = delayCounter - 1;
            if (temp == 0) break;
        } while (1);
        node = (int*)*node;
    }

    head = 0;
    tail = 0;
    maxSize = 0;
    buckets = 0;

    poolBlock = (int*)count;
    while (poolBlock != 0) {
        nextPool = (int*)*poolBlock;
        delete poolBlock;
        poolBlock = nextPool;
    }
    count = 0;
}

// Dead duplicate of ObjectPool::AllocateBuckets.
void SpriteHashTable::AllocateBuckets(int size, int flag) {
    int* newBuckets;
    int count;

    if (buckets != 0) {
        operator delete(buckets);
        buckets = 0;
    }

    if (flag != 0) {
        newBuckets = (int*)operator new(size * 4);
        count = (size * 4) >> 2;
        buckets = (void**)newBuckets;
        for (; count != 0; count--) {
            *newBuckets = 0;
            newBuckets++;
        }
    }
    maxSize = size;
}

/* Function start: 0x4422E0 */
void* SpriteHashTable::AllocateNode() {
    int* newPool;
    int* node;
    int i;
    int delayCounter;

    if (tail == 0) {
        newPool = (int*)new char[growSize * 16 + 4];
        *newPool = count;
        i = growSize;
        count = (int)newPool;
        node = (int*)((char*)newPool + i * 16 - 12);
        i = i - 1;
        while (i >= 0) {
            i = i - 1;
            *node = (int)tail;
            tail = node;
            node = node - 4;
        }
    }

    node = (int*)tail;
    delayCounter = 0;
    tail = (void*)*node;
    head = (void*)((int)head + 1);
    node[2] = delayCounter;
    do {
        int temp = delayCounter;
        delayCounter = delayCounter - 1;
        if (temp == 0) break;
    } while (1);
    delayCounter = 0;
    node[3] = delayCounter;
    do {
        int temp = delayCounter;
        delayCounter = delayCounter - 1;
        if (temp == 0) break;
    } while (1);
    return node;
}

/* Function start: 0x4097B0 */
HashNode* SpriteHashTable::Lookup(volatile int index, int* outSlot) {
    unsigned int slot;
    HashNode* node;

    slot = ((unsigned int)index >> 4) % (unsigned int)maxSize;
    *outSlot = slot;

    if (buckets == 0) {
        return 0;
    }

    node = (HashNode*)buckets[slot];

loop:
    if (node == 0) goto not_found;
    if (node->key == (unsigned int)index) return node;
    node = node->next;
    goto loop;

not_found:
    return 0;
}

/* Function start: 0x4097F0 */
void SpriteHashTable::Resize(int size, int flag) {
    int* newBuckets;
    int count;

    if (buckets != 0) {
        operator delete(buckets);
        buckets = 0;
    }

    if (flag != 0) {
        newBuckets = (int*)operator new(size * 4);
        count = (size * 4) >> 2;
        buckets = (void**)newBuckets;
        for (; count != 0; count--) {
            *newBuckets = 0;
            newBuckets++;
        }
        maxSize = size;
        return;
    }
    maxSize = size;
}

/* Function start: 0x44BF30 */
HashNode* SpriteHashTable::AllocEntry() {
    int* newPool;
    int* node;
    int i;
    int delayCounter;

    if (tail == 0) {
        newPool = (int*)operator new(growSize * 16 + 4);
        *newPool = count;
        i = growSize;
        count = (int)newPool;
        node = (int*)((char*)newPool + i * 16 - 12);
        i = i - 1;
        if (i >= 0) {
            do {
                i = i - 1;
                *node = (int)tail;
                tail = node;
                node = node - 4;
            } while (i >= 0);
        }
    }

    node = (int*)tail;
    delayCounter = 0;
    tail = (void*)*node;
    head = (void*)((int)head + 1);
    node[2] = delayCounter;
    do {
        int temp = delayCounter;
        delayCounter = delayCounter - 1;
        if (temp == 0) break;
    } while (1);
    delayCounter = 0;
    node[3] = delayCounter;
    do {
        int temp = delayCounter;
        delayCounter = delayCounter - 1;
        if (temp == 0) break;
    } while (1);
    return (HashNode*)node;
}

// Dead duplicate of GameLoopHelper::FreeNodeData.
void FreePointerArray(void** arr, int count) {
    int temp;
    temp = count;
    count = count - 1;
    if (temp == 0) {
        return;
    }
    do {
        if (*arr != 0) {
            operator delete(*arr);
            *arr = 0;
        }
        arr = arr + 1;
        temp = count;
        count = count - 1;
    } while (temp != 0);
}

/* Function start: 0x408FB0 */ /* ~96% match */
CombatSprite::CombatSprite() : Parser() {
    spriteTable = 0;
    currentEntry = 0;
    currentNode = 0;
    entryList = 0;
}

/* Function start: 0x409020 */ /* ~97% match */
CombatSprite::~CombatSprite() {
    SpriteHashTable* table;
    HashNode* node;
    int* poolBlock;
    int* nextPool;
    unsigned int i;
    int delayCounter;

    table = spriteTable;
    if (table != 0) {
        if (table->buckets != 0 && table->maxSize != 0) {
            i = 0;
            do {
                node = (HashNode*)table->buckets[i];
                if (node != 0) {
                    do {
                        CleanupSpriteHashArray((void**)&node->reserved, 1);
                        delayCounter = 0;
                        do {
                            int temp = delayCounter;
                            delayCounter = delayCounter - 1;
                            if (temp == 0) break;
                        } while (1);
                        node = node->next;
                    } while (node != 0);
                }
                i = i + 1;
            } while ((unsigned int)table->maxSize > i);
        }
        delete table->buckets;
        table->buckets = 0;
        table->head = 0;
        table->tail = 0;
        poolBlock = (int*)table->count;
        while (poolBlock != 0) {
            nextPool = (int*)*poolBlock;
            delete poolBlock;
            poolBlock = nextPool;
        }
        table->count = 0;
        delete table;
        spriteTable = 0;
    }
}

/* Function start: 0x409120 */ /* ~97% match */
void* CombatSprite::FindSprite(unsigned int id) {
    void* volatile result = 0;
    volatile unsigned int idx;
    HashNode* node = 0;

    if (spriteTable != 0) {
        idx = (id >> 4) % spriteTable->maxSize;
        if (spriteTable->buckets != 0) {
            node = (HashNode*)spriteTable->buckets[idx];
loop:
            if (node == 0) goto loop_end;
            if (node->key == id) goto found;
            node = node->next;
            goto loop;
loop_end:
            node = 0;
        }
    }

found:
    if (node != 0) {
        result = (void*)node->reserved;
    }
    return result;
}

/* Function start: 0x409180 */ /* ~87% match */
int CombatSprite::PlayById(unsigned int param_1) {
    int* puVar2;
    int* piVar1;
    volatile int hashIndex;
    SpriteHashTable* table;

    currentEntry = 0;
    table = spriteTable;
    if (table != 0) {
        hashIndex = (param_1 >> 4) % (unsigned int)table->maxSize;
        puVar2 = (int*)table->buckets;
        if (puVar2 != 0) {
            puVar2 = (int*)puVar2[hashIndex];
loop:
            if (puVar2 == 0) goto not_found;
            if ((unsigned int)puVar2[2] == param_1) goto found;
            puVar2 = (int*)*puVar2;
            goto loop;
        }
not_found:
        puVar2 = 0;
found:
        if (puVar2 != 0) {
            piVar1 = (int*)puVar2[3];
            entryList = piVar1;
            puVar2 = (int*)*piVar1;
            currentNode = puVar2;
            if (puVar2 != 0) {
                currentNode = (int*)*puVar2;
                currentEntry = (SpriteDataEntry*)puVar2[2];
                return 1;
            }
        }
    }
    return 0;
}

/* Function start: 0x409210 */
void CombatSprite::OnProcessStart() {
    g_CurrentSpriteIndex_004686fc = 0;
    g_CurrentSprite_004686f8 = 0;
}

/* Function start: 0x409220 */
/* Function start: 0x40923E */ /* ~98% match */
int CombatSprite::LBLParse(char* line) {
    char token[32];
    int slot;
    SpriteHashTable* currentSprite;
    int currentIndex;

    sscanf(line, "%s", token);

    if (token[0] == 'S') {
        if (spriteTable == 0) {
            spriteTable = new SpriteHashTable(0x11);
        }

        if (g_CurrentSprite_004686f8 != 0) {
            currentSprite = g_CurrentSprite_004686f8;
            currentIndex = g_CurrentSpriteIndex_004686fc;
            SpriteHashTable* table = spriteTable;

            HashNode* entry = table->Lookup(currentIndex, &slot);
            if (entry == 0) {
                if (table->buckets == 0) {
                    table->Resize(table->maxSize, 1);
                }
                entry = table->AllocEntry();
                ((int*)entry)[1] = slot;
                ((int*)entry)[2] = currentIndex;
                ((int*)entry)[0] = ((int*)table->buckets)[slot];
                ((int*)table->buckets)[slot] = (int)entry;
            }
            ((int*)entry)[3] = (int)currentSprite;
            g_CurrentSprite_004686f8 = 0;
        }

        g_CurrentSprite_004686f8 = new SpriteHashTable();

        sscanf(line, " %s %d ", token, &g_CurrentSpriteIndex_004686fc);

        ParseSpriteData(line + 3);

        return 0;
    }
    else {
        if (_stricmp(token, "END") == 0) {
            if (g_CurrentSprite_004686f8 != 0) {
                currentSprite = g_CurrentSprite_004686f8;
                currentIndex = g_CurrentSpriteIndex_004686fc;
                SpriteHashTable* table = spriteTable;

                HashNode* entry = table->Lookup(currentIndex, &slot);
                if (entry == 0) {
                    if (table->buckets == 0) {
                        table->Resize(table->maxSize, 1);
                    }
                    entry = table->AllocEntry();
                    ((int*)entry)[1] = slot;
                    ((int*)entry)[2] = currentIndex;
                    ((int*)entry)[0] = ((int*)table->buckets)[slot];
                    ((int*)table->buckets)[slot] = (int)entry;
                }
                ((int*)entry)[3] = (int)currentSprite;
                g_CurrentSprite_004686f8 = 0;
            }
            return 1;
        }

        if (_stricmp(token, "S") == 0) {
            if (g_CurrentSprite_004686f8 != 0) {
                currentSprite = g_CurrentSprite_004686f8;
                currentIndex = g_CurrentSpriteIndex_004686fc;
                SpriteHashTable* table = spriteTable;

                HashNode* entry = table->Lookup(currentIndex, &slot);
                if (entry == 0) {
                    if (table->buckets == 0) {
                        table->Resize(table->maxSize, 1);
                    }
                    entry = table->AllocEntry();
                    ((int*)entry)[1] = slot;
                    ((int*)entry)[2] = currentIndex;
                    ((int*)entry)[0] = ((int*)table->buckets)[slot];
                    ((int*)table->buckets)[slot] = (int)entry;
                }
                ((int*)entry)[3] = (int)currentSprite;
                g_CurrentSprite_004686f8 = 0;
            }
            return 0;
        }

        if (g_CurrentSprite_004686f8 != 0) {
            ParseSpriteData(line);
        }
        else {
            Parser::ReportUnknownLabel("CombatSprite");
        }

        return 0;
    }
}

// Struct for sprite data entry (8 bytes)
struct SpriteDataEntry {
    int index;      // 0x0 - frame index
    int spriteIdx;  // 0x4 - sprite array index
    SpriteDataEntry(int idx, int spr) : index(idx), spriteIdx(spr) { g_SpriteEntryCount_004686f4++; }
    ~SpriteDataEntry();
};

/* Function start: 0x409500 */ /* ~98% match */
void CombatSprite::ParseSpriteData(char* line) {
    char token[64];
    char spriteName[20];
    int prevTail;
    SpriteDataEntry* entryData;
    int* headPtr;
    int frameIndex;
    char* currentPos;
    int spriteIdx;
    int iVar;
    SpriteHashTable* spriteTable;
    int* tailPtr;
    int* node;
    int* newPool;
    int* growPtr;
    int growCount;

    currentPos = line;

    while (1) {
        frameIndex = 0;
        spriteName[0] = 0;

        if (sscanf(currentPos, "%s", token) == 0) {
            return;
        }

        if (sscanf(token, "%d,%s", &frameIndex, spriteName) != 2) {
            return;
        }

        if (frameIndex == 0) {
            return;
        }

        currentPos = strstr(currentPos, token);
        if (currentPos == 0) {
            return;
        }
        currentPos = currentPos + strlen(token);

        spriteIdx = 0;
        iVar = 0;
        while (1) {
            if (g_TargetList_0046ae58->count == spriteIdx) {
                ShowError("Error! Uknown sprite id=> %s", spriteName);
            }

            if (_stricmp(g_TargetList_0046ae58->targets[spriteIdx]->identifier, spriteName) == 0) {
                break;
            }
            iVar = iVar + 4;
            spriteIdx = spriteIdx + 1;
        }

        if (g_CurrentSprite_004686f8 != 0 && g_CurrentSprite_004686f8->head != 0) {
            SpriteDataEntry* lastEntry = *(SpriteDataEntry**)(g_CurrentSprite_004686f8->maxSize + 8);
            if (frameIndex < lastEntry->index) {
                ShowError("Error! sprite out of sequence %s", token);
            }
        }

        entryData = new SpriteDataEntry(frameIndex, spriteIdx);

        spriteTable = g_CurrentSprite_004686f8;
        headPtr = (int*)spriteTable + 1;
        tailPtr = (int*)&spriteTable->tail;
        prevTail = *headPtr;

        if (*tailPtr == 0) {
            growPtr = (int*)&spriteTable->growSize;
            growCount = *growPtr;
            newPool = (int*)new char[growCount * 12 + 4];
            *newPool = spriteTable->count;
            spriteTable->count = (int)newPool;

            growCount = *growPtr;
            node = (int*)((char*)newPool + growCount * 12 - 8);
            growCount = growCount - 1;
            while (growCount >= 0) {
                growCount = growCount - 1;
                *node = *tailPtr;
                *tailPtr = (int)node;
                node = node - 3;
            }
        }

        node = (int*)*tailPtr;
        *tailPtr = *node;

        node[1] = prevTail;
        node[0] = 0;
        (*(int*)&spriteTable->head)++;
        node[2] = 0;

        iVar = 0;
        do {
            int tmp = iVar;
            iVar--;
            if (tmp == 0) break;
        } while (1);

        node[2] = (int)entryData;

        if ((int*)*headPtr != 0) {
            *((int*)*headPtr) = (int)node;
        } else {
            *(int*)spriteTable = (int)node;
        }
        *headPtr = (int)node;
    }
}

/* Function start: 0x409730 */
int CombatSprite::ProcessFrame(int frame) {
    SpriteDataEntry* currentData;
    int* nextNode;
    Target* target;
    int count;

    count = -1;
    currentData = currentEntry;
    if (currentData != 0 && (count = 0, currentData->index <= frame)) {
loop:
        target = g_TargetList_0046ae58->targets[currentEntry->spriteIdx];
        if (target->active != 0) goto done;
        count++;
        target->Spawn();
        nextNode = currentNode;
        if (nextNode == 0) {
            currentEntry = 0;
            goto done;
        }
        currentNode = (int*)*nextNode;
        currentData = (SpriteDataEntry*)nextNode[2];
        currentEntry = currentData;
        if (currentData->index <= frame) {
            goto loop;
        }
    }
done:
    return count;
}
