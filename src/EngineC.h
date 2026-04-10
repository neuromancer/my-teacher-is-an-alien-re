#ifndef ENGINEC_H
#define ENGINEC_H

#include "SC_CombatBase.h"

class Sprite;

// EngineC - Third combat engine type (Detention mini-game engine)
// Constructor: 0x40BBF0, Destructor: 0x40BCC0
// Vtable: 0x4611B0 (18 entries, inherits from SC_CombatBase)
// Size: 0x120
// Created by SC_Detention's LBLParse (0x40BAD0)
class EngineC : public SC_CombatBase {
public:
    EngineC();
    ~EngineC();

    virtual void method10();    // override at 0x40C0D0

    // Fields from 0xF0 to 0x11F (12 ints)
    int mouseReleased;    // 0xF0 — mouse button release tracker
    int field_0xF4;       // 0xF4
    int field_0xF8;       // 0xF8
    int field_0xFC;       // 0xFC
    int field_0x100;      // 0x100
    Sprite* sprite104;    // 0x104 — Sprite* (deleted in dtor)
    int field_0x108;      // 0x108
    int field_0x10C;      // 0x10C
    Sprite* sprite110;    // 0x110 — Sprite* (deleted in dtor)
    Sprite* sprite114;    // 0x114 — Sprite* (deleted in dtor)
    Sprite* sprite118;    // 0x118 — Sprite* (deleted in dtor)
    int field_0x11C;      // 0x11C
};

#endif // ENGINEC_H
