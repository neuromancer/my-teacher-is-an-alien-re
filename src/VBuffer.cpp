#include "globals.h"
#include "GameConfig.h"
#include "VBuffer.h"
#include "VideoTable.h"
#include "Animation.h"
#include <stdlib.h>
#include <windows.h>
#include "Memory.h"
#include "string.h"
#include "GlyphRect.h"

// g_BackBuffer_0046aa14 and g_InputManager_0046aa08 now in globals.h
extern "C" void SetVideoRes(int width, int height);

static int g_VBufferHandleTableInitialized = 0;
static int g_VBufferHandleTable[0x20];



/* Function start: 0x410EA0 */
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

// GetVideoBufferNameSlot(handle) = 0x4734B0 + handle * 64
extern "C" char* GetVideoBufferNameSlot(int handle);  // 0x44C650

/* Function start: 0x410ED0 */
void RegisterVBufferHandle(int handle)
{
    g_VBufferHandleTable[handle] = handle;
    char* name = GetVideoBufferNameSlot(handle);
    if (name[0] == '\0' && g_AnimFilename_00472c70[0] != '\0') {
        strcpy(name, g_AnimFilename_00472c70);
        g_AnimFilename_00472c70[0] = '\0';
    }
}

/* Function start: 0x410E80 */
int GetVideoBufferCount()
{
    int count = 0;
    int* ptr = g_VBufferHandleTable;
    do {
        if (*ptr != -1) {
            count++;
        }
        ptr++;
    } while (ptr < &g_VBufferHandleTable[0x20]);
    return count;
}

extern "C" void WriteToLog(const char* format, ...);

/* Function start: 0x410F40 */
void DumpVBufferHandles()
{
    int i;

    int count = GetVideoBufferCount();
    WriteToLog("%d Virtual Buffers in use", count);

    i = 0;
    do {
        int handle = g_VBufferHandleTable[i];
        if (handle != -1) {
            char* name = GetVideoBufferNameSlot(i);
            WriteToLog("\tVB handle=%d addr=%d name=%s", i, handle, name);
        }
        i++;
    } while (i < 0x20);
}

/* Function start: 0x410F30 */
void ReleaseVBufferHandle(int handle)
{
    g_VBufferHandleTable[handle] = 0xffffffff;
}

// Forward declarations
void CopyRowTransparent(char* dest, char* src, int count, char transparentColor, char fillColor);
void BlitTransparentRows(int x1, int x2, int y1, int y2, int destX, int destY, VBuffer* srcBuffer, VBuffer* destBuffer, char transparentColor, char fillColor);
void CopyRowReversedTransparent(char* dest, char* src, int count);
void CopyRowReversed(char* dest, char* src, int count);
void BlitRowsReversed(int srcX1, int srcX2, int srcY1, int srcY2, int destX, int destY, VBuffer* srcBuffer, VBuffer* destBuffer, int transparentFlag);
int __cdecl IntersectClipRect(int* clipRect, int* srcRect, int* destRect);
void __cdecl OffsetRect(int* rect, int offsetX, int offsetY);
int __cdecl ClipRectAndAdjust(int* clipRect, int* srcRect, int* destX, int* destY);
int __cdecl ClipRectBottomUp(int* param_1, int* param_2, int* param_3, int* param_4);

extern "C" {
    void ApplyVideoPalette();
    int SetFillColor(int);
    unsigned int GetCurrentVideoMode();
    int InvalidateVideoMode();
    void BlitBufferOpaque(int, int, int, int, int, int, unsigned int, unsigned int);
    void BlitBufferTransparent(int, int, int, int, int, int, unsigned int, unsigned int);
    void ScaleBuffer(void*, void*, unsigned int, unsigned int, unsigned int, unsigned int);
}

/* Function start: 0x425A90 */
extern "C" void SetVideoRes(int width, int height)
{
    int* inputManager;

    if (g_BackBuffer_0046aa14 != 0) {
        if (g_BackBuffer_0046aa14->width == width && g_BackBuffer_0046aa14->height == height) {
            return;
        }
        delete g_BackBuffer_0046aa14;
        g_BackBuffer_0046aa14 = 0;
    }

    g_BackBuffer_0046aa14 = new VBuffer(width, height);

    if (g_BackBuffer_0046aa14->handle != 0) {
        ShowError("workbuff must be first vb created '%d'", g_BackBuffer_0046aa14->handle);
    }

    g_BackBuffer_0046aa14->SetVideoMode();
    g_BackBuffer_0046aa14->ClearScreen(0);

    inputManager = (int*)g_InputManager_0046aa08;
    if (inputManager != 0) {
        inputManager[0x69] = 0;
        inputManager[0x6A] = 0;
        inputManager[0x6B] = width - 1;
        inputManager[0x6C] = height - 1;
    }
}

/* Function start: 0x410F90 */
VBuffer::VBuffer(int width, int height)
{
    InitFields();
    InitWithSize(width, height);
}

/* Function start: 0x410FB0 */
VBuffer::VBuffer(char* filename, int param_2)
{
    InitFields();
    LoadFromFile(filename, param_2);
}

/* Function start: 0x411270 */
void VBuffer::LoadFromFile(char* filename, int param_2)
{
    Release();
    Free();

    Animation anim;
    anim.Open(filename, 0xFE000, -1);

    if (g_GameConfig2_0046aa10->data.rawData[2] == 2) {
        SmackBufferClose((SmackBuf*)anim.vbuffer);
    }

    int w = ((int*)anim.vbuffer)[1]; // vbuffer->width at +0x04
    int h = ((int*)anim.vbuffer)[2]; // vbuffer->height at +0x08
    InitWithSize(w, h);
    anim.ToBufferVB(this);

    if (param_2 != 0) {
        anim.GotoFrame(param_2);
    }

    anim.DoFrame();
}

/* Function start: 0x411330 */
int VBuffer::CheckHit(int x, int y) {
    int offset = (height - 1 - y) * width + x;

    if (clip_x1 > x || clip_x2 < x || clip_y1 > y || clip_y2 < y) {
        return -1;
    }

    unsigned char pixel = ((unsigned char*)GetData())[offset];
    return (int)pixel;
}

/* Function start: 0x410FD0 */
VBuffer::~VBuffer()
{
    Release();
    Free();
}

/* Function start: 0x410FF0 */
void VBuffer::InitFields()
{
    memset(this, 0, sizeof(VBuffer));
    handle = 0xffffffff;
    saved_video_mode = 0xffffffff;
    InitVBufferHandleTable();
}

/* Function start: 0x411020 */
void VBuffer::Free()
{
    if (data != 0) {
        ReleaseVideoBuffer(handle);
        ReleaseVBufferHandle(handle);
        handle = 0xffffffff;
        data = 0;
    }
}

/* Function start: 0x411060 */ /* ~90% match */
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
    DumpVBufferHandles();
    return data;
}

/* Function start: 0x411180 */
void VBuffer::ClearScreen(int color)
{
    SetCurrentVideoMode(handle);
    SetFillColor((unsigned char)color);
    ClearVideoBuffer();
    InvalidateVideoMode();
}

/* Function start: 0x4111B0 */
int VBuffer::SetVideoMode()
{
    int iVar1 = SelectVideoBuffer(handle);
    saved_video_mode = 0xffffffff;
    video_mode_lock_count = 1;
    return iVar1;
}

/* Function start: 0x4111D0 */
void VBuffer::Release()
{
    unsigned int uVar1 = GetCurrentVideoMode();
    if (uVar1 == handle) {
        ::InvalidateVideoMode();
    }
}

/* Function start: 0x4111F0 */
void* VBuffer::GetData()
{
    return data;
}

void VBuffer::Lock()
{
}

/* Function start: 0x411210 */ /* ~92% match */
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

/* Function start: 0x411240 */
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

/* Function start: 0x411380 */
void VBuffer::CallBlitter4(int param_1, int param_2, int param_3, int param_4, int param_5, int param_6)
{
    SetCurrentVideoMode(handle);
    BlitToDevice(param_1, param_2, param_3, param_4, param_5, param_6);
    InvalidateVideoMode();
}

/* Function start: 0x4113C0 */
void VBuffer::CallBlitter5(int param_1, int param_2, int param_3, int param_4, int param_5, int param_6, int param_7, int param_8)
{
    SetCurrentVideoMode(handle);
    StretchBlitBuffer(param_1, param_2, param_3, param_4, param_5, param_6, param_7, param_8);
    InvalidateVideoMode();
}

/* Function start: 0x411410 */
void VBuffer::TPaste(int param_1, int param_2, int param_3, int param_4, int param_5, int param_6)
{
    ShowError("VBuffer::TPaste - Not implemented");
}


/* Function start: 0x411C40 */
void __cdecl BlitTransparentRows(int srcX1, int srcX2, int srcY1, int srcY2, int destX, int destY, VBuffer* srcBuffer, VBuffer* destBuffer, char transparentColor, char fillColor)
{
    int width;
    int height;
    char* srcRow;
    char* destRow;

    width = (srcX2 - srcX1) + 1;
    height = (srcY2 - srcY1) + 1;

    srcRow = (char*)srcBuffer->GetData() + (srcBuffer->clip_y2 - srcY2) * srcBuffer->width + srcX1;

    destRow = (char*)destBuffer->GetData() + (destBuffer->clip_y2 - destY) * destBuffer->width + destX;

    for (; height > 0; height--) {
        CopyRowTransparent(destRow, srcRow, width, transparentColor, fillColor);
        destRow += destBuffer->width;
        srcRow += srcBuffer->width;
    }

    destBuffer->Lock();
    srcBuffer->Lock();
}

/* Function start: 0x411420 */
void VBuffer::BlitTransparent(int param_1, int param_2, int param_3, int param_4, int param_5, int param_6, char param_7, char param_8)
{
    VBuffer local_40((param_2 - param_1) + 1, (param_4 - param_3) + 1);
    local_40.ClearScreen(0);
    BlitTransparentRows(param_1, param_2, param_3, param_4, local_40.clip_x1, local_40.clip_y2, this, &local_40, param_7, param_8);
    local_40.TPaste(local_40.clip_x1, local_40.clip_x2, local_40.clip_y1, local_40.clip_y2, param_5, param_6);
}

/* Function start: 0x4114F0 */
void VBuffer::CallBlitter(int param_1, int param_2, int param_3, int param_4, int param_5, int param_6, int param_7)
{
    //ShowMessage("CallBlitter srcHnd=%d dstHnd=%d pos=(%d,%d)", ((VBuffer*)param_7)->handle, handle, param_5, param_6);
    BlitBufferOpaque(param_1, param_2, param_3, param_4, param_5, param_6, ((VBuffer*)param_7)->handle, handle);
}

/* Function start: 0x411570 */
void VBuffer::CallBlitter2(int param_1, int param_2, int param_3, int param_4, int param_5, int param_6, VBuffer* srcBuffer)
{
    //ShowMessage("CallBlitter2 srcHnd=%d dstHnd=%d pos=(%d,%d)", srcBuffer->handle, handle, param_5, param_6);
    BlitBufferTransparent(param_1, param_2, param_3, param_4, param_5, param_6, srcBuffer->handle, handle);
}

/* Function start: 0x4115B0 */
void VBuffer::CallBlitter3(int param_1, int param_2, int param_3, int param_4, int param_5, int param_6, VBuffer* srcBuffer, char param_8, char param_9)
{
    BlitTransparentRows(param_1, param_2, param_3, param_4, param_5, param_6, srcBuffer, this, param_8, param_9);
}

/* Function start: 0x4115F0 */
void VBuffer::BlitReversedRows(int param_1, int param_2, int param_3, int param_4, int param_5, int param_6, int param_7)
{
    BlitRowsReversed(param_1, param_2, param_3, param_4, param_5, param_6, (VBuffer*)param_7, this, 1);
}

/* Function start: 0x411630 */
void VBuffer::ClipAndBlit(int param_1, int param_2, int param_3, int param_4, int param_5, int param_6, int param_7)
{
    GlyphRect local_2c(clip_x1, clip_y1, clip_x2, clip_y2);
    GlyphRect local_1c(param_1, param_3, param_2, param_4);

    if (ClipRectBottomUp(&local_2c.left, &local_1c.left, &param_5, &param_6) == 0) {
        return;
    }
    CallBlitter(local_1c.left, local_1c.right, local_1c.top, local_1c.bottom, param_5, param_6, param_7);
}

/* Function start: 0x411710 */
void VBuffer::ClipAndPaste(int param_1, int param_2, int param_3, int param_4, int param_5, int param_6, int param_7)
{
    GlyphRect local_2c(clip_x1, clip_y1, clip_x2, clip_y2);
    GlyphRect local_1c(param_1, param_3, param_2, param_4);

    if (ClipRectBottomUp(&local_2c.left, &local_1c.left, &param_5, &param_6) == 0) {
        return;
    }
    CallBlitter2(local_1c.left, local_1c.right, local_1c.top, local_1c.bottom, param_5, param_6, (VBuffer*)param_7);
}

/* Function start: 0x4117F0 */
void VBuffer::ClipAndBlitRegion(int param_1, int param_2, int param_3, int param_4, int param_5, int param_6, int param_7)
{
    GlyphRect local_2c(clip_x1, clip_y1, clip_x2, clip_y2);
    GlyphRect local_1c(param_1, param_3, param_2, param_4);

    if (param_1 == 0x38) {
        if (ClipRectBottomUp(&local_2c.left, &local_1c.left, &param_5, &param_6) == 0) {
            return;
        }
    } else {
        if (ClipRectBottomUp(&local_2c.left, &local_1c.left, &param_5, &param_6) == 0) {
            return;
        }
    }
    CallBlitter2(local_1c.left, local_1c.right, local_1c.top, local_1c.bottom, param_5, param_6, (VBuffer*)param_7);
}

/* Function start: 0x4118E0 */
void VBuffer::ClipAndBlitReversed(int param_1, int param_2, int param_3, int param_4, int param_5, int param_6, int param_7)
{
    GlyphRect local_2c(clip_x1, clip_y1, clip_x2, clip_y2);
    GlyphRect local_1c(param_1, param_3, param_2, param_4);

    if (param_1 != 0x38) {
        if (ClipRectBottomUp(&local_2c.left, &local_1c.left, &param_5, &param_6) == 0) {
            return;
        }
    } else {
        if (ClipRectBottomUp(&local_2c.left, &local_1c.left, &param_5, &param_6) == 0) {
            return;
        }
    }

    BlitReversedRows(local_1c.left, local_1c.right, local_1c.top, local_1c.bottom, param_5, param_6, param_7);
}

/* Function start: 0x411A40 */
void VBuffer::ScaleTCCopy(int param_1, int param_2, VBuffer* srcBuffer, double scale)
{
    int scaledWidth = (int)(srcBuffer->width * scale);
    int scaledHeight = (int)(srcBuffer->height * scale);

    if ((scaledWidth >= 1) && (scaledHeight >= 1)) {
        int local_10 = CreateTableFromBuffer((int)g_Buffer_0046aa04, scaledWidth, scaledHeight);
        if (local_10 < 0) {
            ShowError("VBuffer::ScaleTCCopy");
        }
        void* puVar3 = (void*)GetVideoBufferData(local_10);
        ScaleBuffer(srcBuffer->data, puVar3, srcBuffer->width, srcBuffer->height, scaledWidth, scaledHeight);

        GlyphRect local_30;
        GlyphRect local_20;

        local_30.left = clip_x1;
        local_30.top = clip_y1;
        local_30.right = clip_x2;
        local_30.bottom = clip_y2;
        local_20.left = 0;
        local_20.top = 0;
        local_20.right = scaledWidth - 1;
        local_20.bottom = scaledHeight - 1;

        if (ClipRectBottomUp(&local_30.left, &local_20.left, &param_1, &param_2) == 0) {
            return;
        }
        BlitBufferTransparent(local_20.left, local_20.right, local_20.top, local_20.bottom, param_1, param_2, local_10, handle);
        ReleaseBufferEntry(local_10);
    }
}

/* Function start: 0x411CE0 */
void CopyRowReversedTransparent(char* dest, char* src, int count)
{
    char* destEnd = dest + count - 1;
    if (count == 0) {
        return;
    }

    do {
        while (*src == 0) {
            destEnd--;
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
            char c = *src;
            destEnd--;
            src++;
            count--;
            *(destEnd + 1) = c;
        } while (*src != 0);
    } while (count != 0);
}

/* Function start: 0x411D20 */
void CopyRowReversed(char* dest, char* src, int count)
{
    char* destEnd = dest + count - 1;
    if (count == 0) {
        return;
    }

    do {
        char c = *src;
        src++;
        *destEnd = c;
        destEnd--;
        count--;
    } while (count != 0);
}

/* Function start: 0x411D40 */
void BlitRowsReversed(int srcX1, int srcX2, int srcY1, int srcY2, int destX, int destY, VBuffer* srcBuffer, VBuffer* destBuffer, int transparentFlag)
{
    int width = (srcX2 - srcX1) + 1;
    int height = (srcY2 - srcY1) + 1;

    char* srcRow = (char*)srcBuffer->GetData() + (srcBuffer->clip_y2 - srcY2) * srcBuffer->width + srcX1;
    char* destRow = (char*)destBuffer->GetData() + (destBuffer->clip_y2 - destY) * destBuffer->width + destX;

    if (transparentFlag != 0) {
        if (height > 0) {
            do {
                CopyRowReversedTransparent(destRow, srcRow, width);
                destRow += destBuffer->width;
                srcRow += srcBuffer->width;
                height--;
            } while (height != 0);
        }
    } else {
        if (height > 0) {
            do {
                CopyRowReversed(destRow, srcRow, width);
                destRow += destBuffer->width;
                srcRow += srcBuffer->width;
                height--;
            } while (height != 0);
        }
    }

    destBuffer->Lock();
    srcBuffer->Lock();
}

/* Function start: 0x411E00 */
int __cdecl IntersectClipRect(int* clipRect, int* srcRect, int* destRect)
{
    int left = *srcRect;
    if (*srcRect <= *clipRect) {
        left = *clipRect;
    }
    *destRect = left;

    int right = srcRect[2];
    if (clipRect[2] <= srcRect[2]) {
        right = clipRect[2];
    }
    destRect[2] = right;

    int top = clipRect[1];
    if (clipRect[1] <= srcRect[1]) {
        top = srcRect[1];
    }
    destRect[1] = top;

    int bottom = srcRect[3];
    if (clipRect[3] <= srcRect[3]) {
        bottom = clipRect[3];
    }
    destRect[3] = bottom;

    if ((left <= right) && (top <= bottom)) {
        return 1;
    }
    *destRect = 0;
    destRect[1] = 0;
    destRect[2] = 0;
    destRect[3] = 0;
    return 0;
}

/* Function start: 0x411E80 */
void __cdecl OffsetRect(int* rect, int offsetX, int offsetY)
{
    rect[0] = rect[0] + offsetX;
    rect[2] = rect[2] + offsetX;
    rect[1] = rect[1] + offsetY;
    rect[3] = rect[3] + offsetY;
}

/* Function start: 0x411EA0 */
int __cdecl ClipRectAndAdjust(int* clipRect, int* srcRect, int* destX, int* destY)
{
    GlyphRect guard;

    int offsetX = *destX - srcRect[0];
    int offsetY = *destY - srcRect[1];

    OffsetRect(srcRect, offsetX, offsetY);
    IntersectClipRect(clipRect, srcRect, &guard.left);
    OffsetRect(&guard.left, -offsetX, -offsetY);

    int dx = *destX;
    if (*destX <= *clipRect) {
        dx = *clipRect;
    }
    *destX = dx;

    int dy = clipRect[1];
    if (clipRect[1] <= *destY) {
        dy = *destY;
    }
    *destY = dy;

    if (guard.right == guard.left || guard.bottom == guard.top) {
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

/* Function start: 0x411FC0 */
int __cdecl ClipRectBottomUp(int* param_1, int* param_2, int* param_3, int* param_4)
{
    int result;
    *param_4 = *param_4 + (param_2[1] - param_2[3]);
    result = ClipRectAndAdjust(param_1, param_2, param_3, param_4);
    *param_4 = *param_4 + (param_2[3] - param_2[1]);
    return result;
}

/* Function start: 0x411BF0 */ /* ~97% match */
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


/* Function start: 0x411530 */
void VBuffer::BlitTransparentRegion(int p1, int p2, int p3, int p4, int p5, int p6, int p7)
{
    BlitTransparentRows(p1, p2, p3, p4, p5, p6, (VBuffer*)p7, this, 0, 0);
}
