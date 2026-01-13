#ifndef VIDEOTABLE_H
#define VIDEOTABLE_H

#ifdef __cplusplus
extern "C" {
#endif

int __cdecl CreateTable(int width, int height);
int ClearVideoBuffer(void);
int __cdecl ReleaseVideoBuffer(unsigned int param_1);
int __cdecl GetVideoBufferData(unsigned int handle);
int __cdecl SelectVideoBuffer(int param_1);
int __cdecl BlitToDevice(int param_1, int param_2, int param_3, int param_4, int param_5, int param_6);
int __cdecl StretchBlitBuffer(int srcX1, int srcX2, int srcY1, int srcY2, int destX1, int destX2, int destY1, int destY2);
void __cdecl ReleaseBufferEntry(unsigned int param_1);
int __cdecl CreateTableFromBuffer(int buffer, int width, int height);
int __cdecl SetDrawPosition(int param_1, int param_2);
int __cdecl SetDrawColors(unsigned char param_1, unsigned char param_2);

#ifdef __cplusplus
}
#endif

#endif // VIDEOTABLE_H
