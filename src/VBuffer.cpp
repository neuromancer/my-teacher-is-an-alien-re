#include "VBuffer.h"
#include <stdlib.h>
#include <windows.h>
#include "string.h"

static int g_VBufferHandleTableInitialized = 0;
static int g_VBufferHandleTable[0x20];

/* Function start: 0x41A9A0 */
void InitVBufferHandleTable(void)
{
    if (g_VBufferHandleTableInitialized == 0) {
        int* ptr = g_VBufferHandleTable;
        for (int i = 0x20; i != 0; i--) {
            *ptr = 0xffffffff;
            ptr++;
        }
        g_VBufferHandleTableInitialized = 1;
    }
}

/* Function start: 0x41A9D0 */
void RegisterVBufferHandle(int handle)
{
    g_VBufferHandleTable[handle] = handle;
}

/* Function start: 0x41A9E0 */
void ReleaseVBufferHandle(int handle)
{
    g_VBufferHandleTable[handle] = 0xffffffff;
}

extern "C" {
    extern int DAT_0043826c;
    extern HDC h_0043841c;
    extern int DAT_00437f4c;
    extern int DAT_00438428;
    extern HGDIOBJ DAT_00438424;
    extern int DAT_00437fec;
    extern int DAT_00437f6c;
    extern int DAT_0043836c;
    extern int DAT_004382ec;
    extern int DAT_0043806c;
    extern int DAT_004380ec;
    extern int DAT_0043816c;
    extern int DAT_004381ec;
    void FUN_00423076();
    extern int DAT_00437f54;
    extern int DAT_00437f66;
    extern int DAT_00437f62;
    extern int DAT_00437491;
}

// Forward declarations
void CopyRowTransparent(char* dest, char* src, int count, char transparentColor, char fillColor);
void BlitTransparentRows(int x1, int x2, int y1, int y2, int destX, int destY, VBuffer* srcBuffer, VBuffer* destBuffer, char transparentColor, char fillColor);

extern "C" {
    void ClearScreen();
    void FUN_00422e8f();
    int FUN_00422a01(unsigned int);
    int FUN_00422e71(unsigned int);
    unsigned int GetCurrentVideoMode();
    void InvalidateVideoMode();
    void FUN_0041ae0c(void);
    void FUN_004231ce(int, int, int, int, int, int, unsigned int, unsigned int);
    void FUN_004233e8(int, int, int, int, int, int, unsigned int, unsigned int);
    int FUN_0041b590(int*, int*, int*, int*);
    void FUN_0041af9f(void);
    void FUN_0041afb1(void);
    void FUN_0041b29a(void);
    void FUN_0041b2ac(void);
    int FUN_004230d9(int);
    void FUN_00423296(int, int, int, int, int, int);
    void FUN_0042333a(int, int, int, int, int, int, int, int);
    __int64 __ftol();
    unsigned int FUN_00423703(int, unsigned int, unsigned int);
    void FUN_004234f9(void*, void*, unsigned int, unsigned int, unsigned int, unsigned int);
    void FUN_004234d5(unsigned int);
    extern int DAT_00436964;
    void FUN_00422e1a(int);
}

/* Function start: 0x41a9f0 */
VBuffer* VirtualBufferCreateAndClean(VBuffer* vbuffer, int width, int height)
{
    vbuffer->InitFields();
    vbuffer->VBuffer::VBuffer(width, height);
    return vbuffer;
}

/* Function start: 0x41aa10 */
VBuffer::~VBuffer()
{
    Release();
    Free();
}

/* Function start: 0x41aa30 */
void VBuffer::InitFields()
{
    for (int i = 0; i < 0xc; i++) {
        ((int*)this)[i] = 0;
    }
    this->handle = 0xffffffff;
    *(int*)this = 0xffffffff;
    InitVBufferHandleTable();
}

/* Function start: 0x41aa60 */
void VBuffer::Free()
{
    if (this->data != 0) {
        FUN_00422e1a(this->handle);
        ReleaseVBufferHandle(this->handle);
        this->handle = 0xffffffff;
        this->data = 0;
    }
}

/* Function start: 0x41aaa0 */
VBuffer::VBuffer(unsigned int param_1, unsigned int param_2)
{
    if ((param_1 == 0) || (param_2 == 0)) {
        ShowError("Error! Invalid buffer size specified =>%lu", param_2 * param_1);
    }
    this->width = param_1;
    this->height = param_2;
    this->clip_x1 = 0;
    this->clip_x2 = param_1 - 1;
    this->saved_video_mode = 0;
    this->video_mode_lock_count = param_2 - 1;
    if (this->data != 0) {
        ShowError("Error! Virtual buffer already allocated");
    }

    unsigned int uVar2 = (int)param_1 >> 0x1f;
    if (((param_1 ^ uVar2) - uVar2 & 3 ^ uVar2) != uVar2) {
        do {
            param_1 = param_1 + 1;
            uVar2 = (int)param_1 >> 0x1f;
        } while (((param_1 ^ uVar2) - uVar2 & 3 ^ uVar2) != uVar2);
    }
    uVar2 = (int)param_2 >> 0x1f;
    if (((param_2 ^ uVar2) - uVar2 & 3 ^ uVar2) != uVar2) {
        do {
            param_2 = param_2 + 1;
            uVar2 = (int)param_2 >> 0x1f;
        } while (((param_2 ^ uVar2) - uVar2 & 3 ^ uVar2) != uVar2);
    }
    uVar2 = CreateTable(param_1, param_2);
    this->handle = uVar2;
    if (uVar2 == 0xffffffff) {
        ShowError("VBuffer::Init - Unable To create vb. Table Full");
    }
    if (uVar2 == 0xfffffffe) {
        ShowError("VBuffer::Init - Unable To create vb. No memory");
    }
    this->SetCurrentVideoMode(uVar2);
    FUN_00422e8f();
    this->InvalidateVideoMode();
    this->data = (void*)FUN_00422e71(this->handle);
    RegisterVBufferHandle(this->handle);
}

/* Function start: 0x41abc0 */
void VBuffer::ClearScreen(int color)
{
    this->SetCurrentVideoMode(this->handle);
    FUN_00422a01(color);
    FUN_00423099();
    this->InvalidateVideoMode();
}

/* Function start: 0x41abf0 */
int VBuffer::SetVideoMode()
{
    int iVar1 = FUN_004230d9(this->handle);
    this->saved_video_mode = 0xffffffff;
    this->video_mode_lock_count = 1;
    return iVar1;
}

/* Function start: 0x41ac10 */
void VBuffer::Release()
{
    unsigned int uVar1 = GetCurrentVideoMode();
    if (uVar1 == this->handle) {
        this->InvalidateVideoMode();
    }
}

/* Function start: 0x41ac30 */
void* VBuffer::GetData()
{
    return this->data;
}

/* Function start: 0x41ac40 */
void VBuffer::Lock()
{
}

/* Function start: 0x41ac50 */
int VBuffer::SetCurrentVideoMode(int param_1)
{
    this->video_mode_lock_count = this->video_mode_lock_count + 1;
    unsigned int uVar1 = GetCurrentVideoMode();
    if (param_1 != uVar1) {
        this->saved_video_mode = uVar1;
        uVar1 = FUN_004230d9(param_1);
    }
    return uVar1;
}

/* Function start: 0x41ac80 */
void VBuffer::InvalidateVideoMode()
{
    if ((this->video_mode_lock_count != 0) && (this->video_mode_lock_count = this->video_mode_lock_count - 1, this->video_mode_lock_count == 0)) {
        ::InvalidateVideoMode();
        if (this->saved_video_mode != 0xffffffff) {
            FUN_004230d9(this->saved_video_mode);
            this->saved_video_mode = 0xffffffff;
        }
    }
}

/* Function start: 0x41acb0 */
void VBuffer::CallBlitter4(int param_1, int param_2, int param_3, int param_4, int param_5, int param_6)
{
    this->SetCurrentVideoMode(this->handle);
    FUN_00423296(param_1, param_2, param_3, param_4, param_5, param_6);
    this->InvalidateVideoMode();
}

/* Function start: 0x41acf0 */
void VBuffer::CallBlitter5(int param_1, int param_2, int param_3, int param_4, int param_5, int param_6, int param_7, int param_8)
{
    this->SetCurrentVideoMode(this->handle);
    FUN_0042333a(param_1, param_2, param_3, param_4, param_5, param_6, param_7, param_8);
    this->InvalidateVideoMode();
}

/* Function start: 0x41ad40 */
void VBuffer::TPaste(void)
{
    ShowError("VBuffer::TPaste - Not implemented");
}

/* Function start: 0x41ad50 */
void VBuffer::BlitTransparent(int param_1, int param_2, int param_3, int param_4, int param_5, int param_6, char param_7, char param_8)
{
    VBuffer* local_40 = (VBuffer*)malloc(sizeof(VBuffer));

    if (local_40) {
        VirtualBufferCreateAndClean(local_40, (param_2 - param_1) + 1, (param_4 - param_3) + 1);
        local_40->ClearScreen(0);
        local_40->BlitTransparentRowsFrom(param_1, param_2, param_3, param_4, param_5, param_6, this, param_7, param_8);
        local_40->TPaste();
        local_40->~VBuffer();
        free(local_40);
    }

    try {
        FUN_0041ae0c();
    } catch (...) {

    }
}

/* Function start: 0x41ae20 */
void VBuffer::CallBlitter(int param_1, int param_2, int param_3, int param_4, int param_5, int param_6, int param_7)
{
    FUN_004231ce(param_1, param_2, param_3, param_4, param_5, param_6, ((VBuffer*)param_7)->handle, this->handle);
}

/* Function start: 0x41ae60 */
void VBuffer::CallBlitter2(int param_1, int param_2, int param_3, int param_4, int param_5, int param_6, VBuffer* srcBuffer)
{
    FUN_004233e8(param_1, param_2, param_3, param_4, param_5, param_6, srcBuffer->handle, this->handle);
}

/* Function start: 0x41aea0 */
void VBuffer::CallBlitter3(int param_1, int param_2, int param_3, int param_4, int param_5, int param_6, VBuffer* srcBuffer, char param_8, char param_9)
{
    this->BlitTransparentRowsFrom(param_1, param_2, param_3, param_4, param_5, param_6, srcBuffer, param_8, param_9);
}

/* Function start: 0x41aee0 */
void VBuffer::ClipAndBlit(int param_1, int param_2, int param_3, int param_4, int param_5, int param_6, int param_7)
{
    int local_30 = this->clip_x1;
    int local_28 = this->clip_x2;
    int local_2c = this->saved_video_mode;
    int local_24 = this->video_mode_lock_count;
    int local_20 = param_1;
    int local_1c = param_3;
    int local_18 = param_2;
    int local_14 = param_4;

    __try
    {
        if (FUN_0041b590(&local_30, &local_20, &param_5, &param_6))
        {
            this->CallBlitter(local_20, local_18, local_1c, local_14, param_5, param_6, param_7);
        }
    }
    __finally
    {
        FUN_0041af9f();
        FUN_0041afb1();
    }
}

/* Function start: 0x41AFC0 */
void VBuffer::ClipAndPaste(int param_1, int param_2, int param_3, int param_4, int param_5, int param_6, int param_7)
{
    // TODO: Implement - similar to ClipAndBlit
}

/* Function start: 0x41B110 */
void VBuffer::ScaleTCCopy(int param_1, int param_2, VBuffer* srcBuffer, double scale)
{
    int scaledWidth;
    int scaledHeight;
    void* puVar3;
    int iVar4;

    int local_30[4];  // clip_x1, video_mode_lock_count, clip_x2, saved_video_mode
    int local_20[4];  // x1, y1, scaledWidth-1, scaledHeight-1
    int local_10;

    scaledWidth = (int)(srcBuffer->width * scale);
    scaledHeight = (int)(srcBuffer->height * scale);

    if ((scaledWidth >= 1) && (scaledHeight >= 1)) {
        local_10 = FUN_00423703(DAT_00436964, scaledWidth, scaledHeight);
        if (local_10 < 0) {
            ShowError("VBuffer::ScaleTCCopy");
        }
        puVar3 = (void*)FUN_00422e71(local_10);
        FUN_004234f9(srcBuffer->data, puVar3, srcBuffer->width, srcBuffer->height, scaledWidth, scaledHeight);

        __try {
            local_30[0] = 0;
            local_30[1] = 0;
            local_30[2] = 0;
            local_30[3] = 0;
            local_30[3] = this->saved_video_mode;
            local_30[2] = this->clip_x2;
            local_20[0] = 0;
            local_20[1] = 0;
            local_20[2] = 0;
            local_20[3] = 0;
            local_30[0] = this->clip_x1;
            local_30[1] = this->video_mode_lock_count;
            local_20[2] = scaledWidth - 1;
            local_20[3] = scaledHeight - 1;
            local_20[0] = 0;
            local_20[1] = 0;

            iVar4 = FUN_0041b590(local_30, local_20, &param_1, &param_2);
            if (iVar4 != 0) {
                FUN_004233e8(local_20[0], local_20[2], local_20[1], local_20[3], param_1, param_2, local_10, this->handle);
                FUN_004234d5(local_10);
            }
        } __finally {
            FUN_0041b29a();
            FUN_0041b2ac();
        }
    }
}

/* Function start: 0x41B2C0 */
void CopyRowTransparent(char* dest, char* src, int count, char transparentColor, char fillColor)
{
    if (count != 0) {
        do {
            while (*src == 0) {
                dest++;
                src++;
                count--;
                if (count == 0) {
                    return;
                }
            }
            do {
                if (count == 0) {
                    return;
                }
                char pixel = *src;
                *dest = fillColor;
                if (pixel != transparentColor) {
                    *dest = pixel;
                }
                dest++;
                src++;
                count--;
            } while (*src != 0);
        } while (count != 0);
    }
}

/* Function start: 0x41B310 */
// Member implementation: copy rows from srcBuffer into this VBuffer
void VBuffer::BlitTransparentRowsFrom(int x1, int x2, int y1, int y2, int destX, int destY, VBuffer* srcBuffer, char transparentColor, char fillColor)
{
    int rowCount = (y2 - y1) + 1;
    int srcPitch = srcBuffer->width;
    char* srcData = (char*)srcBuffer->GetData();
    char* srcRow = srcData + (srcBuffer->video_mode_lock_count - y2) * srcPitch + x1;

    int destPitch = this->width;
    char* destData = (char*)this->GetData();
    char* destRow = destData + (this->video_mode_lock_count - destY) * destPitch + destX;

    if (rowCount > 0) {
        do {
            CopyRowTransparent(destRow, srcRow, (x2 - x1) + 1, transparentColor, fillColor);
            destRow += destPitch;
            srcRow += srcPitch;
            rowCount--;
        } while (rowCount != 0);
    }
    this->Lock();
    srcBuffer->Lock();
}

// Preserve original free function symbol as a thin wrapper that delegates to the member
void BlitTransparentRows(int x1, int x2, int y1, int y2, int destX, int destY, VBuffer* srcBuffer, VBuffer* destBuffer, char transparentColor, char fillColor)
{
    if (destBuffer != NULL) {
        destBuffer->BlitTransparentRowsFrom(x1, x2, y1, y2, destX, destY, srcBuffer, transparentColor, fillColor);
    }
}

#include <string.h>

/* Function start: 0x423099 */
int FUN_00423099(void) {
    if (*(char*)&DAT_00437f54) {
        memset((char*)DAT_00437f66, DAT_00437491, DAT_00437f62);
    }
    return 0;
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
