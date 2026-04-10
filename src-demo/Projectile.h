#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "Sprite.h"

// Projectile - Combat projectile (vtable 0x4314b8)
// Extends Sprite with position tracking and velocity
// Size: 0x108 bytes (Sprite 0xD8 + 0x30)
class Projectile : public Sprite {
public:
    int startX;        // 0xD8
    int startY;        // 0xDC
    int currentX;      // 0xE0
    int currentY;      // 0xE4
    int nextX;         // 0xE8
    int nextY;         // 0xEC
    int halfWidth;     // 0xF0
    int halfHeight;    // 0xF4
    float velocityX;   // 0xF8
    float velocityY;   // 0xFC
    int active;        // 0x100
    int field_0x104;   // 0x104

    Projectile();          // 0x4161B0
    void Launch();         // 0x4162C0
    void Update();         // 0x4163E0
    int CheckCollision();  // 0x416500
};

#endif // PROJECTILE_H
