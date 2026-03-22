#include "ObjectPool.h"
#include "Memory.h"
#include <string.h>

extern void FUN_00433f10();

/* Function start: 0x433F10 */
ObjectPool::~ObjectPool()
{
    unsigned int i;
    int* node;
    int* nextNode;

    if (memory != 0 && size > 0) {
        for (i = 0; i < size; i++) {
            node = (int*)((int*)memory)[i];
            while (node != 0) {
                volatile int countdown = 0;
                do {
                    int prev = countdown;
                    countdown--;
                    if (prev == 0) break;
                } while (1);

                int* dataPtr = (int*)(node + 2);
                volatile int counter = 0;
                do {
                    if (*dataPtr != 0) {
                        FreeMemory((void*)*dataPtr);
                        *dataPtr = 0;
                    }
                    dataPtr++;
                    int prev = counter;
                    counter--;
                    if (prev == 0) break;
                } while (1);

                node = (int*)*node;
            }
        }
    }

    FreeMemory(memory);
    memory = 0;
    allocatedCount = 0;
    freeList = 0;

    int* block = (int*)memoryBlock;
    while (block != 0) {
        nextNode = (int*)*block;
        FreeMemory(block);
        block = nextNode;
    }
    memoryBlock = 0;
}

/* Function start: 0x413cf0 */ /* DEMO ONLY - no full game match */
void ObjectPool::MemoryPool_Allocate(unsigned int param_1, int param_2)
{
    if (memory != 0) {
        delete memory;
        memory = 0;
    }
    if (param_2 != 0) {
        memory = new int[param_1];
        memset(memory, 0, param_1 * 4);
        size = param_1;
    }
    else {
        size = param_1;
    }
}

/* Function start: 0x409850 */
void* ObjectPool::Allocate()
{
    if (freeList == 0) {
        int* p = (int*)new char[objectSize * 0x10 + 4];
        *p = (int)memoryBlock;
        memoryBlock = p;

        int i = objectSize;
        int offset = i * 0x10;
        i--;
        p = (int*)((char*)p + offset - 0xc);
        if (i >= 0) {
            do {
                i--;
                *p = (int)freeList;
                freeList = p;
                p = p - 4;
            } while (i >= 0);
        }
    }

    int* p = (int*)freeList;
    int n = 0;
    freeList = (void*)*p;
    allocatedCount++;
    p[2] = n;
    do {
        int prev = n;
        n--;
        if (prev == 0) break;
    } while (1);
    n = 0;
    p[3] = n;
    do {
        int prev = n;
        n--;
        if (prev == 0) break;
    } while (1);
    return p;
}
