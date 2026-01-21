#ifndef OBJECTPOOL_H
#define OBJECTPOOL_H

class ObjectPool
{
public:
    void* memory;       // 0x00 - hash table memory
    int size;           // 0x04 - hash table size
    int allocatedCount; // 0x08
    void* freeList;     // 0x0C
    void* memoryBlock;  // 0x10
    int objectSize;     // 0x14

public:
    void* Allocate();
    void* Allocate_2();
    void MemoryPool_Allocate(unsigned int param_1, int param_2);
};

#endif // OBJECTPOOL_H
