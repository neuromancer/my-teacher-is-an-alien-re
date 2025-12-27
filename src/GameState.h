#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "Parser.h"

class GameState : public Parser {
public:
    ~GameState();
    void ClearStates();
    int FindState(char*);
    int GetState(int);
    int LBLParse(char*);
    void Serialize(int);
    void SetMaxStates2(int);
    void FUN_00420480();
    int* field_0x88;
    int* field_0x8c;
    int field_0x90;
    int field_0x94;
    GameState() {
        this->field_0x88 = 0;
        this->field_0x8c = 0;
        this->field_0x90 = 0;
        this->field_0x94 = 0;
    }
};

#endif // GAMESTATE_H
