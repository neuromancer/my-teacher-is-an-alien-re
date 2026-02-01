#ifndef MCNAVNODE_H
#define MCNAVNODE_H

#include "Parser.h"
#include "ObjectPool.h"

// mCNavNode class
// Size: 0x100 bytes
// Used by mCNavigator for navigation nodes
class mCNavNode : public Parser {
public:
    mCNavNode(); // 0x412d80
    int active;             // 0x88 - activation state (0=inactive, 1=active)
    int flags;              // 0x8c - flags (bit 0: animation, bit 1: sound, bit 2: counter, bit 4: ?, bit 8: random, bit 0x10: terminate)
    int nextNodeId;         // 0x90 - result node ID to return
    int counterLimit;       // 0x94 - max counter value
    int counter;            // 0x98 - current counter
    int animationState;     // 0x9c - animation state for Sprite::SetState2
    char soundName[32];     // 0xa0 - sound name buffer (32 bytes)
    ObjectPool* randomPool; // 0xc0 - pool for random sound selection
    int neighborNodes[6];   // 0xc4 - array of 6 neighbor node IDs (24 bytes, ends at 0xdc)
    int nodeHandle;         // 0xdc - this node's handle/ID
    char nodeName[32];      // 0xe0 - node name (32 bytes, ends at 0x100)

    int Activate();
    int Update();
    int GetNextNode();
    virtual int LBLParse(char*);
    void AddSpriteList(char* name, int id);
};

#endif // MCNAVNODE_H
