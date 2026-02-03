#ifndef OBJECTPOOL_H
#define OBJECTPOOL_H

#include <string.h>

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
    }

public:
    void* Allocate();
    void* Allocate_2();
    void MemoryPool_Allocate(unsigned int param_1, int param_2) {
        if (memory != 0) {
            delete memory;
            memory = 0;
        }
        if (param_2 != 0) {
            memory = new char[param_1 * 4];
            memset(memory, 0, param_1 * 4);
            size = param_1;
        }
        else {
            size = param_1;
        }
    }
};

#endif // OBJECTPOOL_H
