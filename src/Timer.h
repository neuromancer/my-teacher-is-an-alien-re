#ifndef TIMER_H
#define TIMER_H

class Timer {
public:
    int field_0;
    int field_4;
    int field_8;
    int field_c;
    int field_10;
};

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
