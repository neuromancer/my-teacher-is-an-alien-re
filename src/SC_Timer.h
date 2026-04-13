#ifndef SC_TIMER_H
#define SC_TIMER_H

#include "Handler.h"
#include "Timer.h"
#include "SC_OnScreenMessage.h"

// SC_Timer (aka Handler13) - Timer/Event handler (case 0x0D)
// Size: 0xD0 bytes
// Vtable: 0x431060
// Constructor: 0x422C20, Destructor: 0x422D60
class SC_Timer : public Handler {
public:
    SC_Timer();
    ~SC_Timer();

    void Init(SC_MessageParser*);
    int AddMessage(SC_MessageParser*);
    int ShutDown(SC_MessageParser*);
    void Update(int, int);
    int Exit(SC_MessageParser*);

    Timer timer1;           // 0xA0
    Timer timer2;           // 0xB4
    MessageList* list;      // 0xC8
    int field_0xCC;         // 0xCC
};

#endif // SC_TIMER_H
