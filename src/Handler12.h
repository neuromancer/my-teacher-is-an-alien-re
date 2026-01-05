#ifndef HANDLER12_H
#define HANDLER12_H

#include "Parser.h"
#include "Timer.h"

// Handler12 - Handler class with ID 12
// Extends Parser and adds handler-specific fields
// Size: 0xB8 bytes
// Layout:
//   0x00-0x87: Parser base class (size 0x88)
//   0x88-0x9F: 6 dwords (handlerId at 0x88, set to 12)
//   0xA0-0xB3: Timer (size 0x14)
//   0xB4-0xB7: padding
class Handler12 : public Parser {
public:
    Handler12();

    // Fields at 0x88-0x9F (6 dwords)
    int handlerId;      // 0x88 - set to 12
    int field_0x8C;     // 0x8C
    int field_0x90;     // 0x90
    int field_0x94;     // 0x94
    int field_0x98;     // 0x98
    int field_0x9C;     // 0x9C
    
    // Timer at 0xA0
    Timer timer;        // 0xA0 - 0xB3 (size 0x14)
    
    // Padding to 0xB8 total
    int field_0xB4;     // 0xB4
};

#endif // HANDLER12_H
