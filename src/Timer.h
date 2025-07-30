#ifndef TIMER_H
#define TIMER_H

class Timer {
public:
    Timer* Init();
    void Reset();

private:
    unsigned int m_field0;
    unsigned int m_field4;
    unsigned int m_field8;
    unsigned int m_fieldC;
    unsigned int m_field10;
};

#endif // TIMER_H
