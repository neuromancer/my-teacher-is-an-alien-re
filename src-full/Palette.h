#ifndef PALETTE_H
#define PALETTE_H

#include <windows.h>

class Palette {
public:
    int m_size;
    char* m_data;

    Palette();
    Palette(char* filename);
    ~Palette();
    void Load(char* filename);
    void CopyEntries(int start, int count);
    void SetPalette(UINT start, UINT count);
    int Compare(char* data, int size);
    int IsSimilar(void* data, int start, int count);
    int IsPaletteUnchanged();
    void Cleanup();
    void CopyData(void* source);

private:
    void OpenAndReadPaletteFile(char* filename);
    void LoadFile(char* filename);
};

// CreatePaletteBuffer at 0x41EA50 is a placement new for Palette
Palette* __fastcall CreatePaletteBuffer(Palette* buffer);

#endif // PALETTE_H
