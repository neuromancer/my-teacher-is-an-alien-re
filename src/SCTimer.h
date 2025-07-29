#ifndef SCTIMER_H
#define SCTIMER_H

#include "Timer.h"
#include "BaseObject.h"

class SCTimer : public BaseObject {
    friend void Timer_impl_dtor(void* timer);
public:
    void Init();
    virtual ~SCTimer();
    void Update(int, int);
    int Input(void *message);

private:
    char unknown_data[0x88];
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

#endif // SCTIMER_H
