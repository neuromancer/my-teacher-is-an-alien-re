#ifndef ROCKTHROWER_H
#define ROCKTHROWER_H

#include "Parser.h"
#include "SpriteAction.h"
#include <string.h>

class Projectile;
class Sample;

// Weapon - Base class for weapon displays
// Full game vtable: 0x4614B0 (6 entries)
// Size: Parser(0x90) + 0x20 = 0xB0
// vtable layout:
//   [0] LBLParse  [1] OnProcessStart  [2] OnProcessEnd
//   [3] destructor  [4] OnHit  [5] DrawCrosshairs
class Weapon : public Parser {
public:
    int m_posX;         // 0x90 — X position (default 100)
    int m_posY;         // 0x94 — Y position (default 220)
    int m_posZ;         // 0x98 — Z/right (default 199)
    int m_height;       // 0x9C
    SlimeDim m_crosshair; // 0xA0-0xA7 (x=crosshairX, y=crosshairY)
    int m_clicked;      // 0xA8 — clicked/fire flag
    Sample* m_sound;    // 0xAC — Sample* sound

    Weapon() {
        memset(&m_posX, 0, 8 * 4);
        m_posX = 0x64;
        m_posY = 0xdc;
        m_posZ = 0xc7;
        m_height = 0;
        m_sound = 0;
        m_clicked = 0;
    }
    virtual ~Weapon();
    virtual void OnHit();              // vtable[4]
    virtual void DrawCrosshairs();     // vtable[5]
    void UpdateProjectiles();          // 0x427880
};

// RockThrower - Rock throwing weapon
// Full game vtable: 0x461490
// Size: Weapon(0xB0) + 0x18 = 0xC8
// vtable adds [6] CheckTargetHit
class RockThrower : public Weapon {
public:
    int m_itemCount;     // 0xB0 — number of projectiles (default 3)
    Projectile** m_items; // 0xB4 — array of projectile objects
    int m_hitCount;      // 0xB8 — projectile hit count / weapon flags
    int m_holdState;     // 0xBC — mouse hold state
    int m_hitCountFull;  // 0xC0 — hit count (full game)
    int field_5;         // 0xC4

    RockThrower(Parser* parent);    // 0x4274C0
    virtual ~RockThrower();         // 0x427710

    virtual void DrawCrosshairs();         // 0x4279D0 (override)
    virtual int LBLParse(char* line);      // 0x427B20 (override)
    virtual int CheckTargetHit(int);       // 0x427A30 (override)
    void ResetProjectiles();               // 0x4279A0
};

#endif // ROCKTHROWER_H
