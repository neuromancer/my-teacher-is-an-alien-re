#ifndef SPRITE_H
#define SPRITE_H

class Sprite {
public:
    char pad0[0x88];
    void* field136_0x88; // 0x88
    int flags; // 0x8c
    char pad1[0x98 - 0x88 - 8];
    void* field_0x98; // 0x98
    int field_0x9c;   // 0x9c
    int field_0xa0;   // 0xa0
    char pad2[0xac - 0xa0 - 4];
    int field_0xac; // 0xac
    char pad3[0xb4 - 0xac - 4];
    char* filename; // 0xb4
    char pad4[0xd0 - 0xb4 - 4];
    void* animation_data; // 0xd0

    void CheckRanges1();
    int CheckConditions();
    void SetRange(int param_1, int param_2, int param_3);
    void SetState(int param_1);
};

#endif // SPRITE_H
