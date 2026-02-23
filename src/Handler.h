#ifndef HANDLER_H
#define HANDLER_H

#include "Parser.h"
#include <string.h>

struct Message;
class SC_Message;

// Handler - Base class for all game handlers (modules)
// Handlers are scene/screen managers that process messages and render content
//
// vtable layout (extends Parser):
//   +0x00: LBLParse (Handler override)
//   +0x04: OnProcessStart (Parser virtual)
//   +0x08: OnProcessEnd (Parser virtual)
//   +0x0C: Handler scalar deleting destructor (virtual ~Handler)
//   +0x10: Init (virtual) - called when entering handler
//   +0x14: AddMessage (virtual) - process messages
//   +0x18: ShutDown (virtual) - called on system messages
//   +0x1C: Update (virtual) - render/update handler content
//   +0x20: Exit (virtual) - called when leaving handler
//   +0x24: OnInput (virtual) - input handling
//
// Memory layout:
//   0x00-0x87: Parser base class (size 0x88)
//   0x88-0x9F: Handler fields
class Handler : public Parser {
public:
    Handler() {
        memset(&targetAddress, 0, 24);
    }
    virtual ~Handler();

    // Handler overrides LBLParse with "Must define an LBLParse func" error (0x4010C0)
    int LBLParse(char*);

    // Handler::Init (0x417180) and Handler::AddMessage (0x4171B0) defined in ScriptHandler.cpp
    virtual void Init(SC_Message* msg);
    virtual int AddMessage(SC_Message* msg);
    virtual int ShutDown(SC_Message* msg);
    virtual void Update(int param1, int param2);
    virtual int Exit(SC_Message* msg);
    virtual void OnInput(SC_Message* msg);

    int targetAddress;  // 0x88 - handler address identifier
    int sourceAddress;  // 0x8c - source address (from)
    int command;        // 0x90 - message command/type
    int data;           // 0x94 - message data
    int field_98;       // 0x98
    int field_9C;       // 0x9C
};

#endif // HANDLER_H
