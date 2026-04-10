#ifndef PALETTEUTILS_H
#define PALETTEUTILS_H

#include <windows.h>

extern "C" {
    int __cdecl CopyPaletteData(int index, int count, void* dest);
    int __cdecl SetPaletteEntries_(unsigned int start, unsigned int count, void* data);
}

#endif // PALETTEUTILS_H
