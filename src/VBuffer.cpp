#include "globals.h"
#include "VBuffer.h"
#include "VideoTable.h"
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
    void FUN_00423076();
}

// Forward declarations
void CopyRowTransparent(char* dest, char* src, int count, char transparentColor, char fillColor);
void BlitTransparentRows(int x1, int x2, int y1, int y2, int destX, int destY, VBuffer* srcBuffer, VBuffer* destBuffer, char transparentColor, char fillColor);

extern "C" {
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
    unsigned int FUN_00423703(int, unsigned int, unsigned int);
    void FUN_004234f9(void*, void*, unsigned int, unsigned int, unsigned int, unsigned int);
    void FUN_004234d5(unsigned int);
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
    int iVar1 = CreateTable(param_1, param_2);
    this->handle = iVar1;
    if (iVar1 == -1) {
        ShowError("VBuffer::Init - Unable To create vb. Table Full");
        return;
    }
    if (iVar1 == -2) {
        ShowError("VBuffer::Init - Unable To create vb. No memory");
        return;
    }
    this->SetCurrentVideoMode(iVar1);
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
    ClearVideoBuffer();
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
void VBuffer::TPaste(int param_1, int param_2, int param_3, int param_4, int param_5, int param_6)
{
    ShowError("VBuffer::TPaste - Not implemented");
}

/* Function start: 0x41ad50 */
void VBuffer::BlitTransparent(int param_1, int param_2, int param_3, int param_4, int param_5, int param_6, char param_7, char param_8)
{
    int local_40_storage[sizeof(VBuffer) / sizeof(int)];
    VBuffer* local_40 = (VBuffer*)local_40_storage;
    VirtualBufferCreateAndClean(local_40, (param_2 - param_1) + 1, (param_4 - param_3) + 1);
    __try {
        local_40->ClearScreen(0);
        BlitTransparentRows(param_1, param_2, param_3, param_4, local_40->saved_video_mode, local_40->video_mode_lock_count, this, local_40, param_7, param_8);
        local_40->TPaste(local_40->clip_x1, local_40->clip_x2, local_40->saved_video_mode, local_40->video_mode_lock_count, param_5, param_6);
    } __finally {
        FUN_0041ae0c();
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
    int local_2c[4];
    int local_1c[4];

    local_2c[0] = this->clip_x1;
    local_2c[2] = this->clip_x2;
    local_2c[1] = this->saved_video_mode;
    local_2c[3] = this->video_mode_lock_count;
    local_1c[0] = param_1;
    local_1c[2] = param_3;
    local_1c[1] = param_2;
    local_1c[3] = param_4;

    __try
    {
        if (FUN_0041b590(local_2c, local_1c, &param_5, &param_6))
        {
            this->CallBlitter(local_1c[0], local_1c[1], local_1c[2], local_1c[3], param_5, param_6, param_7);
        }
    }
    __finally
    {
        FUN_0041af9f();
        FUN_0041afb1();
    }
}

extern "C" {
    void FUN_0041b07f(void);
    void FUN_0041b091(void);
}

/* Function start: 0x41AFC0 */
void VBuffer::ClipAndPaste(int param_1, int param_2, int param_3, int param_4, int param_5, int param_6, int param_7)
{
    int local_2c[4];
    int local_1c[4];

    local_2c[0] = this->clip_x1;
    local_2c[2] = this->clip_x2;
    local_2c[1] = this->saved_video_mode;
    local_2c[3] = this->video_mode_lock_count;
    local_1c[0] = param_1;
    local_1c[1] = param_3;
    local_1c[2] = param_2;
    local_1c[3] = param_4;

    __try
    {
        if (FUN_0041b590(local_2c, local_1c, &param_5, &param_6))
        {
            this->CallBlitter2(local_1c[0], local_1c[2], local_1c[1], local_1c[3], param_5, param_6, (VBuffer*)param_7);
        }
    }
    __finally
    {
        FUN_0041b07f();
        FUN_0041b091();
    }
}

/* Function start: 0x41B110 */
void VBuffer::ScaleTCCopy(int param_1, int param_2, VBuffer* srcBuffer, double scale)
{
    int local_30[4];
    int local_20[4];
    int local_10;

    int scaledWidth = (int)(srcBuffer->width * scale);
    int scaledHeight = (int)(srcBuffer->height * scale);

    if ((scaledWidth >= 1) && (scaledHeight >= 1)) {
        local_10 = FUN_00423703((int)g_Buffer_00436964, scaledWidth, scaledHeight);
        if (local_10 < 0) {
            ShowError("VBuffer::ScaleTCCopy");
        }
        void* puVar3 = (void*)FUN_00422e71(local_10);
        FUN_004234f9(srcBuffer->data, puVar3, srcBuffer->width, srcBuffer->height, scaledWidth, scaledHeight);

        __try {
            local_30[0] = 0;
            local_30[1] = 0;
            local_30[2] = 0;
            local_30[3] = 0;
            local_30[1] = this->saved_video_mode;
            local_30[2] = this->clip_x2;
            local_20[0] = 0;
            local_20[1] = 0;
            local_20[2] = 0;
            local_20[3] = 0;
            local_30[0] = this->clip_x1;
            scaledWidth = scaledWidth - 1;
            scaledHeight = scaledHeight - 1;
            local_30[3] = this->video_mode_lock_count;
            local_20[2] = scaledWidth;
            local_20[3] = scaledHeight;
            local_20[0] = 0;
            local_20[1] = 0;

            int iVar4 = FUN_0041b590(local_30, local_20, &param_1, &param_2);
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
