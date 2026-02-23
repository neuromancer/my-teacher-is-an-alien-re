#pragma once

#include <windows.h>

class GameWindow
{
public:
    GameWindow();
    void CreateGameWindow(HINSTANCE, int, char*, int);
    int InitGraphics();
    void Shutdown();

    HWND savedActiveWindow; // offset 0x00 - saved active window from GetActiveWindow
    HINSTANCE hInstance;    // offset 0x04
    int field_8;            // offset 0x08
    HWND hWnd;              // offset 0x0C
    HDC hDC;                // offset 0x10
    HGDIOBJ hPalette;       // offset 0x14
    int field_18;           // offset 0x18
    int field_1c;           // offset 0x1C
    int field_20;           // offset 0x20 - fullscreen mode flag
    int field_24;           // offset 0x24
    int field_28;           // offset 0x28 - initialized flag
    char padding_2c[0xCC]; // offset 0x2C - padding to 0xF8
    char baseDir[0x40];    // offset 0xF8 - base directory path for CDData
    char padding_138[4];   // offset 0x138
    int startParam;        // offset 0x13C - start parameter for GameLoop
};
