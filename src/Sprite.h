#ifndef SPRITE_H
#define SPRITE_H

#include "Parser.h"
#include "Animation.h"

class Sprite : public Parser {
public:
    void* ranges; // 0x88
    int flags; // 0x8c
    int field_0x90; // 0x90
    int priority; // 0x94
    void* logic_conditions; // 0x98
    int handle;   // 0x9c
    int num_logic_conditions;   // 0xa0
    int loc_x; // 0xa4
    int loc_y; // 0xa8
    int num_states; // 0xac
    char pad3[0xb4 - 0xac - 4];
    char* sprite_filename; // 0xb4
    char pad4[0xd0 - 0xb4 - 4];
    Animation* animation_data; // 0xd0

    void CheckRanges1();
    int CheckConditions();
    void SetRange(int param_1, int param_2, int param_3);
    void SetState(int param_1);
    void SetState2(int param_1);
    void SetLogic(int param_1, int param_2);
    void InitLogic(int param_1);
    void InitAnimation();
    virtual int LBLParse(char* param_1);
    void Dump();
    unsigned char Do(int, int, int, int);
    void FreeAnimation();
    void StopAnimationSound();
};

#endif // SPRITE_H
