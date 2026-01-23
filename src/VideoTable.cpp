#include "globals.h"
#include <windows.h>
#include <string.h>

// g_WinGDC_0043841c is already declared in globals.h
extern HDC DAT_00437488;
extern int DAT_004374ca;
extern void* DAT_00438438;
extern void* DAT_00438434;

/* Function start: 0x4229EA */
extern "C" int __cdecl SetDrawPosition(int param_1, int param_2)
{
    DAT_004374c2 = param_1;
    DAT_004374ce = param_2;
    return 0;
}

/* Function start: 0x422A2F */
extern "C" int __cdecl SetDrawColors(unsigned char param_1, unsigned char param_2)
{
    DAT_004374c0 = param_1;
    DAT_004374c1 = param_2;
    return 0;
}

/* Function start: 0x422E1A */
extern "C" int __cdecl ReleaseVideoBuffer(unsigned int param_1)
{
    HGLOBAL hMem;
    int* puVar2;
    int iVar1;
    
    if (param_1 < 0x20 && (char)param_1 != *(char*)&DAT_00437f54) {
        iVar1 = param_1 * 4;
        DAT_0043826c[param_1] = 0;
        puVar2 = DAT_00437fec;
        if (g_WinGDC_0043841c != 0) {
            DeleteObject((HGDIOBJ)DAT_00437fec[param_1]);
            puVar2 = DAT_00437f6c;
        }
        hMem = (HGLOBAL)puVar2[param_1];
        GlobalUnlock(hMem);
        GlobalFree(hMem);
    }
    return 0;
}

/* Function start: 0x422E71 */
extern "C" int __cdecl GetVideoBufferData(unsigned int handle)
{
    if (handle > 0x1f) {
        return 0;
    }
    return ((int*)&DAT_0043826c)[handle];
}

/* Function start: 0x422F00 */
extern "C" int __cdecl CreateTable(int width, int height) {
    int i = 0;
    while (i < 32 && ((int*)&DAT_0043826c)[i] != 0) {
        i++;
    }

    if (i == 32) {
        return -1;
    }

    unsigned int aligned_width = (width + 3) & 0xFFFFFFFC;
    int image_size;

    if (g_WinGDC_0043841c == 0) {
        image_size = aligned_width * height;
    } else {
        image_size = 1064 - DAT_00437f4c;
    }

    HGLOBAL hMem = GlobalAlloc(0x42, image_size + DAT_00437f4c);
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
    int data_offset = DAT_00437f4c;

    if (g_WinGDC_0043841c != 0) {
        hDib = ((HGLOBAL(*)(HDC, BITMAPINFO*, void**))DAT_00438428)(g_WinGDC_0043841c, bmi, (void**)&bmi);
        if (hDib == 0) {
            GlobalUnlock(hMem);
            GlobalFree(hMem);
            return -2;
        }
        HGDIOBJ oldObj = SelectObject(g_WinGDC_0043841c, hDib);
        if (DAT_00438424 == 0) {
            DAT_00438424 = oldObj;
        }
        // FUN_00423076 - Clear the newly created DIB buffer
        // This function reads from caller's EBP frame, so we inline it here
        memset(bmi, 0, aligned_width * height);
        SelectObject(g_WinGDC_0043841c, oldObj);
        data_offset = 0;
    }

    ((HGLOBAL*)&DAT_00437fec)[i] = hDib;
    ((HGLOBAL*)&DAT_00437f6c)[i] = hMem;
    ((void**)&DAT_0043826c)[i] = (void*)((char*)bmi + data_offset);
    ((unsigned int*)&DAT_0043836c)[i] = aligned_width;
    ((int*)&DAT_004382ec)[i] = height;
    ((unsigned int*)&DAT_0043806c)[i] = aligned_width - 1;
    ((int*)&DAT_004380ec)[i] = height - 1;
    ((int*)&DAT_0043816c)[i] = 0;
    ((int*)&DAT_004381ec)[i] = 0;

    return i;
}

/* Function start: 0x423099 */
extern "C" int ClearVideoBuffer(void) {
    if (*(char*)&DAT_00437f54 >= 0) {
        memset((char*)DAT_00437f66, DAT_00437491, DAT_00437f62);
    }
    return 0;
}

// Video table globals (defined in globals.cpp)
extern int DAT_004374c6;
extern int DAT_00437f5e;
extern int DAT_004374d2;
extern int DAT_004374d6;  // Line width horizontal
extern int DAT_004374da;  // Line width vertical
extern int DAT_004374de;
extern int DAT_004374e2;
extern int DAT_004374e6;
extern int DAT_004374ea;

/* Function start: 0x4230D9 */
extern "C" int __cdecl SelectVideoBuffer(int param_1) {
    if (param_1 < 0x20) {
        if (DAT_0043826c[param_1] == 0) {
            // Buffer invalid/empty
            return 0xfffffffe; 
        } else {
            // Set current video mode
            if (g_WinGDC_0043841c != 0) {
                // WinG or similar context selection
                SelectObject(g_WinGDC_0043841c, (HGDIOBJ)DAT_00437fec[param_1]);
            }
            
            *(char*)&DAT_00437f54 = (char)param_1;
            int width = DAT_0043836c[param_1];
            DAT_004374c6 = width;
            DAT_00437f5e = width;
            
            int height = DAT_004382ec[param_1];
            DAT_004374d2 = height;
            DAT_004374ca = height - 1;
            
            DAT_004374de = DAT_0043816c[param_1];
            DAT_004374e2 = DAT_0043806c[param_1];
            DAT_004374e6 = DAT_004381ec[param_1];
            DAT_004374ea = DAT_004380ec[param_1];
            
            DAT_00437f66 = DAT_0043826c[param_1];
            
            // Replicating DS capture from assembly if possible, otherwise ignore
            // _asm { mov word ptr [DAT_00437f6a], ds }
             
            // Calculate size
            unsigned int size = (unsigned int)DAT_004374c6 * (unsigned int)DAT_004374d2;
            DAT_00437f62 = (int)size; // Low 32 bits
            
            return 0;
        }
    }
    return 0xffffffff;
}

typedef int (__stdcall *WinGBitBltFunc)(HDC, int, int, int, int, HDC, int, int);
typedef int (__stdcall *WinGStretchBltFunc)(HDC, int, int, int, int, HDC, int, int, int, int);

/* Function start: 0x423296 */
extern "C" int __cdecl BlitToDevice(int param_1, int param_2, int param_3, int param_4, int param_5, int param_6)
{
    int iVar1;
    int w;
    int h;

    if (*(char*)&DAT_00437f54 >= 0) {
        if (g_WinGDC_0043841c == 0) {
            // Non-WinG path: flip Y coordinates
            iVar1 = DAT_004374ca - param_3;
            param_3 = DAT_004374ca - param_4;
            param_4 = iVar1;
        }
        
        iVar1 = (param_6 + param_3) - param_4;
        w = (param_2 - param_1) + 1;
        h = (param_4 - param_3) + 1;
        
        if (g_WinGDC_0043841c == 0) {
            // Use SetDIBitsToDevice
            int bitmapInfo = DAT_00437f66 - DAT_00437f4c;
            SetDIBitsToDevice(DAT_00437488, param_5, iVar1, w, h, param_1, param_3, 0, DAT_004374d2,
                              (void*)(bitmapInfo + DAT_00437f4c),
                              (BITMAPINFO*)bitmapInfo,
                              DAT_00437f50);
        } else {
            // Use WinG BitBlt via function pointer
            ((WinGBitBltFunc)DAT_00438434)(DAT_00437488, param_5, iVar1, w, h, g_WinGDC_0043841c, param_1, param_3);
        }
    }
    return 0;
}

/* Function start: 0x42333A */
extern "C" int __cdecl StretchBlitBuffer(int srcX1, int srcX2, int srcY1, int srcY2, int destX1, int destX2, int destY1, int destY2)
{
    int srcWidth;
    int srcHeight;
    int destWidth;
    int destHeight;
    int wingDC;
    int newY2;

    if (*(char*)&DAT_00437f54 >= 0) {
        wingDC = (int)g_WinGDC_0043841c;
        if (wingDC != 0) {
            newY2 = srcY2;
        }
        else {
            newY2 = DAT_004374ca - srcY1;
            srcY1 = DAT_004374ca - srcY2;
        }

        srcWidth = (srcX2 - srcX1) + 1;
        destWidth = (destX2 - destX1) + 1;
        destHeight = (destY2 - destY1) + 1;
        srcHeight = (newY2 - srcY1) + 1;

        if (wingDC != 0) {
            ((WinGStretchBltFunc)DAT_00438438)(DAT_00437488, destX1, destY1, destWidth, destHeight, (HDC)wingDC, srcX1, srcY1, srcWidth, srcHeight);
        }
        else {
            int bitmapInfo = DAT_00437f66 - DAT_00437f4c;
            StretchDIBits(DAT_00437488, destX1, destY1, destWidth, destHeight, srcX1, srcY1, srcWidth, srcHeight,
                          (void*)(bitmapInfo + DAT_00437f4c),
                          (BITMAPINFO*)bitmapInfo,
                          DAT_00437f50, 0xcc0020);
        }
    }
    return 0;
}

/* Function start: 0x4234D5 */
extern "C" void __cdecl ReleaseBufferEntry(unsigned int param_1)
{
    int* arr;
    unsigned int idx;

    idx = param_1;
    if (idx > 0x1f) {
        return;
    }
    if ((char)idx == *(char*)&DAT_00437f54) {
        return;
    }
    arr = DAT_0043826c;
    arr[idx] = 0;
}

// Forward declaration
extern "C" int __cdecl VideoFillRect(int param_1, int param_2, int param_3, int param_4);

/* Function start: 0x423703 */
extern "C" int __cdecl CreateTableFromBuffer(int buffer, int width, int height)
{
    int* ptr;
    int counter;
    int val;

    counter = 0x20;
    ptr = DAT_0043826c;
    if (g_WinGDC_0043841c == 0) {
        do {
            val = *ptr;
            ptr++;
            counter--;
        } while (counter != 0 && val != 0);
        if (val == 0) {
            int idx = 0x1f - counter;
            ptr[-1] = buffer + DAT_00437f4c;
            int offset = idx * 4;
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
            DAT_0043836c[idx] = width;
            DAT_004382ec[idx] = height;
            DAT_0043806c[idx] = width - 1;
            DAT_004380ec[idx] = height - 1;
            DAT_0043816c[idx] = 0;
            DAT_004381ec[idx] = 0;
            return idx;
        }
    }
    return -1;
}

/* Function start: 0x4237C6 */
extern "C" int __cdecl VideoFillRect(int param_1, int param_2, int param_3, int param_4)
{
    int iVar1;
    int iVar2;
    int count;
    unsigned char* puVar1;

    if ((signed char)DAT_00437f54 >= 0) {
        iVar1 = DAT_004374ca - param_4;
        iVar2 = DAT_004374ca - param_3;
        count = (1 - iVar1) + iVar2;
        int rowWidth = (param_2 + 1) - param_1;
        int stride = DAT_00437f5e - rowWidth;
        puVar1 = (unsigned char*)(param_1 + iVar1 * DAT_00437f5e + DAT_00437f66);

        do {
            int remaining = rowWidth;
            // Fill by dwords first
            while (remaining >= 4) {
                *(unsigned int*)puVar1 = DAT_00437491;
                puVar1 += 4;
                remaining -= 4;
            }
            // Fill remaining word
            if (remaining >= 2) {
                *(unsigned short*)puVar1 = (unsigned short)DAT_00437491;
                puVar1 += 2;
                remaining -= 2;
            }
            // Fill remaining byte
            if (remaining >= 1) {
                *puVar1 = (unsigned char)DAT_00437491;
                puVar1 += 1;
            }
            puVar1 += stride;
            count--;
        } while (count != 0);
    }
    return 0;
}

// Forward declaration for ClipAndVideoFillRect
extern "C" int __cdecl ClipAndVideoFillRect(int param_1, int param_2, int param_3, int param_4);

/* Function start: 0x423843 */
extern "C" int __cdecl DrawRectOutline(int param_1, int param_2, int param_3, int param_4)
{
    int iVar1;
    int iVar2;

    // Top edge
    iVar2 = DAT_004374da;
    iVar1 = param_3 + DAT_004374da + -1;
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
    iVar2 = DAT_004374d6;
    iVar1 = param_1 + DAT_004374d6 + -1;
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

/* Function start: 0x424423 */
extern "C" int __cdecl ClipAndVideoFillRect(int param_1, int param_2, int param_3, int param_4)
{
    if (param_1 <= DAT_004374e2) {
        if (param_1 < DAT_004374de) {
            param_1 = DAT_004374de;
        }
        if (DAT_004374de <= param_2) {
            if (DAT_004374e2 < param_2) {
                param_2 = DAT_004374e2;
            }
            if (param_3 <= DAT_004374ea) {
                if (param_3 < DAT_004374e6) {
                    param_3 = DAT_004374e6;
                }
                if (DAT_004374e6 <= param_4) {
                    if (DAT_004374ea < param_4) {
                        param_4 = DAT_004374ea;
                    }
                    VideoFillRect(param_1, param_2, param_3, param_4);
                }
            }
        }
    }
    return 0;
}