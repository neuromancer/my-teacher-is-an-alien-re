#include "ObjectPool.h"
#include "Memory.h"

/* Function start: 0x413d50 */
void* ObjectPool::Allocate()
{
    if (this->freeList == 0) {
        int* p = (int*)AllocateMemory(this->blockSize * 0x10 + 4);
        *p = (int)this->memoryBlock;
        this->memoryBlock = p;

        int i = this->blockSize;
        int offset = i * 0x10;
        i--;
        p = (int*)((char*)p + offset - 0xc);
        if (i >= 0) {
            do {
                int prev = (int)this->freeList;
                i--;
                *p = prev;
                this->freeList = p;
                p = p - 4;
            } while (i >= 0);
        }
    }

    int* p = (int*)this->freeList;
    this->freeList = (void*)*p;
    this->allocatedCount++;
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
