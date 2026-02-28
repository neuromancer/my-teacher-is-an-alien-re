#ifndef SC_ONSCREENMESSAGE_H
#define SC_ONSCREENMESSAGE_H

#include "Handler.h"
#include "Timer.h"
#include "Memory.h"
#include "LinkedList.h"

// SC_OnScreenMessage - Handler class with ID 4 (full game)
// Size: 0xC0 bytes
// vtable: 0x461a60
// Constructor: 0x4482F0
// Destructor: 0x448420
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

    // Fields starting at 0xA8 (Handler base is 0xA8 in full game)
    Timer timer;                // 0xA8 - 0xBB (size 0x14)
    MessageList* m_messageList; // 0xBC
};

#endif // SC_ONSCREENMESSAGE_H
