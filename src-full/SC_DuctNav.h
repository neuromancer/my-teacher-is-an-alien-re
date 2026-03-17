#ifndef SC_DUCTNAV_H
#define SC_DUCTNAV_H

#include "Handler.h"

// SC_DuctNav - Duct navigation handler (case 46/0x2E)
// Constructor: 0x43AF10
// Size: 0x258
// Vtable: 0x461850
// References: "SC_DuctNav", "DUCT_SWITCH", "SaveGame__.sav"
class Sprite;
class Palette;

class SC_DuctNav : public Handler {
public:
    int field_0xA8[53];        // 0xA8-0x17B
    int slotRects[10 * 4];    // 0x17C-0x21B (save slot hitbox rects)
    void* cancelBtn;           // 0x21C
    void* field_0x220;         // 0x220
    void* field_0x224;         // 0x224
    void* field_0x228;         // 0x228
    void* deleteBtn;           // 0x22C
    void* scrollUpBtn;         // 0x230
    void* scrollDownBtn;       // 0x234
    Sprite* editFocusSprite;   // 0x238
    Sprite* choiceFocusSprite; // 0x23C
    Palette* fontPalette;      // 0x240
    Sprite* menuSprite;        // 0x244
    int field_0x248[4];        // 0x248-0x257

    SC_DuctNav();                                  // 0x43AF10
    virtual ~SC_DuctNav();                         // 0x43B030 (sdtor)

    virtual int LBLParse(char* line);              // [0] 0x43D5B0
    virtual void OnProcessStart();                 // [1] 0x43D250
    virtual void Init(SC_Message* msg);            // [4] 0x43B0F0
    virtual int AddMessage(SC_Message* msg);       // [5] 0x43BB10
    virtual int ShutDown(SC_Message* msg);         // [6] 0x43B2C0
    virtual void Update(int p1, int p2);           // [7] 0x43B7E0
    virtual int Exit(SC_Message* msg);             // [8] 0x43C100
};

#endif // SC_DUCTNAV_H
