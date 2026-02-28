#ifndef SC_TIMER_H
#define SC_TIMER_H

#include "Handler.h"
#include "Timer.h"
#include "Queue.h"

class SC_Timer : public Handler {
public:
    SC_Timer();
    ~SC_Timer();

    void Init(SC_Message*);
    int AddMessage(SC_Message*);
    int ShutDown(SC_Message*);
    void Update(int, int);
    int Exit(SC_Message*);

private:
    Timer timer1; // 0xa0
    Timer timer2; // 0xb4
    Queue* m_eventList; // 0xc8
};

// Convenience macros to refer to the inherited fields by their SC_Timer-specific names
#define m_messageId handlerId
#define m_messageData moduleParam

#endif // SC_TIMER_H
