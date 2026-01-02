#ifndef SC_ONSCREENMESSAGE_H
#define SC_ONSCREENMESSAGE_H

#include "Queue.h"
#include "Timer.h"
#include "Parser.h"

class SC_OnScreenMessage : Parser {
public:
    SC_OnScreenMessage();
    void Destroy(int free);
    void Free();
    void Update(int, int);
    void AddMessage();
    void Timer_DecrementCounter_wrapper();
    void Timer_DecrementCounter_wrapper_2();

private:
    int m_address;     // 0x88 - handler address identifier
    int m_fromAddress; // 0x8c - source address
    char pad2[0xa0 - 0x8c - 4];
    Queue messageQueue; // 0xa0
    Timer timer; // 0xa4
};

#endif // SC_ONSCREENMESSAGE_H
