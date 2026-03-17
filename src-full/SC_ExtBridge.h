#ifndef SC_EXTBRIDGE_H
#define SC_EXTBRIDGE_H

#include "Handler.h"
#include "SpriteAction.h"

// SC_ExtBridge - Exterior bridge scene handler (case 65/0x41)
// Constructor: 0x4399E0
// Size: 0xC0
// Vtable: 0x4617D8
class SpriteAction;
class Palette;

class SC_ExtBridge : public Handler {
public:
    SpriteAction* actionMsg;  // 0xA8
    int engine;               // 0xAC — EngineA* (virtual delete)
    Palette* palette;         // 0xB0
    SlimeDim dim;             // 0xB4 (8 bytes)
    int bgSoundId;            // 0xBC

    SC_ExtBridge();                                // 0x4399E0
    virtual ~SC_ExtBridge();                       // 0x439AA0 (sdtor)

    virtual int LBLParse(char* line);              // [0] 0x43A290
    virtual void Init(SC_Message* msg);            // [4] 0x439BB0
    virtual int AddMessage(SC_Message* msg);       // [5] 0x43A0C0
    virtual int ShutDown(SC_Message* msg);         // [6] 0x439F30
    virtual void Update(int p1, int p2);           // [7] 0x43A030
    virtual int Exit(SC_Message* msg);             // [8] 0x43A1D0

    void ProcessEscape();                          // 0x43A210
};

#endif // SC_EXTBRIDGE_H
