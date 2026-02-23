#include "ObjectPool.h"
#include "Memory.h"
#include <string.h>

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
