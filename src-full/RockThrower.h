#ifndef ROCKTHROWER_H
#define ROCKTHROWER_H

#include "Parser.h"
#include <string.h>

class Projectile;

// Weapon - Base class for weapon displays
// Demo vtable: 0x4313A8, Full game vtable: 0x4614B0
// Size: Parser + 0x20 (8 dword fields)
// Demo: 0xA8, Full game: 0xB0
// vtable layout (6 entries):
//   [0] LBLParse  [1] OnProcessStart  [2] OnProcessEnd
//   [3] destructor  [4] OnHit  [5] DrawCrosshairs
class Weapon : public Parser {
public:
    int m_posX;         // demo 0x88, full 0x90 — X position (default 100)
    int m_posY;         // demo 0x8C, full 0x94 — Y position (default 220)
    int m_posZ;         // demo 0x90, full 0x98 — Z/right (default 199)
    int m_height;       // demo 0x94, full 0x9C
    int m_crosshairX;   // demo 0x98, full 0xA0
    int m_crosshairY;   // demo 0x9C, full 0xA4
    int m_clicked;      // demo 0xA0, full 0xA8 — clicked flag
    int m_sound;        // demo 0xA4, full 0xAC — Sample* sound

    Weapon() {
        m_crosshairX = 0;
        m_crosshairY = 0;
        memset(&m_posX, 0, 8 * 4);
        m_height = 0;
        m_sound = 0;
        m_posX = 0x64;
        m_clicked = 0;
        m_posY = 0xdc;
        m_posZ = 0xc7;
    }
    virtual ~Weapon();
    virtual void OnHit();           // vtable[4]
    virtual void DrawCrosshairs();  // vtable[5]
    void DrawExplosion();           // non-virtual
};

// RockThrower - Rock throwing weapon
// Demo vtable: 0x4314D0, Full game vtable: 0x461490
// Size: Weapon + 0x18 (full game) / 0x10 (demo)
// Demo: 0xB8, Full game: 0xC8
// vtable adds [6] UpdateProjectiles
class RockThrower : public Weapon {
public:
    int m_itemCount;     // demo 0xA8, full 0xB0 — number of projectiles (default 3)
    Projectile** m_items; // demo 0xAC, full 0xB4 — array of projectile objects
    int field_2;         // demo 0xB0, full 0xB8
    int field_3;         // demo 0xB4, full 0xBC
    int field_4;         // full 0xC0 (full game only)
    int field_5;         // full 0xC4 (full game only)

    RockThrower();
    RockThrower(Parser* parent);    // full game ctor: 0x4274C0
    virtual ~RockThrower();

    virtual void DrawCrosshairs();  // override
    virtual int LBLParse(char* line);
    virtual void UpdateProjectiles(); // vtable[6]
};

#endif // ROCKTHROWER_H
