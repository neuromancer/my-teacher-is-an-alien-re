#ifndef HANDLER8_H
#define HANDLER8_H

#include "Handler.h"

class SC_Message;

// Handler8 - Command 8 Handler
// Not instantiated in the full game (no case in CreateHandler).
// Constructor/destructor exist in the binary as COMDATs.
// Layout kept for field compatibility.
class Handler8 : public Handler {
public:
    Handler8();
    ~Handler8();

    // Handler8-specific fields
    SC_Message* message;    // 0xA0
    int field_A4;           // 0xA4
};

#endif // HANDLER8_H
