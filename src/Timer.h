#ifndef TIMER_H
#define TIMER_H

#include <windows.h>
#include "BaseObject.h"

class Timer  : public BaseObject {
public:
    Timer* Init();
    void Reset();
    int Update();
    int field_0;
    int field_4;
    int field_8;
    int field_c;
    int field_10;
};

class TimedEvent {
public:
    TimedEvent(const TimedEvent& other);
    void Init();
    int Update();
    void SetData(void* data);
    ~TimedEvent();

    TimedEvent* Create(void* callback, void* data);

    int field_0;
    int field_4;
    int field_8;
    int field_c;
    int field_10;
    Timer timer; // at offset 0x14
    char data_10[0x20];
    int field_30;
    int field_34;
    int field_38;
    int field_3c;
    char data_40[0x40];
    int field_80;
    int field_84;
    int field_88;
    int field_8c;
    int field_90;
    int field_94;
    int field_98;
    int field_9c;
    int field_a0;
    int field_a4;
    int field_a8;
    int field_ac;
    int field_b0;
    int field_b4;
    int field_b8;
    int field_bc;
};

class TimeOut {
public:
    void Start(int timeout);
private:
    int active;
    int timeout;
    Timer* timer;
};

#endif // TIMER_H
