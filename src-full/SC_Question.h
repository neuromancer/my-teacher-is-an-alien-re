#ifndef SC_QUESTION_H
#define SC_QUESTION_H

#include "Parser.h"
#include "Queue.h"
class MMPlayer;

class SC_Message : public Parser {
public:
    int targetAddress;  // 0x90 - destination handler address / SpriteAction*
    int sourceAddress;  // 0x94 - source/from address
    int command;        // 0x98 - message type/command code
    int data;           // 0x9c - associated data
    int priority;       // 0xa0 - queue priority
    int param1;         // 0xa4
    int param2;         // 0xa8
    int clickX;         // 0xac - click position X
    int clickY;         // 0xb0 - click position Y
    int mouseX;         // 0xb4 - current mouse X
    int mouseY;         // 0xb8 - current mouse Y
    int lastKey;        // 0xbc - last key pressed
    int time;           // 0xc0 - time value
    int userPtr;        // 0xc4 - user pointer (param8)

    SC_Message() {}
    SC_Message(int, int, int, int, int, int, int, int, int, int) {}
    ~SC_Message();

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
    int field_88;               // 0x88
    int field_8C;               // 0x8C
    Queue* messageQueue;        // 0x90
    int field_94;               // 0x94 (flags, bit 8 used)
    int state;                  // 0x98 - 0=new, 1=active, 2=answered
    char label[128];            // 0x9C-0x11B
    void* mouseControl;         // 0x11C (MMPlayer*)
    void* dialogPtr;            // 0x120 (SCI_Dialog*)
    int actionIndex[3];         // 0x124, 0x128, 0x12C
    unsigned int questionId;    // 0x130
    int field_134;              // 0x134

    SC_Question(int id, int dialog);
    ~SC_Question();

    void Update(int x, int y);
    void Finalize();
    void InitState();
    int OnInput(SC_Message* msg);
    virtual int LBLParse(char* line);
};

#endif
