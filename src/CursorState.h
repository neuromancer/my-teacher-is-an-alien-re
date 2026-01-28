#ifndef CURSORSTATE_H
#define CURSORSTATE_H

// CursorState - Stores cursor state data
// Size: 0x24 bytes (9 dwords)
// Constructor: 0x416B80
class CursorState {
public:
    int field_0x00; // 0x00
    int field_0x04; // 0x04
    int field_0x08; // 0x08
    int field_0x0c; // 0x0c
    int field_0x10; // 0x10
    int field_0x14; // 0x14
    int field_0x18; // 0x18
    int field_0x1c; // 0x1c
    int field_0x20; // 0x20

    CursorState(); // 0x416B80
};

#endif // CURSORSTATE_H
