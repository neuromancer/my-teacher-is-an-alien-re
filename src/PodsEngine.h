#ifndef PODSENGINE_H
#define PODSENGINE_H

#include "SC_CombatBase.h"
#include "SpriteAction.h"

class Sprite;
class HashTable;

// Counter/limit pair at 0xF4 — dtor-less (the original never unwinds it; only the
// SlimeDim range at 0xFC has a destructor). Zeroing ctor matches the x=0;y=0 stores.
struct PodsCounter {
    int x;
    int y;
    PodsCounter() { memset(this, 0, 8); }
};

// PodsEngine - Combat engine for SC_Pods (pod race mini-game)
// Constructor: 0x440860, Destructor: 0x440950
// Vtable: 0x4618C0
// Size: 0x118 (SC_CombatBase 0xF0 + 10 ints)
// Created by SC_Pods::LBLParse via FUN_00440860
class PodsEngine : public SC_CombatBase {
public:
    PodsEngine();
    ~PodsEngine();

    virtual int LBLParse(char* line);     // [0] 0x441140
    virtual void OnProcessEnd();          // [2] 0x441000
    virtual int UpdateSprites();          // [9] 0x440920
    virtual void ProcessFrame();          // [10] 0x440A50
    virtual int HandleAction(int* param); // [13] 0x440FC0

    // Fields from 0xF0 to 0x117 (10 ints)
    Sprite* podsBgSprite; // 0xF0 — background sprite (deleted in dtor)
    PodsCounter counter;  // 0xF4-0xFB — progress counter (x) / limit (y=3), dtor-less
    SlimeDim podRange;    // 0xFC-0x103 — range pair (has dtor; unwound at state 1)
    int reserved_0x104;      // 0x104
    HashTable* hashTable; // 0x108 — HashTable* (deleted in dtor)
    int field_0x10C;      // 0x10C
    int field_0x110;      // 0x110
    int reserved_0x114;      // 0x114
};

#endif // PODSENGINE_H
