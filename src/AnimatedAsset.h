#ifndef ANIMATEDASSET_H
#define ANIMATEDASSET_H

#include "VBuffer.h"

class AnimatedAsset {
public:
    VBuffer* buffer; // 0x0
    int field_0x4;
    int field_0x8;
    int field_0xc;
    char field_0x10;
    char pad_11;
    char field_0x12;
    char pad_13;
    int field_0x14;
    int field_0x18;
    int field_0x1c;
    int field_0x20;
    int field_0x24;
    int field_0x28;
    int field_0x2c;
    int field_0x30;
    int field_0x34;
    int field_0x38;

    AnimatedAsset();
    void LoadAnimatedAsset(char* param_1);
    void RenderText(char* text, int param_2);
    int DrawChar(int, int, int);
    ~AnimatedAsset();
};

#endif // ANIMATEDASSET_H
