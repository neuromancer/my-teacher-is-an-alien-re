#ifndef TIMEDEVENT_H
#define TIMEDEVENT_H

class TimedEvent {
public:
    TimedEvent();
    TimedEvent(const TimedEvent& other);
    ~TimedEvent();

    void* Init();
    void SetData(int);
    int Update();
    void Delete();
    void* Create(void*, void*);

private:
    int field_0x0;
    int field_0x4;
    int field_0x8;
    int field_0xc;
    char field_0x10[0x20];
    int field_0x30;
    int field_0x34;
    int field_0x38;
    int field_0x3c;
    char field_0x40[0x40];
    int field_0x80;
    int field_0x84;
    int field_0x88;
    int field_0x8c;
    int field_0x90;
    int field_0x94;
    int field_0x98;
    int field_0x9c;
    int field_0xa0;
    int field_0xa4;
    int field_0xa8;
    int field_0xac;
    int field_0xb0;
    int field_0xb4;
    int field_0xb8;
    int field_0xbc;
};

#endif // TIMEDEVENT_H
