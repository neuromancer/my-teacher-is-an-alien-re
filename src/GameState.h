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
    int* field_0x88;
    int* field_0x8c;
    int field_0x90;
    int field_0x94;
    GameState();
};

#endif // GAMESTATE_H
