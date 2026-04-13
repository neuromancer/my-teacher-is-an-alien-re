#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "Sprite.h"
#include "SpriteAction.h"

// Projectile - Combat projectile
// Full game vtable: 0x461478
// Extends Sprite (0xF8) with position tracking and velocity
// Size: 0x128 bytes (Sprite 0xF8 + 0x30)
class Projectile : public Sprite {
public:
    int owner;           // 0xF8 — owner pointer
    SlimeDim startPos;   // 0xFC-0x103 (x=startX, y=startY)
    SlimeDim currentPos; // 0x104-0x10B (x=currentX, y=currentY)
    SlimeDim halfDim;    // 0x10C-0x113 (x=halfWidth, y=halfHeight)
    SlimeDim velocity;   // 0x114-0x11B (x=velocityX, y=velocityY as float bits)
    int active;          // 0x11C
    SlimeDim nextPos;    // 0x120-0x127 (x=nextX, y=nextY)

    Projectile(int owner); // 0x427150
    void Launch();         // 0x427270
    void Update();         // 0x427390
};

#endif // PROJECTILE_H
