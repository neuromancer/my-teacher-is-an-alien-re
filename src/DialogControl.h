#ifndef DIALOGCONTROL_H
#define DIALOGCONTROL_H

#include "Parser.h"
#include "GlyphRect.h"

class Sprite;
class MouseControl;

// DialogControl - Dialog UI control with multiple mouse areas
// Size: 0x140 bytes
// Constructor: 0x409230
// Destructor: 0x4092E0
// vtable: 0x4311b8
// Inherits from Parser (0x88 bytes)
//
// Layout:
//   0x00-0x87: Parser base class
//   0x88: Sprite* (destroyed via Sprite destructor)
//   0x8c-0x94: MouseControl* array[3]
//   0x98-0x12b: additional fields
//   0x12c-0x13b: embedded GlyphRect
//   0x13c-0x13f: remaining field
class DialogControl : public Parser {
public:
    DialogControl();
    ~DialogControl();

    Sprite* sprite;              // 0x88
    MouseControl* mouseArea1;    // 0x8c
    MouseControl* mouseArea2;    // 0x90
    MouseControl* mouseArea3;    // 0x94
    int field_98;                // 0x98
    int field_9c;                // 0x9c
    int field_a0;                // 0xa0
    int field_a4;                // 0xa4
    int field_a8;                // 0xa8
    int field_ac;                // 0xac
    int field_b0;                // 0xb0
    int field_b4;                // 0xb4
    int field_b8;                // 0xb8
    int field_bc;                // 0xbc
    int field_c0;                // 0xc0
    int field_c4;                // 0xc4
    int field_c8;                // 0xc8
    int field_cc;                // 0xcc
    int field_d0;                // 0xd0
    int field_d4;                // 0xd4
    int field_d8;                // 0xd8
    int field_dc;                // 0xdc
    int field_e0;                // 0xe0
    int field_e4;                // 0xe4
    int field_e8;                // 0xe8
    int field_ec;                // 0xec
    int field_f0;                // 0xf0
    int field_f4;                // 0xf4
    int field_f8;                // 0xf8
    int field_fc;                // 0xfc
    int field_100;               // 0x100
    int field_104;               // 0x104
    int field_108;               // 0x108
    int field_10c;               // 0x10c
    int field_110;               // 0x110
    int field_114;               // 0x114
    int field_118;               // 0x118 - set to 1 in constructor
    int field_11c;               // 0x11c - set to 1 in constructor
    int field_120;               // 0x120 - set to -1 in constructor
    int field_124;               // 0x124 - set to -1 in constructor
    int field_128;               // 0x128
    GlyphRect glyphRect;         // 0x12c-0x13b (embedded, 16 bytes)
    int field_13c;               // 0x13c
};

#endif // DIALOGCONTROL_H
