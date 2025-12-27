#ifndef TIMER_H
#define TIMER_H

class Timer {
public:
    Timer();
    ~Timer();
    Timer* Init();
    void Reset();
    unsigned int Update();
    void Wait(unsigned int);
    void FUN_00418ef0();
    void FUN_00418ee0();

private:
    unsigned int m_pauseDuration;
    unsigned int m_unknown4;
    unsigned int m_startTime;
    unsigned int m_currentTime;
    unsigned int m_elapsedTime;
};

void Timer_DecrementCounter();

#endif // TIMER_H
