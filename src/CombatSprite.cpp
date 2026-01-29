#include "CombatSprite.h"
#include "Memory.h"
#include "string.h"
#include "EngineSubsystems.h"
#include "globals.h"
#include "Target.h"
#include <stdio.h>
#include <string.h>

// Global variables
SpriteHashTable* g_CurrentSprite = 0;   // 0x436348
int g_CurrentSpriteIndex = 0;           // 0x43634c
int DAT_00436344 = 0;                   // sprite data entry counter

// External declaration for Parser destructor helper
extern void FUN_0041556a();

/* Function start: 0x415220 */
void SpriteHashTable::Clear() {
    int* node;
    int* poolBlock;
    int* nextPool;
    int delayCounter;

    node = (int*)SpriteHashTable::buckets;
    while (node != 0) {
        delayCounter = 0;
        do {
            int temp = delayCounter;
            delayCounter = delayCounter - 1;
            if (temp == 0) break;
        } while (1);
        node = (int*)*node;
    }

    SpriteHashTable::head = 0;
    SpriteHashTable::tail = 0;
    SpriteHashTable::maxSize = 0;
    SpriteHashTable::buckets = 0;

    poolBlock = (int*)SpriteHashTable::count;
    while (poolBlock != 0) {
        nextPool = (int*)*poolBlock;
        delete poolBlock;
        poolBlock = nextPool;
    }
    SpriteHashTable::count = 0;
}

/* Function start: 0x415270 */
void SpriteHashTable::AllocateBuckets(int size, int flag) {
    int* newBuckets;
    int count;

    if (SpriteHashTable::buckets != 0) {
        delete SpriteHashTable::buckets;
        SpriteHashTable::buckets = 0;
    }

    if (flag != 0) {
        newBuckets = (int*)AllocateMemory(size * 4);
        count = (size * 4) >> 2;
        SpriteHashTable::buckets = (void**)newBuckets;
        while (count != 0) {
            *newBuckets = 0;
            newBuckets++;
            count--;
        }
    }
    SpriteHashTable::maxSize = size;
}

/* Function start: 0x4152D0 */
void* SpriteHashTable::AllocateNode() {
    int* newPool;
    int* node;
    int i;
    int delayCounter;

    if (SpriteHashTable::tail == 0) {
        newPool = (int*)AllocateMemory(SpriteHashTable::growSize * 16 + 4);
        *newPool = SpriteHashTable::count;
        i = SpriteHashTable::growSize;
        SpriteHashTable::count = (int)newPool;
        node = (int*)((char*)newPool + i * 16 - 12);
        i = i - 1;
        while (i >= 0) {
            *node = (int)SpriteHashTable::tail;
            SpriteHashTable::tail = node;
            node = node - 4;
            i = i - 1;
        }
    }

    node = (int*)SpriteHashTable::tail;
    SpriteHashTable::tail = (void*)*node;
    SpriteHashTable::head = (void*)((int)SpriteHashTable::head + 1);
    node[2] = 0;
    delayCounter = 0;
    do {
        int temp = delayCounter;
        delayCounter = delayCounter - 1;
        if (temp == 0) break;
    } while (1);
    node[3] = 0;
    delayCounter = 0;
    do {
        int temp = delayCounter;
        delayCounter = delayCounter - 1;
        if (temp == 0) break;
    } while (1);
    return node;
}

/* Function start: 0x415c10 */
void* SpriteHashTable::Lookup(int index, int* outSlot) {
    int* node;

    *outSlot = ((unsigned int)index >> 4) % (unsigned int)SpriteHashTable::maxSize;

    if (SpriteHashTable::buckets == 0) {
        return 0;
    }

    node = (int*)SpriteHashTable::buckets[*outSlot];
    while (node != 0) {
        if (node[2] == index) {
            return node;
        }
        node = (int*)*node;
    }
    return 0;
}

/* Function start: 0x415c50 */
void SpriteHashTable::Resize(int size, int flag) {
    int* newBuckets;
    int count;

    if (SpriteHashTable::buckets != 0) {
        delete SpriteHashTable::buckets;
        SpriteHashTable::buckets = 0;
    }

    if (flag != 0) {
        newBuckets = (int*)AllocateMemory(size * 4);
        count = (size * 4) >> 2;
        SpriteHashTable::buckets = (void**)newBuckets;
        while (count != 0) {
            *newBuckets = 0;
            newBuckets++;
            count--;
        }
        SpriteHashTable::maxSize = size;
        return;
    }
    SpriteHashTable::maxSize = size;
}

/* Function start: 0x415cb0 */
void* SpriteHashTable::AllocEntry() {
    int* newPool;
    int* node;
    int i;
    int delayCounter;

    if (SpriteHashTable::tail == 0) {
        newPool = (int*)AllocateMemory(SpriteHashTable::growSize * 16 + 4);
        *newPool = SpriteHashTable::count;
        i = SpriteHashTable::growSize;
        SpriteHashTable::count = (int)newPool;
        node = (int*)((char*)newPool + i * 16 - 12);
        i = i - 1;
        while (i >= 0) {
            *node = (int)SpriteHashTable::tail;
            SpriteHashTable::tail = node;
            node = node - 4;
            i = i - 1;
        }
    }

    node = (int*)SpriteHashTable::tail;
    SpriteHashTable::tail = (void*)*node;
    SpriteHashTable::head = (void*)((int)SpriteHashTable::head + 1);
    node[2] = 0;
    delayCounter = 0;
    do {
        int temp = delayCounter;
        delayCounter = delayCounter - 1;
        if (temp == 0) break;
    } while (1);
    node[3] = 0;
    delayCounter = 0;
    do {
        int temp = delayCounter;
        delayCounter = delayCounter - 1;
        if (temp == 0) break;
    } while (1);
    return node;
}

/* Function start: 0x415340 */
void FreePointerArray(void** arr, int count) {
    int temp;
    temp = count;
    count = count - 1;
    if (temp == 0) {
        return;
    }
    do {
        if (*arr != 0) {
            delete *arr;
            *arr = 0;
        }
        arr = arr + 1;
        temp = count;
        count = count - 1;
    } while (temp != 0);
}

/* Function start: 0x415380 */
void FreeNestedHashTables(void** arr, int count) {
    SpriteHashTable* table;
    int* node;
    int* poolBlock;
    int* nextPool;
    int temp;

    temp = count;
    count = count - 1;
    if (temp == 0) {
        return;
    }
    do {
        table = (SpriteHashTable*)*arr;
        if (table != 0) {
            node = (int*)table->buckets;
            if (node != 0) {
                do {
                    FreePointerArray((void**)(node + 2), 1);
                    node = (int*)*node;
                } while (node != 0);
            }
            table->head = 0;
            table->tail = 0;
            table->maxSize = 0;
            table->buckets = 0;
            poolBlock = (int*)table->count;
            while (poolBlock != 0) {
                nextPool = (int*)*poolBlock;
                delete poolBlock;
                poolBlock = nextPool;
            }
            table->count = 0;
            delete table;
            *arr = 0;
        }
        arr = arr + 1;
        temp = count;
        count = count - 1;
    } while (temp != 0);
}

/* Function start: 0x415410 */
CombatSprite::CombatSprite() : Parser() {
    CombatSprite::spriteTable = 0;
    CombatSprite::field_0x8c = 0;
    CombatSprite::field_0x90 = 0;
    CombatSprite::field_0x94 = 0;
}

/* Function start: 0x415480 */
CombatSprite::~CombatSprite() {
    SpriteHashTable* table;
    int* node;
    int* poolBlock;
    int* nextPool;
    unsigned int i;
    int offset;
    int delayCounter;

    table = CombatSprite::spriteTable;
    if (table != 0) {
        if (table->buckets != 0 && table->maxSize != 0) {
            i = 0;
            offset = 0;
            do {
                node = *(int**)((char*)table->buckets + offset);
                if (node != 0) {
                    do {
                        FreeNestedHashTables((void**)(node + 3), 1);
                        delayCounter = 0;
                        do {
                            int temp = delayCounter;
                            delayCounter = delayCounter - 1;
                            if (temp == 0) break;
                        } while (1);
                        node = (int*)*node;
                    } while (node != 0);
                }
                offset = offset + 4;
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
        CombatSprite::spriteTable = 0;
    }
}

/* Function start: 0x4155E0 */
int CombatSprite::PlayById(unsigned int param_1) {
    int* puVar2;
    int* piVar1;
    unsigned int hashIndex;

    CombatSprite::field_0x8c = 0;
    piVar1 = (int*)CombatSprite::spriteTable;
    if (piVar1 != 0) {
        hashIndex = (param_1 >> 4) % (unsigned int)piVar1[1];
        puVar2 = (int*)*piVar1;
        if (puVar2 != 0) {
            puVar2 = (int*)puVar2[hashIndex];
            while (puVar2 != 0) {
                if ((unsigned int)puVar2[2] == param_1) goto found;
                puVar2 = (int*)*puVar2;
            }
        }
        puVar2 = 0;
    found:
        if (puVar2 != 0) {
            piVar1 = (int*)puVar2[3];
            CombatSprite::field_0x94 = (int)piVar1;
            puVar2 = (int*)*piVar1;
            CombatSprite::field_0x90 = (int)puVar2;
            if (puVar2 != 0) {
                CombatSprite::field_0x90 = *puVar2;
                CombatSprite::field_0x8c = puVar2[2];
                return 1;
            }
        }
    }
    return 0;
}

/* Function start: 0x41569E */
int CombatSprite::LBLParse(char* line) {
    char token[52];
    int slot;
    void* entry;
    SpriteHashTable* table;
    SpriteHashTable* currentSprite;
    int currentIndex;

    sscanf(line, "%s", token);

    if (token[0] == 'S') {
        // Create sprite table if it doesn't exist
        if (CombatSprite::spriteTable == 0) {
            CombatSprite::spriteTable = new SpriteHashTable(0x11);
        }

        // Commit current sprite to table if exists
        if (g_CurrentSprite != 0) {
            currentSprite = g_CurrentSprite;
            currentIndex = g_CurrentSpriteIndex;
            table = CombatSprite::spriteTable;

            entry = table->Lookup(currentIndex, &slot);
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
            g_CurrentSprite = 0;
        }

        // Create new sprite entry
        g_CurrentSprite = new SpriteHashTable();

        // Parse " %s %d " format to get label and index
        sscanf(line, " %s %d ", token, &g_CurrentSpriteIndex);

        // Parse sprite data starting after "Sxx"
        CombatSprite::ParseSpriteData(line + 3);

        return 0;
    }
    else {
        // Check for END marker
        if (_stricmp(token, "END") == 0) {
            // Commit current sprite if exists
            if (g_CurrentSprite != 0) {
                currentSprite = g_CurrentSprite;
                currentIndex = g_CurrentSpriteIndex;
                table = CombatSprite::spriteTable;

                entry = table->Lookup(currentIndex, &slot);
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
                g_CurrentSprite = 0;
            }
            return 1;
        }

        // Check for continuation marker
        if (_stricmp(token, "S") == 0) {
            // Commit current sprite if exists
            if (g_CurrentSprite != 0) {
                currentSprite = g_CurrentSprite;
                currentIndex = g_CurrentSpriteIndex;
                table = CombatSprite::spriteTable;

                entry = table->Lookup(currentIndex, &slot);
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
                g_CurrentSprite = 0;
            }
            return 0;
        }

        // Continue parsing sprite data or error
        if (g_CurrentSprite != 0) {
            CombatSprite::ParseSpriteData(line);
        }
        else {
            ShowError("CombatSprite");
        }

        return 0;
    }
}

// Struct for sprite data entry (8 bytes)
struct SpriteDataEntry {
    int index;      // 0x0 - frame index
    int spriteIdx;  // 0x4 - sprite array index
};

/* Function start: 0x415960 */
void CombatSprite::ParseSpriteData(char* line) {
    char token[64];
    char spriteName[20];
    int frameIndex;
    char* currentPos;
    int spriteIdx;
    int iVar;
    SpriteDataEntry* entryData;
    SpriteHashTable* spriteTable;
    int* tailPtr;
    int* headPtr;
    int* node;
    int* newPool;
    int prevTail;
    int growCount;

    currentPos = line;

parseLoop:
    frameIndex = 0;
    spriteName[0] = 0;

    // Parse "%s" from current position
    if (sscanf(currentPos, "%s", token) == 0) {
        return;
    }

    // Parse "%d,%s" from token
    if (sscanf(token, "%d,%s", &frameIndex, spriteName) != 2) {
        return;
    }

    if (frameIndex == 0) {
        return;
    }

    // Find token in currentPos and advance past it
    currentPos = strstr(currentPos, token);
    if (currentPos == 0) {
        return;
    }
    currentPos = currentPos + strlen(token);

    // Find sprite index by name in TargetList
    spriteIdx = 0;
    iVar = 0;
    while (1) {
        if (((TargetList*)DAT_00435f0c)->count == spriteIdx) {
            ShowError("Error! Uknown sprite id=> %s", spriteName);
        }

        if (_stricmp(((TargetList*)DAT_00435f0c)->targets[spriteIdx]->identifier, spriteName) == 0) {
            break;
        }
        iVar = iVar + 4;
        spriteIdx = spriteIdx + 1;
    }

    // Validate sequence if g_CurrentSprite has entries
    if (g_CurrentSprite != 0 && (int)g_CurrentSprite->head != 0) {
        if (frameIndex < *(int*)(*(int*)((int)g_CurrentSprite->tail + 8))) {
            ShowError("Error! sprite out of sequence %s", token);
        }
    }

    // Allocate sprite data entry (8 bytes)
    entryData = (SpriteDataEntry*)AllocateMemory(8);
    if (entryData != 0) {
        entryData->index = frameIndex;
        entryData->spriteIdx = spriteIdx;
        DAT_00436344++;
    } else {
        entryData = 0;
    }

    // Add to g_CurrentSprite linked list (similar to SpriteHashTable::AllocEntry)
    spriteTable = g_CurrentSprite;
    tailPtr = (int*)&spriteTable->tail;
    headPtr = (int*)spriteTable + 1;  // buckets (used as head pointer here)
    prevTail = *headPtr;

    // If no free nodes, allocate more
    if (*tailPtr == 0) {
        growCount = *(int*)((int)spriteTable + 0x14);  // growSize
        newPool = (int*)AllocateMemory(growCount * 12 + 4);
        *newPool = *(int*)((int)spriteTable + 0x10);  // link to previous pool
        *(int*)((int)spriteTable + 0x10) = (int)newPool;

        // Build free list from new pool
        node = (int*)((char*)newPool + growCount * 12 - 8);
        growCount--;
        while (growCount >= 0) {
            *node = *tailPtr;
            *tailPtr = (int)node;
            node = node - 3;
            growCount--;
        }
    }

    // Pop node from free list
    node = (int*)*tailPtr;
    *tailPtr = *node;

    // Initialize node
    node[1] = prevTail;
    node[0] = 0;
    (*(int*)&spriteTable->head)++;  // count (head used as count in this context)
    node[2] = 0;

    // destructor delay loop
    iVar = 0;
    do {
        int tmp = iVar;
        iVar--;
        if (tmp == 0) break;
    } while (1);

    // Store entry data
    node[2] = (int)entryData;

    // Link node into list
    if ((int*)*headPtr == 0) {
        *(int*)spriteTable = (int)node;
        *headPtr = (int)node;
    } else {
        *((int*)*headPtr) = (int)node;
        *headPtr = (int)node;
    }

    goto parseLoop;
}

/* Function start: 0x415B90 */
int CombatSprite::ProcessFrame(int frame) {
    int* currentData;
    int* nextNode;
    Target* target;
    int count;

    count = -1;
    currentData = (int*)CombatSprite::field_0x8c;
    if (currentData != 0 && (count = 0, *currentData <= frame)) {
        while (target = ((TargetList*)DAT_00435f0c)->targets[*(int*)(CombatSprite::field_0x8c + 4)],
               target->active == 0) {
            count++;
            target->Spawn();
            nextNode = (int*)CombatSprite::field_0x90;
            if (nextNode == 0) {
                CombatSprite::field_0x8c = 0;
                return count;
            }
            CombatSprite::field_0x90 = *nextNode;
            currentData = (int*)nextNode[2];
            CombatSprite::field_0x8c = (int)currentData;
            if (*currentData > frame) {
                return count;
            }
        }
    }
    return count;
}
