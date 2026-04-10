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

// mCNavigator - Navigation graph parser
// Full game vtable: 0x461C00
// Constructor: 0x44B8D0, Destructor: 0x44B950
// Size: Parser(0x90) + 0x18 = 0xA8
//
// Field layout confirmed by constructor (zeros 6 dwords at +0x90)
// and LBLParse (0x44BB60) field accesses.
class mCNavigator : public Parser
{
public:
    int bearing;           // 0x90 - current direction index (0-5: N,E,S,W,U,D)
    int startingNode;      // 0x94 - initial node ID (default 1)
    void* currentNode;     // 0x98 - active mCNavNode*
    ObjectPool* navNodePool; // 0x9C - hash table of NavNodes
    Sprite* sprite;        // 0xA0 - navigation sprite
    int previousNodeId;    // 0xA4 - last visited node ID

public:
    mCNavigator();                                   // 0x44B8D0
    ~mCNavigator();                                  // 0x44B950

    virtual int LBLParse(char*);                     // 0x44BB60
    virtual void OnProcessEnd();                     // 0x44BB50
    void SetField(unsigned int);                     // 0x44BA80
    void SetMode(int);                               // 0x44BA70
    void SetNavParams(unsigned int param1, int param2); // 0x44BAC0
    void* FindNodeInPool(unsigned int nodeId);       // 0x44BAF0
    int Update();                                    // 0x44BE70
};

#endif // MCNAVIGATOR_H
