#ifndef PALETTE_H
#define PALETTE_H

#include <windows.h>
#include "PaletteBuffer.h"

PaletteBuffer* __fastcall CreatePaletteBuffer(PaletteBuffer* buffer);

class Palette {
public:
    Palette();
    ~Palette();
    void Load(char* filename);
    UINT SetEntries(HPALETTE hpal, UINT iStart, UINT cEntries, PALETTEENTRY* pPalEntries);
    void SetPalette(UINT start, UINT count);
    int Compare(void* data, int size);

private:
    void OpenAndReadPaletteFile(char* filename);

    int m_size;
    void* m_data;
};

#endif // PALETTE_H
