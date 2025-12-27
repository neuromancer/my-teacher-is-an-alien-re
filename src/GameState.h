#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "Parser.h"

class GameState : public Parser {
public:
    ~GameState();
    void ClearStates();
    int FindState(char* stateName);
    int GetState(int stateIndex);
    int LBLParse(char* line);
    void Serialize(int mode);
    void SetMaxStates(int count);


    int* stateValues;      // 0x88 - array of state values (saved/loaded in Serialize)
    char** stateLabels;    // 0x8c - array of pointers to label strings
    int maxStates;         // 0x90 - maximum number of states
    int reserved;          // 0x94 - unused, initialized to 0

    GameState() {
        this->stateValues = 0;
        this->stateLabels = 0;
        this->maxStates = 0;
        this->reserved = 0;
    }
};

#endif // GAMESTATE_H
