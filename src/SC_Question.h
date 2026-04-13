#ifndef SC_QUESTION_H
#define SC_QUESTION_H

#include "Parser.h"
#include "Queue.h"
class MMPlayer;
class SCI_Dialog;

class SC_MessageParser : public Parser {
public:
    int targetAddress;  // 0x90 - destination handler address / SpriteAction*
    int field_94;       // 0x94

    SC_MessageParser() {}
    SC_MessageParser(int addr) { targetAddress = addr; }
    ~SC_MessageParser() {}

    virtual int LBLParse(char* param_1);

    void Dump(int unused);
};

void ParseSpriteAction(void* param_1, void* param_2);

// SC_Question - Question/dialog system (full game)
// Constructor: 0x414780, Destructor: 0x4148F0
// Vtable: 0x4612D8
// Extends Parser directly
class SC_Question : public Parser
{
public:
    Queue* messageQueue;        // 0x90
    int field_94;               // 0x94 (flags, bit 8 used)
    int state;                  // 0x98 - 0=new, 1=active, 2=answered
    char label[128];            // 0x9C-0x11B
    MMPlayer* mouseControl;     // 0x11C
    SCI_Dialog* dialogPtr;      // 0x120
    int actionIndex[3];         // 0x124, 0x128, 0x12C
    unsigned int questionId;    // 0x130
    int field_134;              // 0x134

    SC_Question(int id, SCI_Dialog* dialog);
    ~SC_Question();

    void Update(int x, int y);
    void Finalize();
    void InitState();
    int OnInput(SC_MessageParser* msg);
    virtual int LBLParse(char* line);
};

#endif
