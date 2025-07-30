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
    static void __fastcall FUN_00419010(void**);

private:
    unsigned int m_field0;
    unsigned int m_field4;
    unsigned int m_field8;
    unsigned int m_fieldC;
    unsigned int m_field10;
};

void Timer_DecrementCounter();

#endif // TIMER_H
