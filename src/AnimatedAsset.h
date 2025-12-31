#ifndef ANIMATEDASSET_H
#define ANIMATEDASSET_H

#include "VBuffer.h"
#include "GlyphRect.h"

class AnimatedAsset {
public:
    VBuffer* buffer; // 0x0
    int firstChar; // 0x4 - starting character code
    int text_x; // 0x8 - current x position for rendering text
    int text_y; // 0xC - current y position for rendering text
    char color; // 0x10 - blit color / mode
    char attr; // 0x11 - additional attribute (e.g., tint / flag)
    char glyphValue; // 0x12 - value used to detect glyph pixels
    char pad_13;
    GlyphRect* glyphTable; // 0x14 - pointer to glyph rectangle table
    int useBuffer; // 0x18 - render target selector / flag
    int useAttr; // 0x1C - whether attr/color is active
    int glyphCount; // 0x20 - number of glyphs
    int spaceWidth; // 0x24 - width of space
    int tabWidth; // 0x28 - width of tab
    int glyphHeight; // 0x2C - height (rows) per glyph
    int charAdvance; // 0x30 - inter-character advance
    int reserved_34; // 0x34 - unknown / reserved
    int reserved_38; // 0x38 - unknown / reserved

    AnimatedAsset* Init();
    void LoadAnimatedAsset(char* param_1);
    void BuildGlyphTable();
    int IsCharSupported(int ch);
    int ComputeTextMetrics(void* text);
    void PrepareText(char* text);
    void RenderText(char* text, int param_2);
    int DrawChar(int, int, int);

    //void CleanupCharAdvance();
    //void CleanupTextX();
    int GetGlobalTextX();
    int GetGlobalTextY();
    int GetGlobalTextAlign();
    int GetGlobalVertAlign();


    ~AnimatedAsset();
};

#endif // ANIMATEDASSET_H
