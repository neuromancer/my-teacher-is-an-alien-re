#ifndef HANDLER2_H
#define HANDLER2_H

#include "Handler.h"
#include "Palette.h"
#include "Sprite.h"
#include "Sample.h"

// Handler2 - Command 2 Handler (Teacher Screen to Leave)
// Size: 0xB0
// vtable: 0x4312b8
// Layout:
//   0x00-0x9F: Handler base class
//   0xA0: Palette*
//   0xA4: Sprite*
//   0xA8: Sample*
//   0xAC: field_AC
class Handler2 : public Handler {
public:
    Handler2();
    ~Handler2();

    // Virtual method overrides
    virtual void Init(SC_Message* msg);
    virtual int HandleMessage(SC_Message* msg);
    virtual void Update(SC_Message* msg);
    virtual void Draw(int param1, int param2);
    virtual int Exit(SC_Message* msg);

    // Handler2-specific fields (handlerId through field_9C are in Handler base)
    Palette* palette;   // 0xA0
    Sprite* sprite;     // 0xA4
    Sample* sample;     // 0xA8
    int field_AC;       // 0xAC
};

#endif // HANDLER2_H

