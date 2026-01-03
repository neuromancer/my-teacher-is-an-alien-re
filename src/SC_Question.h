#ifndef SC_QUESTION_H
#define SC_QUESTION_H

#include "Parser.h"
#include "Queue.h"

class MouseControl;

class SC_Message : public Parser {
public:
    int targetAddress;  // 0x88 - destination handler address
    int sourceAddress;  // 0x8c - source/from address
    int command;        // 0x90 - message type/command code
    int data;           // 0x94 - associated data
    int priority;       // 0x98 - queue priority
    int param1;         // 0x9c
    int param2;         // 0xa0
    int clickX;         // 0xa4 - mouse click X (param9)
    int clickY;         // 0xa8 - mouse click Y (param10)
    int mouseX;         // 0xac - current mouse X
    int mouseY;         // 0xb0 - current mouse Y
    int field_b4;
    int field_b8;
    int userPtr;        // 0xbc - user pointer (param8)

    SC_Message(int targetAddress, int sourceAddress, int command, int data, int priority, int param1, int param2, int userPtr, int clickX, int clickY);
    ~SC_Message();
};

class SC_Question : public Parser
{
public:
    MouseControl* mouseControl; // 0x88
    Queue* messageQueue;        // 0x8c
    int questionId;             // 0x90 - question ID passed to constructor
    int field_94;
    int state;                  // 0x98 - 0=new, 2=already answered
    char label[128];            // 0x9c

    SC_Question(int id);
    ~SC_Question();
    
    /* Function start: 0x406930 */
    void Update(int x, int y);
    
    /* Function start: 0x406AF0 */
    virtual int LBLParse(char* line);
};

#endif
