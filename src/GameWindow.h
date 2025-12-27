#pragma once

#include <windows.h>

class GameWindow
{
public:
    void CreateGameWindow(HINSTANCE, int, char*, int);
    int InitGraphics();
    void FUN_00422430();

private:
    int field_0;
    HINSTANCE hInstance;
    int field_8;
    HWND hWnd;
    int field_10;
    int field_14;
    int field_18;
    int field_1c;
    int field_20;
};
