#ifndef TIMER_H
#define TIMER_H

void Timer_DecrementCounter();

struct Timer {
    unsigned int m_pauseDuration;
    unsigned int m_unknown4;
    unsigned int m_startTime;
    unsigned int m_currentTime;
    unsigned int m_elapsedTime;

    Timer();
    inline ~Timer() {
        Timer_DecrementCounter();
    }
    void Reset();
    unsigned int Update();
    void Wait(unsigned int);
};

#endif // TIMER_H
