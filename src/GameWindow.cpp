#include "GameWindow.h"
#include "globals.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <smack.h>

extern "C" {
int GetColorBitDepth();
void ShowMessage(const char *);
int ShowMessageYesNo(char *, ...);
void InitVideoSystem();
void InitMouseSettings();
int SetCursorVisible(unsigned int);
void AddToStringTable(char *);
int InvalidateVideoMode();
int CleanupVideoSystem();
}

extern "C" int* GetWindowWidth();

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
    startupParam = 0;
    initialized = 1;
    fullscreenMode = 1;
    savedActiveWindow = GetActiveWindow();
}

/* Function start: 0x420140 */
GameWindow::~GameWindow() {
    Shutdown();
}

/* Function start: 0x420150 */
void GameWindow::CreateGameWindow(HINSTANCE param_1, int param_2, char *param_3,
                                  int param_4) {
  WNDCLASSEXA local_30;

  hInstance = param_1;
  displayMode = param_2;
  ParseCommandLine(param_3);
  if (fullscreenMode != 0) {
    if (displayMode == 0) {
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
      local_30.lpszClassName = "Teacher v(0.950)";
      local_30.hIconSm = local_30.hIcon;
      RegisterClassExA(&local_30);
    }
    SmackSetSystemRes(2);
    hWnd = CreateWindowExA(8, "Teacher v(0.950)", "Teacher v(0.950)",
                                 0x80000000, 0, 0, 0x280, 0x1e0,
                                 (HWND)0x0, (HMENU)0x0, param_1, (LPVOID)0x0);
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
      local_30.lpszClassName = "Teacher v(0.950)";
      local_30.hIconSm = local_30.hIcon;
      RegisterClassExA(&local_30);
    }
    int iVar2 = (GetSystemMetrics(0) - 0x280) / 2;
    int iVar3 = (GetSystemMetrics(1) - 0x1e0) / 2;
    hWnd = CreateWindowExA(0, "Teacher v(0.950)", "Teacher v(0.950)",
                                 0x80000000, iVar2, iVar3, 0x280, 0x1e0,
                                 (HWND)0x0, (HMENU)0x0, param_1, (LPVOID)0x0);
  }
  ShowWindow(hWnd, param_4);
}

/* Function start: 0x420300 */
void __stdcall ParseCommandLine(char *param_1) {
  int iVar1;
  int *piVar2;
  char **piVar3;
  char *local_50[20];
  
  iVar1 = ParseCommandLineArgs(param_1, local_50, 20);
  piVar2 = &g_CmdLineAudioMode_0043d558;
  for (int iVar4 = 0x16; iVar4 != 0; iVar4 = iVar4 - 1) {
    *piVar2 = 0;
    piVar2 = piVar2 + 1;
  }
  for (piVar3 = local_50; iVar1 > 0; iVar1--) {
    if (_strnicmp(*piVar3, "-mis", 4) == 0) {
      strcpy(g_GameWindow.baseDir, *piVar3 + 4);
    }
    else if (_strcmpi(*piVar3, "-w") == 0) {
      *GetWindowWidth() = 1;
    }
    else if (_strcmpi(*piVar3, "-dd") == 0) {
      g_CmdLineInputMode_0043d560 = 1;
    }
    else if (_strnicmp(*piVar3, "-lq", 3) == 0) {
      AddToStringTable(*piVar3 + 3);
    }
    else if (_strnicmp(*piVar3, "/mis=", 5) == 0) {
      g_StartBlock_00472e2c = atoi(*piVar3 + 5);
    }
    else if (_strcmpi(*piVar3, "-dd3") == 0) {
      g_CmdLineInputMode_0043d560 = 2;
    }
    else if (_strcmpi(*piVar3, "/nods") == 0) {
      g_DirectSoundFlag_0046a87c = 0;
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
      *GetWindowWidth() = 0;
    }
    piVar3 = piVar3 + 1;
  }
}

/* Function start: 0x4204F0 */
int GameWindow::InitGraphics(void) {
  if (GetColorBitDepth() != 8) {
    int result = ShowMessageYesNo(
        "For Optimum performance, Set your display to 256 Colors.\nWould you like to exit now to set your display?");
    if (result == 6) {
      return 0;
    }
  }
  InitVideoSystem();
  InitMouseSettings();
  SetCursorVisible(0);
  return 1;
}

/* Function start: 0x420530 */
void GameWindow::Shutdown() {
    if (initialized == 0) {
        return;
    }
    initialized = 0;
    if (fullscreenMode != 0) {
        SmackSetSystemRes(0);
    }
    SetCursorVisible(1);
    InvalidateVideoMode();
    CleanupVideoSystem();
    DeleteObject(hPalette);
    ReleaseDC(hWnd, hDC);
    SetActiveWindow(savedActiveWindow);
}

/* Function start: 0x4205D0 */
extern "C" void* GetGameWindowHandle() {
  return g_GameWindow.hWnd;
}

/* Function start: 0x420600 */
extern "C" int* GetWindowWidth() {
  return &g_WindowWidth_00472d10;
}

// g_ScreenWidth_00472d08, g_ScreenHeight_00472d0c — defined in globals.cpp

/* Function start: 0x4205E0 */
extern "C" int* GetScreenWidth() {
  return &g_ScreenWidth_00472d08;
}

/* Function start: 0x4205F0 */
extern "C" int* GetScreenHeight() {
  return &g_ScreenHeight_00472d0c;
}

int SetDeviceContext(HDC);
HPALETTE CreateSystemPalette(void);
int SelectAndRealizePalette(HPALETTE);

/* Function start: 0x420690 */
LRESULT CALLBACK GameWindowProc(HWND param_1, UINT param_2, WPARAM param_3, unsigned int param_4) {
  switch (param_2) {
  case 1: // WM_CREATE
    g_WindowDC_00472d00 = GetDC(param_1);
    SetDeviceContext(g_WindowDC_00472d00);
    g_WindowPalette_00472d04 = CreateSystemPalette();
    SelectAndRealizePalette(g_WindowPalette_00472d04);
    return 0;
  case 2: // WM_DESTROY
    PostQuitMessage(0);
    return 0;
  case 5: // WM_SIZE
    *(unsigned int *)GetScreenWidth() = (unsigned short)param_4;
    *(unsigned int *)GetScreenHeight() = (unsigned short)((unsigned int)param_4 >> 0x10);
    return 0;
  case 7: // WM_SETFOCUS
    SelectAndRealizePalette(g_WindowPalette_00472d04);
    InvalidateRect(param_1, (RECT *)0x0, 1);
    return 0;
  case 0x1c: // WM_ACTIVATEAPP
    g_ActivateAppState_00472d14 = param_3;
    return 0;
  default:
    return DefWindowProcA(param_1, param_2, param_3, param_4);
  }
}
