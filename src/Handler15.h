#ifndef HANDLER15_H
#define HANDLER15_H

#include "Handler.h"
#include "Timer.h"
#include "SC_OnScreenMessage.h"

// Handler15 - SC_OnScreenMessage handler class with ID 15
// Size: 0xB8 bytes
// vtable: 0x4311e8
// Constructor: 0x40A2E0
// Destructor: 0x40A410
//
// Layout:
//   0x00-0x87: Parser base class (via Handler)
//   0x88-0x9F: Handler fields (handlerId + 5 fields, zeroed)
//   0xA0: MessageList* list
//   0xA4-0xB7: Timer timer (size 0x14)
class Handler15 : public Handler {
public:
    Handler15();
    ~Handler15();

    // Virtual method overrides
    virtual void Init(SC_Message* msg);
    virtual int HandleMessage(SC_Message* msg);
    virtual int Update(SC_Message* msg);
    virtual void Draw(int param1, int param2);
    virtual int Exit(SC_Message* msg);

    // Handler15-specific fields starting at 0xA0
    MessageList* list;      // 0xA0
    Timer timer;            // 0xA4 - 0xB7 (size 0x14)
};

#endif // HANDLER15_H
