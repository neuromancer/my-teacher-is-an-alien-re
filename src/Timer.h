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

class BaseObject {
public:
    void Init();
};

class TimedEvent {
public:
    void Init();
    void SetData(void* data);
    int Update();

    int field_0;
    int field_4;
    int field_8;
    int field_c;
    int field_10;
    Timer *timer;
};

class SCTimer : public BaseObject {
    friend void Timer_impl_dtor(void* timer);
public:
    void Init();
    virtual ~SCTimer();
    int Input(void *message);

private:
    char unknown_data[0x84];
    int field_0x88[6];
    Timer timer1;
    Timer timer2;
    void* field_0xc8;
};

struct Message {
    char unknown[0x88];
    int field_0x88;
    int field_0x8c;
    char unknown2[0xc];
    int field_0x98;
    void* field_0x9c;
    char unknown3[0x1c];
    int field_0xb8;
    int field_0xbc;
};

struct Queue {
    void* head;
    void* tail;
    void* current;
    int mode;
};

#endif // TIMER_H
