#ifndef ROCKTHROWER_H
#define ROCKTHROWER_H

#include "Parser.h"

// Weapon - Base class for weapons (vtable 0x4313a8)
// Intermediate class between Parser and RockThrower
// Size: 0xa8 (168 bytes)
class Weapon : public Parser {
public:
    // Fields from 0x88 to 0xa8 (32 bytes, 8 dwords)
    int m_posX;         // 0x88 - X position
    int m_posY;         // 0x8c - Y position
    int m_posZ;         // 0x90 - Z position or height
    int field_0x94;     // 0x94
    int field_0x98;     // 0x98
    int field_0x9c;     // 0x9c
    int field_0xa0;     // 0xa0
    int field_0xa4;     // 0xa4

    void OnHit();           // 0x415E00
    void DrawCrosshairs();  // 0x415E20
    void DrawExplosion();   // 0x415F10
};

// RockThrower - Rock throwing weapon (vtable 0x4314d0)
// Used in combat engine for throwing rocks
// Size: 0xb8 (184 bytes)
class RockThrower : public Weapon {
public:
    // Fields from 0xa8 to 0xb8 (16 bytes, 4 dwords)
    int m_itemCount;    // 0xa8 - number of items/projectiles
    void* m_items;      // 0xac - pointer to array of projectile objects
    int field_0xb0;     // 0xb0
    int field_0xb4;     // 0xb4

    RockThrower();
    ~RockThrower();

    virtual int LBLParse(char* line);
};

#endif // ROCKTHROWER_H
