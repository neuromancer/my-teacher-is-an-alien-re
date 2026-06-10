#ifndef OBJECTPOOL_H
#define OBJECTPOOL_H

#include <string.h>
#include "Memory.h"

class ObjectPool
{
public:
    void* memory;       // 0x00 - hash table memory
    int size;           // 0x04 - hash table size
    int allocatedCount; // 0x08
    void* freeList;     // 0x0C
    void* memoryBlock;  // 0x10
    int objectSize;     // 0x14

    ObjectPool(int sz, int objSz) {
        memory = 0;
        size = sz;
        allocatedCount = 0;
        freeList = 0;
        memoryBlock = 0;
        objectSize = objSz;
        if (objSz != 10) {
            if (memory != 0) {
                FreeMemory(memory);
                memory = 0;
            }
            int numBuckets = objSz + (int)((double)objSz * 0.3);
            int* buckets = new int[numBuckets];
            memset(buckets, 0, numBuckets * 4);
            memory = buckets;
            size = numBuckets;
        }
    }

    ~ObjectPool();

public:
    void* Allocate();
    void* Allocate_2();
    void AllocateBuckets(unsigned int size, int flag);
};

#endif // OBJECTPOOL_H
