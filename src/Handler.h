#ifndef HANDLER_H
#define HANDLER_H

#include "ScriptHandler.h"

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
// Memory layout:
//   0x00-0x87: Parser base class (size 0x88)
//   0x88-0x97: ScriptHandler fields (targetAddress, sourceAddress, command, data)
//   0x98-0x9F: Handler-specific fields
class Handler : public ScriptHandler {
public:
    Handler() {}
    virtual ~Handler();

    // Default Init calls CopyCommandData (0x417180), default AddMessage calls WriteMessageAddress (0x4171B0)
    // In the original binary, the vtable points directly to these ScriptHandler functions.
    // Inline wrappers ensure derived classes calling Handler::Init/AddMessage
    // generate a direct CALL to CopyCommandData/WriteMessageAddress.
    virtual void Init(SC_Message* msg) { CopyCommandData((ScriptHandler*)msg); }
    virtual int AddMessage(SC_Message* msg) { return WriteMessageAddress((Message*)msg); }
    virtual int ShutDown(SC_Message* msg);
    virtual void Update(int param1, int param2);
    virtual int Exit(SC_Message* msg);
    virtual void OnInput(SC_Message* msg);

    // Handler-specific fields
    int field_98;       // 0x98
    int field_9C;       // 0x9C
};

#endif // HANDLER_H
