#ifndef PALETTEUTILS_H
#define PALETTEUTILS_H

#include <windows.h>


    int __cdecl CopyPaletteData(int index, int count, unsigned char* dest);
    int __cdecl SetPaletteEntries_(unsigned int start, unsigned int count, unsigned char* data);


int __cdecl SetFillColor(int param_1);

int __cdecl ApplyVideoPalette(void);
#endif // PALETTEUTILS_H
