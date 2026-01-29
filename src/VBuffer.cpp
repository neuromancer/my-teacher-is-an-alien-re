#include "globals.h"
#include "VBuffer.h"
#include "VideoTable.h"
#include <stdlib.h>
#include <windows.h>
#include "Memory.h"
#include "string.h"
#include "GlyphRect.h"

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

// Forward declarations
void CopyRowTransparent(char* dest, char* src, int count, char transparentColor, char fillColor);
void BlitTransparentRows(int x1, int x2, int y1, int y2, int destX, int destY, VBuffer* srcBuffer, VBuffer* destBuffer, char transparentColor, char fillColor);
int __cdecl IntersectClipRect(int* clipRect, int* srcRect, int* destRect);
void __cdecl OffsetRect(int* rect, int offsetX, int offsetY);
int __cdecl ClipRectBottomUp(int* param_1, int* param_2, int* param_3, int* param_4);

extern "C" {
    void ApplyVideoPalette();
    int SetFillColor(unsigned char);
    unsigned int GetCurrentVideoMode();
    void InvalidateVideoMode();
    void BlitBufferOpaque(int, int, int, int, int, int, unsigned int, unsigned int);
    void BlitBufferTransparent(int, int, int, int, int, int, unsigned int, unsigned int);
    void ScaleBuffer(void*, void*, unsigned int, unsigned int, unsigned int, unsigned int);
}

/* Function start: 0x41A8C0 */
extern "C" void InitWorkBuffer(int width, int height)
{
    if (g_WorkBuffer_00436974 != 0) {
        delete g_WorkBuffer_00436974;
        g_WorkBuffer_00436974 = 0;
    }

    g_WorkBuffer_00436974 = new VBuffer(width, height);
    if (g_WorkBuffer_00436974->handle != 0) {
        ShowError("workbuff must be first vb created '%d'", g_WorkBuffer_00436974->handle);
    }
    g_WorkBuffer_00436974->SetVideoMode();
    g_WorkBuffer_00436974->ClearScreen(0);
}

/* Function start: 0x41a9f0 */
VBuffer::VBuffer(int width, int height)
{
    InitFields();
    InitWithSize(width, height);
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
    handle = 0xffffffff;
    *(int*)this = 0xffffffff;
    InitVBufferHandleTable();
}

/* Function start: 0x41aa60 */
void VBuffer::Free()
{
    if (data != 0) {
        ReleaseVideoBuffer(handle);
        ReleaseVBufferHandle(handle);
        handle = 0xffffffff;
        data = 0;
    }
}

/* Function start: 0x41aaa0 */
void* VBuffer::InitWithSize(int param_1, int param_2)
{
    if ((param_1 == 0) || (param_2 == 0)) {
        ShowError("Error! Invalid buffer size specified =>%lu", param_2 * param_1);
    }
    width = param_1;
    height = param_2;
    clip_x1 = 0;
    clip_x2 = param_1 - 1;
    clip_y1 = 0;
    clip_y2 = param_2 - 1;
    if (data != 0) {
        ShowError("Error! Virtual buffer already allocated");
    }

    while (param_1 % 4 != 0) {
        param_1 = param_1 + 1;
    }
    while (param_2 % 4 != 0) {
        param_2 = param_2 + 1;
    }
    int iVar1 = CreateTable(param_1, param_2);
    handle = iVar1;
    if (iVar1 == -1) {
        ShowError("VBuffer::Init - Unable To create vb. Table Full");
        return data;
    }
    if (iVar1 == -2) {
        ShowError("VBuffer::Init - Unable To create vb. No memory");
        return data;
    }
    SetCurrentVideoMode(iVar1);
    ApplyVideoPalette();
    InvalidateVideoMode();
    data = (void*)GetVideoBufferData(handle);
    RegisterVBufferHandle(handle);
    return data;
}

/* Function start: 0x41abc0 */
void VBuffer::ClearScreen(int color)
{
    SetCurrentVideoMode(handle);
    SetFillColor(color);
    ClearVideoBuffer();
    InvalidateVideoMode();
}

/* Function start: 0x41abf0 */
int VBuffer::SetVideoMode()
{
    int iVar1 = SelectVideoBuffer(handle);
    saved_video_mode = 0xffffffff;
    video_mode_lock_count = 1;
    return iVar1;
}

/* Function start: 0x41ac10 */
void VBuffer::Release()
{
    unsigned int uVar1 = GetCurrentVideoMode();
    if (uVar1 == handle) {
        InvalidateVideoMode();
    }
}

/* Function start: 0x41ac30 */
void* VBuffer::GetData()
{
    return data;
}

/* Function start: 0x41ac40 */
void VBuffer::Lock()
{
}

/* Function start: 0x41ac50 */
int VBuffer::SetCurrentVideoMode(int param_1)
{
    video_mode_lock_count = video_mode_lock_count + 1;
    unsigned int uVar1 = GetCurrentVideoMode();
    if (param_1 != uVar1) {
        saved_video_mode = uVar1;
        uVar1 = SelectVideoBuffer(param_1);
    }
    return uVar1;
}

/* Function start: 0x41ac80 */
void VBuffer::InvalidateVideoMode()
{
    if ((video_mode_lock_count != 0) && (video_mode_lock_count = video_mode_lock_count - 1, video_mode_lock_count == 0)) {
        ::InvalidateVideoMode();
        if (saved_video_mode != 0xffffffff) {
            SelectVideoBuffer(saved_video_mode);
            saved_video_mode = 0xffffffff;
        }
    }
}

/* Function start: 0x41acb0 */
void VBuffer::CallBlitter4(int param_1, int param_2, int param_3, int param_4, int param_5, int param_6)
{
    SetCurrentVideoMode(handle);
    BlitToDevice(param_1, param_2, param_3, param_4, param_5, param_6);
    InvalidateVideoMode();
}

/* Function start: 0x41acf0 */
void VBuffer::CallBlitter5(int param_1, int param_2, int param_3, int param_4, int param_5, int param_6, int param_7, int param_8)
{
    SetCurrentVideoMode(handle);
    StretchBlitBuffer(param_1, param_2, param_3, param_4, param_5, param_6, param_7, param_8);
    InvalidateVideoMode();
}

/* Function start: 0x41ad40 */
void VBuffer::TPaste(int param_1, int param_2, int param_3, int param_4, int param_5, int param_6)
{
    ShowError("VBuffer::TPaste - Not implemented");
}


/* Function start: 0x41B310 */
void __cdecl BlitTransparentRows(int srcX1, int srcX2, int srcY1, int srcY2, int destX, int destY, VBuffer* srcBuffer, VBuffer* destBuffer, char transparentColor, char fillColor)
{
    int width = (srcX2 - srcX1) + 1;
    int height = (srcY2 - srcY1) + 1;
    
    int srcOffset = (srcBuffer->clip_y2 - srcY2) * srcBuffer->width + srcX1;
    char* srcRow = (char*)srcBuffer->GetData() + srcOffset;

    int destOffset = (destBuffer->clip_y2 - destY) * destBuffer->width + destX;
    char* destRow = (char*)destBuffer->GetData() + destOffset;

    if (height > 0) {
        do {
            CopyRowTransparent(destRow, srcRow, width, transparentColor, fillColor);
            destRow += destBuffer->width;
            srcRow += srcBuffer->width;
            height--;
        } while (height != 0);
    }
    
    destBuffer->Lock();
    srcBuffer->Lock();
}

/* Function start: 0x41ad50 */
void VBuffer::BlitTransparent(int param_1, int param_2, int param_3, int param_4, int param_5, int param_6, char param_7, char param_8)
{
    VBuffer local_40((param_2 - param_1) + 1, (param_4 - param_3) + 1);
    local_40.ClearScreen(0);
    BlitTransparentRows(param_1, param_2, param_3, param_4, local_40.clip_x1, local_40.clip_y2, this, &local_40, param_7, param_8);
    local_40.TPaste(local_40.clip_x1, local_40.clip_x2, local_40.clip_y1, local_40.clip_y2, param_5, param_6);
}

/* Function start: 0x41ae20 */
void VBuffer::CallBlitter(int param_1, int param_2, int param_3, int param_4, int param_5, int param_6, int param_7)
{
    //ShowMessage("CallBlitter srcHnd=%d dstHnd=%d pos=(%d,%d)", ((VBuffer*)param_7)->handle, handle, param_5, param_6);
    BlitBufferOpaque(param_1, param_2, param_3, param_4, param_5, param_6, ((VBuffer*)param_7)->handle, handle);
}

/* Function start: 0x41ae60 */
void VBuffer::CallBlitter2(int param_1, int param_2, int param_3, int param_4, int param_5, int param_6, VBuffer* srcBuffer)
{
    //ShowMessage("CallBlitter2 srcHnd=%d dstHnd=%d pos=(%d,%d)", srcBuffer->handle, handle, param_5, param_6);
    BlitBufferTransparent(param_1, param_2, param_3, param_4, param_5, param_6, srcBuffer->handle, handle);
}

/* Function start: 0x41aea0 */
void VBuffer::CallBlitter3(int param_1, int param_2, int param_3, int param_4, int param_5, int param_6, VBuffer* srcBuffer, char param_8, char param_9)
{
    BlitTransparentRows(param_1, param_2, param_3, param_4, param_5, param_6, srcBuffer, this, param_8, param_9);
}

/* Function start: 0x41aee0 */
void VBuffer::ClipAndBlit(int param_1, int param_2, int param_3, int param_4, int param_5, int param_6, int param_7)
{
    GlyphRect local_2c;
    local_2c.left = clip_x1;
    local_2c.right = clip_x2;
    local_2c.top = clip_y1;
    local_2c.bottom = clip_y2;
    GlyphRect local_1c;
    local_1c.left = param_1;
    local_1c.top = param_3;
    local_1c.right = param_2;
    local_1c.bottom = param_4;

    if (ClipRectBottomUp(&local_2c.left, &local_1c.left, &param_5, &param_6))
    {
        CallBlitter(local_1c.left, local_1c.right, local_1c.top, local_1c.bottom, param_5, param_6, param_7);
    }
}

/* Function start: 0x41AFC0 */
void VBuffer::ClipAndPaste(int param_1, int param_2, int param_3, int param_4, int param_5, int param_6, int param_7)
{
    GlyphRect local_2c;
    local_2c.left = clip_x1;
    local_2c.right = clip_x2;
    local_2c.top = clip_y1;
    local_2c.bottom = clip_y2;
    GlyphRect local_1c;
    local_1c.left = param_1;
    local_1c.top = param_3;
    local_1c.right = param_2;
    local_1c.bottom = param_4;

    if (ClipRectBottomUp(&local_2c.left, &local_1c.left, &param_5, &param_6))
    {
        CallBlitter2(local_1c.left, local_1c.right, local_1c.top, local_1c.bottom, param_5, param_6, (VBuffer*)param_7);
    }
}

/* Function start: 0x41B110 */
void VBuffer::ScaleTCCopy(int param_1, int param_2, VBuffer* srcBuffer, double scale)
{
    int scaledWidth = (int)(srcBuffer->width * scale);
    int scaledHeight = (int)(srcBuffer->height * scale);

    if ((scaledWidth >= 1) && (scaledHeight >= 1)) {
        int local_10 = CreateTableFromBuffer((int)g_Buffer_00436964, scaledWidth, scaledHeight);
        if (local_10 < 0) {
            ShowError("VBuffer::ScaleTCCopy");
        }
        void* puVar3 = (void*)GetVideoBufferData(local_10);
        ScaleBuffer(srcBuffer->data, puVar3, srcBuffer->width, srcBuffer->height, scaledWidth, scaledHeight);

        GlyphRect local_30;
        local_30.left = 0;
        local_30.top = 0;
        local_30.right = 0;
        local_30.bottom = 0;
        GlyphRect local_20;
        local_20.left = 0;
        local_20.top = 0;
        local_20.right = 0;
        local_20.bottom = 0;

        local_30.left = clip_x1;
        local_30.right = clip_x2;
        local_30.top = clip_y1;
        local_30.bottom = clip_y2;
        local_20.right = scaledWidth - 1;
        local_20.bottom = scaledHeight - 1;

        if (ClipRectBottomUp(&local_30.left, &local_20.left, &param_1, &param_2)) {
            BlitBufferTransparent(local_20.left, local_20.right, local_20.top, local_20.bottom, param_1, param_2, local_10, handle);
            ReleaseBufferEntry(local_10);
        }
    }
}

/* Function start: 0x41B3B0 */
int __cdecl IntersectClipRect(int* clipRect, int* srcRect, int* destRect)
{
    int x1 = srcRect[0];
    if (srcRect[0] <= clipRect[0]) {
        x1 = clipRect[0];
    }
    destRect[0] = x1;
    
    int x2 = srcRect[2];
    if (clipRect[2] <= srcRect[2]) {
        x2 = clipRect[2];
    }
    destRect[2] = x2;
    
    int y1 = srcRect[1];
    if (srcRect[1] <= clipRect[1]) {
        y1 = clipRect[1];
    }
    destRect[1] = y1;
    
    int y2 = srcRect[3];
    if (clipRect[3] <= srcRect[3]) {
        y2 = clipRect[3];
    }
    destRect[3] = y2;
    
    if ((x1 < x2) && (y1 < y2)) {
        return 1;
    }
    destRect[0] = 0;
    destRect[1] = 0;
    destRect[2] = 0;
    destRect[3] = 0;
    return 0;
}

/* Function start: 0x41B430 */
void __cdecl OffsetRect(int* rect, int offsetX, int offsetY)
{
    rect[0] = rect[0] + offsetX;
    rect[2] = rect[2] + offsetX;
    rect[1] = rect[1] + offsetY;
    rect[3] = rect[3] + offsetY;
}

/* Function start: 0x41B450 */
int __cdecl ClipRectAndAdjust(int* clipRect, int* srcRect, int* destX, int* destY)
{
    if ((clipRect[2] < *destX) || (clipRect[3] < *destY)) {
        srcRect[0] = 0;
        srcRect[1] = 0;
        srcRect[2] = 0;
        srcRect[3] = 0;
        *destX = 0;
        *destY = 0;
        return 0;
    }
    
    GlyphRect guard;
    guard.left = 0;
    guard.top = 0;
    guard.right = 0;
    guard.bottom = 0;
    OffsetRect(srcRect, *destX, *destY);
    IntersectClipRect(clipRect, srcRect, &guard.left);
    OffsetRect(&guard.left, -(*destX), -(*destY));
    
    int iVar1 = *destX;
    if (*destX <= *clipRect) {
        iVar1 = *clipRect;
    }
    *destX = iVar1;
    
    iVar1 = clipRect[1];
    if (clipRect[1] <= *destY) {
        iVar1 = *destY;
    }
    *destY = iVar1;
    
    if ((guard.right == guard.left) || (guard.bottom == guard.top)) {
        srcRect[0] = 0;
        srcRect[1] = 0;
        srcRect[2] = 0;
        srcRect[3] = 0;
        *destX = 0;
        *destY = 0;
        return 0;
    }
    
    srcRect[0] = guard.left;
    srcRect[1] = guard.top;
    srcRect[2] = guard.right;
    srcRect[3] = guard.bottom;
    return 1;
}

/* Function start: 0x41B590 */
int __cdecl ClipRectBottomUp(int* param_1, int* param_2, int* param_3, int* param_4)
{
    int result;
    *param_4 = *param_4 + (param_2[1] - param_2[3]);
    result = ClipRectAndAdjust(param_1, param_2, param_3, param_4);
    *param_4 = *param_4 + (param_2[3] - param_2[1]);
    return result;
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
