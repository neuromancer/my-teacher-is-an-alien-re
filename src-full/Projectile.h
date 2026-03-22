#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "Sprite.h"

// Projectile - Combat projectile (vtable 0x4314b8)
// Extends Sprite with position tracking and velocity
// Size: 0x108 bytes (Sprite 0xD8 + 0x30)
class Projectile : public Sprite {
public:
    int field_0xF8;    // 0xF8
    int startX;        // 0xFC
    int startY;        // 0x100
    int currentX;      // 0x104
    int currentY;      // 0x108
    int halfWidth;     // 0x10C
    int halfHeight;    // 0x110
    float velocityX;   // 0x114
    float velocityY;   // 0x118
    int active;        // 0x11C
    int nextX;         // 0x120
    int nextY;         // 0x124

    Projectile();          // 0x4161B0
    void Launch();         // 0x4162C0
    void Update();         // 0x4163E0 (demo)
    void UpdateFull();     // 0x427390 (full game)
    int CheckCollision();  // 0x416500
};

#endif // PROJECTILE_H
