#ifndef TEXTINPUT_H
#define TEXTINPUT_H

#include "GlyphFont.h"

// TextInput - text input field widget (0x14 bytes)
// Constructor: 0x445680, Destructor: 0x4456F0, ProcessKey: 0x445710
class TextInput {
public:
    char* origBuf;     // 0x00
    char* editBuf;     // 0x04
    int maxLen;        // 0x08
    GlyphFont* font;   // 0x0C
    int maxWidth;      // 0x10
    TextInput(char* str, int len, void* fnt, void* wid);
    ~TextInput();
    int ProcessKey(int key);
};

#endif // TEXTINPUT_H
