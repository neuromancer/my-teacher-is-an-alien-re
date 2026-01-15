#ifndef HANDLER_H
#define HANDLER_H

#include "Parser.h"

class SC_Message;

// Handler - Base class for all game handlers (modules)
// Handlers are scene/screen managers that process messages and render content
// 
// vtable layout (extends Parser):
//   +0x00: Parser::~Parser (base destructor)
//   +0x04: Parser::LBLParse
//   +0x08: Parser::OnProcessStart
//   +0x0C: Handler scalar deleting destructor
//   +0x10: Init (virtual) - called when entering handler
//   +0x14: HandleMessage (virtual) - process messages
//   +0x18: Update (virtual) - called on system messages
//   +0x1C: Draw (virtual) - render handler content
//   +0x20: Exit (virtual) - called when leaving handler
//   +0x24: OnInput (virtual) - input handling
//
// Memory layout:
//   0x00-0x87: Parser base class (size 0x88)
//   0x88: handlerId - identifies the handler type (1, 2, 4, 5, etc.)
//   0x8C+: Handler-specific fields
class Handler : public Parser {
public:
    Handler();
    virtual ~Handler();
    
    // Virtual methods - these form the handler interface
    // Override in derived classes to implement handler behavior
    virtual void Init(SC_Message* msg);
    virtual int HandleMessage(SC_Message* msg);
    virtual void Update(SC_Message* msg);
    virtual void Draw(int param1, int param2);
    virtual int Exit(SC_Message* msg);
    virtual void OnInput();
    
    // Copy command/data from message to handler fields
    void CopyCommandData(SC_Message* msg);
    
    // Write handler address to message
    int WriteMessageAddress(SC_Message* msg);

    // Common handler fields
    int handlerId;      // 0x88 - handler type identifier
    int field_8C;       // 0x8C - often sourceAddress
    int field_90;       // 0x90 - often command
    int field_94;       // 0x94 - often data
    int field_98;       // 0x98
    int field_9C;       // 0x9C
};

#endif // HANDLER_H
