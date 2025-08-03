#ifndef OBJECTPOOL_H
#define OBJECTPOOL_H

class ObjectPool
{
public:
    void* _field_0x0;
    int _field_0x4;
    int allocatedCount;
    void* freeList;
    void* memoryBlock;
    int blockSize;

public:
    void* Allocate();
};

#endif // OBJECTPOOL_H
