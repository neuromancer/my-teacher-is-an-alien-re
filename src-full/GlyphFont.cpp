#include "GlyphFont.h"
#include "InvSlotItem.h"
#include "Animation.h"
#include <string.h>
#include <stdio.h>

extern char* ResolveAssetPath(char* path);
extern char DAT_0046bd74;

/* Function start: 0x43A830 */
void GlyphFont::LoadFont(char* param) {
    char nameBuf[128];
    int lastChar;

    if (param == 0) return;

    int count = sscanf(param, "%s %d %d", nameBuf, &firstChar, &lastChar);

    if (strcmp(fontName, nameBuf) == 0) return;

    strcpy(fontName, nameBuf);

    if (count <= 2) {
        lastChar = 0x7e;
    }
    if (count <= 1) {
        firstChar = 0x21;
    }

    numChars = lastChar - firstChar + 1;

    if (bitmapData != 0) {
        ((VBuffer*)bitmapData)->~VBuffer();
        free(bitmapData);
        bitmapData = 0;
    }

    Animation* anim = new Animation();

    char* resolved = ResolveAssetPath(nameBuf);
    anim->Open(resolved, 0xfe000, -1);

    VBuffer* vbuf = new VBuffer(anim->smk->Width, anim->smk->Height);

    bitmapData = vbuf;
    anim->CloseSmackerFile();
    anim->CloseSmackerBuffer();
    if (anim != 0) {
        delete anim;
    }

    InitGlyphTable();

    if (IsValidChar(0x41)) {
        int w = GetTextWidth(&DAT_0046bd74);
        w = (w * 2) / 3;
        spaceWidth = w;
        tabWidth = w << 2;
    }
}

/* Function start: 0x43AA20 */
void GlyphFont::InitGlyphTable() {
    // TODO: implement
}

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
