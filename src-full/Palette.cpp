#include "Palette.h"
#include "PaletteUtils.h"
#include "string.h"

#include <stdio.h>
#include <string.h>
#include "Memory.h"
#include "Animation.h"

extern char* __cdecl ResolveAssetPath(char* name);
extern "C" int FileExists(const char*);

/* Function start: 0x41DBE0 */
Palette* __fastcall InitPalette(Palette* buffer)
{
    buffer->Palette::Palette();
    return buffer;
}

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

/* Function start: 0x41DBF0 */
Palette::Palette(char* filename)
{
    m_size = 0;
    m_data = 0;
    m_size = 0x300;
    m_data = new char[0x300];
    LoadFile(filename);
}

/* Function start: 0x41DC40 */ /* ~96% match */
Palette::Palette()
{
    m_size = 0;
    m_data = 0;
    m_size = 0x300;
    m_data = new char[0x300];
}

/* Function start: 0x41DCA0 */
void Palette::CopyData(void* source)
{
    char* dst = m_data;
    char* src = (char*)source;
    int i = 0;
    while (i < m_size) {
        *dst = *src;
        src++;
        dst++;
        i++;
    }
}

/* Function start: 0x41DCC0 */
void Palette::LoadFile(char* filename)
{
    char* path = ResolveAssetPath(filename);
    if (!FileExists(path)) {
        ShowError("Palette::Load - Can't find palette file '%s'", filename);
    }

    int len = strlen(filename);
    if (strnicmp(filename + len - 4, ".smk", 4) == 0) {
        Animation anim;
        anim.Open(filename, 0xFE000, -1);
        CopyData(anim.smk->Palette);
    } else {
        FILE* file = fsopen(filename, "rb");
        if (file == 0) {
            ShowError("Palette::Load - Could not open palette file '%s'", filename);
        }

        if (strnicmp(filename + len - 4, ".col", 4) == 0) {
            fseek(file, 8, 0);
        } else {
            fseek(file, 0xd, 0);
        }

        fread(m_data, m_size, 1, file);
        fclose(file);
    }
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

/* Function start: 0x41DC10 */
Palette::~Palette()
{
    if (m_data != 0) {
        delete m_data;
        m_data = 0;
    }
    m_size = 0;
}

// Load delegates to LoadFile in the full game
void Palette::Load(char* filename)
{
    LoadFile(filename);
}

/* Function start: 0x41EAC0 */ /* DEMO ONLY - no full game match */
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

/* Function start: 0x41DE00 */
void Palette::CopyEntries(int start, int count)
{
    CopyPaletteData(start, count, (unsigned char*)m_data + start * 3);
}

/* Function start: 0x41DE20 */
void Palette::SetPalette(UINT start, UINT count)
{
    SetPaletteEntries_(start, count, (unsigned char*)m_data + start * 3);
}

/* Function start: 0x41DE40 */
extern "C" void __cdecl SetPaletteEntriesAnimation(void *palette, unsigned int start, unsigned int count) {
  SetPaletteEntries_(start, count, (unsigned char *)palette + start * 3);
}

/* Function start: 0x41DC70 */
void Palette::CopyRGBData(int source, int startIdx, int count) {
    char* src = (char*)(source + startIdx * 3);
    char* dst = m_data + startIdx * 3;
    int bytes = count * 3;
    memcpy(dst, src, bytes);
}

/* Function start: 0x41DFE0 */
/* Function start: 0x41DFE0 */
int Palette::IsSimilar(void* data, int start, int count) {
    int ofs = start * 3;
    int end = count * 3;
    char* dst = (char*)data + ofs;
    char* src = m_data + ofs;
    while (end > ofs) {
        int diff = 0;
        diff = (unsigned char)(*dst ^ *src);
        if (diff & 0xFFFFFFFC) {
            return 0;
        }
        ofs++;
        dst++;
        src++;
    }
    return 1;
}

/* Function start: 0x41EBB0 */ /* DEMO ONLY - no full game match */
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

/* Function start: 0x41EC60 */ /* DEMO ONLY - no full game match */
int Palette::Compare(char* data, int size)
{
    if (data != 0 && m_data != 0) {
        for (int i = 0; i < size; i++) {
            if (data[i] != m_data[i]) {
                return 0;
            }
        }
        return 1;
    }
    return 0;
}

/* Function start: 0x425550 */
void Palette::PlaySound(int) {}

/* Function start: 0x4256D0 */
int Palette::CheckSound(int) { return 0; }

