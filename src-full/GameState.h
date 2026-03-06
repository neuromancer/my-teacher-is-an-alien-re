#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "Parser.h"

class GameState : public Parser {
public:
    GameState() {
        field_0x88 = 0;
        field_0x8C = 0;
        stateValues = 0;
        stateLabels = 0;
        maxStates = 0;
        reserved = 0;
    }
    GameState(char* filename, char* format, int id) {
        field_0x88 = 0;
        field_0x8C = 0;
        stateValues = 0;
        stateLabels = 0;
        maxStates = 0;
        reserved = 0;
        ParseFile(this, filename, format, id);
    }
    ~GameState();
    void ClearStates();
    int FindState(char* stateName);
    char *GetState(int stateIndex);
    int LBLParse(char* line);
    void Serialize(int mode);
    void SetMaxStates(int count);


    int field_0x88;        // 0x88 - full game only
    int field_0x8C;        // 0x8C - full game only
    int* stateValues;      // 0x90 - array of state values (saved/loaded in Serialize)
    char** stateLabels;    // 0x94 - array of pointers to label strings
    int maxStates;         // 0x98 - maximum number of states
    int reserved;          // 0x9C - unused, initialized to 0
};

#endif // GAMESTATE_H
