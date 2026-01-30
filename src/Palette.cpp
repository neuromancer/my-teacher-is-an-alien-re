#include "Palette.h"
#include "PaletteUtils.h"
#include "string.h"

#include <stdio.h>
#include <string.h>
#include "Memory.h"

// NOTE: CreatePaletteBuffer at 0x41EA50 is effectively a placement new for Palette

// Cleanup is equivalent to explicit destructor call ~Palette()
// Original Animation::Play calls destructor explicitly before operator delete
void Palette::Cleanup()
{
    if (m_data != 0) {
        delete m_data;
        m_data = 0;
    }
    m_size = 0;
}







extern "C" {
}

/* Function start: 0x41EA50 */
Palette::Palette()
{
    m_size = 0;
    m_data = 0;
    m_size = 0x300;
    m_data = new char[0x300];
}

/* Function start: 0x41EA50 - CreatePaletteBuffer is effectively a placement new for Palette */
Palette* __fastcall CreatePaletteBuffer(Palette* buffer)
{
    if (buffer != 0) {
        buffer->m_size = 0;
        buffer->m_data = 0;
        buffer->m_size = 0x300;
        buffer->m_data = new char[0x300];
    }
    return buffer;
}

/* Function start: 0x41EA80 */
Palette::~Palette()
{
    if (m_data != 0) {
        delete m_data;
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

    int len = strlen(filename);
    if (len > 4 && strnicmp(filename + len - 4, ".col", 4) == 0) {
        fseek(file, 8, 0);
    } else {
        fseek(file, 0xd, 0);
    }

    fread(m_data, m_size, 1, file);
    fclose(file);
}

/* Function start: 0x41EB50 */
void Palette::CopyEntries(int start, int count)
{
    CopyPaletteData(start, count, (unsigned char*)m_data + start * 3);
}

/* Function start: 0x41EB70 */
void Palette::SetPalette(UINT start, UINT count)
{
    SetPaletteEntries_(start, count, (unsigned char*)m_data + start * 3);
}

/* Function start: 0x41EB90 */
extern "C" void __cdecl SetPaletteEntriesAnimation(void *palette, unsigned int start, unsigned int count) {
  SetPaletteEntries_(start, count, (unsigned char *)palette + start * 3);
}

/* Function start: 0x41EBB0 */
int Palette::IsPaletteUnchanged()
{
    Palette* tempPalette;
    int result;

    tempPalette = new Palette();
    tempPalette->CopyEntries(0, 0x100);
    result = Compare(tempPalette->m_data, 0x300);
    if (tempPalette != 0) {
        tempPalette->~Palette();
        delete (void*)tempPalette;
    }
    return result;
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

