#ifndef SC_ONSCREENMESSAGE_H
#define SC_ONSCREENMESSAGE_H

#include "Handler.h"
#include "Timer.h"
#include "Memory.h"
#include "LinkedList.h"

// SC_OnScreenMessage - Handler class with ID 15
// Size: 0xB8 bytes
// vtable: 0x4311e8
// Constructor: 0x40A2E0
// Destructor: 0x40A410
class SC_OnScreenMessage : public Handler {
public:
    SC_OnScreenMessage();
    virtual ~SC_OnScreenMessage();

    // Virtual method overrides from Handler
    virtual void Init(SC_Message* msg);
    virtual int AddMessage(SC_Message* msg);
    virtual int ShutDown(SC_Message* msg);
    virtual void Update(int param1, int param2);
    virtual int Exit(SC_Message* msg);

    void Copy(SC_OnScreenMessage* other);

    // Fields starting at 0xA0 (0x00-0x87 is Parser, 0x88-0x9F is Handler base fields)
    MessageList* m_messageList; // 0xA0
    Timer timer;                // 0xA4 - 0xB7 (size 0x14)
};

#endif // SC_ONSCREENMESSAGE_H
