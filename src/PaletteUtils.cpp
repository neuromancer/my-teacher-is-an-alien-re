#include <windows.h>
#include "globals.h"
#include "string.h"

// Color setup globals
extern char DAT_00437490;      // Color value after lookup
extern char DAT_00437520[];    // Palette identity map (256 bytes)
extern char g_BgrPalette_00437b48[];    // BGR palette buffer
static char DAT_00437495 = 0;  // Current color index

extern "C" {

/* Function start: 0x422A01 */
int __cdecl SetFillColor(unsigned char param_1)
{
    DAT_00437495 = param_1;
    unsigned char mappedColor = DAT_00437520[param_1];
    DAT_00437490 = mappedColor;
    // Match original: MOV AH,AL; MOVZX EAX,AX; MOV EDX,EAX; SHL EDX,0x10; OR EAX,EDX
    unsigned short word = mappedColor | (mappedColor << 8);
    unsigned int dword = word | (word << 16);
    g_FillColorDword_00437491 = dword;
    return 0;
}

/* Function start: 0x422E8F */
int __cdecl ApplyVideoPalette(void)
{
    if ((signed char)g_CurrentVideoBuffer_00437f54 >= 0) {
        if (g_WinGDC_0043841c != (HDC)0) {
            // Use SetDIBColorTable via function pointer
            typedef void (__cdecl *SetDIBColorTableFunc)(HDC, unsigned int, unsigned int, void*);
            // DAT_00437b4c is at offset 4 into g_BgrPalette_00437b48
            ((SetDIBColorTableFunc)g_WinGSetDIBColorTable_0043842c)(g_WinGDC_0043841c, 0, 0x100, &g_BgrPalette_00437b48[4]);
        } else {
            if (g_DibModeFlag_00437f50 != 0) {
                // Fill 256 shorts with sequential values 0-255
                short* dst = (short*)(g_VideoBufferBase_00437f66 - 0x200);
                short value = 0;
                int count = 0x100;
                do {
                    *dst = value;
                    value++;
                    count--;
                    dst++;
                } while (count != 0);
            } else {
                // Copy 256 DWORDs from DAT_00437b4c to video buffer
                // Original used REP MOVSD with ES segment
                memcpy((void*)(g_VideoBufferBase_00437f66 - 0x400), &g_BgrPalette_00437b48[4], 0x400);
            }
        }
    }
    return 0;
}

}

extern "C" {

/* Function start: 0x4238D9 */
int __cdecl CopyPaletteData(int index, int count, unsigned char* dest)
{
    if (count != 0) {
        unsigned char* src = (unsigned char*)&g_LogPalette_00437720[(index + 1) * 4];
        do {
            dest[0] = src[0];
            dest[1] = src[1];
            dest[2] = src[2];
            dest += 3;
            src += 4;
            count--;
        } while (count != 0);
    }
    return 0;
}

/* Function start: 0x423905 */
int __cdecl SetPaletteEntries_(unsigned int start, unsigned int count, unsigned char* data)
{
    unsigned int i;

    if ((signed char)g_CurrentVideoBuffer_00437f54 >= 0 && count != 0) {
        int offset = (start + 1) * 4;
        PALETTEENTRY* pe = (PALETTEENTRY*)&g_LogPalette_00437720[offset];
        unsigned char* src = data;

        // Copy RGB data to palette entries
        for (i = 0; i < count; i++) {
            pe[i].peRed = src[0];
            pe[i].peGreen = src[1];
            pe[i].peBlue = src[2];
            pe[i].peFlags = 1;
            src += 3;
        }

        // Convert to RGBQUAD format (BGR order)
        RGBQUAD* quad = (RGBQUAD*)&g_BgrPalette_00437b48[offset];
        for (i = 0; i < count; i++) {
            quad[i].rgbBlue = pe[i].peBlue;
            quad[i].rgbGreen = pe[i].peGreen;
            quad[i].rgbRed = pe[i].peRed;
            quad[i].rgbReserved = 0;
        }

        if (g_WinGDC_0043841c == (HDC)0) {
            if (g_DibModeFlag_00437f50 == 0) {
                // Copy to video buffer
                unsigned int* dst = (unsigned int*)(g_VideoBufferBase_00437f66 - 0x400);
                unsigned int* srcQuad = (unsigned int*)&g_BgrPalette_00437b48[4];
                for (i = 0x100; i != 0; i--) {
                    *dst++ = *srcQuad++;
                }
            }
        } else {
            // Use SetDIBColorTable via function pointer
            typedef void (__cdecl *SetDIBColorTableFunc)(HDC, unsigned int, unsigned int, RGBQUAD*);
            ((SetDIBColorTableFunc)g_WinGSetDIBColorTable_0043842c)(g_WinGDC_0043841c, start, count, (RGBQUAD*)&g_BgrPalette_00437b48[offset]);
        }

        AnimatePalette(g_Palette_0043748c, start, count, pe);
    }
    return 0;
}

/* Function start: 0x4239C8 */
int __cdecl GetGlobalVertAlign()
{
    return (int)(signed char)g_TextAlignV_004374c1;
}

/* Function start: 0x4239D0 */
int __cdecl GetGlobalTextAlign()
{
    return (int)(signed char)g_TextAlignH_004374c0;
}

/* Function start: 0x4239D8 */
int __cdecl GetGlobalTextY()
{
    return g_DrawPosY_004374ce;
}

/* Function start: 0x4239DE */
int __cdecl GetGlobalTextX()
{
    return g_DrawPosX_004374c2;
}

} // extern "C"
