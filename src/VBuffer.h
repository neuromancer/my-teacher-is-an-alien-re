#ifndef VBUFFER_H
#define VBUFFER_H

class VBuffer {
public:
    VBuffer(int width, int height);
    ~VBuffer();
    VBuffer* VirtualBufferCreateAndClean(int width, int height);
    void SetCurrentVideoMode(int mode);
    int GetSomething();
    int SetVideoMode(void* param_1, void* param_2);
    void InvalidateVideoMode();
    void ClearScreen(unsigned char param_1);
    void Free();
    void Release();
    void Initialize();
    void ScaleTCCopy(int param_1, int param_2, int param_3);
    void Clear();
private:
    int videoMode; // 0x0
    int field1_0x4; // 0x4
    char pad_0x8[0x8];
    void* data;
    int width; // 0x14
    int height; // 0x18
    void* field_0x1c;
    int field_0x20;
    int field_0x24;
    int field_0x28;
    int field_0x2c;
};

#endif // VBUFFER_H
