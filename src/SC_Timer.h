#ifndef SC_TIMER_H
#define SC_TIMER_H

#include "Parser.h"
#include "Timer.h"
#include "Queue.h"

class SC_Timer : public Parser {
public:
    SC_Timer();
    ~SC_Timer();

    void Update(int, int);
    void AddMessage(int);
    int Input(void*);

private:
    int m_messageId; // 0x88
    int m_messageData; // 0x8c
    int field_0x90;
    int field_0x94;
    int field_0x98;
    int field_0x9c;
    Timer timer1; // 0xa0
    Timer timer2; // 0xb4
    Queue* m_eventList; // 0xc8
};

#endif // SC_TIMER_H
