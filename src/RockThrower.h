#ifndef ROCKTHROWER_H
#define ROCKTHROWER_H

#include "Parser.h"
#include <string.h>

class Projectile;

// Weapon - Base class for weapons (vtable 0x4313a8)
// Intermediate class between Parser and RockThrower
// Size: 0xa8 (168 bytes)
// vtable layout:
//   [0] LBLParse  [1] OnProcessStart  [2] OnProcessEnd
//   [3] destructor  [4] OnHit  [5] DrawCrosshairs
class Weapon : public Parser {
public:
    // Fields from 0x88 to 0xa8 (32 bytes, 8 dwords)
    int m_posX;         // 0x88 - X position
    int m_posY;         // 0x8c - Y position
    int m_posZ;         // 0x90 - Z position or height
    int field_0x94;     // 0x94
    int m_crosshairX;   // 0x98 - crosshair X position
    int m_crosshairY;   // 0x9c - crosshair Y position
    int field_0xa0;     // 0xa0
    int field_0xa4;     // 0xa4

    Weapon() {
        m_crosshairX = 0;
        m_crosshairY = 0;
        memset(&m_posX, 0, 8 * 4);
        field_0x94 = 0;
        field_0xa4 = 0;
        m_posX = 0x64;
        field_0xa0 = 0;
        m_posY = 0xdc;
        m_posZ = 0xc7;
    }
    virtual ~Weapon();
    virtual void OnHit();           // 0x415E00 - vtable[4]
    virtual void DrawCrosshairs();  // vtable[5] - base: 0x411930
    void DrawExplosion();           // 0x415F10 - non-virtual
};

// RockThrower - Rock throwing weapon (vtable 0x4314d0)
// Used in combat engine for throwing rocks
// Size: 0xb8 (184 bytes)
class RockThrower : public Weapon {
public:
    // Fields from 0xa8 to 0xb8 (16 bytes, 4 dwords)
    int m_itemCount;    // 0xa8 - number of items/projectiles
    Projectile** m_items; // 0xac - pointer to array of projectile objects
    int field_0xb0;     // 0xb0
    int field_0xb4;     // 0xb4

    RockThrower();
    virtual ~RockThrower();

    void UpdateProjectiles(); // 0x416880
    virtual void DrawCrosshairs();  // 0x416960 - override

    virtual int LBLParse(char* line);
};

#endif // ROCKTHROWER_H
