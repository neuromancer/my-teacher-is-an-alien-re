#ifndef MCNAVNODE_H
#define MCNAVNODE_H

#include "Parser.h"
#include "ObjectPool.h"

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
    void* neighborNodes[6];         // 0x90 - neighbor node pointers indexed by bearing
    int nodeHandle;                 // 0xA8 - this node's handle/ID
    char nodeName[36];              // 0xAC - node name (up to 0xCF)
};

// mCNavNode_TypeA - Navigation node type A
// Vtable: 0x461B60
class mCNavNode_TypeA : public mCNavNode {
public:
    mCNavNode_TypeA(char* line) : mCNavNode(line) {}
    virtual int LBLParse(char* line);
    virtual int virtual4();
    virtual int Activate();
};

// mCNavNode_TypeB - Navigation node type B
// Vtable: 0x461B80
class mCNavNode_TypeB : public mCNavNode {
public:
    mCNavNode_TypeB(char* line) : mCNavNode(line) {}
    virtual int LBLParse(char* line);
    virtual int virtual4();
    virtual int Activate();
};

// mCNavNode_TypeC - Navigation node type C
// Vtable: 0x461BA0
class mCNavNode_TypeC : public mCNavNode {
public:
    mCNavNode_TypeC(char* line) : mCNavNode(line) {}
    virtual int virtual4();
    virtual int Activate();
};

// mCNavNode_TypeD - Navigation node type D
// Vtable: 0x461BC0
class mCNavNode_TypeD : public mCNavNode {
public:
    mCNavNode_TypeD(char* line) : mCNavNode(line) {}
    virtual int LBLParse(char* line);
    virtual int virtual4();
    virtual int Activate();
};

// mCNavNode_TypeE - Navigation node type E
// Vtable: 0x461BE0
class mCNavNode_TypeE : public mCNavNode {
public:
    mCNavNode_TypeE(char* line) : mCNavNode(line) {}
    virtual int LBLParse(char* line);
    virtual int virtual4();
    virtual int Activate();
};

#endif // MCNAVNODE_H
