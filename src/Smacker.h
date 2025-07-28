#ifndef SMACKER_H
#define SMACKER_H

struct SmackStruct {
    int version; // 0x0
    int width; // 0x4
    int height; // 0x8
    int total_frames; // 0xc
    int mspf; // 0x10
    char unknown[884-20]; // 0x14
    int current_frame; // 0x374
};

extern "C" {
    void __stdcall SmackClose(SmackStruct* smk);
    void __stdcall SmackBufferClose(int handle);
    SmackStruct* __stdcall SmackOpen(const char* name, int flags, int extra_buf);
    void __stdcall SmackToBuffer(SmackStruct* smk, int x, int y, int pitch, int height, void* buf, int flags);
    void __stdcall SmackNextFrame(SmackStruct* smk);
    void __stdcall SmackGoto(SmackStruct* smk, int frame);
    void __stdcall SmackSoundUseDirectSound(int param_1, int param_2);
    SmackStruct* __stdcall SmackBufferOpen(int uVar1, int param_2, int param_3, int param_4, int param_5, int param_6);
}

#endif // SMACKER_H
