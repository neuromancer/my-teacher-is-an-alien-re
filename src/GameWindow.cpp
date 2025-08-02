#include "GameWindow.h"

extern "C" {
    void FUN_00422250(char*);
    LRESULT WINAPI FUN_00422590(HWND, UINT, WPARAM, LPARAM);
    void _SmackSetSystemRes_4(int);
}

/* Function start: 0x4220A0 */
void GameWindow::CreateGameWindow(HINSTANCE param_1, int param_2, char *param_3, int param_4)
{
    WNDCLASSEXA local_30;

    this->hInstance = param_1;
    this->field_8 = param_2;
    FUN_00422250(param_3);
    if (this->field_20 == 0) {
        if (param_2 == 0) {
            local_30.cbClsExtra = 0;
            local_30.cbWndExtra = 0;
            local_30.hInstance = param_1;
            local_30.cbSize = 0x30;
            local_30.style = 0xb;
            local_30.lpfnWndProc = (WNDPROC)FUN_00422590;
            local_30.hIcon = LoadIconA(param_1, "IDI_ICON1");
            local_30.hCursor = LoadCursorA((HINSTANCE)0x0, (LPCSTR)0x7f00);
            local_30.hbrBackground = (HBRUSH)0x0;
            local_30.lpszMenuName = (LPCSTR)0x0;
            local_30.lpszClassName = "Teacher Demo";
            local_30.hIconSm = local_30.hIcon;
            RegisterClassExA(&local_30);
        }
        int iVar2 = GetSystemMetrics(0);
        iVar2 = (iVar2 + -0x280) / 2;
        int iVar3 = GetSystemMetrics(1);
        iVar3 = (iVar3 + -0x1e0) / 2;
        DWORD dwExStyle = 0;
        this->hWnd = CreateWindowExA(dwExStyle, "Teacher Demo", "Teacher Demo", 0x80000000, iVar2, iVar3, 0x280, 0x1e0, (HWND)0x0, (HMENU)0x0, param_1, (LPVOID)0x0);
    }
    else {
        if (this->field_8 == 0) {
            local_30.cbClsExtra = 0;
            local_30.cbWndExtra = 0;
            local_30.hInstance = this->hInstance;
            local_30.cbSize = 0x30;
            local_30.style = 0xb;
            local_30.lpfnWndProc = (WNDPROC)FUN_00422590;
            local_30.hIcon = LoadIconA(param_1, "IDI_ICON1");
            local_30.hCursor = LoadCursorA((HINSTANCE)0x0, (LPCSTR)0x7f00);
            local_30.hbrBackground = (HBRUSH)0x0;
            local_30.lpszMenuName = (LPCSTR)0x0;
            local_30.lpszClassName = "Teacher Demo";
            local_30.hIconSm = local_30.hIcon;
            RegisterClassExA(&local_30);
        }
        _SmackSetSystemRes_4(2);
        this->hWnd = CreateWindowExA(8, "Teacher Demo", "Teacher Demo", 0x80000000, 0, 0, 0x280, 0x1e0, (HWND)0x0, (HMENU)0x0, param_1, (LPVOID)0x0);
    }
    ShowWindow(this->hWnd, param_4);
}
