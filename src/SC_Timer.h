#ifndef SC_TIMER_H
#define SC_TIMER_H

#include "ScriptHandler.h"
#include "Timer.h"
#include "Queue.h"

class SC_Timer : public ScriptHandler {
public:
    SC_Timer();
    ~SC_Timer();

    void Update(int, int);
    void AddMessage(int);
    int Input(void*);

private:
    // ScriptHandler provides: targetAddress(0x88), sourceAddress(0x8c), command(0x90), data(0x94)
    // Using targetAddress as m_messageId and sourceAddress as m_messageData
    int field_0x98;
    int field_0x9c;
    Timer timer1; // 0xa0
    Timer timer2; // 0xb4
    Queue* m_eventList; // 0xc8
};

// Convenience macros to refer to the inherited fields by their SC_Timer-specific names
#define m_messageId targetAddress
#define m_messageData sourceAddress

#endif // SC_TIMER_H
