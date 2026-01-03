#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "Parser.h"

class GameState : public Parser {
public:
    GameState();
    ~GameState();
    void ClearStates();
    int FindState(char* stateName);
    char *GetState(int stateIndex);
    int LBLParse(char* line);
    void Serialize(int mode);
    void SetMaxStates(int count);


    int* stateValues;      // 0x88 - array of state values (saved/loaded in Serialize)
    char** stateLabels;    // 0x8c - array of pointers to label strings
    int maxStates;         // 0x90 - maximum number of states
    int reserved;          // 0x94 - unused, initialized to 0
};

#endif // GAMESTATE_H
