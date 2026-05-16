#include "SoundTracker.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <io.h>
#include "globals.h"

struct SoundPool {
    int* head;      // 0x00
    int* tail;      // 0x04
    int count;      // 0x08
    int* freeList;  // 0x0C
    int* blockList; // 0x10
    int blockSize;  // 0x14

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
    int* node;
    int* next;
    int* data;
    int i;

    Cleanup();
    if (g_SoundPool_00469134 != 0) {
        p = g_SoundPool_00469134;
        node = p->head;
        while (node != 0) {
            data = (int*)(node + 2);
            i = 0;
            do {
                if (*data != 0) {
                    operator delete((void*)*data);
                    *data = 0;
                }
                data = data + 1;
                int tmp = i;
                i = i - 1;
                if (tmp == 0) break;
            } while (1);
            node = (int*)node[0];
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
    int* pool = (int*)g_SoundPool_00469134;
    int* volatile node = (int*)pool[0];  // head
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
                return *(__int64*)&entry->posLo;
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
void SoundTracker::Store(char* fname, char* keyName, int posLo, int posHi) {
    int* pool = (int*)g_SoundPool_00469134;

    // If pool is full, evict LRU entry
    if (pool[2] == g_SoundTrackerField1_00469128) {
        ShowError("FilePosCache::Set - Error \nMore than %d labels found in .mis files \nIncrease Cache size", g_SoundTrackerField1_00469128);

        // Evict node at g_CacheLRUNode_00469138
        int* poolPtr = (int*)g_SoundPool_00469134;
        int* evictNode = (int*)g_CacheLRUNode_00469138;

        // Unlink from list
        if ((int*)poolPtr[0] == evictNode) {
            ((int*)g_SoundPool_00469134)[0] = evictNode[0];
        } else {
            int* prev = (int*)evictNode[1];
            prev[0] = evictNode[0];
        }

        int* evictPrev = (int*)evictNode[1];
        int* backLinkOwner = poolPtr;
        if ((int*)poolPtr[1] != evictNode) {
            backLinkOwner = (int*)evictNode[0];
        }
        backLinkOwner[1] = (int)evictPrev;

        // Free entry data
        int* entrySlot = &evictNode[2];
        int i = 0;
        do {
            int* entryData = (int*)*entrySlot;
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
        evictNode[0] = poolPtr[3];
        poolPtr[3] = (int)evictNode;
        poolPtr[2]--;
    }

    // Allocate new entry
    FilePosEntry* newEntry = new FilePosEntry(fname, keyName, posLo, posHi);

    // Push to pool
    int* pool2 = (int*)g_SoundPool_00469134;
    int* tailPtr = &pool2[1];
    int* freeList = &pool2[3];
    int tail = *tailPtr;

    if (*freeList == 0) {
        int* growPtr = &pool2[5];
        int allocSize = *growPtr;
        allocSize = allocSize + allocSize * 2;
        allocSize = allocSize << 2;
        allocSize += 4;
        int* block = (int*)operator new(allocSize);
        block[0] = pool2[4];
        pool2[4] = (int)block;
        int i = *growPtr;
        int span = i + i * 2;
        i--;
        int* p = (int*)((char*)block + span * 4 - 8);
        if (i >= 0) {
            do {
                int next = *freeList;
                i--;
                *p = next;
                *freeList = (int)p;
                p -= 3;
            } while (i >= 0);
        }
    }

    int* node = (int*)*freeList;
    *freeList = node[0];
    int n = 0;
    node[1] = tail;
    node[0] = 0;
    pool2[2]++;
    node[2] = 0;
    {
        do {
            int tmp = n;
            n--;
            if (tmp == 0) break;
        } while (1);
    }
    node[2] = (int)newEntry;

    if ((int*)*tailPtr != 0) {
        *(int*)*tailPtr = (int)node;
    } else {
        pool2[0] = (int)node;
    }
    *tailPtr = (int)node;
}

/* Function start: 0x412400 */
void SoundTracker::ClearPool() {
    SoundPool* p;
    int* node;
    int* data;
    int i;
    int* next;

    p = g_SoundPool_00469134;
    node = p->head;
    while (node != 0) {
        data = node + 2;
        i = 0;
        do {
            if (*data != 0) {
                operator delete((void*)*data);
                *data = 0;
            }
            data = data + 1;
            int tmp = i;
            i = i - 1;
            if (tmp == 0) break;
        } while (1);
        node = (int*)node[0];
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
    int* volatile node;
    char* data;

    FILE* fp = fsopen("cfg\\miscache.dat", "w");
    node = g_SoundPool_00469134->head;
    while (node != 0) {
        data = (char*)node[2] + 4;
        node = (int*)node[0];
        sprintf(lineBuf, "%-32s %-32s %4lu %4d \n",
                data, data + 0x20, *(int*)(data + 0x44), *(int*)(data - 4));
        EncryptAndWrite(lineBuf, fp);
    }
    fclose(fp);
}
