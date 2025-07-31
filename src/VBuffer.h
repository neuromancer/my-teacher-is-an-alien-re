#ifndef VBUFFER_H
#define VBUFFER_H

class VBuffer {
public:
    char pad[0x1c];
    int field19_0x1c;
    char pad2[0x28 - 0x1c - 4];
    void* field_0x28;
    void* field_0x2c;
};

extern "C" {
    void __cdecl VBuffer_SetCurrentVideoMode(VBuffer* this_ptr, int);
    void __cdecl VBuffer_InvalidateVideoMode(VBuffer* this_ptr);
}

#endif // VBUFFER_H
