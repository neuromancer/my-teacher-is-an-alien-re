#ifndef SC_SAVELOAD_H
#define SC_SAVELOAD_H

#include "Handler.h"
#include "Timer.h"

class Sprite;
class Palette;
class T_MenuHotspot;

// SC_SaveLoad - Save/load dialog handler (case 47/0x2F)
// Constructor: 0x421C40
// Size: 0xD8
// Vtable: 0x4613E8
class SC_SaveLoad : public Handler {
public:
    SC_SaveLoad();
    ~SC_SaveLoad();

    Timer timer;                // 0xA8 (0x14 bytes)
    int field_BC;               // 0xBC
    int field_C0;               // 0xC0
    Palette* palette;           // 0xC4
    Sprite* sprite;             // 0xC8
    T_MenuHotspot* btnYes;      // 0xCC
    T_MenuHotspot* btnNo;       // 0xD0
    T_MenuHotspot* btnCancel;   // 0xD4
};

#endif // SC_SAVELOAD_H
