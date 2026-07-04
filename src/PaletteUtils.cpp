#include <windows.h>
#include "globals.h"
#include "string.h"



#pragma optimize("y", off)

/* Function start: 0x4525EC */
int __cdecl SetFillColor(int param_1)
{
    unsigned char al;
    unsigned int eax;
    unsigned int edx;

    g_ColorIndex_0046d07d = param_1;
    al = g_PaletteMap_0046d108[param_1];
    g_TextColor_0046d078 = al;
    eax = al | (al << 8);
    edx = eax << 16;
    eax = eax | edx;
    g_FillColorDword_0046d079 = eax;
    return 0;
}

#pragma optimize("y", on)

/* Function start: 0x452964 */
int __cdecl ApplyVideoPalette(void)
{
    if ((signed char)g_CurrentVideoBuffer_0046db3c >= 0) {
        if (g_WinGDC_0046e004 != (HDC)0) {
            ((void (__stdcall *)(HDC, int, int, void*))g_WinGSetDIBColorTable_0046e014)(g_WinGDC_0046e004, 0, 0x100, g_BgrPaletteData_0046d734);
        } else {
            if (g_DibModeFlag_0046db38 != 0) {
                short* dst = (short*)(g_VideoBufferBase_0046db4e - 0x200);
                short value = 0;
                int count = 0x100;
                do {
                    *dst = value;
                    value++;
                    count--;
                    dst++;
                } while (count != 0);
            } else {
                memcpy((void*)(g_VideoBufferBase_0046db4e - 0x400), g_BgrPaletteData_0046d734, 0x400);
            }
        }
    }
    return 0;
}





#pragma optimize("y", off)

/* Function start: 0x453384 */
int __cdecl CopyPaletteData(int index, int count, unsigned char* dest)
{
    if (count != 0) {
        unsigned char* src = (unsigned char*)&g_LogPalette_0046d308[(index + 1) * 4];
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

    if ((signed char)g_CurrentVideoBuffer_0046db3c >= 0 && count != 0) {
        int offset = (start + 1) * 4;
        PALETTEENTRY* pe = (PALETTEENTRY*)&g_LogPalette_0046d308[offset];
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
        RGBQUAD* quad = (RGBQUAD*)&g_BgrPaletteData_0046d734[start * 4];
        for (i = 0; i < count; i++) {
            quad[i].rgbBlue = pe[i].peBlue;
            quad[i].rgbGreen = pe[i].peGreen;
            quad[i].rgbRed = pe[i].peRed;
            quad[i].rgbReserved = 0;
        }

        if (g_WinGDC_0046e004 == (HDC)0) {
            if (g_DibModeFlag_0046db38 == 0) {
                // Copy to video buffer
                unsigned int* dst = (unsigned int*)(g_VideoBufferBase_0046db4e - 0x400);
                unsigned int* srcQuad = (unsigned int*)g_BgrPaletteData_0046d734;
                for (i = 0x100; i != 0; i--) {
                    *dst++ = *srcQuad++;
                }
            }
        } else {
            // Use SetDIBColorTable via function pointer
            typedef void (__cdecl *SetDIBColorTableFunc)(HDC, unsigned int, unsigned int, RGBQUAD*);
            ((SetDIBColorTableFunc)g_WinGSetDIBColorTable_0046e014)(g_WinGDC_0046e004, start, count, (RGBQUAD*)&g_BgrPaletteData_0046d734[start * 4]);
        }

        AnimatePalette(g_Palette_0046d074, start, count, pe);
    }
    return 0;
}

#pragma optimize("y", on)

/* Function start: 0x453CB2 */
int __cdecl GetGlobalVertAlign()
{
    return (int)(signed char)g_TextAlignV_0046d0a9;
}

/* Function start: 0x453CBA */
int __cdecl GetGlobalTextAlign()
{
    return (int)(signed char)g_TextAlignH_0046d0a8;
}

/* Function start: 0x453CC2 */
int __cdecl GetGlobalTextY()
{
    return g_DrawPosY_0046d0b6;
}

/* Function start: 0x453CC8 */
int __cdecl GetGlobalTextX()
{
    return g_DrawPosX_0046d0aa;
}

 // extern "C"
