#include "globals.h"
#include <windows.h>
#include <string.h>

extern "C" {
    void FUN_00423076();
}

/* Function start: 0x422E71 */
int __cdecl GetVideoBufferData(unsigned int handle)
{
    if (handle > 0x1f) {
        return 0;
    }
    return ((int*)&DAT_0043826c)[handle];
}

/* Function start: 0x422F00 */
int __cdecl CreateTable(int width, int height) {
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
int ClearVideoBuffer(void) {
    if (*(char*)&DAT_00437f54 >= 0) {
        memset((char*)DAT_00437f66, DAT_00437491, DAT_00437f62);
    }
    return 0;
}
