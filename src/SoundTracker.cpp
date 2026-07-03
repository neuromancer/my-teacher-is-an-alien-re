#include "SoundTracker.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <io.h>
#include "globals.h"

struct FilePosEntry;

// Pooled cache list node — layout: next(+0x00), prev(+0x04), entry(+0x08); 12 bytes
struct SoundPoolNode {
    SoundPoolNode* next;    // 0x00
    SoundPoolNode* prev;    // 0x04
    FilePosEntry* entry;    // 0x08
};

struct SoundPool {
    SoundPoolNode* head;     // 0x00
    SoundPoolNode* tail;     // 0x04
    int count;               // 0x08
    SoundPoolNode* freeList; // 0x0C
    int* blockList;          // 0x10 — raw node blocks; block[0] = next block
    int blockSize;           // 0x14

    SoundPool(int param) {
        count = 0;
        freeList = 0;
        tail = 0;
        head = 0;
        blockList = 0;
        blockSize = g_SoundTrackerField1_00469128;
    }
    ~SoundPool();
};

extern "C" int FileExists(const char*);
extern "C" char* internal_ReadLine(char*, int, FILE*);
extern "C" void ShowError(const char* format, ...);
extern void EncryptAndWrite(char*, FILE*);
extern "C" FILE* fsopen(const char*, const char*);
// sscanf from stdio.h
// _findfirst/_findnext from io.h
// FUN_00454510 = sprintf — CRT function
// FUN_0044e470 = MakeAudioName — implemented in string.cpp

#include "Timer.h"

/* Function start: 0x412000 */
SoundTracker::SoundTracker(int param) {
    field_0 = 0;
    g_SoundTrackerField1_00469128 = param;
    g_SoundPool_00469134 = new SoundPool(param);
    Init();
}

/* Function start: 0x4120A0 */
SoundTracker::~SoundTracker() {
    SoundPool* p;
    SoundPoolNode* node;
    int* next;
    FilePosEntry** entrySlot;
    int i;

    Cleanup();
    if (g_SoundPool_00469134 != 0) {
        p = g_SoundPool_00469134;
        node = p->head;
        while (node != 0) {
            entrySlot = &node->entry;
            i = 0;
            do {
                if (*entrySlot != 0) {
                    operator delete(*entrySlot);
                    *entrySlot = 0;
                }
                entrySlot = entrySlot + 1;
                int tmp = i;
                i = i - 1;
                if (tmp == 0) break;
            } while (1);
            node = node->next;
        }
        p->count = 0;
        p->freeList = 0;
        p->tail = 0;
        p->head = 0;
        next = p->blockList;
        while (next != 0) {
            int* tmp = (int*)next[0];
            operator delete(next);
            next = tmp;
        }
        p->blockList = 0;
        operator delete(p);
        g_SoundPool_00469134 = 0;
    }
}

struct FilePosEntry {
    int accessCount;    // +0x00
    char filename[32];  // +0x04
    char key[36];       // +0x24
    int posLo;          // +0x48
    int posHi;          // +0x4C

    FilePosEntry(char* fname, char* keyName, int lo, int hi) {
        strcpy(filename, fname);
        strcpy(key, keyName);
        posLo = lo;
        posHi = hi;
        accessCount = 0;
    }
};

/* Function start: 0x412130 */
__int64 SoundTracker::Lookup(char* fname, char* keyName) {
    SoundPool* pool = g_SoundPool_00469134;
    SoundPoolNode* volatile node = pool->head;
    g_CacheLRUNode_00469138 = node;

    if (node == 0) {
        goto not_found;
    }

    while (node != 0) {
        SoundPoolNode* next = node->next;
        FilePosEntry* entry = node->entry;

        // Compare filename
        if (strcmp(entry->filename, fname) == 0) {
            // Compare key
            if (strcmp(entry->key, keyName) == 0) {
                // Hit
                g_CacheHitCount_0046912c++;
                entry->accessCount++;
                return *(__int64*)&entry->posLo;
            }
        }

        if (entry->accessCount < 0x2710) {
            g_CacheLRUNode_00469138 = node;
        }
        node = next;
    }

not_found:
    g_CacheMissCount_00469130++;
    return -1;
}

/* Function start: 0x412210 */
void SoundTracker::Store(char* fname, char* keyName, int posLo, int posHi) {
    SoundPool* pool = g_SoundPool_00469134;

    // If pool is full, evict LRU entry
    if (pool->count == g_SoundTrackerField1_00469128) {
        ShowError("FilePosCache::Set - Error \nMore than %d labels found in .mis files \nIncrease Cache size", g_SoundTrackerField1_00469128);

        // Evict node at g_CacheLRUNode_00469138
        SoundPool* poolPtr = g_SoundPool_00469134;
        SoundPoolNode* evictNode = (SoundPoolNode*)g_CacheLRUNode_00469138;

        // Unlink from list
        if (poolPtr->head == evictNode) {
            g_SoundPool_00469134->head = evictNode->next;
        } else {
            SoundPoolNode* prev = evictNode->prev;
            prev->next = evictNode->next;
        }

        SoundPoolNode* evictPrev = evictNode->prev;
        // Pool header doubles as a node: head/tail alias next/prev
        SoundPoolNode* backLinkOwner = (SoundPoolNode*)poolPtr;
        if (poolPtr->tail != evictNode) {
            backLinkOwner = evictNode->next;
        }
        backLinkOwner->prev = evictPrev;

        // Free entry data
        FilePosEntry** entrySlot = &evictNode->entry;
        int i = 0;
        do {
            FilePosEntry* entryData = *entrySlot;
            if (entryData != 0) {
                operator delete(entryData);
                *entrySlot = 0;
            }
            entrySlot++;
            int tmp = i;
            i--;
            if (tmp == 0) break;
        } while (1);

        // Return node to free list
        evictNode->next = poolPtr->freeList;
        poolPtr->freeList = evictNode;
        poolPtr->count--;
    }

    // Allocate new entry
    FilePosEntry* newEntry = new FilePosEntry(fname, keyName, posLo, posHi);

    // Push to pool
    SoundPool* pool2 = g_SoundPool_00469134;
    SoundPoolNode** tailPtr = &pool2->tail;
    SoundPoolNode** freeList = &pool2->freeList;
    SoundPoolNode* tail = *tailPtr;

    if (*freeList == 0) {
        int* growPtr = &pool2->blockSize;
        int allocSize = *growPtr;
        allocSize = allocSize + allocSize * 2;
        allocSize = allocSize << 2;
        allocSize += 4;
        int* block = (int*)operator new(allocSize);
        block[0] = (int)pool2->blockList;
        pool2->blockList = block;
        int i = *growPtr;
        int span = i + i * 2;
        i--;
        SoundPoolNode* p = (SoundPoolNode*)((char*)block + span * 4 - 8);
        if (i >= 0) {
            do {
                SoundPoolNode* next = *freeList;
                i--;
                p->next = next;
                *freeList = p;
                p -= 1;
            } while (i >= 0);
        }
    }

    SoundPoolNode* node = *freeList;
    *freeList = node->next;
    int n = 0;
    node->prev = tail;
    node->next = 0;
    pool2->count++;
    node->entry = 0;
    {
        do {
            int tmp = n;
            n--;
            if (tmp == 0) break;
        } while (1);
    }
    node->entry = newEntry;

    if (*tailPtr != 0) {
        (*tailPtr)->next = node;
    } else {
        pool2->head = node;
    }
    *tailPtr = node;
}

/* Function start: 0x412400 */
void SoundTracker::ClearPool() {
    SoundPool* p;
    SoundPoolNode* node;
    FilePosEntry** entrySlot;
    int i;
    int* next;

    p = g_SoundPool_00469134;
    node = p->head;
    while (node != 0) {
        entrySlot = &node->entry;
        i = 0;
        do {
            if (*entrySlot != 0) {
                operator delete(*entrySlot);
                *entrySlot = 0;
            }
            entrySlot = entrySlot + 1;
            int tmp = i;
            i = i - 1;
            if (tmp == 0) break;
        } while (1);
        node = node->next;
    }
    p->count = 0;
    p->freeList = 0;
    p->tail = 0;
    p->head = 0;
    next = p->blockList;
    while (next != 0) {
        int* tmp = (int*)next[0];
        operator delete(next);
        next = tmp;
    }
    p->blockList = 0;
    g_CacheMissCount_00469130 = 0;
    g_CacheHitCount_0046912c = 0;
    g_CacheLRUNode_00469138 = 0;
}

/* Function start: 0x412480 */
void SoundTracker::Init() {
    Timer timer;
    int maxTime;
    char lineBuf[256];
    struct _finddata_t findData;
    int fileTime;
    char fileName[260];
    char sectionBuf[64];
    char filePath[64];
    int filePos[2];

    maxTime = 0;
    g_SoundTrackerField3_00469140 = 0;
    ClearPool();
    timer.Reset();

    remove("cfg\\miscache.dat");

    if (FileExists("cfg\\miscache.dat") != 0) {
        long handle = _findfirst("mis\\*.mis", &findData);
        if (handle != -1) {
            do {
                fileTime = findData.time_write;
                if (maxTime <= fileTime) {
                    maxTime = fileTime;
                }
            } while (_findnext(handle, &findData) == 0);
        }
        _findfirst("cfg\\miscache.dat", &findData);
        if (findData.time_write < maxTime) {
            remove("cfg\\miscache.dat");
        }
    }

    if (FileExists("cfg\\miscache.dat") != 0) {
        FILE* fp = fsopen("cfg\\miscache.dat", "r");
        char* result = internal_ReadLine(lineBuf, 0xff, fp);
        while (result != 0) {
            sscanf(lineBuf, " %s %s %lu %d", filePath, sectionBuf, &filePos[0], &filePos[1]);
            filePos[0] = filePos[0];
            filePos[1] = 0;
            Store(filePath, sectionBuf, filePos[0], 0);
            result = internal_ReadLine(lineBuf, 0xff, fp);
        }
        fclose(fp);
    } else {
        long handle = _findfirst("mis\\*.mis", &findData);
        if (handle != -1) {
            do {
                char misPath[64];
                sprintf(misPath, "mis\\%s", findData.name);
                FILE* fp = fsopen(misPath, "r");
                char* result = internal_ReadLine(lineBuf, 0xff, fp);
                while (result != 0) {
                    sscanf(lineBuf, " %s ", sectionBuf);
                    if (sectionBuf[0] == '[') {
                        fgetpos(fp, (fpos_t*)&filePos[0]);
                        Store(misPath, sectionBuf, filePos[0], filePos[1]);
                    }
                    g_SoundTrackerField3_00469140++;
                    result = internal_ReadLine(lineBuf, 0xff, fp);
                }
                fclose(fp);
            } while (_findnext(handle, &findData) == 0);
            Cleanup();
        }
    }

    g_SoundTrackerField2_0046913c = timer.Update();
}

/* Function start: 0x412730 */
void SoundTracker::Cleanup() {
    char lineBuf[256];
    SoundPoolNode* volatile node;
    char* data;

    FILE* fp = fsopen("cfg\\miscache.dat", "w");
    node = g_SoundPool_00469134->head;
    while (node != 0) {
        data = (char*)node->entry + 4;
        node = node->next;
        sprintf(lineBuf, "%-32s %-32s %4lu %4d \n",
                data, data + 0x20, *(int*)(data + 0x44), *(int*)(data - 4));
        EncryptAndWrite(lineBuf, fp);
    }
    fclose(fp);
}
