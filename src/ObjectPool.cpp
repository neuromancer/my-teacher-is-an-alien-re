#include "ObjectPool.h"
#include "Memory.h"
#include <string.h>

/* Function start: 0x413d50 */
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
                int prev = (int)freeList;
                i--;
                *p = prev;
                freeList = p;
                p = p - 4;
            } while (i >= 0);
        }
    }

    int* p = (int*)freeList;
    freeList = (void*)*p;
    allocatedCount++;
    int n = 0;
    p[2] = n;
    while (n--)
        ;
    n = 0;
    p[3] = n;
    while (n--)
        ;
    return p;
}
