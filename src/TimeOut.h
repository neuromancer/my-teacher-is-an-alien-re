#ifndef TIMEOUT_H
#define TIMEOUT_H

#include "Timer.h"

class TimeOut
{
public:
    TimeOut();
    ~TimeOut();
    void Start(int timeout);
    int IsTimeOut();
    void Stop();

    int m_isActive;
    int m_timeout;
    Timer *m_timer;
};

// InitTimeOut at 0x421920 - constructs TimeOut via default constructor
TimeOut* __fastcall InitTimeOut(TimeOut* buffer);

#endif // TIMEOUT_H
