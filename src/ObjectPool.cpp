#include "ObjectPool.h"
#include "Memory.h"
#include <string.h>

/* Function start: 0x433F10 */
ObjectPool::~ObjectPool()
{
    unsigned int i;
    int* node;
    int* nextNode;

    if (memory != 0) {
        for (i = 0; i < (unsigned int)size; i++) {
            node = (int*)((int*)memory)[i];
            while (node != 0) {
                int countdown = 0;
                do {
                    int prev = countdown;
                    countdown--;
                    if (prev == 0) break;
                } while (1);

                int* dataPtr = (int*)(node + 2);
                int counter = 0;
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

/* Function start: 0x44C580 */
void ObjectPool::AllocateBuckets(unsigned int newSize, int flag) {
    if (memory != 0) {
        operator delete(memory);
        memory = 0;
    }
    if (flag != 0) {
        memory = operator new(newSize * 4);
        memset(memory, 0, newSize * 4);
    }
    size = newSize;
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
