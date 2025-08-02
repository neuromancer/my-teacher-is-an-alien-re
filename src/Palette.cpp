#include "Palette.h"
#include <stdio.h>
#include <string.h>

extern "C" {
    void* AllocateMemory_Wrapper(int size);
    void FreeFromGlobalHeap(void*);
    FILE* _fsopen(const char* filename, const char* mode, int shflag);
    void ShowError(const char*, ...);
    int _strnicmp(const char*, const char*, size_t);
    int FUN_00425f30(FILE* stream, long offset, int origin);
    size_t __fread_lk(void* ptr, size_t size, size_t count, FILE* stream);
    int _fclose(FILE* stream);
    UINT CopyPaletteData(int, int, void*);
    void FUN_00423905(UINT, UINT, void*);
}

/* Function start: 0x41EA50 */
Palette::Palette()
{
    m_size = 0;
    m_data = 0;
    m_size = 0x300;
    m_data = AllocateMemory_Wrapper(0x300);
}

/* Function start: 0x41EA80 */
Palette::~Palette()
{
    if (m_data != 0) {
        FreeFromGlobalHeap(m_data);
        m_data = 0;
    }
    m_size = 0;
}

/* Function start: 0x41EAB0 */
void Palette::Load(char* filename)
{
    OpenAndReadPaletteFile(filename);
}

/* Function start: 0x41EAC0 */
void Palette::OpenAndReadPaletteFile(char* filename)
{
    FILE* file = _fsopen(filename, "rb", 0x40);
    if (file == 0) {
        ShowError("Error! Could not open palette file '%s'", filename);
    }

    long offset = 0xd;
    int len = strlen(filename);
    if (len > 4) {
        if (_strnicmp(filename + len - 4, ".col", 4) == 0) {
            offset = 8;
        }
    }

    FUN_00425f30(file, offset, 0);
    __fread_lk(m_data, m_size, 1, file);
    _fclose(file);
}

/* Function start: 0x41EB50 */
UINT Palette::SetEntries(HPALETTE hpal, UINT iStart, UINT cEntries, PALETTEENTRY* pPalEntries)
{
    return CopyPaletteData((int)hpal, iStart, (unsigned char*)m_data + (int)hpal * 3);
}

/* Function start: 0x41EB70 */
void Palette::SetPalette(UINT start, UINT count)
{
    FUN_00423905(start, count, (unsigned char*)m_data + start * 3);
}
