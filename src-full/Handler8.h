#ifndef HANDLER8_H
#define HANDLER8_H

#include "Handler.h"

class SC_Message;

// Handler8 - Command 8 Handler (Cinematic Player)
// Size: 0xa8 bytes
// vtable: 0x431138
// Inherits from Handler (which inherits from Parser)
// Layout:
//   0x00-0x87: Parser base class (size 0x88)
//   0x88: handlerId
//   0x8C: moduleParam
//   0x90: savedCommand
//   0x94: savedMsgData
//   0x98: field_98
//   0x9C: field_9C
//   0xA0: message - SC_Message* (deleted in destructor)
//   0xA4: field_A4
class Handler8 : public Handler {
public:
    Handler8();
    ~Handler8();

    // Virtual method overrides
    virtual void Init(SC_Message* msg);
    virtual int AddMessage(SC_Message* msg);
    virtual int ShutDown(SC_Message* msg);
    virtual void Update(int param1, int param2);
    virtual int Exit(SC_Message* msg);

    // Helper method
    void ProcessMessage();

    // Handler8-specific fields (field_98 and field_9C are inherited from Handler)
    SC_Message* message;    // 0xA0
    int field_A4;           // 0xA4
};

#endif // HANDLER8_H
