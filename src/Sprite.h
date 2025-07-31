#ifndef SPRITE_H
#define SPRITE_H

class Sprite {
public:
    char pad0[0x88];
    void* field136_0x88; // 0x88
    char pad1[0xac - 0x88 - 4];
    int field_0xac; // 0xac
    char pad2[0xb4 - 0xac - 4];
    char* filename; // 0xb4
    char pad3[0xd0 - 0xb4 - 4];
    void* animation_data; // 0xd0

    void CheckRanges1();
};

#endif // SPRITE_H
