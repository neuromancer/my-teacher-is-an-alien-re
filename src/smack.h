#ifndef SMACK_H
#define SMACK_H

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

struct Smack {
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

Smack* __stdcall SmackOpen(char* filename, int flags, int extra);
void __stdcall SmackGoto(Smack* smack, int frame);
int __stdcall SmackDoFrame(Smack* smack);
void __stdcall SmackToBuffer(Smack* smack, int, int, int, int, void*, int);
void __stdcall SmackNextFrame(Smack* smack);
void __stdcall SmackClose(Smack* smack);
void* __stdcall SmackBufferOpen(void* smack, int, int, int, int, int);
void __stdcall SmackSoundOnOff(Smack* smack, int onoff);
void __stdcall SmackBufferClose(void* smack);
void __stdcall SmackBufferNewPalette(void* smack, void* palette, int);
void __stdcall SmackColorRemap(void* smack, void* remap_table, int num_entries, int);
int __stdcall SmackWait(Smack* smack);

#ifdef __cplusplus
}
#endif

#endif // SMACK_H
