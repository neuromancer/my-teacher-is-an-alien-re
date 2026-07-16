#ifndef SPRITE_H
#define SPRITE_H

#include "Parser.h"
#include "Animation.h"
#include "Range.h"
#include "SpriteAction.h"  // for SlimeDim

struct LogicCondition {
    int state_index;  // 0x00
    int type;         // 0x04
    int field_8;      // 0x08

    LogicCondition() {}
    ~LogicCondition();
};

class Sprite : public Parser {
public:
    // Full game layout confirmed from constructor asm (0x44C660),
    // ProcessSprite asm (0x40DF30), and Target::AdvanceHotspot (0x4428E0)
    Range* ranges; // 0x90
    int flags; // 0x94
    int handle;   // 0x98 — also current range state (set by ResetAnimation)
    int priority; // 0x9c
    LogicCondition* logic_conditions; // 0xa0
    int rightBound; // 0xa4
    int num_logic_conditions;   // 0xa8
    SlimeDim loc; // 0xac — position (set by LOC keyword, ProcessSprite reads this)
    int num_states; // 0xb4 — range count (set by ConfigStates)
    int handleValue; // 0xb8 — HANDLE value (set by full game LBLParse HANDLE keyword)
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
    void CheckRanges1();                           // 0x44CF50
    int CheckConditions();                         // 0x44CFE0
    void SetLogic(int param_1, int param_2, int param_3 = 0); // 0x44D530
    void SetSpriteLogic(char* gsName, char* condition, int value); // 0x44D420
    void InitLogic(int param_1);                   // 0x44D5C0
    void InitAnimation();                          // 0x44C880
    void OpenAnimation();                          // 0x44CA50
    virtual int LBLParse(char* param_1);           // 0x44D700
    virtual void OnProcessEnd();                   // 0x44D6E0
    void FreeAnimation();                          // 0x44CB10
    void StopAnimationSound();                     // 0x44C9D0
    void ResetAnimation(int param1, int param2);   // 0x44CB40
    int Do(int, int, double);                       // 0x44CCF0
    void ConfigRange(int, int, int, int);          // 0x44D210
    void ConfigStates(int);                        // 0x44D2A0
};


#endif // SPRITE_H
