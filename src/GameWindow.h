#pragma once

#include <windows.h>

class GameWindow
{
public:
    GameWindow();
    void CreateGameWindow(HINSTANCE, int, char*, int);
    int InitGraphics();
    void FUN_00422430();

    HWND savedActiveWindow; // offset 0x00 - saved active window from GetActiveWindow
    HINSTANCE hInstance;    // offset 0x04
    int field_8;            // offset 0x08
    HWND hWnd;              // offset 0x0C
    int field_10;           // offset 0x10
    int field_14;           // offset 0x14
    int field_18;           // offset 0x18
    int field_1c;           // offset 0x1C
    int field_20;           // offset 0x20
    int field_24;           // offset 0x24
    int field_28;           // offset 0x28
};
