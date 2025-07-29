#ifndef VBUFFER_H
#define VBUFFER_H

#include "BaseObject.h"

class VBuffer : public BaseObject {
public:
    VBuffer(int width, int height);
    ~VBuffer();
    VBuffer* VirtualBufferCreateAndClean(int width, int height);
    void SetCurrentVideoMode(int mode);
    int GetSomething();
    int SetVideoMode(void* param_1, void* param_2);
    void InvalidateVideoMode();
    void ClearScreen(unsigned char param_1);
    void Draw(int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8);
    void Free();
    void Release();
    void Initialize();
    void ScaleTCCopy(int param_1, int param_2, int param_3);
    void Clear();
public:
    int videoMode; // 0x0
    int field1_0x4; // 0x4
    int field_0x8;
    int field_0xc;
    void* data; // 0x10
    int width; // 0x14
    int height; // 0x18
    void* field_0x1c;
    int field_0x20;
    int field_0x24;
    int field_0x28;
    int field_0x2c;
};

#endif // VBUFFER_H
