#ifndef HANDLER12_H
#define HANDLER12_H

#include "Handler.h"
#include "Timer.h"

// Handler12 - Handler class with ID 12 (Text Display Handler)
// Size: 0xB8 bytes
// vtable: 0x431028
// Layout:
//   0x00-0x9F: Handler base class
//   0xA0-0xB3: Timer (size 0x14)
//   0xB4-0xB7: padding
class Handler12 : public Handler {
public:
    Handler12();
    ~Handler12();

    // Virtual method overrides
    virtual void Init(SC_Message* msg);
    virtual int HandleMessage(SC_Message* msg);
    virtual void Update(SC_Message* msg);
    virtual void Draw();
    virtual void Exit();

    // Handler12-specific fields (handlerId through field_9C are in Handler base)
    Timer timer;        // 0xA0 - 0xB3 (size 0x14)
    
    // Padding to 0xB8 total
    int field_0xB4;     // 0xB4
};

#endif // HANDLER12_H

