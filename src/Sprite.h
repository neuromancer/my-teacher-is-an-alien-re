#ifndef SPRITE_H
#define SPRITE_H

#include "Parser.h"
#include "Animation.h"
#include "Range.h"

struct LogicCondition {
    int state_index;
    int type;

    LogicCondition() {}
    ~LogicCondition() {}
};

class Sprite : public Parser {
public:
    Range* ranges; // 0x88
    int flags; // 0x8c
    int current_state; // 0x90
    int priority; // 0x94
    LogicCondition* logic_conditions; // 0x98
    int handle;   // 0x9c
    int num_logic_conditions;   // 0xa0
    int loc_x; // 0xa4
    int loc_y; // 0xa8
    int num_states; // 0xac
    int field_0xb0;
    char sprite_filename[28]; // 0xb4
    Animation* animation_data; // 0xd0

    Sprite(char* filename);
    ~Sprite();
    void CheckRanges1();
    int CheckConditions();
    void SetRange(int param_1, int param_2, int param_3);
    void SetState(int param_1);
    void SetState2(int param_1);
    void SetLogic(int param_1, int param_2);
    void InitLogic(int param_1);
    void Init();
    void InitAnimation();
    virtual int LBLParse(char* param_1);
    void Dump();
    int Do(int, int, double);
    void FreeAnimation();
    void StopAnimationSound();
};

#endif // SPRITE_H
