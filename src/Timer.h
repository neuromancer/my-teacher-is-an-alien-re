#ifndef TIMER_H
#define TIMER_H

struct Timer {
    unsigned int m_pauseDuration;
    unsigned int m_unknown4;
    unsigned int m_startTime;
    unsigned int m_currentTime;
    unsigned int m_elapsedTime;

    //~Timer();
    Timer* Init();
    void Reset();
    unsigned int Update();
    void Wait(unsigned int);
};

void Timer_DecrementCounter();

#endif // TIMER_H
