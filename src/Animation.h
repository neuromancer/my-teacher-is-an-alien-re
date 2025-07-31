#ifndef ANIMATION_H
#define ANIMATION_H

#include "VBuffer.h"

struct SmkObject {
    int version;
    int width;
    int height;
    int frame_count;
    int mspf;
    char unknown1[84];
    int field_0x68;
    char unknown2[884 - 20 - 84 - 4];
    int current_frame;
};

struct AnimationData {
    char pad[0x18];
    int y_offset;
    int field_0x1c;
};

class Animation {
public:
    void** vtable;
    void* field_0x4;
    VBuffer* vbuffer; // 0x8
    SmkObject* smk; // 0xc
    char pad2[0x18 - 0xc - 4];
    AnimationData* data; // 0x18
    unsigned int flags; // 0x1c
    void* palette; // 0x20
    int* smack_buffer; // 0x24
    void* smack_handle; // 0x28

    void AnimationInit();
    void Open(char*, int, int);
    void ToBuffer();
    void ToBuffer(AnimationData* buffer);
    void OpenAndConvertToBuffer(char* filename);
    void Play(char* filename, unsigned int flags);
    void MainLoop();
    void FreeVBuffer();
    static void DoFrame(Animation*);
};

#endif // ANIMATION_H
