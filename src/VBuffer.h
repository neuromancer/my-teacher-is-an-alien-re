#ifndef VBUFFER_H
#define VBUFFER_H

class VBuffer {
public:
    int saved_video_mode; // 0x00
    int video_mode_lock_count; // 0x04
    char pad[0x08]; // 0x08-0x0f
    void* data; // 0x10
    int width; // 0x14
    int height; // 0x18
    int handle; // 0x1c
    int clip_y1; // 0x20
    int clip_y2; // 0x24
    int clip_x1; // 0x28
    int clip_x2; // 0x2c

    VBuffer(unsigned int param_1, unsigned int param_2);
    void InitWithSize(unsigned int param_1, unsigned int param_2);
    ~VBuffer();
    void Release();
    void Free();
    void ClearScreen(int color);
    void TPaste(int param_1, int param_2, int param_3, int param_4, int param_5, int param_6);
    void BlitTransparent(int param_1, int param_2, int param_3, int param_4, int param_5, int param_6, char param_7, char param_8);
    void CallBlitter(int param_1, int param_2, int param_3, int param_4, int param_5, int param_6, int param_7);
    void CallBlitter2(int param_1, int param_2, int param_3, int param_4, int param_5, int param_6, VBuffer* srcBuffer);
    void CallBlitter3(int param_1, int param_2, int param_3, int param_4, int param_5, int param_6, VBuffer* srcBuffer, char param_8, char param_9);
    void ClipAndBlit(int param_1, int param_2, int param_3, int param_4, int param_5, int param_6, int param_7);
    void ClipAndPaste(int param_1, int param_2, int param_3, int param_4, int param_5, int param_6, int param_7);
    int SetVideoMode();
    void* GetData();
    void Lock();
    int SetCurrentVideoMode(int param_1);
    void InvalidateVideoMode();
    void CallBlitter4(int param_1, int param_2, int param_3, int param_4, int param_5, int param_6);
    void CallBlitter5(int param_1, int param_2, int param_3, int param_4, int param_5, int param_6, int param_7, int param_8);
    void InitFields();
    void ScaleTCCopy(int param_1, int param_2, VBuffer* srcBuffer, double scale);

    // Copy rectangle rows from srcBuffer into this buffer with transparent handling


    // Member function version to match __thiscall calling convention (0x41A9F0)
    //VBuffer* CreateAndClean(int width, int height);
};
int __cdecl CalculateBufferSize(int param_1,unsigned int param_2);

#endif // VBUFFER_H
