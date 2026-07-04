#include "globals.h"
#include <windows.h>
#include <string.h>

// Also declared in string.h; kept as local prototypes because pulling the whole
// header into this hand-asm-matched TU shifts MSVC 4.20's register allocation.
void ShowMessage(char *param_1, ...);
void WriteToMessageLog(const char *msg, ...);


#pragma optimize("y", off)

/* Function start: 0x4524C2 */
int __cdecl SetDrawPosition(int param_1, int param_2)
{
    g_DrawPosX_0046d0aa = param_1;
    g_DrawPosY_0046d0b6 = param_2;
    return 0;
}

/* Function start: 0x453C9B */
int __cdecl SetDrawColors(unsigned char param_1, unsigned char param_2)
{
    g_TextAlignH_0046d0a8 = param_1;
    g_TextAlignV_0046d0a9 = param_2;
    return 0;
}

/* Function start: 0x453CCE */
int __cdecl GetPixelAt(int x, int y)
{
    short result;

    result = -1;
    if (*(signed char*)&g_CurrentVideoBuffer_0046db3c >= 0) {
        if (x >= g_ClipLeft_0046d0c6 && x <= g_ClipRight_0046d0ca &&
            y >= g_ClipTop_0046d0ce && y <= g_ClipBottom_0046d0d2) {
            result = *(unsigned char*)(x + (unsigned int)(g_VideoBufferHeightM1_0046d0b2 - y) * (unsigned int)g_VideoBufferStride_0046db46 + g_VideoBufferBase_0046db4e);
        } else {
            result = -1;
        }
    }
    return result;
}

/* Function start: 0x4528EF */
int __cdecl ReleaseVideoBuffer(unsigned int param_1)
{
    HGLOBAL hMem;
    int* puVar2;

    if (param_1 < 0x20 && (char)param_1 != g_CurrentVideoBuffer_0046db3c) {
        g_VBufDataPtrs_0046de54[param_1] = 0;
        puVar2 = g_VBufDibHandles_0046dbd4;
        if (g_WinGDC_0046e004 != 0) {
            DeleteObject((HGDIOBJ)g_VBufDibHandles_0046dbd4[param_1]);
            puVar2 = g_VBufMemHandles_0046db54;
        }
        hMem = (HGLOBAL)puVar2[param_1];
        GlobalUnlock(hMem);
        GlobalFree(hMem);
    }
    return 0;
}

/* Function start: 0x452946 */
int __cdecl GetVideoBufferData(unsigned int handle)
{
    int result;

    result = 0;
    if (handle <= 0x1f) {
        handle <<= 2;
        result = *(int*)((char*)g_VBufDataPtrs_0046de54 + handle);
    }
    return result;
}

/* Function start: 0x4529D5 */
int __cdecl CreateTable(int width, int height) {
    int i = 0;
    while (i < 32 && g_VBufDataPtrs_0046de54[i] != 0) {
        i++;
    }

    if (i == 32) {
        return -1;
    }

    unsigned int aligned_width = (width + 3) & 0xFFFFFFFC;
    int image_size;

    if (g_WinGDC_0046e004 == 0) {
        image_size = aligned_width * height;
    } else {
        image_size = 1064 - g_BitmapHeaderSize_0046db34;
    }

    HGLOBAL hMem = GlobalAlloc(0x42, image_size + g_BitmapHeaderSize_0046db34);
    if (hMem == 0) {
        return -2;
    }

    BITMAPINFO* bmi = (BITMAPINFO*)GlobalLock(hMem);
    bmi->bmiHeader.biSize = 40;
    bmi->bmiHeader.biWidth = aligned_width;
    bmi->bmiHeader.biHeight = height;
    bmi->bmiHeader.biPlanes = 1;
    bmi->bmiHeader.biBitCount = 8;

    HGLOBAL hDib = hMem;
    int data_offset = g_BitmapHeaderSize_0046db34;

    if (g_WinGDC_0046e004 != 0) {
        hDib = ((HGLOBAL(*)(HDC, BITMAPINFO*, void**))g_WinGCreateDIB_0046e010)(g_WinGDC_0046e004, bmi, (void**)&bmi);
        if (hDib == 0) {
            return -2;
        }
        HGDIOBJ oldObj = SelectObject(g_WinGDC_0046e004, hDib);
        if (g_WinGBitmap_0046e00c == 0) {
            g_WinGBitmap_0046e00c = oldObj;
        }
        memset(bmi, 0, aligned_width * height);
        SelectObject(g_WinGDC_0046e004, oldObj);
        data_offset = 0;
    }

    g_VBufDibHandles_0046dbd4[i] = (int)hDib;
    g_VBufMemHandles_0046db54[i] = (int)hMem;
    g_VBufDataPtrs_0046de54[i] = (int)((char*)bmi + data_offset);
    g_VBufWidths_0046df54[i] = aligned_width;
    g_VBufHeights_0046ded4[i] = height;
    g_VBufMaxX_0046dc54[i] = aligned_width - 1;
    g_VBufMaxY_0046dcd4[i] = height - 1;
    g_VBufClipLeft_0046dd54[i] = 0;
    g_VBufClipTop_0046ddd4[i] = 0;

    return i;
}

#pragma optimize("y", on)

/* Function start: 0x452B6E */
int ClearVideoBuffer(void) {
    if (g_CurrentVideoBuffer_0046db3c >= 0) {
        memset((char*)g_VideoBufferBase_0046db4e, (char)g_FillColorDword_0046d079, g_VideoBufferSize_0046db4a);
    }
    return 0;
}

// Video table globals (defined in globals.cpp, declared in globals.h)

#pragma optimize("y", off)

/* Function start: 0x452BAE */
int __cdecl SelectVideoBuffer(unsigned int param_1) {
    int width;
    int height;

    if (param_1 <= 0x1f) {
        if (g_VBufDataPtrs_0046de54[param_1] == 0) {
            return 0xfffffffe;
        } else {
            if (g_WinGDC_0046e004 != 0) {
                SelectObject(g_WinGDC_0046e004, (HGDIOBJ)g_VBufDibHandles_0046dbd4[param_1]);
            }

            g_CurrentVideoBuffer_0046db3c = (char)param_1;
            *(char*)&g_CurrentVideoBuffer_0046db3c = (char)param_1;

            width = g_VBufWidths_0046df54[param_1];
            g_VideoBufferStride_0046db46 = width;
            g_VideoBufferWidth_0046d0ae = width;
            g_VideoBufferWidth_0046d0ae = width;
            g_VideoBufferStride_0046db46 = width;

            height = g_VBufHeights_0046ded4[param_1];
            g_VideoBufferHeight_0046d0ba = height;
            g_VideoBufferHeight_0046d0ba = height;
            g_VideoBufferHeightM1_0046d0b2 = height - 1;
            g_VideoBufferHeightM1_0046d0b2 = height - 1;

            g_ClipLeft_0046d0c6 = g_VBufClipLeft_0046dd54[param_1];
            g_ClipRight_0046d0ca = g_VBufMaxX_0046dc54[param_1];
            g_ClipTop_0046d0ce = g_VBufClipTop_0046ddd4[param_1];
            g_ClipBottom_0046d0d2 = g_VBufMaxY_0046dcd4[param_1];
            g_ClipLeft_0046d0c6 = g_VBufClipLeft_0046dd54[param_1];
            g_ClipRight_0046d0ca = g_VBufMaxX_0046dc54[param_1];
            g_ClipTop_0046d0ce = g_VBufClipTop_0046ddd4[param_1];
            g_ClipBottom_0046d0d2 = g_VBufMaxY_0046dcd4[param_1];

            g_VideoBufferBase_0046db4e = g_VBufDataPtrs_0046de54[param_1];
            g_VideoBufferBase_0046db4e = g_VBufDataPtrs_0046de54[param_1];

            g_VideoBufferSize_0046db4a = (int)((unsigned int)g_VideoBufferWidth_0046d0ae * (unsigned int)g_VideoBufferHeight_0046d0ba);
            g_VideoBufferSize_0046db4a = (int)((unsigned int)g_VideoBufferWidth_0046d0ae * (unsigned int)g_VideoBufferHeight_0046d0ba);

            return 0;
        }
    }
    return 0xffffffff;
}

typedef int (__stdcall *WinGBitBltFunc)(HDC, int, int, int, int, HDC, int, int);
typedef int (__stdcall *WinGStretchBltFunc)(HDC, int, int, int, int, HDC, int, int, int, int);

/* Function start: 0x452D6B */
int __cdecl BlitToDevice(int param_1, int param_2, int param_3, int param_4, int param_5, int param_6)
{
    int iVar1;
    int w;
    int h;
    int wingDC;

    if (*(char*)&g_CurrentVideoBuffer_0046db3c >= 0) {
        wingDC = (int)g_WinGDC_0046e004;
        if (wingDC != 0) {
            h = param_4;
        } else {
            // Non-WinG path: flip Y coordinates
            iVar1 = g_VideoBufferHeightM1_0046d0b2 - param_3;
            param_3 = g_VideoBufferHeightM1_0046d0b2 - param_4;
            h = iVar1;
        }

        iVar1 = (param_6 + param_3) - h;
        w = (param_2 - param_1) + 1;
        h = (h - param_3) + 1;

        if (wingDC != 0) {
            // Use WinG BitBlt via function pointer
            ((WinGBitBltFunc)g_WinGBitBlt_0046e01c)(g_MainDC_0046d070, param_5, iVar1, w, h, (HDC)wingDC, param_1, param_3);
        } else {
            // Use SetDIBitsToDevice
            int bitmapInfo = g_VideoBufferBase_0046db4e - g_BitmapHeaderSize_0046db34;
            SetDIBitsToDevice(g_MainDC_0046d070, param_5, iVar1, w, h, param_1, param_3, 0, g_VideoBufferHeight_0046d0ba,
                              (void*)(bitmapInfo + g_BitmapHeaderSize_0046db34),
                              (BITMAPINFO*)bitmapInfo,
                              g_DibModeFlag_0046db38);
        }
    }
    return 0;
}

/* Function start: 0x452E0F */
int __cdecl StretchBlitBuffer(int srcX1, int srcX2, int srcY1, int srcY2, int destX1, int destX2, int destY1, int destY2)
{
    int srcWidth;
    int srcHeight;
    int destWidth;
    int destHeight;
    int wingDC;
    int newY2;

    if (*(char*)&g_CurrentVideoBuffer_0046db3c >= 0) {
        wingDC = (int)g_WinGDC_0046e004;
        if (wingDC != 0) {
            newY2 = srcY2;
        }
        else {
            newY2 = g_VideoBufferHeightM1_0046d0b2 - srcY1;
            srcY1 = g_VideoBufferHeightM1_0046d0b2 - srcY2;
        }

        srcWidth = (srcX2 - srcX1) + 1;
        destWidth = (destX2 - destX1) + 1;
        destHeight = (destY2 - destY1) + 1;
        srcHeight = (newY2 - srcY1) + 1;

        if (wingDC != 0) {
            ((WinGStretchBltFunc)g_WinGStretchBlt_0046e020)(g_MainDC_0046d070, destX1, destY1, destWidth, destHeight, (HDC)wingDC, srcX1, srcY1, srcWidth, srcHeight);
        }
        else {
            int bitmapInfo = g_VideoBufferBase_0046db4e - g_BitmapHeaderSize_0046db34;
            StretchDIBits(g_MainDC_0046d070, destX1, destY1, destWidth, destHeight, srcX1, srcY1, srcWidth, srcHeight,
                          (void*)(bitmapInfo + g_BitmapHeaderSize_0046db34),
                          (BITMAPINFO*)bitmapInfo,
                          g_DibModeFlag_0046db38, 0xcc0020);
        }
    }
    return 0;
}

/* Function start: 0x452FAA */
int __cdecl ReleaseBufferEntry(unsigned int param_1)
{
    int result;

    result = 0;
    if (param_1 <= 0x1f && (char)param_1 != g_CurrentVideoBuffer_0046db3c) {
        g_VBufDataPtrs_0046de54[param_1] = result;
    }
    return result;
}

// Forward declaration
int __cdecl VideoFillRect(int param_1, int param_2, int param_3, int param_4);

/* Function start: 0x4531D8 */
int __cdecl CreateTableFromBuffer(int buffer, int width, int height)
{
    int* ptr;
    int counter;
    int val;

    counter = 0x20;
    ptr = g_VBufDataPtrs_0046de54;
    if (g_WinGDC_0046e004 == 0) {
        do {
            val = *ptr;
            ptr++;
            counter--;
        } while (counter != 0 && val != 0);
        if (val == 0) {
            int idx = 0x1f - counter;
            ptr[-1] = buffer + g_BitmapHeaderSize_0046db34;
            int* dest = (int*)buffer;
            for (int j = 10; j != 0; j--) {
                *dest = 0;
                dest++;
            }
            dest[-10] = 0x28;
            dest[-9] = width;
            dest[-8] = height;
            *(short*)(dest - 7) = 1;
            *(short*)((char*)(dest - 7) + 2) = 8;
            g_VBufWidths_0046df54[idx] = width;
            g_VBufHeights_0046ded4[idx] = height;
            g_VBufMaxX_0046dc54[idx] = width - 1;
            g_VBufMaxY_0046dcd4[idx] = height - 1;
            g_VBufClipLeft_0046dd54[idx] = 0;
            g_VBufClipTop_0046ddd4[idx] = 0;

            return idx;
        }
    }
    return -1;
}

/* Function start: 0x4524D9 */
int __cdecl VideoFillRect(int param_1, int param_2, int param_3, int param_4)
{
    int iVar1;
    int iVar2;
    int count;
    unsigned char* puVar1;

    if ((signed char)g_CurrentVideoBuffer_0046db3c >= 0) {
        iVar1 = g_VideoBufferHeightM1_0046d0b2 - param_4;
        iVar2 = g_VideoBufferHeightM1_0046d0b2 - param_3;
        count = (1 - iVar1) + iVar2;
        int rowWidth = (param_2 + 1) - param_1;
        int stride = g_VideoBufferStride_0046db46 - rowWidth;
        puVar1 = (unsigned char*)(param_1 + iVar1 * g_VideoBufferStride_0046db46 + g_VideoBufferBase_0046db4e);

        do {
            memset(puVar1, (char)g_FillColorDword_0046d079, rowWidth);
            puVar1 += rowWidth + stride;
            count--;
        } while (count != 0);
    }
    return 0;
}

// Forward declaration for ClipAndVideoFillRect
int __cdecl ClipAndVideoFillRect(int param_1, int param_2, int param_3, int param_4);

/* Function start: 0x452556 */
int __cdecl DrawRectOutline(int param_1, int param_2, int param_3, int param_4)
{
    int iVar1;
    int iVar2;

    // Top edge
    iVar2 = g_LineWidthV_0046d0c2;
    iVar1 = param_3 + g_LineWidthV_0046d0c2 + -1;
    if (param_4 < iVar1) {
        iVar1 = param_4;
    }
    ClipAndVideoFillRect(param_1, param_2, param_3, iVar1);

    // Bottom edge
    iVar2 = (param_4 - iVar2) + 1;
    if (iVar2 < param_3) {
        iVar2 = param_3;
    }
    ClipAndVideoFillRect(param_1, param_2, iVar2, param_4);

    // Left edge
    iVar2 = g_LineWidthH_0046d0be;
    iVar1 = param_1 + g_LineWidthH_0046d0be + -1;
    if (param_2 < iVar1) {
        iVar1 = param_2;
    }
    ClipAndVideoFillRect(param_1, iVar1, param_3, param_4);

    // Right edge
    iVar2 = (param_2 - iVar2) + 1;
    if (iVar2 < param_1) {
        iVar2 = param_1;
    }
    ClipAndVideoFillRect(iVar2, param_2, param_3, param_4);

    return 0;
}

/* Function start: 0x453D37 */
int __cdecl ClipAndVideoFillRect(int param_1, int param_2, int param_3, int param_4)
{
    if (param_1 <= g_ClipRight_0046d0ca) {
        if (param_1 < g_ClipLeft_0046d0c6) {
            param_1 = g_ClipLeft_0046d0c6;
        }
        if (g_ClipLeft_0046d0c6 <= param_2) {
            if (g_ClipRight_0046d0ca < param_2) {
                param_2 = g_ClipRight_0046d0ca;
            }
            if (param_3 <= g_ClipBottom_0046d0d2) {
                if (param_3 < g_ClipTop_0046d0ce) {
                    param_3 = g_ClipTop_0046d0ce;
                }
                if (g_ClipTop_0046d0ce <= param_4) {
                    if (g_ClipBottom_0046d0d2 < param_4) {
                        param_4 = g_ClipBottom_0046d0d2;
                    }
                    VideoFillRect(param_1, param_2, param_3, param_4);
                }
            }
        }
    }
    return 0;
}
