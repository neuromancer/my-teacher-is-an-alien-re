#ifndef INVSLOTITEM_H
#define INVSLOTITEM_H

// 16-byte embedded object, ctor 0x40D0C0, dtor 0x401130
struct InvSlotItem {
    int field_0;
    int field_4;
    int field_8;
    int field_C;
    InvSlotItem() { field_0 = 0; field_4 = 0; field_8 = 0; field_C = 0; }
    ~InvSlotItem();
};

#endif // INVSLOTITEM_H
