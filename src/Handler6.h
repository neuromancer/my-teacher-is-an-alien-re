#ifndef HANDLER6_H
#define HANDLER6_H

#include "Handler.h"
#include "Hotspot.h"
#include "Palette.h"
#include "SpriteList.h"

struct SC_Message;

// Handler6 - Command 6 Handler (Search Screen Handler)
// Size: 0x640 bytes
// vtable: 0x4310e8
// Inherits from Handler for consistency with other Handler classes
// Layout:
//   0x00-0x9F: Handler base class (Parser + Handler fields)
//   0xA0-0x5FF: Parent class data (from FUN_402730)
//   0x600: Palette* pointer
//   0x604: SpriteList* ambient; // Inferred to be SpriteList* (FUN_0041f800 is SpriteList::DoAll)
//   0x608-0x62F: Hotspot* array[10]
//   0x630: Hotspot* current hotspot
//   0x634: int hotspot count
//   0x638: int active hotspots
//   0x63C: int counter
class Handler6 : public Handler {
public:
    Handler6();
    ~Handler6();

    // Virtual method overrides
    virtual void Init(SC_Message* msg);
    virtual int HandleMessage(SC_Message* msg);
    virtual void Update(SC_Message* msg);
    virtual void Draw(int param1, int param2);  // FUN_404890 - takes 2 params
    virtual int Exit(SC_Message* msg);          // FUN_404870 - takes 1 param, returns int

    // Helper methods
    int CountActiveHotspots();
    int FindClickedHotspot();

    // Parent class data 0xA0-0x5FF (set by FUN_402730)
    char parentData[0x560];  // 0xA0 to 0x5FF

    // Handler6-specific fields starting at 0x600
    Palette* palette;          // 0x600
    SpriteList* ambient;          // 0x604
    Hotspot* hotspots[10];     // 0x608-0x62F
    Hotspot* currentHotspot;   // 0x630
    int hotspotCount;       // 0x634
    int activeHotspots;     // 0x638
    int counter;            // 0x63C
};

#endif // HANDLER6_H




