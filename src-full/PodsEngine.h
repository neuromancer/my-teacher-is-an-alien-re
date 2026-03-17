#ifndef PODSENGINE_H
#define PODSENGINE_H

#include "SC_CombatBase.h"

class Sprite;

// PodsEngine - Combat engine for SC_Pods (pod race mini-game)
// Constructor: 0x440860, Destructor: 0x440950
// Vtable: 0x4618C0
// Size: 0x118 (SC_CombatBase 0xF0 + 10 ints)
// Created by SC_Pods::LBLParse via FUN_00440860
class PodsEngine : public SC_CombatBase {
public:
    PodsEngine();
    ~PodsEngine();

    // Fields from 0xF0 to 0x117 (10 ints)
    Sprite* bgSprite;     // 0xF0 — background sprite (deleted in dtor)
    int field_0xF4;       // 0xF4 — zeroed, then set to 0
    int field_0xF8;       // 0xF8 — set to 3
    int field_0xFC;       // 0xFC
    int field_0x100;      // 0x100
    int field_0x104;      // 0x104
    int field_0x108;      // 0x108 — object pointer (deleted in dtor with funclet)
    int field_0x10C;      // 0x10C
    int field_0x110;      // 0x110
    int field_0x114;      // 0x114
};

#endif // PODSENGINE_H
