#ifndef GLYPHFONT_H
#define GLYPHFONT_H

// GlyphFont - bitmap font rendering class (0x78 bytes)
// Constructor: 0x43A6E0, Destructor: 0x43A770
// LoadFont: 0x43A830, InitGlyphTable: 0x43AA20
// IsValidChar: 0x43ABA0, DrawChar: 0x43ABD0, GetTextWidth: 0x43AD50
class GlyphFont {
public:
    void* bitmapData;    // 0x00 - bitmap surface pointer
    int firstChar;       // 0x04 - first character code in font
    int field_08;        // 0x08
    int field_0C;        // 0x0C
    char field_10;       // 0x10 - color/flag byte (init 2)
    char field_11;       // 0x11 - color/flag byte
    char field_12;       // 0x12 - marker byte (init 1)
    char pad_13;         // 0x13
    int* glyphTable;     // 0x14 - array of 16-byte glyph entries
    int field_18;        // 0x18
    int field_1C;        // 0x1C
    int numChars;        // 0x20 - number of characters in font
    int spaceWidth;      // 0x24
    int tabWidth;        // 0x28
    int field_2C;        // 0x2C
    int letterSpacing;   // 0x30
    int field_34;        // 0x34
    char fontName[64];   // 0x38 - font filename (0x40 bytes to 0x78)

    int IsValidChar(int ch);      // 0x43ABA0
    int GetTextWidth(char* str);  // 0x43AD50
};

#endif // GLYPHFONT_H
