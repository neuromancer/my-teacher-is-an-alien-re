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

class Smacker {
public:
    void NextFrame();
    void Goto(int frame);
private:
    int field_0x0;
    int field_0x4;
    int field_0x8;
    SmackStruct* smk; // 0xc
};

extern "C" {
    __declspec(dllimport) void __stdcall SmackClose(SmackStruct* smk);
    __declspec(dllimport) void __stdcall SmackBufferClose(int handle);
    __declspec(dllimport) SmackStruct* __stdcall SmackOpen(const char* name, int flags, int extra_buf);
    __declspec(dllimport) void __stdcall SmackToBuffer(SmackStruct* smk, int x, int y, int pitch, int height, void* buf, int flags);
    __declspec(dllimport) void __stdcall SmackNextFrame(SmackStruct* smk);
    __declspec(dllimport) void __stdcall SmackGoto(SmackStruct* smk, int frame);
    __declspec(dllimport) void __stdcall SmackSoundOnOff(SmackStruct* smk, int on);
    __declspec(dllimport) void __stdcall SmackSoundUseDirectSound(int param_1, int param_2);
    __declspec(dllimport) SmackStruct* __stdcall SmackBufferOpen(int uVar1, int param_2, int param_3, int param_4, int param_5, int param_6);
    __declspec(dllimport) void __stdcall SmackDoFrame(SmackStruct* smk);
}

#endif // SMACKER_H
