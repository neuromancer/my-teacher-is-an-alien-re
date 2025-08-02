#ifndef SC_TIMER_H
#define SC_TIMER_H

#include "ZBuffer.h"

class SC_Timer : public ZBuffer {
public:
    SC_Timer();
    ~SC_Timer();

    void Update(int, int);
    void AddMessage(int);
    int Input(void*);

private:
    char pad3[0xc8 - 0xa0 - sizeof(Timer)];
    void* m_eventList;
};

#endif // SC_TIMER_H
