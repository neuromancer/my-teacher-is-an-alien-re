#ifndef HANDLER5_H
#define HANDLER5_H

#include "Handler.h"
#include "Timer.h"
#include "Sprite.h"

// Handler5 - Command 5 Handler (Game State Handler)
// Size: 0xF0
// vtable: 0x4312e8
// Layout:
//   0x00-0x9F: Handler base class
//   0xA0-0xB3: Timer (size 0x14)
//   0xB4: Sprite* (gamestat.smk)
//   0xB8-0xEC: various fields/rectangles
class Handler5 : public Handler {
public:
    Handler5();
    ~Handler5();

    // Virtual method overrides
    virtual void Init(SC_Message* msg);
    virtual int HandleMessage(SC_Message* msg);
    virtual int Update(SC_Message* msg);
    virtual void Draw(int param1, int param2);
    virtual int Exit(SC_Message* msg);

    // Handler5-specific fields (handlerId through field_9C are in Handler base)
    Timer timer;        // 0xA0 - 0xB3 (size 0x14)
    
    Sprite* sprite;     // 0xB4
    
    // Rectangle fields 0xB8-0xDC
    int field_B8;       // 0xB8
    int field_BC;       // 0xBC
    int field_C0;       // 0xC0
    int field_C4;       // 0xC4
    int field_C8;       // 0xC8
    int field_CC;       // 0xCC
    int field_D0;       // 0xD0
    int field_D4;       // 0xD4
    int field_D8;       // 0xD8
    int field_DC;       // 0xDC
    
    // Additional fields
    int field_E0;       // 0xE0
    int field_E4;       // 0xE4
    int field_E8;       // 0xE8
    int field_EC;       // 0xEC
};

#endif // HANDLER5_H

