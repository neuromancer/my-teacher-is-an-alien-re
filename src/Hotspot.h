#ifndef HOTSPOT_H
#define HOTSPOT_H

#include "Sprite.h"

class Hotspot {
public:
    char pad[0x88];
    Sprite* sprite; // 0x88
    unsigned char Do();
};

#endif // HOTSPOT_H
