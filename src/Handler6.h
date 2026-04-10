#ifndef HANDLER6_H
#define HANDLER6_H

#include "Handler.h"

// Handler6 - Not instantiated in the full game (no case in CreateHandler).
// Constructor exists as COMDAT at 0x4327C0.
// Size: 0xA8 (same as Handler, no additional fields)
class Handler6 : public Handler {
public:
    Handler6();   // 0x4327C0
    ~Handler6();
};

#endif // HANDLER6_H
