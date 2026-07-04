#ifndef GRAPHICS_H
#define GRAPHICS_H

int __cdecl DrawCircle(int param_1);
int DrawFontText(char* text, int len);
int __cdecl DrawLine(int param_1, int param_2);
void FlipScreen();
int GetColorBitDepth(void);
int GetMousePosition(int* out_x, int* out_y);
int InitMouseSettings(void);
int InitVideoSystem(void);
void SetFontColor(int index);
void SetFontPosition(int x, int y);

int GetCurrentVideoMode();
int InvalidateVideoMode();
int __cdecl DrawEllipse(int cx, int cy, int rx, int ry);
int CleanupVideoSystem(void);
int ScaleClientToBufferX(int x);
int ScaleClientToBufferY(int y);
#endif // GRAPHICS_H
