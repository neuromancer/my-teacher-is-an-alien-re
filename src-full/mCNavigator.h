#ifndef MCNAVIGATOR_H
#define MCNAVIGATOR_H

#include "Sprite.h"
#include "Parser.h"
#include "ObjectPool.h"

struct NavNode {
    NavNode* next;
    int bucketIndex;
    int key;
    void* value;
};

class mCNavigator : public Parser
{
public:
    int field_88; // 0x88
    Sprite* sprite; // 0x8c
    ObjectPool* navNodePool; // 0x90
    void* currentNode; // 0x94
    int bearing; // 0x98
    int startingNode; // 0x9c
    int previousNodeId; // 0xA0
    int field_A4; // 0xA4

public:
    mCNavigator();
    ~mCNavigator();

    virtual int LBLParse(char*);
    virtual void OnProcessEnd();
    void SetBearing(char*);
    void SetField(unsigned int);    // 0x44BA80
    void SetMode(int);              // 0x44BA70
    void SetNavParams(unsigned int param1, int param2);
    int Update();
};

#endif // MCNAVIGATOR_H
