#ifndef HANDLER8_H
#define HANDLER8_H

#include "Handler.h"

class SC_Message;

// Handler8 - Command 8 Handler
// Not instantiated in the full game (no case in CreateHandler).
// Constructor/destructor exist in the binary as COMDATs.
// Size: 0xA8 (same as Handler — no additional fields)
// Uses Handler::field_A0 as SC_Message* and Handler::field_A4 directly.
class Handler8 : public Handler {
public:
    Handler8();
    ~Handler8();
};

#endif // HANDLER8_H
