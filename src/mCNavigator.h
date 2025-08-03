#ifndef MCNAVIGATOR_H
#define MCNAVIGATOR_H

#include "Sprite.h"
#include "Parser.h"

class ObjectPool
{
public:
    void* memory;
    int size;
    int allocatedCount;
    void* freeList;
    void* memoryBlock;
    int objectSize;

public:
    void* Allocate_2();
};

struct NavNode {
    NavNode* next;
    int field_4;
    int field_8;
    void* field_C;
};

class mCNavigator : public Parser
{
public:
    // Other members...
    Sprite* sprite; // 0x8c
    ObjectPool* navNodePool; // 0x90
    void* currentNode; // 0x94
    int field_98; // 0x98
    int startingNode; // 0x9c
    int field_A0; // 0xA0

public:
    mCNavigator();
    ~mCNavigator();

    int LBLParse(char*);
    void SetBearing(char*);
    void SetCurrentNode();
    int Update();
    void Allocate();
    void MemoryPool_Allocate(unsigned int size, int param_2);
};

#endif // MCNAVIGATOR_H
