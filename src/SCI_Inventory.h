#ifndef SCI_INVENTORY_H
#define SCI_INVENTORY_H

#include "IconBar.h"
#include "InvSlotItem.h"

class Sprite;
class Sample;
class Palette;
class T_MenuHotspot;
class T_MenuButton;

// 24-byte embedded object, ctor 0x43E560, dtor 0x43E580
struct InvPanel {
    int item1;          // +0x00 — first required item ID
    int item2;          // +0x04 — second required item ID
    int item3;          // +0x08 — third required item ID
    int resultItem;     // +0x0C — combined result item ID
    int actionId;       // +0x10 — action to trigger on combine
    int gameStateIdx;   // +0x14 — game state index to check
    InvPanel();
    ~InvPanel();
};

// SCI_Inventory - Inventory screen (case 30/0x1E)
// Constructor: 0x43E360, Destructor: 0x43E5B0
// Size: 0x198
// Vtable: 0x461888
// Extends IconBar
// References: "inventor.mis", "[INVENTORY]"
class SCI_Inventory : public IconBar {
public:
    SCI_Inventory();
    ~SCI_Inventory();

    // Virtual overrides
    virtual int LBLParse(char* line);           // 0x43FDD0
    virtual void Init(SC_MessageParser* msg);         // 0x43E920
    virtual int AddMessage(SC_MessageParser* msg);    // 0x43EA60
    virtual int ShutDown(SC_MessageParser* msg);      // 0x43E990
    virtual void Update(int param1, int param2); // 0x43EE20
    virtual int Exit(SC_MessageParser* msg);          // 0x43EFD0
    virtual void Serialize(void* param);         // 0x43F9B0

    // Helper methods
    void DisplayPanels(int param);              // 0x43F840
    void ProcessInventory();                    // 0x43F420
    void* FindItem(int itemID);                 // 0x43F490
    int* FindItemInList(int itemID);            // 0x43F7F0

    Rect slots[8];         // 0xA8-0x127 — inventory slot hit-test rectangles
    int selectedSlot;              // 0x128 — currently selected slot index (-1 = none)
    int scrollOffset;              // 0x12C — scroll position in item list
    InvPanel panels[3];            // 0x130-0x177 — display panels
    Sprite* bgSprite;              // 0x178 — background sprite
    Sample* clickSound;            // 0x17C — item click sound effect
    Palette* palette;              // 0x180 — inventory screen palette
    void* itemPool;                // 0x184 — TimedEventPool* (inlined ctor; accessed via raw int* throughout)
    T_MenuButton* putBackButton;    // 0x188 — "put back" button (T_MenuButton, 0xA8)
    T_MenuButton* useButton;        // 0x18C — "use item" button
    T_MenuButton* scrollDownBtn;    // 0x190 — scroll down button (msg 0x12)
    T_MenuButton* scrollUpBtn;      // 0x194 — scroll up button (msg 0x11)
};

#endif // SCI_INVENTORY_H
