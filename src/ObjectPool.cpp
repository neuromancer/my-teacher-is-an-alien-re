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
        p = p + i * 4 - 3;
        for (i--; i >= 0; i--) {
            *p = (int)this->freeList;
            this->freeList = p;
            p = p - 4;
        }
    }

    int* p = (int*)this->freeList;
    this->freeList = (void*)*p;
    this->allocatedCount++;
    p[2] = 0;
    p[3] = 0;
    return p;
}
