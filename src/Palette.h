#ifndef PALETTE_H
#define PALETTE_H

#include <windows.h>

class Palette {
public:
    int m_size;
    char* m_data;

    Palette();
    ~Palette();
    void Load(char* filename);
    void CopyEntries(int start, int count);
    void SetPalette(UINT start, UINT count);
    int Compare(char* data, int size);
    int IsPaletteUnchanged();
    void Cleanup();

private:
    void OpenAndReadPaletteFile(char* filename);
};

// CreatePaletteBuffer at 0x41EA50 is a placement new for Palette
Palette* __fastcall CreatePaletteBuffer(Palette* buffer);

#endif // PALETTE_H
