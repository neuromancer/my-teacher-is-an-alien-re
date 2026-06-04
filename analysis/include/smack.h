#ifndef TEACHER_ANALYSIS_SMACK_H
#define TEACHER_ANALYSIS_SMACK_H

#include "windows.h"
#include "mss.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SmackStruct {
    unsigned int Version;
    unsigned int Width;
    unsigned int Height;
    unsigned int Frames;
    unsigned int FrameNum;
    unsigned int MSPerFrame;
    unsigned int SmackerType;
    unsigned int LargestInTrack[7];
    unsigned int tablesize;
    unsigned int codesize;
    unsigned int absize;
    unsigned int detailsize;
    unsigned int typesize;
    unsigned int TrackType[7];
    unsigned int extra[192];
    unsigned int NewPalette;
    unsigned char Palette[1024];
} Smack, *HSMACK;

typedef struct SmackBufStruct {
    unsigned int Reversed;
    unsigned int SurfaceType;
    unsigned int BlitType;
    unsigned int FullScreen;
    unsigned int Width;
    unsigned int Height;
    unsigned int Pitch;
    unsigned int Zoomed;
    unsigned int ZWidth;
    unsigned int ZHeight;
    unsigned int DispColors;
    unsigned int MaxPalColors;
    unsigned int PalColorsInUse;
    unsigned int StartPalColor;
    unsigned char Palette[1024];
    unsigned char Unk43C[1024];
} SmackBuf;

int SmackSetSystemRes(int);
int SmackSoundUseMSS(HDIGDRIVER);
HSMACK SmackOpen(const char*, unsigned int, int);
void SmackClose(HSMACK);
void SmackDoFrame(HSMACK);
void SmackNextFrame(HSMACK);
void SmackGoto(HSMACK, unsigned int);
int SmackWait(HSMACK);
void SmackSoundOnOff(HSMACK, int);
void SmackSoundCheck(void);
void SmackToBuffer(HSMACK, unsigned int, unsigned int, unsigned int, unsigned int, void*, unsigned int);
SmackBuf* SmackBufferOpen(HWND, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);
void SmackBufferClose(SmackBuf*);
void SmackBufferNewPalette(SmackBuf*, void*, int);
void SmackColorRemap(HSMACK, void*, unsigned int, void*);
void SmackSummary(void*, void*);

#ifdef __cplusplus
}
#endif

#endif
