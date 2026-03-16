#ifndef SCI_INVENTORY_H
#define SCI_INVENTORY_H

#include "IconBar.h"
#include "InvSlotItem.h"

class Sprite;
class Sample;
class Palette;
class T_MenuHotspot;

// 24-byte embedded object, ctor 0x43E560, dtor 0x43E580
struct InvPanel {
    int field_0;
    int field_4;
    int field_8;
    int field_C;
    int field_10;
    int field_14;
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
    virtual void Init(SC_Message* msg);         // 0x43E920
    virtual int AddMessage(SC_Message* msg);    // 0x43EA60
    virtual int ShutDown(SC_Message* msg);      // 0x43E990
    virtual void Update(int param1, int param2); // 0x43EE20
    virtual int Exit(SC_Message* msg);          // 0x43EFD0
    virtual void Serialize(void* param);         // 0x43F9B0

    // Helper methods
    void DisplayPanels(int param);              // 0x43F840
    void ProcessInventory();                    // 0x43F420
    void* FindItem(int itemID);                 // 0x43F490
    int* FindItemInList(int itemID);            // 0x43F7F0

    InvSlotItem slots[8];         // 0xA8-0x127 — inventory slot hit-test rectangles
    int selectedSlot;              // 0x128 — currently selected slot index (-1 = none)
    int scrollOffset;              // 0x12C — scroll position in item list
    InvPanel panels[3];            // 0x130-0x177 — display panels
    Sprite* bgSprite;              // 0x178 — background sprite
    Sample* clickSound;            // 0x17C — item click sound effect
    Palette* palette;              // 0x180 — inventory screen palette
    void* itemPool;                // 0x184 — TimedEventPool* (inlined ctor; accessed via raw int* throughout)
    T_MenuHotspot* putBackButton;    // 0x188 — "put back" button (unknown class, has Sprite* at +0x90, rect at +0x94)
    T_MenuHotspot* useButton;        // 0x18C — "use item" button
    T_MenuHotspot* scrollDownBtn;    // 0x190 — scroll down button (msg 0x12)
    T_MenuHotspot* scrollUpBtn;      // 0x194 — scroll up button (msg 0x11)
};

#endif // SCI_INVENTORY_H
