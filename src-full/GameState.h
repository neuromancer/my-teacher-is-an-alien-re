#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "Parser.h"

class GameState : public Parser {
public:
    GameState() {
        stateValues = 0;
        stateLabels = 0;
        maxStates = 0;
        reserved = 0;
    }
    GameState(char* filename, char* format, int id) {
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
    void FUN_00409f20(int);           // state validation/setup
    int GetStateValue(int);           // get state value by index
    int FindLabel(char* name);        // find label index (0x433AE0)
    int FUN_00433bb0(int*);           // state check (0x433BB0)
    int FUN_00432e20(char*);          // lookup by name (0x432E20)

    int* stateValues;      // 0x90 - array of state values (saved/loaded in Serialize)
    char** stateLabels;    // 0x94 - array of pointers to label strings
    int maxStates;         // 0x98 - maximum number of states
    int reserved;          // 0x9C - unused, initialized to 0
};

#endif // GAMESTATE_H
