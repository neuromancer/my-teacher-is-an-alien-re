#ifndef HANDLER16_H
#define HANDLER16_H

#include "Handler.h"

// Forward declaration
class Parser;
class Engine;

// Handler16 - SC_Combat1 handler class with ID 16
// Size: 0xF8 bytes
// vtable: 0x431318
// Constructor: 0x410650
// Destructor: 0x410760
//
// Layout:
//   0x00-0x87: Parser base class (via Handler)
//   0x88: handlerId = 16
//   0x8C-0x9F: Handler fields
//   0x98: isInitialized (state flag)
//   0xA0: savedScreenX
//   0xA4: savedScreenY
//   0xA8: screenWidth (default 320 = 0x140)
//   0xAC: screenHeight (default 200 = 0xC8)
//   0xB0-0xEF: filename buffer (64 bytes)
//   0xF0: savedRendererState
//   0xF4: scriptParser pointer
class Handler16 : public Handler {
public:
    Handler16();
    ~Handler16();

    // Virtual method overrides
    virtual void Init(SC_Message* msg);
    virtual int HandleMessage(SC_Message* msg);
    virtual int Update(SC_Message* msg);
    virtual void Draw(int param1, int param2);
    virtual int Exit(SC_Message* msg);
    virtual int LBLParse(char* line);

    // Helper method
    void ProcessMessage();

    // Handler16-specific fields starting at 0xA0
    // Note: field_98 from Handler is used as isInitialized flag
    int savedScreenX;       // 0xA0
    int savedScreenY;       // 0xA4
    int screenWidth;        // 0xA8 - default 320
    int screenHeight;       // 0xAC - default 200
    char filename[64];      // 0xB0 - 0xEF
    int savedRendererState; // 0xF0
    Parser* scriptParser;   // 0xF4
};

#endif // HANDLER16_H
