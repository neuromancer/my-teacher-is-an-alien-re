#ifndef SPRITELIST_H
#define SPRITELIST_H

// SpriteList class is now merged into MMPlayer
// This header provides backwards compatibility
#include "MMPlayer.h"

#include "LinkedList.h"

// SpriteData - the data stored in each node
// This is the same layout as CharSprite
struct SpriteData {
    int field_0x00;           // 0x00
    char name[64];            // 0x04
    int priority;             // 0x44
    int state;                // 0x48
    int rect_x;               // 0x4c
    int rect_y;               // 0x50
    int rect_w;               // 0x54
    int rect_h;               // 0x58
};

#endif // SPRITELIST_H
