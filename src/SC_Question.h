#ifndef SC_QUESTION_H
#define SC_QUESTION_H

#include "Parser.h"
#include "Queue.h"

class MouseControl;

class SC_Message : public Parser {
public:
    int field_88;
    int field_8c;
    int field_90;
    int field_94;
    int field_98;
    int field_9c;
    int field_a0;
    int field_a4;
    int field_a8;
    int field_ac;
    int field_b0;
    int field_b4;
    int field_b8;
    int field_bc;

    SC_Message(int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9, int p10);
};

class SC_Question : public Parser
{
public:
    MouseControl* mouseControl; // 0x88
    Queue* messageQueue;        // 0x8c
    int field_90;
    int field_94;
    int field_98;
    char label[128];            // 0x9c

    SC_Question();
    
    /* Function start: 0x406AF0 */
    virtual int LBLParse(char* line);
};

#endif
