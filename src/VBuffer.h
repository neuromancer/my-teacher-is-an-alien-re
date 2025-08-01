#ifndef VBUFFER_H
#define VBUFFER_H

class VBuffer {
public:
    char pad[0x18];
    int y_offset;
    int field_0x1c;
    char pad2[0x28 - 0x1c - 4];
    void* field_0x28;
    void* field_0x2c;

    void Init(unsigned int param_1, unsigned int param_2);
    void VirtualBufferCreateAndClean(int width, int height);
    void ClearScreen(int color);
    void TPaste(void);
    void BlitTransparent(int param_1, int param_2, int param_3, int param_4, int param_5, int param_6, char param_7, char param_8);
};

extern "C" {
    void __fastcall VBuffer_SetCurrentVideoMode(VBuffer* this_ptr, int);
    void __fastcall VBuffer_InvalidateVideoMode(VBuffer* this_ptr);
}

#endif // VBUFFER_H
