#include "GameWindow.h"
#include "globals.h"
#include <string.h>
#include <stdio.h>
#include <smack.h>

extern "C" {
void __stdcall FUN_00422250(char *);
LRESULT WINAPI FUN_00422590(HWND, UINT, WPARAM, LPARAM);
int GetColorBitDepth();
void ShowMessage(const char *);
void InitVideoSystem();
void InitMouseSettings();
int SetCursorVisible(unsigned int);

int FUN_00419800(char *, char **, int);
void FUN_00419220(char *);
int* FUN_00422500();
}

/* Function start: 0x4220A0 */
void GameWindow::CreateGameWindow(HINSTANCE param_1, int param_2, char *param_3,
                                  int param_4) {
  int iVar2;
  int iVar3;
  WNDCLASSEXA local_30;
  DWORD dwExStyle;

  this->hInstance = param_1;
  this->field_8 = param_2;
  FUN_00422250(param_3);
  if (this->field_20 != 0) {
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
    SmackSetSystemRes(2);
    dwExStyle = 8;
    iVar2 = 0;
    iVar3 = 0;
  } else {
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
    iVar2 = GetSystemMetrics(0);
    iVar2 = (iVar2 + -0x280) / 2;
    iVar3 = GetSystemMetrics(1);
    iVar3 = (iVar3 + -0x1e0) / 2;
    dwExStyle = 0;
  }
  this->hWnd = CreateWindowExA(dwExStyle, "Teacher Demo", "Teacher Demo",
                               0x80000000, iVar2, iVar3, 0x280, 0x1e0,
                               (HWND)0x0, (HMENU)0x0, param_1, (LPVOID)0x0);
  ShowWindow(this->hWnd, param_4);
}

extern "C" void __stdcall FUN_00422250(char *param_1);

/* Function start: 0x422250 */
void __stdcall FUN_00422250(char *param_1) {
  int iVar1;
  int *piVar2;
  int iVar3;
  char *local_50[20];
  
  iVar1 = FUN_00419800(param_1, local_50, 20);
  piVar2 = &DAT_0043d558;
  for (iVar3 = 0x15; iVar3 != 0; iVar3 = iVar3 + -1) {
    *piVar2 = 0;
    piVar2 = piVar2 + 1;
  }
  if (0 < iVar1) {
    for(iVar3 = 0; iVar3 < iVar1; iVar3++) {
      char *arg = local_50[iVar3];
      if (_strnicmp(arg, "-mis", 4) == 0) {
        strcpy(DAT_0043d568, arg + 4);
      }
      else if (_strcmpi(arg, "-w") == 0) {
        *FUN_00422500() = 1;
      }
      else if (_strcmpi(arg, "-f") == 0) {
         *FUN_00422500() = 0;
      }
      else if (_strcmpi(arg, "-dd") == 0) {
        DAT_0043d560 = 1;
      }
      else if (_strcmpi(arg, "-dd3") == 0) {
        DAT_0043d560 = 2;
      }
      else if (_strnicmp(arg, "-lq", 3) == 0) {
        FUN_00419220(arg + 3);
      }
      else if (_strcmpi(arg, "-db") == 0) {
        *(char *)&DAT_0043d558 = 1;
      }
      else if (_strcmpi(arg, "-ds") == 0) {
        *(char *)&DAT_0043d558 = 2;
      }
      else if (_strcmpi(arg, "-test") == 0) {
        DAT_0043d55c = 1;
      }
    }
  }
}

/* Function start: 0x4223F0 */
int GameWindow::InitGraphics(void) {
  if (GetColorBitDepth() != 8) {
    ShowMessage(
        "For Optimum performance, please set your display to 256 colors.");
  }
  InitVideoSystem();
  InitMouseSettings();
  SetCursorVisible(0);
  return 1;
}

/* Function start: 0x422430 */
void GameWindow::FUN_00422430() {
}

extern GameWindow g_GameWindow;
extern "C" void* GetGameWindowHandle();

/* Function start: 0x4224D0 */
void* GetGameWindowHandle() {
  return g_GameWindow.hWnd;
}
