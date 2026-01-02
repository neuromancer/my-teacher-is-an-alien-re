#ifndef SC_ONSCREENMESSAGE_H
#define SC_ONSCREENMESSAGE_H

#include "Queue.h"
#include "Timer.h"
#include "ScriptHandler.h"

class SC_OnScreenMessage : ScriptHandler {
public:
    SC_OnScreenMessage();
    void Destroy(int free);
    void Free();
    void Update(int, int);
    void AddMessage();
    void Copy(SC_OnScreenMessage* other);
    void Timer_DecrementCounter_wrapper();
    void Timer_DecrementCounter_wrapper_2();

    // targetAddress inherited from ScriptHandler at 0x88
    // sourceAddress inherited from ScriptHandler at 0x8c
    // command inherited from ScriptHandler at 0x90
    // data inherited from ScriptHandler at 0x94
    char pad2[0xa0 - 0x98];
    Queue messageQueue; // 0xa0
    Timer timer;
};

#endif // SC_ONSCREENMESSAGE_H
