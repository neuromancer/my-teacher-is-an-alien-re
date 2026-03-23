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
    int IsSimilar(void* data, int start, int count);
    void Cleanup();
    void CopyData(void* source);
    void FadeTo(int targetR, int targetG, int targetB, float fraction, int start, int count); // 0x41DE60
    void SetAndApply(unsigned int start, unsigned int count); // 0x41DF10
    void CopyRGBData(int source, int startIdx, int count); // 0x41DC70
    void PlaySound(int);              // 0x425550
    int CheckSound(int);              // 0x4256D0

    void LoadFile(char* filename);

};

// CreatePaletteBuffer at 0x41EA50 is a placement new for Palette
Palette* __fastcall CreatePaletteBuffer(Palette* buffer);

// InitPalette at 0x41DBE0 - constructs Palette via default constructor
Palette* __fastcall InitPalette(Palette* buffer);

#endif // PALETTE_H
