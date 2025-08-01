#include <windows.h>
#include "GameState.h"
#include "Sound.h"
#include "VBuffer.h"
#include "GameWindow.h"

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
	void FUN_00424940(void*);
	void _AIL_shutdown_0();
	void FUN_0041aa10(VBuffer*);
	void FUN_0041e310();
	void FUN_00421840();
	void FUN_004227a0(void*);
	void FUN_00421ea0(void*);
	void FUN_0040d230();
	void FUN_0040c5d0();
	void FUN_00422430(void*);
}

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
  void* temp;
  temp = g_TextManager;
  if (temp != 0) {
    FUN_00421010((void*)temp);
    FUN_00424940((void*)temp);
    g_TextManager = 0;
  }
  temp = g_WorkBuffer;
  if (temp != 0) {
    FUN_0041aa10((VBuffer*)temp);
    FUN_00424940((void*)temp);
    g_WorkBuffer = 0;
  }
  temp = DAT_0043696c;
  if (temp != 0) {
    FUN_0041e310();
    FUN_00424940((void*)temp);
    DAT_0043696c = 0;
  }
  temp = DAT_00436968;
  if (temp != 0) {
    FUN_00421840();
    FUN_00424940((void*)temp);
    DAT_00436968 = 0;
  }
  temp = DAT_00436970;
  if (temp != 0) {
    FUN_004227a0((void*)temp);
    FUN_00424940((void*)temp);
    DAT_00436970 = 0;
  }
  temp = DAT_0043697c;
  if (temp != 0) {
    FUN_00421ea0((void*)temp);
    FUN_00424940((void*)temp);
    DAT_0043697c = 0;
  }
  temp = DAT_00436964;
  if (temp != 0) {
    FUN_00424940((void*)temp);
    DAT_00436964 = 0;
  }
  temp = DAT_00436960;
  if (temp != 0) {
    FUN_00424940((void*)temp);
    DAT_00436960 = 0;
  }
}
