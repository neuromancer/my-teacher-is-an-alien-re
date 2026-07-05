#ifndef SC_DUCTNAV_H
#define SC_DUCTNAV_H

#include "Handler.h"
#include "Timer.h"
#include "GlyphRect.h"

// SC_DuctNav - Duct navigation / save/load handler (case 46/0x2E)
// Constructor: 0x43AF10
// Size: 0x258
// Vtable: 0x461850
// References: "SC_DuctNav", "DUCT_SWITCH", "SaveGame__.sav"
class Sprite;
class Palette;
class T_MenuButton;
class TextInput;
struct SaveFilePool;

class SC_DuctNav : public Handler {
public:
    Timer timer;                   // 0xA8 (0x14 bytes)
    int scrollOffset;              // 0xBC
    int selectedRow;               // 0xC0
    char searchPattern[128];       // 0xC4-0x143 (save file search pattern)
    char saveFilename[56];         // 0x144-0x17B (current save filename)
    GlyphRect slotRects[10];       // 0x17C-0x21B (save slot hitbox rects, 160 bytes)
    T_MenuButton* cancelBtn;       // 0x21C
    T_MenuButton* saveBtn;         // 0x220
    T_MenuButton* loadBtn;         // 0x224
    T_MenuButton* overwriteBtn;    // 0x228
    T_MenuButton* deleteBtn;       // 0x22C
    T_MenuButton* scrollUpBtn;     // 0x230
    T_MenuButton* scrollDownBtn;   // 0x234
    Sprite* editFocusSprite;       // 0x238
    Sprite* choiceFocusSprite;     // 0x23C
    Palette* fontPalette;          // 0x240
    Sprite* menuSprite;            // 0x244
    SaveFilePool* saveFileList;    // 0x248 (linked list pool for save file entries)
    TextInput* field_0x24C;        // 0x24C
    void* fileArchive;             // 0x250 (FileArchive* - 0x48 byte struct)
    int field_0x254;               // 0x254

    SC_DuctNav();                                  // 0x43AF10
    virtual ~SC_DuctNav();                         // 0x43B030 (sdtor)

    virtual int LBLParse(char* line);              // [0] 0x43D5B0
    virtual void OnProcessStart();                 // [1] 0x43D250
    virtual void Init(SC_MessageParser* msg);            // [4] 0x43B0F0
    virtual int AddMessage(SC_MessageParser* msg);       // [5] 0x43BB10
    virtual void ShutDown(SC_MessageParser* msg);         // [6] 0x43B2C0
    virtual void Update(int p1, int p2);           // [7] 0x43B7E0
    virtual int Exit(SC_MessageParser* msg);             // [8] 0x43C100

    void ReadSaveFiles(char* pattern);             // 0x43C5D0
    void WriteSaveFile();                          // 0x43C8B0
    void LoadSaveFile();                           // 0x43CA50
};

#endif // SC_DUCTNAV_H
