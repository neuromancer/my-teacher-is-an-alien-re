#ifndef HANDLER13_H
#define HANDLER13_H

#include "Handler.h"
#include "Timer.h"
#include "SC_OnScreenMessage.h"
#include "SC_Question.h"

// Handler13 - Handler class with ID 13
// Size: 0xD0 bytes
// vtable: 0x431060
// Constructor: 0x401B60
// Destructor: 0x401CA0
//
// Layout:
//   0x00-0x87: Parser base class (via Handler)
//   0x88-0x9F: Handler fields (handlerId + 5 fields, zeroed)
//   0xA0-0xB3: Timer timer1 (size 0x14)
//   0xB4-0xC7: Timer timer2 (size 0x14)
//   0xC8: MessageList* list
//   0xCC-0xCF: padding
class Handler13 : public Handler {
public:
    Handler13();
    ~Handler13();

    // Virtual method overrides
    virtual void Init(SC_Message* msg);
    virtual int AddMessage(SC_Message* msg);
    virtual int ShutDown(SC_Message* msg);
    virtual void Update(int param1, int param2);
    virtual int Exit(SC_Message* msg);

    // Handler13-specific fields starting at 0xA0
    Timer timer1;           // 0xA0 - 0xB3 (size 0x14)
    Timer timer2;           // 0xB4 - 0xC7 (size 0x14)
    MessageList* list;      // 0xC8
    int field_0xCC;         // 0xCC - padding
};

#endif // HANDLER13_H
