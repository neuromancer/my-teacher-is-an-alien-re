#ifndef ANIMATION_H
#define ANIMATION_H

#include "VBuffer.h"
#include "Smacker.h"

class Animation;

class Smack {
    friend class Animation;
public:
    void UpdateSmacker(int param_1);
private:
    char pad_0x0[0xc];
    SmackStruct* smacker_data; // 0xc
    char pad_0x10[0x18 - 0x10];
    int field_0x18; // 0x18
};

class Animation {
public:
    void SetRange(int param_1, int param_2, int param_3);
    void SetState(int param_1);
    void SetLogic(int param_1, int param_2);
    void AllocateLogic(int param_1);
    void CheckRanges1();
    void ToBuffer();
    void ToBuffer2(void* param_1);
    void CleanArray10();
    void VBInit();
    void Free();
    void DoFrame();
    void MainLoop();
private:
    char pad_0x0[0x8];
    VBuffer* virtualBuffer; // 0x8
    SmackStruct* smkStruct; // 0xc
    char pad_0x10[0x18 - 0x10];
    void* field0_0x0; // 0x18
    char pad_0x1c[0x24 - 0x1c];
    SmackStruct* smkStruct2; // 0x24
    int field_0x28; // 0x28
    char pad_0x2c[0x88 - 0x2c];
    int* field136_0x88;
    int flags;
    int field138_0x90;
    char pad_0x94[0x98 - 0x94];
    int field140_0x98;
    char pad_0x9c[0xa0 - 0x9c];
    int field142_0xa0;
    char pad_0xa4[0xac - 0xa4];
    int field_0xac;
    char pad_0xb0[0xb4 - 0xb0];
    char* filename;
    char pad_0xb8[0xd0 - 0xb8];
    Smack* animation_data;
};

#endif // ANIMATION_H
