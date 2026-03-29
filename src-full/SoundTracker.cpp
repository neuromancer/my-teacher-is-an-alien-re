#include "SoundTracker.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <io.h>

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
        blockSize = param;
    }
    ~SoundPool();
};

extern int g_SoundTrackerField1_00469128;
extern SoundPool* g_SoundPool_00469134;
extern int g_CacheHitCount_0046912c;
extern int g_CacheMissCount_00469130;
extern void* g_CacheLRUNode_00469138;
extern int g_SoundTrackerField3_00469140;
extern int g_SoundTrackerField2_0046913c;

extern "C" int FileExists(const char*);
extern "C" char* internal_ReadLine(char*, int, FILE*);
extern void EncryptAndWrite(char*, FILE*);
// sscanf from stdio.h
// _findfirst/_findnext from io.h
// FUN_00454510 = sprintf — CRT function
// FUN_0044e470 = MakeAudioName — implemented in string.cpp

#include "Timer.h"
#include "FilePosCache.h"
extern void* g_FilePosCache;

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
        FILE* fp = _fsopen("cfg\\miscache.dat", "r", 0x40);
        char* result = internal_ReadLine(lineBuf, 0xff, fp);
        while (result != 0) {
            sscanf(lineBuf, " %s %s %lu %d", filePath, sectionBuf, &filePos[0], &filePos[1]);
            filePos[0] = filePos[0];
            filePos[1] = 0;
            ((FilePosCache*)g_FilePosCache)->Store(filePath, sectionBuf, filePos[0], 0);
            result = internal_ReadLine(lineBuf, 0xff, fp);
        }
        fclose(fp);
    } else {
        long handle = _findfirst("mis\\*.mis", &findData);
        if (handle != -1) {
            do {
                char misPath[64];
                sprintf(misPath, "mis\\%s", findData.name);
                FILE* fp = _fsopen(misPath, "r", 0x40);
                char* result = internal_ReadLine(lineBuf, 0xff, fp);
                while (result != 0) {
                    sscanf(lineBuf, " %s ", sectionBuf);
                    if (sectionBuf[0] == '[') {
                        fgetpos(fp, (fpos_t*)&filePos[0]);
                        ((FilePosCache*)g_FilePosCache)->Store(misPath, sectionBuf, filePos[0], filePos[1]);
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
    int* node;
    int* data;

    FILE* fp = _fsopen("cfg\\miscache.dat", "w", 0x40);
    node = g_SoundPool_00469134->head;
    while (node != 0) {
        int* next = (int*)node[0];
        data = node + 2;
        sprintf(lineBuf, "%-32s %-32s %4lu %4d \n", data + 1, data + 9, data[0x12], data[0]);
        EncryptAndWrite(lineBuf, fp);
        node = next;
    }
    fclose(fp);
}
