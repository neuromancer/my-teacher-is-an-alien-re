#ifndef SCI_ICONBARMODULE_H
#define SCI_ICONBARMODULE_H

#include "IconBar.h"
#include "Sprite.h"
#include "SpriteAction.h"
#include "InvSlotItem.h"

class SC_MessageParser;
class TimeOut;
class MMPlayer;
class Palette;
class T_Hotspot;
struct Queue;

// SCI_IconBarModule - Icon bar module handler (case 32/0x20)
// Constructor: 0x401000
// Size: 0x130, Vtable: 0x461000
// Extends IconBar
class SCI_IconBarModule : public IconBar {
public:
    SCI_IconBarModule();
    ~SCI_IconBarModule();

    void Init(SC_MessageParser* msg);            // 0x4013E0
    void ShutDown(SC_MessageParser* msg);         // 0x401CF0
    void Update(int param1, int param2);   // 0x401D80
    int Exit(SC_MessageParser* msg);             // 0x401FF0
    int AddMessage(SC_MessageParser* msg);       // 0x401E40
    int FindClickedIcon(int x, int y);     // 0x4020A0
    void UpdateCursor();                   // 0x402100
    int LBLParse(char* line);              // 0x4022B0
    void Serialize(void* param);           // 0x4021C0

    int exitTarget;               // 0xA8
    int mode;                   // 0xAC — 0=normal icon bar, nonzero=alternate mode
    int currentRoom;            // 0xB0
    int field_B4;               // 0xB4
    int hasBoundaryRect;        // 0xB8 — enables boundary rect hit test
    int staticSceneFound;               // 0xBC
    int roomInitialized;        // 0xC0
    int skipActionsCount;       // 0xC4 — if nonzero, skip NUM_ACTIONS increment
    SlimeDim videoDim;          // 0xC8 — screen dimensions for SetVideoRes
    Rect boundRect;             // 0xD0 — boundary rect
    TimeOut* timeout;           // 0xE0 — inactivity timeout
    Palette* field_E4;          // 0xE4
    MMPlayer* mmPlayer;         // 0xE8 — background animation/cursor
    T_Hotspot* icons[15];       // 0xEC-0x127 — hotspot pointers
    Queue* field_128;           // 0x128
    int field_12C;              // 0x12C
};

#endif // SCI_ICONBARMODULE_H
