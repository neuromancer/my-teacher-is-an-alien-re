#include "globals.h"
#include <windows.h>
#include <string.h>

extern "C" {
    void FUN_00423076();
}

extern int DAT_0043841c;
extern HDC DAT_00437488;
extern int DAT_004374ca;
extern void* DAT_00438438;

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

    if (h_0043841c == 0) {
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

    if (h_0043841c != 0) {
        hDib = ((HGLOBAL(*)(HDC, BITMAPINFO*, void**))DAT_00438428)(h_0043841c, bmi, (void**)&bmi);
        if (hDib == 0) {
            GlobalUnlock(hMem);
            GlobalFree(hMem);
            return -2;
        }
        HGDIOBJ oldObj = SelectObject(h_0043841c, hDib);
        if (DAT_00438424 == 0) {
            DAT_00438424 = oldObj;
        }
        FUN_00423076();
        SelectObject(h_0043841c, oldObj);
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
/* Function start: 0x423703 */
extern "C" int __cdecl CreateTableFromBuffer(int buffer, int width, int height)
{
    int* ptr;
    int counter;
    int val;

    counter = 0x20;
    ptr = DAT_0043826c;
    if (DAT_0043841c == 0) {
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

typedef int (__stdcall *WinGStretchBltFunc)(HDC, int, int, int, int, HDC, int, int, int, int);

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
        wingDC = DAT_0043841c;
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