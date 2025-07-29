#ifndef TIMER_H
#define TIMER_H

#include <windows.h>

class Timer {
public:
    Timer* Init();
    void Reset();
    void Update();
    int field_0;
    int field_4;
    int field_8;
    int field_c;
    int field_10;
};

void Timer::Reset() {
    this->field_8 = timeGetTime();
    this->field_0 = 0;
}

class TimedEvent {
public:
    void Init();
    int Update();

    int field_0;
    int field_4;
    int field_8;
    int field_c;
    int field_10;
    Timer timer;
};

#endif // TIMER_H
