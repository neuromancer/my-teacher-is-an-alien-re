#include "FilePosCache.h"
#include "Memory.h"
#include "SC_Question.h"
#include "globals.h"
#include <string.h>


// g_FilePosCache_0046928c — defined in globals.cpp

/* Function start: 0x412130 */
__int64 FilePosCache::Lookup(char* fname, char* keyName) {
    int* pool = (int*)g_SoundPool_00469134;
    int* node = (int*)pool[0];  // head
    g_CacheLRUNode_00469138 = (void*)node;

    if (node == 0) {
        goto not_found;
    }

    while (node != 0) {
        int* next = (int*)node[0];
        FilePosEntry* entry = (FilePosEntry*)node[2];

        // Compare filename
        if (strcmp(entry->filename, fname) == 0) {
            // Compare key
            if (strcmp(entry->key, keyName) == 0) {
                // Hit
                g_CacheHitCount_0046912c++;
                entry->accessCount++;
                __int64 result;
                ((int*)&result)[0] = entry->posLo;
                ((int*)&result)[1] = entry->posHi;
                return result;
            }
        }

        if (entry->accessCount < 0x2710) {
            g_CacheLRUNode_00469138 = (void*)node;
        }
        node = next;
    }

not_found:
    g_CacheMissCount_00469130++;
    return -1;
}

/* Function start: 0x412210 */
void FilePosCache::Store(char* fname, char* keyName, int posLo, int posHi) {
    int* pool = (int*)g_SoundPool_00469134;

    // If pool is full, evict LRU entry
    if (pool[2] == g_SoundTrackerField1_00469128) {
        ShowError("FilePosCache::Set - Error \nMore than %d labels found in .mis files \nIncrease Cache size", g_SoundTrackerField1_00469128);

        // Evict node at g_CacheLRUNode_00469138
        int* evictNode = (int*)g_CacheLRUNode_00469138;
        int* poolPtr = (int*)g_SoundPool_00469134;

        // Unlink from list
        if ((int*)poolPtr[0] == evictNode) {
            poolPtr[0] = evictNode[0];
        } else {
            int* prev = (int*)evictNode[1];
            prev[0] = evictNode[0];
        }

        int* evictPrev = (int*)evictNode[1];
        if ((int*)poolPtr[1] == evictNode) {
            poolPtr[1] = (int)evictPrev;
        } else {
            int* next = (int*)evictNode[0];
            next[1] = (int)evictPrev;
        }

        // Free entry data
        int* entrySlot = &evictNode[2];
        int edi = 0;
        do {
            int* entryData = (int*)*entrySlot;
            if (entryData != 0) {
                FreeMemory(entryData);
                *entrySlot = 0;
            }
            entrySlot++;
            int tmp = edi;
            edi--;
        } while (edi != 0);

        // Return node to free list
        int* reloadPool = (int*)g_SoundPool_00469134;
        evictNode[0] = reloadPool[3];
        reloadPool[3] = (int)evictNode;
        reloadPool[2]--;
    }

    // Allocate new entry
    FilePosEntry* newEntry = (FilePosEntry*)AllocateMemory(0x50);
    if (newEntry != 0) {
        strcpy(newEntry->filename, fname);
        strcpy(newEntry->key, keyName);
        newEntry->posLo = posLo;
        newEntry->posHi = posHi;
        newEntry->accessCount = 0;
    }

    // Push to pool
    int* pool2 = (int*)g_SoundPool_00469134;
    int* tailPtr = &pool2[1];
    int* freeList = &pool2[3];
    int tail = *tailPtr;

    if (*freeList == 0) {
        int* growPtr = &pool2[5];
        int growBy = *growPtr;
        int allocSize = growBy * 12 + 4;
        int* block = (int*)AllocateMemory(allocSize);
        block[0] = pool2[4];
        pool2[4] = (int)block;
        int i = *growPtr;
        i--;
        int* p = (int*)((char*)block + *growPtr * 12 - 8);
        while (i >= 0) {
            *p = *freeList;
            *freeList = (int)p;
            p -= 3;
            i--;
        }
    }

    int* node = (int*)*freeList;
    *freeList = node[0];
    node[1] = tail;
    node[0] = 0;
    pool2[2]++;
    node[2] = 0;
    { volatile int n = 0; while (n-- != 0); }
    node[2] = (int)newEntry;

    if ((int*)tail != 0) {
        *(int*)tail = (int)node;
    } else {
        pool2[0] = (int)node;
    }
    *tailPtr = (int)node;
}
