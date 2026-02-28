#ifndef HANDLER_H
#define HANDLER_H

#include "Parser.h"
#include <string.h>

class SC_Message;

// Handler - Base class for all game handlers (modules)
// Handlers are scene/screen managers that process messages and render content
//
// vtable layout (extends Parser):
//   +0x00: LBLParse (Parser virtual)
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
// Memory layout (full game):
//   0x00-0x8F: Parser base class (size 0x90, full game has 2 extra fields at 0x88/0x8C)
//   0x90: handlerId - identifies the handler type (2, 4, 5, 6, etc.)
//   0x94+: Handler-specific fields
//   Total Handler size: 0xA8
class Handler : public Parser {
public:
    Handler() { memset(&handlerId, 0, 24); }
    virtual ~Handler();

    // Virtual methods - these form the handler interface
    // Override in derived classes to implement handler behavior
    virtual void Init(SC_Message* msg);
    virtual int AddMessage(SC_Message* msg);
    virtual int ShutDown(SC_Message* msg);
    virtual void Update(int param1, int param2);
    virtual int Exit(SC_Message* msg);
    virtual void OnInput(void* param);

    // Copy command/data from message to handler fields
    void CopyCommandData(SC_Message* msg);

    // Write handler address to message
    int WriteMessageAddress(SC_Message* msg);

    // Common handler fields
    int handlerId;      // 0x90 - handler type identifier
    int moduleParam;    // 0x94 - handler-specific parameter
    int savedCommand;   // 0x98 - saved command from CopyCommandData
    int savedMsgData;   // 0x9C - saved data from CopyCommandData
    int field_A0;       // 0xA0
    int field_A4;       // 0xA4
};

#endif // HANDLER_H
