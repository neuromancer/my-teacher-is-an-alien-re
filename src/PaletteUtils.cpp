#include <windows.h>

extern "C" {
    extern char DAT_00437720[0x400];    // Palette entries array
    extern char DAT_00437b48[0x400];    // Palette RGBQUAD array
    extern int DAT_00437f54;            // Palette initialized flag
    extern int DAT_00437f50;            // ColorUse flag
    extern int DAT_00437f66;            // Video buffer offset
    extern short DAT_00437f6a;          // Video buffer segment
    extern HDC h_0043841c;              // Device context
    extern void* DAT_0043842c;          // SetDIBColorTable function pointer
    extern HPALETTE hPal_0043748c;      // Palette handle
}

extern "C" {

/* Function start: 0x4238D9 */
int __cdecl CopyPaletteData(int index, int count, unsigned char* dest)
{
    if (count != 0) {
        unsigned char* src = (unsigned char*)&DAT_00437720[(index + 1) * 4];
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

    if (DAT_00437f54 >= 0 && count != 0) {
        int offset = (start + 1) * 4;
        PALETTEENTRY* pe = (PALETTEENTRY*)&DAT_00437720[offset];
        unsigned char* src = data;

        // Copy RGB data to palette entries
        for (i = 0; i < count; i++) {
            pe[i].peRed = src[0];
            pe[i].peGreen = src[1];
            pe[i].peBlue = src[2];
            pe[i].peFlags = 1;
            src += 3;
        }

        // Convert to RGBQUAD format
        RGBQUAD* quad = (RGBQUAD*)&DAT_00437b48[offset];
        for (i = 0; i < count; i++) {
            quad[i].rgbBlue = pe[i].peRed;
            quad[i].rgbGreen = pe[i].peGreen;
            quad[i].rgbRed = pe[i].peBlue;
            quad[i].rgbReserved = 0;
        }

        if (h_0043841c == (HDC)0) {
            if (DAT_00437f50 == 0) {
                // Copy to video buffer
                unsigned int* dst = (unsigned int*)(DAT_00437f66 - 0x400);
                unsigned int* srcQuad = (unsigned int*)&DAT_00437b48[4];
                for (i = 0x100; i != 0; i--) {
                    *dst++ = *srcQuad++;
                }
            }
        } else {
            // Use SetDIBColorTable via function pointer
            typedef void (__cdecl *SetDIBColorTableFunc)(HDC, unsigned int, unsigned int, RGBQUAD*);
            ((SetDIBColorTableFunc)DAT_0043842c)(h_0043841c, start, count, (RGBQUAD*)&DAT_00437b48[offset]);
        }

        AnimatePalette(hPal_0043748c, start, count, pe);
    }
    return 0;
}

} // extern "C"
