#include <windows.h>
#include "GameState.h"
#include "Sound.h"
#include "VBuffer.h"
#include "GameWindow.h"
#include "Memory.h"
#include "AnimatedAsset.h"

Sound* g_sound;

GameState* g_GameState = (GameState*)0x00436998;
extern int DAT_004373bc;
extern int FUN_00422510();

void *g_TextManager = (void*)0x00436990;
VBuffer *g_WorkBuffer = (VBuffer*)0x00436974;
void *DAT_0043696c = (void*)0x0043696c;
void *DAT_00436968 = (void*)0x00436968;
void *DAT_00436970 = (void*)0x00436970;
void *DAT_0043697c = (void*)0x0043697c;
void *DAT_00436964 = (void*)0x00436964;
void *DAT_00436960 = (void*)0x00436960;

extern "C" {
	void FUN_00421010(void*);
	void FUN_004227a0(void*);
	void FUN_00421ea0(void*);
	void FUN_0040d230();
	void FUN_0040c5d0();
	void FUN_00422430(void*);
    int CalculateBufferSize(int, int);
    void CheckDebug();
    void ClearMessageLog();
    void CreateGameObject_1();
    void InitWorkBuffer(int, int);
    void* JoystickManager_Constructor(void*, int);
    void* Sound_Init(void*, int, int, int);
    void SetStateFlag(int, int);
    void SetCursorVisible(int);
}

void _AIL_shutdown_0();
void FUN_0041e310();
void FUN_00421840();

extern HWND DAT_0043de7c;
extern int DAT_0043d55c;


/* Function start: 0x4192F0 */
int ProcessMessages()
{
    MSG local_1c;
    int iVar1;

    do {
        iVar1 = PeekMessageA(&local_1c, NULL, 0, 0, PM_REMOVE);
        while (iVar1 != 0) {
            TranslateMessage(&local_1c);
            if (local_1c.message == WM_KEYDOWN) {
                DAT_004373bc = local_1c.wParam;
                if (0x7f < local_1c.wParam) {
                    DAT_004373bc = local_1c.wParam & 0x2f;
                }
                if (local_1c.wParam == VK_F12) {
                    return 1;
                }
            }
            DispatchMessageA(&local_1c);
            iVar1 = PeekMessageA(&local_1c, NULL, 0, 0, PM_REMOVE);
        }
        iVar1 = FUN_00422510();
        if (iVar1 != 0) {
            return 0;
        }
    } while (1);
}

extern "C" {
    int FUN_00423cd9();
    void FUN_00419170(char*);
    void FUN_00423aac();
    void FUN_00423a54();
    void FUN_00422d98(int);
}

/* Function start: 0x4223F0 */
int InitGraphics(void)
{
    if (FUN_00423cd9() != 8) {
        FUN_00419170("For Optimum performance, please set your display to 256 colors.");
    }
    FUN_00423aac();
    FUN_00423a54();
    FUN_00422d98(0);
    return 1;
}

/* Function start: 0x41A3D0 */
void InitGameSystems(void)
{
    DAT_00436960 = AllocateMemory(0x100);
    DAT_00436964 = AllocateMemory(CalculateBufferSize(0x280,0x1e0));
    CheckDebug();
    ClearMessageLog();
    CreateGameObject_1();
    InitWorkBuffer(0x280,0x1e0);
    void* pJoystickManager = AllocateMemory(0x1b8);
    if (pJoystickManager != (void *)0x0) {
        DAT_00436968 = JoystickManager_Constructor(pJoystickManager, *(char*)((int)DAT_00436970 + 0x44));
    }
    void* pSound = AllocateMemory(0x3c);
    if (pSound != (void *)0x0) {
        DAT_0043696c = Sound_Init(pSound,0x5622,8,1);
    }
    g_TextManager = new AnimatedAsset();
    ((AnimatedAsset*)g_TextManager)->LoadAnimatedAsset("elements\\barrel06.smk");
    SetStateFlag(0,1);
    SetCursorVisible(0);
}

/* Function start: 0x422520 */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR lpCmdLine, int nCmdShow)
{
  GameWindow gameWindow;
  gameWindow.CreateGameWindow(hInstance, nCmdShow, lpCmdLine, 0);
  if (InitGraphics() == 0) {
    return 0;
  }
  UpdateWindow(DAT_0043de7c);
  if (DAT_0043d55c == 0) {
    FUN_0040d230();
  }
  else {
    FUN_0040c5d0();
  }
  FUN_00422430(&gameWindow);
  return 1;
}

/* Function start: 0x41A550 */
void ShutdownGameSystems(void)
{
  if (g_TextManager != 0) {
    FUN_00421010(g_TextManager);
    FreeMemory(g_TextManager);
    g_TextManager = 0;
  }
  if (g_WorkBuffer != 0) {
    g_WorkBuffer->~VBuffer();
    FreeMemory(g_WorkBuffer);
    g_WorkBuffer = 0;
  }
  if (DAT_0043696c != 0) {
    _AIL_shutdown_0();
    FreeMemory(DAT_0043696c);
    DAT_0043696c = 0;
  }
  if (DAT_00436968 != 0) {
    FUN_00421840();
    FreeMemory(DAT_00436968);
    DAT_00436968 = 0;
  }
  if (DAT_00436970 != 0) {
    FUN_004227a0(DAT_00436970);
    FreeMemory(DAT_00436970);
    DAT_00436970 = 0;
  }
  if (DAT_0043697c != 0) {
    FUN_00421ea0(DAT_0043697c);
    FreeMemory(DAT_0043697c);
    DAT_0043697c = 0;
  }
  if (DAT_00436964 != 0) {
    FreeMemory(DAT_00436964);
    DAT_00436964 = 0;
  }
  if (DAT_00436960 != 0) {
    FreeMemory(DAT_00436960);
    DAT_00436960 = 0;
  }
}
