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
    // Full game layout confirmed from constructor asm (0x44C660),
    // ProcessSprite asm (0x40DF30), and Target::AdvanceHotspot (0x4428E0)
    Range* ranges; // 0x90
    int flags; // 0x94
    int handle;   // 0x98
    int priority; // 0x9c
    int current_state; // 0xa0
    LogicCondition* logic_conditions; // 0xa4
    int num_logic_conditions;   // 0xa8
    int num_states; // 0xac
    int field_0xb0; // 0xb0
    int loc_x; // 0xb4
    int loc_y; // 0xb8
    char sprite_filename[28]; // 0xbc-0xd7
    int field_0xd8; // 0xd8
    int field_0xdc; // 0xdc
    int field_0xe0; // 0xe0
    int field_0xe4; // 0xe4
    int field_0xe8; // 0xe8
    int field_0xec; // 0xec
    Animation* animation_data; // 0xf0 — confirmed by InitAnimation (0x44C880)
    int field_0xf4; // 0xf4
    // total Sprite size = 0xF8 (full game)

    Sprite(char* filename);
    ~Sprite();
    void CheckRanges1();
    int CheckConditions();
    void SetRange(int param_1, int param_2, int param_3, int param_4 = 0);
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
    void ResetAnimation(int param1, int param2);  // 0x44CB40
    int RenderAt(int, int, int, int);              // 0x44CCF0
    void ConfigRange(int, int, int, int);          // 0x44D210
    void ConfigStates(int);                        // 0x44D2A0
};

#endif // SPRITE_H
