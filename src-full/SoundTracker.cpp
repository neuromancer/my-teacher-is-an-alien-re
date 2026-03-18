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

extern int DAT_00469128;
extern SoundPool* DAT_00469134;
extern int DAT_0046912c;
extern int DAT_00469130;
extern void* DAT_00469138;
extern int DAT_00469140;
extern char DAT_00469310[];
extern char DAT_00469304[];
extern char DAT_00469300[];
extern char DAT_0046933c[];
extern char DAT_004692f0[];
extern char DAT_004692e8[];
extern char DAT_00469324[];
extern int DAT_0046913c;
extern char DAT_00468448[];

extern "C" void FUN_00455130(char*);
extern "C" int FUN_00425fa0(char*);
extern "C" void* FUN_00455110(char*, char*);
extern "C" char* FUN_00426400(char*, int, void*);
extern "C" void FUN_00426490(char*, void*);
extern "C" int FUN_00454850(char*, ...);
extern "C" long FUN_00454d20(char*, void*);
extern "C" int FUN_00454e70(long, void*);
extern "C" void FUN_00455040(void*);
extern "C" void FUN_004550b0(void*, int*);
extern "C" int FUN_00454510(char*, ...);
extern "C" int FUN_0044e470(char*);

extern void __fastcall FUN_00421880(void*);
extern void __fastcall FUN_004218c0(void*);
extern int __fastcall FUN_004218e0(void*);
extern void FUN_00412210(char*, char*, int, int);

/* Function start: 0x412000 */
SoundTracker::SoundTracker(int param) {
    field_0 = 0;
    DAT_00469128 = param;
    DAT_00469134 = new SoundPool(param);
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
    if (DAT_00469134 != 0) {
        p = DAT_00469134;
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
        DAT_00469134 = 0;
    }
}

/* Function start: 0x412400 */
void SoundTracker::ClearPool() {
    SoundPool* p;
    int* node;
    int* data;
    int i;
    int* next;

    p = DAT_00469134;
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
    DAT_00469130 = 0;
    DAT_0046912c = 0;
    DAT_00469138 = 0;
}

/* Function start: 0x412480 */
void SoundTracker::Init() {
    int timer[5];
    int maxTime;
    char lineBuf[256];
    long findData[3];
    int fileTime;
    char fileName[260];
    char sectionBuf[64];
    char filePath[64];
    int filePos[2];

    maxTime = 0;
    FUN_00421880(timer);
    DAT_00469140 = 0;
    ClearPool();
    FUN_004218c0(timer);

    FUN_00455130(DAT_00469310);

    if (FUN_00425fa0(DAT_00469310) != 0) {
        long handle = FUN_00454d20(DAT_00469304, findData);
        if (handle != -1) {
            do {
                fileTime = *(int*)((char*)findData + 0x14);
                if (maxTime <= fileTime) {
                    maxTime = fileTime;
                }
            } while (FUN_00454e70(handle, findData) == 0);
        }
        FUN_00454d20(DAT_00469310, findData);
        if (*(int*)((char*)findData + 0x14) < maxTime) {
            FUN_00455130(DAT_00469310);
        }
    }

    if (FUN_00425fa0(DAT_00469310) != 0) {
        void* fp = FUN_00455110(DAT_00469310, DAT_00469300);
        char* result = FUN_00426400(lineBuf, 0xff, fp);
        while (result != 0) {
            sscanf(lineBuf, DAT_004692f0, filePath, sectionBuf, &filePos[0], &filePos[1]);
            filePos[0] = filePos[0];
            filePos[1] = 0;
            FUN_00412210(filePath, sectionBuf, filePos[0], 0);
            result = FUN_00426400(lineBuf, 0xff, fp);
        }
        FUN_00455040(fp);
    } else {
        long handle = FUN_00454d20(DAT_00469304, findData);
        if (handle != -1) {
            do {
                char misPath[64];
                FUN_00454510(misPath, DAT_004692e8, (char*)findData + 0x1e);
                void* fp = FUN_00455110(misPath, DAT_00469300);
                char* result = FUN_00426400(lineBuf, 0xff, fp);
                while (result != 0) {
                    FUN_00454850(sectionBuf, DAT_00468448, lineBuf);
                    if (sectionBuf[0] == '[') {
                        FUN_004550b0(fp, &filePos[0]);
                        FUN_00412210(misPath, sectionBuf, filePos[0], filePos[1]);
                    }
                    DAT_00469140++;
                    result = FUN_00426400(lineBuf, 0xff, fp);
                }
                FUN_00455040(fp);
            } while (FUN_00454e70(handle, findData) == 0);
            Cleanup();
        }
    }

    DAT_0046913c = FUN_004218e0(timer);
}

/* Function start: 0x412730 */
void SoundTracker::Cleanup() {
    char lineBuf[256];
    int* node;
    int* data;

    void* fp = FUN_00455110(DAT_00469310, DAT_0046933c);
    node = DAT_00469134->head;
    while (node != 0) {
        int* next = (int*)node[0];
        data = node + 2;
        sprintf(lineBuf, DAT_00469324, data + 1, data + 9, data[0x12], data[0]);
        FUN_00426490(lineBuf, fp);
        node = next;
    }
    FUN_00455040(fp);
}
