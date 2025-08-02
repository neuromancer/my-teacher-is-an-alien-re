#ifndef SC_ONSCREENMESSAGE_H
#define SC_ONSCREENMESSAGE_H

#include "Queue.h"
#include "Timer.h"

class SC_OnScreenMessage {
public:
    void Update(int, int);
    void AddMessage();

private:
    char pad[0x88];
    int field_0x88;
    int field_0x8c;
    char pad2[0xa0 - 0x8c - 4];
    Queue messageQueue; // 0xa0
    Timer timer; // 0xa4
};

#endif // SC_ONSCREENMESSAGE_H
