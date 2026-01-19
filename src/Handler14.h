#ifndef HANDLER14_H
#define HANDLER14_H

#include "Handler.h"
#include "Timer.h"
#include "SC_OnScreenMessage.h"

// Handler14 - SC_Sound handler class with ID 14
// Size: 0xB8 bytes
// vtable: 0x431240
// Constructor: 0x40B7E0
// Destructor: 0x40B910
//
// Layout:
//   0x00-0x87: Parser base class (via Handler)
//   0x88-0x9F: Handler fields (handlerId + 5 fields, zeroed)
//   0xA0-0xB3: Timer timer (size 0x14)
//   0xB4: MessageList* list
class Handler14 : public Handler {
public:
    Handler14();
    ~Handler14();

    // Virtual method overrides
    virtual void Init(SC_Message* msg);
    virtual int HandleMessage(SC_Message* msg);
    virtual int Update(SC_Message* msg);
    virtual void Draw(int param1, int param2);
    virtual int Exit(SC_Message* msg);

    // Handler14-specific fields starting at 0xA0
    Timer timer;            // 0xA0 - 0xB3 (size 0x14)
    MessageList* list;      // 0xB4
};

#endif // HANDLER14_H
