#ifndef HANDLER1_H
#define HANDLER1_H

#include "Handler.h"
#include "Palette.h"
#include "Sprite.h"

// Handler1 - Command 1 Handler (Demo Intro Screen)
// Size: 0xA8
// vtable: 0x4310b8
class Handler1 : public Handler {
public:
    Handler1();
    ~Handler1();
    
    // Virtual method overrides
    virtual void Init(SC_Message* msg);
    virtual int AddMessage(SC_Message* msg);
    virtual int ShutDown(SC_Message* msg);
    virtual void Update(int param1, int param2);
    virtual int Exit(SC_Message* msg);

    // Handler1-specific fields (targetAddress through field_9C are in Handler base)
    Palette* palette;   // 0xA0
    Sprite* sprite;     // 0xA4
};

#endif // HANDLER1_H

