#ifndef HANDLER6_H
#define HANDLER6_H

#include "Handler.h"
#include "Hotspot.h"
#include "Palette.h"
#include "SpriteList.h"

class SC_Message;

// Handler6 - Command 6 Handler (Search Screen Handler)
// Full game: Size 0xA8 (same as Handler, no additional fields)
// vtable: 0x4616B0
// Constructor: 0x4327C0 - just calls Parser::Parser() + inline Handler init
// In the full game, Handler6 extends Handler directly (not IconBar)
class Handler6 : public Handler {
public:
    Handler6();
    ~Handler6();

    // Virtual method overrides
    virtual void Init(SC_Message* msg);
    virtual int AddMessage(SC_Message* msg);
    virtual int ShutDown(SC_Message* msg);
    virtual void Update(int param1, int param2);
    virtual int Exit(SC_Message* msg);
    virtual int LBLParse(char* line);

    // Helper methods (DEMO ONLY)
    int CountActiveHotspots();
    int FindClickedHotspot();

    // DEMO ONLY - these were IconBar methods in the demo
    void InitIconBar(SC_Message* msg);
    void CleanupIconBar(SC_Message* msg);
    int CheckButtonClick(SC_Message* msg);

    // Handler6-specific fields (DEMO ONLY - full game has no extra fields)
    Palette* palette;          // 0xA8 (was 0x600 in demo)
    SpriteList* ambient;       // 0xAC
    Hotspot* hotspots[10];     // 0xB0-0xD7
    Hotspot* currentHotspot;   // 0xD8
    int hotspotCount;          // 0xDC
    int activeHotspots;        // 0xE0
    int counter;               // 0xE4
};

#endif // HANDLER6_H
