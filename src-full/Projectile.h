#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "Sprite.h"

// Projectile - Combat projectile
// Full game vtable: 0x461478
// Extends Sprite (0xF8) with position tracking and velocity
// Size: 0x128 bytes (Sprite 0xF8 + 0x30)
class Projectile : public Sprite {
public:
    int owner;         // 0xF8 — owner pointer
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

    Projectile();          // 0x434660
    Projectile(int owner); // 0x427150
    void Launch();         // 0x427270
    void Update();         // 0x427390
};

#endif // PROJECTILE_H
