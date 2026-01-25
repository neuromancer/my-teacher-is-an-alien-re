#ifndef HANDLER6_H
#define HANDLER6_H

#include "IconBar.h"
#include "Hotspot.h"
#include "Palette.h"
#include "SpriteList.h"

class SC_Message;

// Handler6 - Command 6 Handler (Search Screen Handler)
// Size: 0x640 bytes
// vtable: 0x4310e8
// Inherits from IconBar (which inherits from Handler)
// Layout:
//   0x00-0x5FF: IconBar base class
//   0x600: Palette* pointer
//   0x604: SpriteList* ambient
//   0x608-0x62F: Hotspot* array[10]
//   0x630: Hotspot* current hotspot
//   0x634: int hotspot count
//   0x638: int active hotspots
//   0x63C: int counter
class Handler6 : public IconBar {
public:
    Handler6();
    ~Handler6();

    // Virtual method overrides
    virtual void Init(SC_Message* msg);
    virtual int HandleMessage(SC_Message* msg);
    virtual int Update(SC_Message* msg);
    virtual void Draw(int param1, int param2);
    virtual int Exit(SC_Message* msg);
    virtual int LBLParse(char* line);

    // Helper methods
    int CountActiveHotspots();
    int FindClickedHotspot();

    // Handler6-specific fields starting at 0x600
    Palette* palette;          // 0x600
    SpriteList* ambient;       // 0x604
    Hotspot* hotspots[10];     // 0x608-0x62F
    Hotspot* currentHotspot;   // 0x630
    int hotspotCount;          // 0x634
    int activeHotspots;        // 0x638
    int counter;               // 0x63C
};

#endif // HANDLER6_H




