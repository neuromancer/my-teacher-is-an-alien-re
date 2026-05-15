#include "GameLoopHelper.h"
#include <string.h>

extern "C" void WriteToLog(const char*, ...);
extern void FreeMemory(void*);

struct AnimData {
    char name[64];    // 0x00 - 0x3F
    int handle;       // 0x40
    AnimData(char* n, int h) {
        memset(this, 0, sizeof(AnimData));
        if (n != (char*)0) {
            strcpy(name, n);
        }
        handle = h;
    }
    ~AnimData();
};

/* Function start: 0x41A6D0 */
int __cdecl CompareNodePriority(void* a, void* b) {
    AnimData* pa = *(AnimData**)a;
    AnimData* pb = *(AnimData**)b;
    return (pb->handle - pa->handle) == 0;
}

/* Function start: 0x41A6F0 */
void __cdecl FreeNodeData(void* ptr, int count) {
    while (count-- != 0) {
        AnimData** data = (AnimData**)ptr;
        if (*data != 0) {
            FreeMemory(*data);
            *data = 0;
        }
        ptr = (char*)ptr + 4;
    }
}

/* Function start: 0x41A730 */
GameLoopHelper::~GameLoopHelper() {
    AnimPool* p;
    int* node;
    int* next;

    PostProcess();
    p = pool;
    if (p != 0) {
        node = p->head;
        while (node != 0) {
            FreeNodeData((void*)(node + 2), 1);
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
        pool = 0;
    }
}

/* Function start: 0x41A7A0 */
void GameLoopHelper::AddAnimation(char* name, int handle) {
    AnimData* data;
    AnimPool* p;
    int* node;
    int* tail;
    int i;

    data = new AnimData(name, handle);
    p = pool;
    tail = p->tail;
    if (p->freeList == 0) {
        int* newBlock = (int*)operator new(p->blockSize * 0xc + 4);
        *newBlock = (int)p->blockList;
        p->blockList = newBlock;
        i = p->blockSize;
        node = newBlock + i * 3 - 2;
        i = i - 1;
        if (i >= 0) {
            do {
                *node = (int)p->freeList;
                p->freeList = node;
                node = node - 3;
                i = i - 1;
            } while (i >= 0);
        }
    }
    node = p->freeList;
    p->freeList = (int*)node[0];
    node[1] = (int)tail;
    node[0] = 0;
    p->count = p->count + 1;
    node[2] = 0;
    i = 0;
    do {
        int tmp = i;
        i = i - 1;
        if (tmp == 0) break;
    } while (1);
    node[2] = (int)data;
    if (p->tail == 0) {
        p->head = node;
    } else {
        *(int*)(int)p->tail = (int)node;
    }
    p->tail = node;
}

/* Function start: 0x41A8D0 */
void GameLoopHelper::RemoveAnimation(int handle) {
    char searchKey[0x44];
    char* searchPtr;
    AnimPool* p;
    int* node;

    memset(searchKey, 0, 0x44);
    *(int*)(searchKey + 0x40) = handle;
    searchPtr = searchKey;

    p = pool;
    node = p->head;
    while (node != 0) {
        if (CompareNodePriority((void*)(node + 2), (void*)&searchPtr) != 0) {
            goto found;
        }
        node = (int*)node[0];
    }
    node = 0;

found:
    p = pool;
    if (p->head == node) {
        p->head = (int*)node[0];
    } else {
        *(int*)node[1] = node[0];
    }
    if (p->tail == node) {
        p->tail = (int*)node[1];
    } else {
        *(int*)(node[0] + 4) = node[1];
    }
    FreeNodeData((void*)(node + 2), 1);
    node[0] = (int)p->freeList;
    p->freeList = node;
    p->count = p->count - 1;
}

/* Function start: 0x41A960 */
void GameLoopHelper::PostProcess() {
    int idx;
    char* data;

    WriteToLog("REPORT of in use animations");
    idx = 1;
    int* volatile node = pool->head;
    if (node != 0) {
        do {
            data = (char*)node[2];
            node = (int*)node[0];
            WriteToLog("  %2.2d. Name=%-32s. smk addr=%lu", idx, data, *(unsigned int*)(data + 0x40));
            idx++;
        } while (node != 0);
    }
    WriteToLog("******* END REPORT ********");
}
