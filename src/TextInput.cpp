#include "TextInput.h"
#include <string.h>
#include <stdlib.h>

/* Function start: 0x445680 */
TextInput::TextInput(char* str, int len, void* fnt, void* wid) {
    origBuf = 0;
    editBuf = 0;
    maxLen = 0;
    font = 0;
    maxWidth = 0;
    editBuf = str;
    maxLen = len;
    origBuf = (char*)operator new(len + 1);
    strcpy(origBuf, editBuf);
    font = (AnimatedAsset*)fnt;
    maxWidth = (int)wid;
}

/* Function start: 0x4456F0 */
TextInput::~TextInput() {
    if (origBuf != 0) {
        operator delete(origBuf);
        origBuf = 0;
    }
}

/* Function start: 0x445710 */
int TextInput::ProcessKey(int key) {
    int len = strlen(editBuf);
    int result = 0;

    switch (key) {
    default:
        if (key == 0x2D) key = 0x5F;
        if ((key >= 0x41 && key <= 0x5A) || (key >= 0x30 && key <= 0x39) ||
            key == 0x5F || key == 0x20) {
            editBuf[len] = (char)key;
            if (font->GetTextWidth(editBuf) > maxWidth) {
                editBuf[len] = 0;
                break;
            }
            len++;
            if (len > maxLen - 1) {
                len = maxLen - 1;
            }
            editBuf[len] = 0;
        }
        break;
    case 8:
        len--;
        if (len < 0) len = 0;
        editBuf[len] = 0;
        break;
    case 0xD:
        strcpy(origBuf, editBuf);
        result = 1;
        break;
    case 0x1B:
        strcpy(editBuf, origBuf);
        result = 1;
        break;
    }
    return result;
}
