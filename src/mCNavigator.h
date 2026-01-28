#ifndef MCNAVIGATOR_H
#define MCNAVIGATOR_H

#include "Sprite.h"
#include "Parser.h"
#include "ObjectPool.h"

struct NavNode {
    NavNode* next;
    int field_4;
    int field_8;
    void* field_C;
};

class mCNavigator : public Parser
{
public:
    int field_88; // 0x88
    Sprite* sprite; // 0x8c
    ObjectPool* navNodePool; // 0x90
    void* currentNode; // 0x94
    int field_98; // 0x98
    int startingNode; // 0x9c
    int field_A0; // 0xA0
    int field_A4; // 0xA4

public:
    mCNavigator();
    ~mCNavigator();

    virtual int LBLParse(char*);
    void SetBearing(char*);
    void SetCurrentNode();
    int Update();
};

#endif // MCNAVIGATOR_H
