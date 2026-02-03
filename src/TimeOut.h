#ifndef TIMEOUT_H
#define TIMEOUT_H

#include "Timer.h"

class TimeOut
{
public:
    void Start(int timeout);
    int IsTimeOut();
    void Stop();

    int m_isActive;
    int m_timeout;
    Timer m_timer;
};

#endif // TIMEOUT_H
