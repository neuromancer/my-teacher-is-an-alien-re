#ifndef SC_SAVELOAD_H
#define SC_SAVELOAD_H

#include "Handler.h"
#include "Timer.h"

class Sprite;
class Palette;
class T_MenuHotspot;
class T_MenuButton;

// SC_SaveLoad - Save/load dialog handler (case 47/0x2F)
// Constructor: 0x421C40
// Size: 0xD8
// Vtable: 0x4613E8
class SC_SaveLoad : public Handler {
public:
    SC_SaveLoad();
    ~SC_SaveLoad();

    void Init(SC_MessageParser* msg);              // 0x4221A0
    void ShutDown(SC_MessageParser* msg);            // 0x422280
    void Update(int param1, int param2);     // 0x4222F0
    int AddMessage(SC_MessageParser* msg);         // 0x4223F0
    int Exit(SC_MessageParser* msg);               // 0x4226C0

    Timer timer;                // 0xA8 (0x14 bytes)
    int returnCommand;               // 0xBC
    int returnData;               // 0xC0
    Palette* palette;           // 0xC4
    Sprite* sprite;             // 0xC8
    T_MenuButton* btnYes;       // 0xCC
    T_MenuButton* btnNo;        // 0xD0
    T_MenuButton* btnCancel;    // 0xD4
};

#endif // SC_SAVELOAD_H
