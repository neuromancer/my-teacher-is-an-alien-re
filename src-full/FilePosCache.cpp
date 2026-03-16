#include "FilePosCache.h"
#include "Memory.h"
#include "SC_Question.h"
#include <string.h>

struct SoundPool;
extern SoundPool* DAT_00469134;  // cache pool
extern void* DAT_00469138;       // LRU tracking node
extern int DAT_00469128;         // max cache size
extern int DAT_0046912c;         // hit counter
extern int DAT_00469130;         // miss counter

FilePosCache* g_FilePosCache_46928c = 0;  // DAT_0046928c

/* Function start: 0x412130 */
__int64 FilePosCache::Lookup(char* fname, char* keyName) {
    int* pool = (int*)DAT_00469134;
    int* node = (int*)pool[0];  // head
    DAT_00469138 = (void*)node;

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
                DAT_0046912c++;
                entry->accessCount++;
                __int64 result;
                ((int*)&result)[0] = entry->posLo;
                ((int*)&result)[1] = entry->posHi;
                return result;
            }
        }

        if (entry->accessCount < 0x2710) {
            DAT_00469138 = (void*)node;
        }
        node = next;
    }

not_found:
    DAT_00469130++;
    return -1;
}

/* Function start: 0x412210 */
void FilePosCache::Store(char* fname, char* keyName, int posLo, int posHi) {
    int* pool = (int*)DAT_00469134;

    // If pool is full, evict LRU entry
    if (pool[2] == DAT_00469128) {
        ShowError("FilePosCache::Set - Error \nMore than %d labels found in .mis files \nIncrease Cache size", DAT_00469128);

        // Evict node at DAT_00469138
        int* evictNode = (int*)DAT_00469138;
        FilePosEntry* evictEntry = (FilePosEntry*)evictNode[2];

        // Unlink from list
        if ((int*)pool[0] == evictNode) {
            pool[0] = evictNode[0];
        } else {
            int* prev = (int*)evictNode[1];
            prev[0] = evictNode[0];
        }
        if ((int*)pool[1] == evictNode) {
            pool[1] = evictNode[1];
        } else {
            int* next = (int*)evictNode[0];
            next[1] = evictNode[1];
        }

        // Free entry data
        if (evictEntry != 0) {
            FreeMemory(evictEntry);
            evictNode[2] = 0;
        }
        { volatile int n = 0; while (n-- != 0); }

        // Return node to free list
        evictNode[0] = pool[3];
        pool[3] = (int)evictNode;
        pool[2]--;
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
    int* freeList = &pool[3];
    if (*freeList == 0) {
        int growBy = pool[5];
        int* block = (int*)AllocateMemory(growBy * 12 + 4);
        block[0] = pool[4];
        pool[4] = (int)block;
        int i = growBy - 1;
        int* p = (int*)((char*)block + growBy * 12 - 8);
        while (i >= 0) {
            *p = *freeList;
            *freeList = (int)p;
            p -= 3;
            i--;
        }
    }

    int* node = (int*)*freeList;
    int* tail = (int*)pool[1];
    *freeList = node[0];
    node[1] = (int)tail;
    node[0] = 0;
    pool[2]++;
    node[2] = 0;
    { volatile int n = 0; while (n-- != 0); }
    node[2] = (int)newEntry;

    if (tail != 0) {
        tail[0] = (int)node;
    } else {
        pool[0] = (int)node;
    }
    pool[1] = (int)node;
}
