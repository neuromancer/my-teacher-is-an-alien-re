#include "GameWindow.h"
#include "globals.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <smack.h>

extern "C" {
int GetColorBitDepth();
void ShowMessage(const char *);
void InitVideoSystem();
void InitMouseSettings();
int SetCursorVisible(unsigned int);
void AddToStringTable(char *);
int InvalidateVideoMode();
int CleanupVideoSystem();
}

extern "C" int* GetWindowedModeFlag();
LRESULT CALLBACK GameWindowProc(HWND, UINT, WPARAM, unsigned int);

void __stdcall ParseCommandLine(char *);
int ParseCommandLineArgs(char *, char **, int);

/* Function start: 0x420110 */
GameWindow::GameWindow() {
    // Zero entire object (0xb dwords = 44 bytes = 0x2c)
    int* p = (int*)this;
    for (int i = 0xb; i != 0; i--) {
        *p = 0;
        p++;
    }
    field_24 = 0;
    field_28 = 1;
    field_20 = 1;
    savedActiveWindow = GetActiveWindow();
}

/* Function start: 0x420150 */ /* ~96% match */
void GameWindow::CreateGameWindow(HINSTANCE param_1, int param_2, char *param_3,
                                  int param_4) {
  int iVar2;
  int iVar3;
  WNDCLASSEXA local_30;
  DWORD dwExStyle;

  hInstance = param_1;
  field_8 = param_2;
  ParseCommandLine(param_3);
  if (field_20 != 0) {
    if (field_8 == 0) {
      local_30.cbClsExtra = 0;
      local_30.cbWndExtra = 0;
      local_30.hInstance = hInstance;
      local_30.cbSize = 0x30;
      local_30.style = 0xb;
      local_30.lpfnWndProc = (WNDPROC)GameWindowProc;
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
      local_30.lpfnWndProc = (WNDPROC)GameWindowProc;
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
  hWnd = CreateWindowExA(dwExStyle, "Teacher Demo", "Teacher Demo",
                               0x80000000, iVar2, iVar3, 0x280, 0x1e0,
                               (HWND)0x0, (HMENU)0x0, param_1, (LPVOID)0x0);
  ShowWindow(hWnd, param_4);
}

/* Function start: 0x422250 */ /* DEMO ONLY - no full game match */
void __stdcall ParseCommandLine(char *param_1) {
  int iVar1;
  int *piVar2;
  char **piVar3;
  char *local_50[20];
  
  iVar1 = ParseCommandLineArgs(param_1, local_50, 20);
  piVar2 = &g_CmdLineAudioMode_0043d558;
  for (int iVar4 = 0x15; iVar4 != 0; iVar4 = iVar4 - 1) {
    *piVar2 = 0;
    piVar2 = piVar2 + 1;
  }
  for (piVar3 = local_50; iVar1 > 0; iVar1--) {
    if (_strnicmp(*piVar3, "-mis", 4) == 0) {
      strcpy(g_CmdLineDataPath_0043d568, *piVar3 + 4);
    }
    else if (_strcmpi(*piVar3, "-w") == 0) {
      *GetWindowedModeFlag() = 1;
    }
    else if (_strcmpi(*piVar3, "-dd") == 0) {
      g_CmdLineInputMode_0043d560 = 1;
    }
    else if (_strcmpi(*piVar3, "-dd3") == 0) {
      g_CmdLineInputMode_0043d560 = 2;
    }
    else if (_strnicmp(*piVar3, "-lq", 3) == 0) {
      AddToStringTable(*piVar3 + 3);
    }
    else if (_strcmpi(*piVar3, "-db") == 0) {
      *(char *)&g_CmdLineAudioMode_0043d558 = 1;
    }
    else if (_strcmpi(*piVar3, "-ds") == 0) {
      *(char *)&g_CmdLineAudioMode_0043d558 = 2;
    }
    else if (_strcmpi(*piVar3, "-test") == 0) {
      g_DebugFlag_0043d55c = 1;
    }
    else if (_strcmpi(*piVar3, "-f") == 0) {
      *GetWindowedModeFlag() = 0;
    }
    piVar3 = piVar3 + 1;
  }
}

/* Function start: 0x4223F0 */ /* DEMO ONLY - no full game match */
int GameWindow::InitGraphics(void) {
  if (GetColorBitDepth() != 8) {
    ShowMessage(
        "For Optimum performance, please set your display to 256 Colors");
  }
  InitVideoSystem();
  InitMouseSettings();
  SetCursorVisible(0);
  return 1;
}

/* Function start: 0x420530 */
void GameWindow::Shutdown() {
    if (field_28 == 0) {
        return;
    }
    field_28 = 0;
    if (field_20 != 0) {
        SmackSetSystemRes(0);
    }
    SetCursorVisible(1);
    InvalidateVideoMode();
    CleanupVideoSystem();
    DeleteObject(hPalette);
    ReleaseDC(hWnd, hDC);
    SetActiveWindow(savedActiveWindow);
}

extern GameWindow g_GameWindow;

/* Function start: 0x420610 */
extern "C" void* GetGameWindowHandle() {
  return g_GameWindow.hWnd;
}

/* Function start: 0x420600 */
extern "C" int* GetWindowWidth() {
  return &DAT_0043de88;
}

/* Function start: 0x4205E0 */
extern "C" int* GetWindowHeight() {
  return &DAT_0043de8c;
}

/* Function start: 0x4205F0 */
extern "C" int* GetWindowedModeFlag() {
  return &DAT_0043de90;
}

int SetDeviceContext(HDC);
HPALETTE CreateSystemPalette(void);
int SelectAndRealizePalette(HPALETTE);

/* Function start: 0x422590 */ /* DEMO ONLY - no full game match */
LRESULT CALLBACK GameWindowProc(HWND param_1, UINT param_2, WPARAM param_3, unsigned int param_4) {
  switch (param_2) {
  case 1: // WM_CREATE
    DAT_0043de80 = GetDC(param_1);
    SetDeviceContext(DAT_0043de80);
    DAT_0043de84 = CreateSystemPalette();
    SelectAndRealizePalette(DAT_0043de84);
    return 0;
  case 2: // WM_DESTROY
    PostQuitMessage(0);
    return 0;
  case 5: // WM_SIZE
    *(unsigned int *)GetWindowWidth() = (unsigned short)param_4;
    *(unsigned int *)GetWindowHeight() = (unsigned short)((unsigned int)param_4 >> 0x10);
    return 0;
  case 7: // WM_SETFOCUS
    SelectAndRealizePalette(DAT_0043de84);
    InvalidateRect(param_1, (RECT *)0x0, 1);
    return 0;
  case 0x1c: // WM_ACTIVATEAPP
    DAT_0043de94 = param_3;
    return 0;
  default:
    return DefWindowProcA(param_1, param_2, param_3, param_4);
  }
}
