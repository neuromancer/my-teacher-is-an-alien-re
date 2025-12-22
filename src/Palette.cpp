#include "Palette.h"
#include "PaletteBuffer.h"
#include "PaletteUtils.h"
#include "string.h"

#include <stdio.h>
#include <string.h>
#include "Memory.h"

/* Function start: 0x41EA50 */
Palette::Palette()
{
    m_size = 0;
    m_data = 0;
    m_size = 0x300;
    m_data = AllocateMemory(0x300);
}

/* Function start: 0x41EA80 */
Palette::~Palette()
{
    if (m_data != 0) {
        FreeMemory(m_data);
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
    FILE* file = fsopen(filename, "rb");
    if (file == 0) {
        ShowError("Error! Could not open palette file '%s'", filename);
    }

    long offset = 0xd;
    int len = strlen(filename);
    if (len > 4) {
        if (strnicmp(filename + len - 4, ".col", 4) == 0) {
            offset = 8;
        }
    }

    fseek(file, offset, 0);
    fread(m_data, m_size, 1, file);
    fclose(file);
}

/* Function start: 0x41EB50 */
UINT Palette::SetEntries(HPALETTE hpal, UINT iStart, UINT cEntries, PALETTEENTRY* pPalEntries)
{
    return CopyPaletteData((int)hpal, iStart, (unsigned char*)m_data + (int)hpal * 3);
}

/* Function start: 0x41EB70 */
void Palette::SetPalette(UINT start, UINT count)
{
    SetPaletteEntries_(start, count, (unsigned char*)m_data + start * 3);
}

/* Function start: 0x41EC60 */
int Palette::Compare(void* data, int size)
{
    if (data != 0 && m_data != 0) {
        for (int i = 0; i < size; i++) {
            if (((char*)data)[i] != ((char*)m_data)[i]) {
                return 0;
            }
        }
        return 1;
    }
    return 0;
}

/* Function start: 0x41EA50 */
PaletteBuffer* __fastcall CreatePaletteBuffer(PaletteBuffer* buffer)
{
    buffer->size = 0;
    buffer->data = 0;
    buffer->size = 0x300;
    buffer->data = (void*)AllocateMemory(0x300);
    return buffer;
}
