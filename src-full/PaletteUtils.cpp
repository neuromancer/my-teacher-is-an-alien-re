#include <windows.h>
#include "globals.h"
#include "string.h"

// Color setup globals
extern char DAT_00437490;      // Color value after lookup
extern char DAT_00437520[];    // Palette identity map (256 bytes)
extern char g_BgrPalette_00437b48[];    // BGR palette buffer
static char DAT_00437495 = 0;  // Current color index

extern "C" {

/* Function start: 0x4525EC */
int __cdecl SetFillColor(unsigned char param_1)
{
    unsigned char al;
    unsigned int eax;
    unsigned int edx;

    DAT_00437495 = param_1;
    al = DAT_00437520[param_1];
    DAT_00437490 = al;
    eax = al | (al << 8);
    edx = eax << 16;
    eax = eax | edx;
    g_FillColorDword_00437491 = eax;
    return 0;
}

/* Function start: 0x452964 */
int __cdecl ApplyVideoPalette(void)
{
    if ((signed char)g_CurrentVideoBuffer_00437f54 >= 0) {
        if (g_WinGDC_0043841c != (HDC)0) {
            ((void (__cdecl *)(HDC, int, int, void*))g_WinGSetDIBColorTable_0043842c)(g_WinGDC_0043841c, 0, 0x100, &g_BgrPalette_00437b48[4]);
        } else {
            if (g_DibModeFlag_00437f50 != 0) {
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
                memcpy((void*)(g_VideoBufferBase_00437f66 - 0x400), &g_BgrPalette_00437b48[4], 0x400);
            }
        }
    }
    return 0;
}

}

extern "C" {

/* Function start: 0x453384 */
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

/* Function start: 0x4533B0 */
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

/* Function start: 0x452C9B */
int __cdecl GetGlobalVertAlign()
{
    return (int)(signed char)g_TextAlignV_004374c1;
}

/* Function start: 0x453CBA */
int __cdecl GetGlobalTextAlign()
{
    return (int)(signed char)g_TextAlignH_004374c0;
}

/* Function start: 0x453CC2 */
int __cdecl GetGlobalTextY()
{
    return g_DrawPosY_004374ce;
}

/* Function start: 0x4205D0 */
int __cdecl GetGlobalTextX()
{
    return g_DrawPosX_004374c2;
}

} // extern "C"
