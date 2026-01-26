#ifndef SC_SOUND_H
#define SC_SOUND_H

#include "Handler.h"
#include "Timer.h"
#include "SC_OnScreenMessage.h"

class SoundItem;

// SC_Sound - Sound handler class with ID 14
// Size: 0xB8 bytes
// vtable: 0x431240
// Constructor: 0x40B7E0
// Destructor: 0x40B910
//
// Layout:
//   0x00-0x87: Parser base class (via Handler)
//   0x88-0x9F: Handler fields (handlerId + 5 fields, zeroed)
//   0xA0-0xB3: Timer timer (size 0x14)
//   0xB4: MessageList* list
class SC_Sound : public Handler {
public:
    SC_Sound();
    ~SC_Sound();

    // Virtual method overrides
    virtual void Init(SC_Message* msg);
    virtual int AddMessage(SC_Message* msg);
    virtual int ShutDown(SC_Message* msg);
    virtual void Update(int param1, int param2);
    virtual int Exit(SC_Message* msg);

    // Find or create SoundItem in the list
    SoundItem* FindOrCreateSound(int soundId);

    // SC_Sound-specific fields starting at 0xA0
    Timer timer;            // 0xA0 - 0xB3 (size 0x14)
    MessageList* list;      // 0xB4
};

#endif // SC_SOUND_H
