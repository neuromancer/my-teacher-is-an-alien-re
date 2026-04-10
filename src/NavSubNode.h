#ifndef NAVSUBNODE_H
#define NAVSUBNODE_H

#include "Parser.h"
#include "ObjectPool.h"

class Sprite;

// Stick direction chars global (5 directions)
extern char* g_StickDirChars;

// g_NavSprite_004360a4 — declared in globals.h

// Free functions
int FindStickDirIndex(char ch);

// NavSubNode -- base class for navigation sub-nodes stored in mCNavNode::neighborNodes[]
// vtable: 0x461AC8
// Size: 0xA0 (Parser 0x90 + 0x10)
// Constructor: 0x449BD0 (also used as intermediate for derived classes via 0x44A010)
// Destructor: 0x449C90
class NavSubNode : public Parser {
public:
    NavSubNode() : Parser() {
        int* p = &parentNode;
        p[0] = 0;
        p[1] = 0;
        p[2] = 0;
        p[3] = 0;
        bearing = -1;
    }
    virtual ~NavSubNode();              // 0x449C90, sdtor at 0x449CF0

    // Virtual methods (extending Parser's 3: LBLParse, OnProcessStart, OnProcessEnd)
    // +0x0C: virtual destructor (sdtor) -- automatic
    virtual int virtual4();             // +0x10
    virtual int Activate();             // +0x14
    virtual int GetNextNode();          // +0x18
    virtual void virtual7();            // +0x1C

    // Fields (after Parser base at 0x90)
    int parentNode;     // 0x90 -- pointer to parent mCNavNode (cast to int)
    int field_94;       // 0x94
    int nodeHandle;     // 0x98 -- parsed with N%d
    int bearing;        // 0x9C -- direction index, default -1
};

// OnDir_SubNode -- nav sub-node with stick direction mappings
// vtable: 0x461AE8
// Size: 0xD0 (NavSubNode 0xA0 + 0x30)
// Constructor: 0x449BD0
// Destructor: 0x449D80
class OnDir_SubNode : public NavSubNode {
public:
    OnDir_SubNode();                    // 0x449BD0
    virtual ~OnDir_SubNode();           // 0x449D80, sdtor at 0x449D60

    virtual int LBLParse(char*);        // 0x449E70 (parses THEN_GOTO lines)

    // Fields (after NavSubNode at 0xA0)
    int field_A0;           // 0xA0
    int destNode[5];        // 0xA4 -- destination node IDs indexed by stick direction
    int bearingForDir[5];   // 0xB8 -- bearing indices indexed by stick direction
    int field_CC;           // 0xCC
};

// BG_SubNode -- background animation sub-node
// vtable: 0x461B08
// Size: 0xB8 (NavSubNode 0xA0 + 0x18)
// Constructor: 0x44A010
// Destructor: 0x44A0E0
class BG_SubNode : public NavSubNode {
public:
    BG_SubNode();                       // 0x44A010
    virtual ~BG_SubNode();              // 0x44A0E0, sdtor at 0x44A0C0

    virtual int LBLParse(char*);        // 0x44A4D0 (not in our address range)
    virtual int Activate();             // +0x14 = 0x44A1F0 (DoAction)
    virtual void virtual7();            // +0x1C = 0x44A1D0 (Init)

    void AddSpriteList(unsigned int spriteId); // 0x44A380

    // Fields (after NavSubNode at 0xA0)
    int state;              // 0xA0 -- 0=init, 1=playing, 2=done
    int spriteHandle;       // 0xA4 -- animation sprite handle
    int frameCounter;       // 0xA8 -- current frame count
    int maxFrames;          // 0xAC -- max frames (0 = unlimited)
    ObjectPool* spritePool; // 0xB0 -- hash pool of sprite IDs
    int field_B4;           // 0xB4
};

#endif // NAVSUBNODE_H
