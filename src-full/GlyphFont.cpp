#include "GlyphFont.h"
#include "InvSlotItem.h"

/* Function start: 0x43AD50 */
int GlyphFont::GetTextWidth(char* str) {
    int width = 0;

    if (str == 0) return 0;

    while (*str != 0) {
        int ch = (int)(signed char)*str;
        if (ch == 0x20) {
            width += spaceWidth;
        } else if (ch == 0x9) {
            width += tabWidth;
        } else {
            Rect* rects = (Rect*)glyphTable;
            Rect glyph = rects[ch - firstChar];
            width = width - glyph.left + glyph.right;
        }
        str++;
        width += letterSpacing;
    }
    return width;
}

/* Function start: 0x43ABA0 */
int GlyphFont::IsValidChar(int ch) {
    if (ch == 0x20 || ch == 0x9) {
        return 1;
    }
    int idx = ch - firstChar;
    if (idx < 0 || numChars - 1 < idx) {
        return 0;
    }
    return 1;
}
