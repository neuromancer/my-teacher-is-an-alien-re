#ifndef MCNAVNODE_H
#define MCNAVNODE_H

#include "Parser.h"
#include "ObjectPool.h"
#include "NavSubNode.h"

class SpriteAction;

// mCNavNode - Navigation node for mCNavigator
// Full game: Size 0xD0, vtable 0x461B50 (parsing) → 0x461B30 (runtime, 8 entries)
// Constructor: 0x44AE10
//
// Full game vtable layout (0x461B30, set after parsing):
//   +0x00: LBLParse        (0x44A9C0 / 0x44AF40)
//   +0x04: OnProcessStart  (inherited)
//   +0x08: OnProcessEnd    (inherited)
//   +0x0C: virtual3        (0x44A980)
//   +0x10: virtual4        (0x44AB40)
//   +0x14: Activate        (0x44A9A0)
//   +0x18: GetNextNode     (0x44ADA0)
//   +0x1C: virtual7        (0x449CE0)
//
// Full game field layout (Parser base = 0x90):
//   0x90-0xA7: neighborNodes[6] (indexed by bearing direction)
//   0xA8: nodeHandle
//   0xAC: nodeName[36]
class mCNavNode : public Parser {
public:
    mCNavNode(char* line);          // 0x44AE10
    ~mCNavNode();                   // 0x44AEC0

    // Virtual methods (8 entries total)
    virtual void virtual3();        // +0x0C
    virtual int virtual4();         // +0x10
    virtual int Activate();         // +0x14
    virtual int GetNextNode();      // +0x18
    virtual void virtual7();        // +0x1C
    virtual int LBLParse(char*);    // +0x00 (override)

    // Non-virtual wrappers: look up neighbor by bearing and dispatch
    void CallActivateNeighbor();    // 0x44B7E0
    int CallActivate();             // 0x44B830
    int CallGetNextNode();          // 0x44B880

    // Fields (after Parser 0x90 base)
    NavSubNode* neighborNodes[6];   // 0x90 - neighbor sub-nodes indexed by bearing
    int nodeHandle;                 // 0xA8 - this node's handle/ID
    char nodeName[36];              // 0xAC - node name (up to 0xCF)
};

// mCNavNode_TypeA (DO_ANIM) - plays an animation on activation
// Vtable: 0x461B60, size 0xA8 = NavSubNode(0xA0) + 2 ints
class mCNavNode_TypeA : public NavSubNode {
public:
    int animHandle;     // 0xA0 - animation resource handle
    int reservedA4;     // 0xA4

    mCNavNode_TypeA(char* line) { animHandle = 0; reservedA4 = 0; }
    virtual int LBLParse(char* line);
    virtual void virtual4();
    virtual int Activate();
};

// mCNavNode_TypeB (DO_CHECKMESSAGE) - 2-way branch based on game state condition
// Vtable: 0x461B80, size 0xB8 = NavSubNode(0xA0) + 6 ints
class mCNavNode_TypeB : public NavSubNode {
public:
    SpriteAction* conditionPtr; // 0xA0 - condition checked by GameState::CheckCondition
    int destNode[2];     // 0xA4 - destination node IDs [false, true]
    int destBearing[2];  // 0xAC - destination bearings  [false, true]
    int reservedB4;      // 0xB4

    mCNavNode_TypeB(char* line) {
        conditionPtr = 0;
        destNode[0] = 0;
        destNode[1] = 0;
        destBearing[0] = 0;
        destBearing[1] = 0;
        reservedB4 = 0;
    }
    virtual int LBLParse(char* line);
    virtual void virtual4();
    virtual int Activate();
};

// mCNavNode_TypeC (DO_EXIT) - exit navigation node, empty stubs
// Vtable: 0x461BA0, size 0xA0 (no extra fields used)
class mCNavNode_TypeC : public NavSubNode {
public:
    mCNavNode_TypeC(char* line) {}
    virtual void virtual4();
    virtual int Activate();
};

// mCNavNode_TypeD (DO_POSTMESSAGE) - enqueues a SpriteAction on activation
// Vtable: 0x461BC0, size 0xA8 = NavSubNode(0xA0) + 2 ints
class mCNavNode_TypeD : public NavSubNode {
public:
    int* action;         // 0xA0 - SpriteAction* to enqueue
    int reservedA4;      // 0xA4

    mCNavNode_TypeD(char* line) { action = 0; reservedA4 = 0; }
    virtual int LBLParse(char* line);
    virtual void virtual4();
    virtual int Activate();
};

// mCNavNode_TypeLogic (DO_LOGICFNC) - parses and dispatches a logic action
// Vtable: 0x461B30, size 0xA8 = NavSubNode(0xA0) + 2 ints
class mCNavNode_TypeLogic : public NavSubNode {
public:
    SpriteAction* action;  // 0xA0
    int reservedA4;        // 0xA4

    mCNavNode_TypeLogic(char* line) { action = 0; reservedA4 = 0; }
    virtual ~mCNavNode_TypeLogic();
    virtual int LBLParse(char* line);
    virtual void virtual4();
    virtual int Activate();
};

// mCNavNode_TypeE (DO_MOUSE) - sets cursor mode from NO_MOUSE/AUTO_MOUSE/state name
// Vtable: 0x461BE0, size 0xA8 = NavSubNode(0xA0) + 2 ints
class mCNavNode_TypeE : public NavSubNode {
public:
    int stateValue;      // 0xA0 - -1=NO_MOUSE, 0=AUTO_MOUSE, else GameState index
    int reservedA4;      // 0xA4

    mCNavNode_TypeE(char* line) { stateValue = 0; reservedA4 = 0; }
    virtual int LBLParse(char* line);
    virtual void virtual4();
    virtual int Activate();
};

#endif // MCNAVNODE_H
