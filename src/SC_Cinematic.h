#ifndef SC_CINEMATIC_H
#define SC_CINEMATIC_H

#include "Handler.h"
#include <string.h>

class Animation;
class Palette;
class SpriteAction;

// SC_Cinematic - Cinematic/cutscene player (case 3)
// Constructor: 0x42FBD0
// Destructor: 0x42FC90
// Size: 0xD8
// Vtable: 0x461648
// References: "CINE_SUMMARY"
class SC_Cinematic : public Handler {
public:
    SC_Cinematic();
    ~SC_Cinematic();

    void Init(SC_MessageParser* msg);         // 0x42FCF0
    int ShutDown(SC_MessageParser* msg);      // 0x4300D0
    void Update(int param1, int param2);// 0x430350
    int AddMessage(SC_MessageParser* msg);    // 0x4306A0
    int Exit(SC_MessageParser* msg);          // 0x4306E0
    void EndCinematic();                // 0x430730

    Palette* palette;   // 0xA8
    Animation* animation; // 0xAC
    int savedZBState;   // 0xB0 — saved ZBufferManager state
    int savedRenderCtx; // 0xB4 — saved render context from GameEngine
    int flags;          // 0xB8 — playback flags (bit0=no-skip, bit1=no-palette, bit2=blank-after, etc.)
    int startX;         // 0xBC — start position X
    int startY;         // 0xC0 — start position Y
    int volume;         // 0xC4 — audio volume (default 100)
    int soundParam;     // 0xC8 — sound parameter
    int waitForInput;   // 0xCC — 1 = waiting for click after last frame
    SpriteAction* pendingAction; // 0xD0 — action to execute on end
    int field_D4;       // 0xD4
};

#endif // SC_CINEMATIC_H
