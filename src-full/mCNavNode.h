#ifndef MCNAVNODE_H
#define MCNAVNODE_H

#include "Parser.h"
#include "ObjectPool.h"

// mCNavNode - Navigation node for mCNavigator
// Full game: Size 0xD0, vtable 0x461B50 (parsing) → 0x461B30 (runtime, 8 entries)
// Constructor: 0x44AE10 (full game), 0x412D80 (demo)
//
// Full game vtable layout (0x461B30, set after parsing):
//   +0x00: LBLParse        (0x44A9C0 / 0x44AF40)
//   +0x04: OnProcessStart  (inherited)
//   +0x08: OnProcessEnd    (inherited)
//   +0x0C: virtual3        (0x44A980)
//   +0x10: virtual4        (0x44AB40)
//   +0x14: virtual5        (0x44A9A0) — called by FUN_0044b830 (Activate)
//   +0x18: virtual6        (0x44ADA0) — called by FUN_0044b880 (GetNextNode)
//   +0x1C: virtual7        (0x449CE0)
//
// Full game field layout (Parser base = 0x90):
//   0x90-0xA7: neighborNodes[6] (indexed by bearing direction)
//   0xA8: nodeHandle
//   0xAC: nodeName[36]
class mCNavNode : public Parser {
public:
    // Full game constructor
    mCNavNode(char* line);          // 0x44AE10

    // Demo constructor
    mCNavNode();                    // 0x412D80

    // Virtual methods (full game runtime vtable has 8 entries)
    // Parser virtuals: LBLParse(+0), OnProcessStart(+4), OnProcessEnd(+8)
    // mCNavNode adds 5 more:
    virtual void virtual3();        // +0x0C
    virtual int virtual4();         // +0x10
    virtual int Activate();         // +0x14 — called via FUN_0044b830
    virtual int GetNextNode();      // +0x18 — called via FUN_0044b880
    virtual void virtual7();        // +0x1C

    virtual int LBLParse(char*);

    // Non-virtual wrappers: look up neighbor by bearing and dispatch
    void CallActivateNeighbor();    // 0x44B7E0 — calls neighbor->virtual7()
    int CallActivate();             // 0x44B830 — calls neighbor->Activate()
    int CallGetNextNode();          // 0x44B880 — calls neighbor->GetNextNode()

    // Full game fields (after Parser 0x90 base)
    void* neighborNodes[6];         // 0x90 - neighbor node pointers indexed by bearing
    int nodeHandle;                 // 0xA8 - this node's handle/ID
    char nodeName[36];              // 0xAC - node name (up to 0xCF)

    // Demo-only methods
    void SetNavLink(char* direction, int nodeId);
    void AddSpriteList(char* name, int id);
};

#endif // MCNAVNODE_H
