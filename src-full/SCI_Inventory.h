#ifndef SCI_INVENTORY_H
#define SCI_INVENTORY_H

#include "IconBar.h"

// 16-byte embedded object, ctor 0x40D0C0, dtor 0x401130
struct InvSlotItem {
    int field_0;
    int field_4;
    int field_8;
    int field_C;
    InvSlotItem();
    ~InvSlotItem();
};

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
    virtual void OnInput(void* param);           // 0x43F9B0

    // Helper methods
    void DisplayPanels(int param);              // 0x43F840
    void ProcessInventory();                    // 0x43F420
    void* FindItem(int itemID);                 // 0x43F490
    int* FindItemInList(int itemID);            // 0x43F7F0

    InvSlotItem field_A8[8];  // 0xA8-0x127 (128 bytes)
    int field_128;            // 0x128
    int field_12C;            // 0x12C
    InvPanel field_130[3];    // 0x130-0x177 (72 bytes)
    int field_178;            // 0x178 - Sprite*
    int field_17C;            // 0x17C
    int field_180;            // 0x180 - Palette*
    int field_184;            // 0x184 - inventory list
    int field_188;            // 0x188
    int field_18C;            // 0x18C
    int field_190;            // 0x190
    int field_194;            // 0x194
};

#endif // SCI_INVENTORY_H
