#ifndef HOTSPOT_H
#define HOTSPOT_H

#include "Sprite.h"

class Hotspot {
public:
    char pad[0x88];
    Sprite* sprite; // 0x88
    void* list1; // 0x8c
    void* list2; // 0x90
    void* list3; // 0x94
    char pad2[0x11c - 0x94 - 4];
    int state; // 0x11c
    int field_0x120;
    int field_0x124;
    int field_0x128;

    unsigned char Do();
    int SetState(int state);
    int GetState();
    void CleanupSprites();
    int Update(int, int, int);
};

#endif // HOTSPOT_H
