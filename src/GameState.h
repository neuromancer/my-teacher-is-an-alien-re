#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "Parser.h"

class GameState : public Parser {
public:
    int FindState(char*);
    int GetState(int);
    int LBLParse(char*);
    void Serialize(int);
    void SetMaxStates2(int);
    int* field_0x88;
    int* field_0x8c;
    int field_0x90;
};

#endif // GAMESTATE_H
