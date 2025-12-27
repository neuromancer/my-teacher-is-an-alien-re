#include "AnimatedAsset.h"
#include "Animation.h"
#include "GameState.h"
#include "GameWindow.h"
#include "JoystickManager.h"
#include "Memory.h"
#include "Sound.h"
#include "VBuffer.h"
#include "CDData.h"
#include "string.h"
#include <mbctype.h>
#include <mbstring.h>
#include <mss.h>
#include <windows.h>

#include <mbctype.h>
#include <mbstring.h>
#include <mss.h>
#include <windows.h>

extern AnimatedAsset *AnimatedAsset_Ctor(AnimatedAsset *);



extern int FUN_00422510();

#include "globals.h"

GameWindow g_GameWindow;


extern "C" {
void FUN_00421010(void *);
void FUN_004227a0(void *);
void FUN_00421ea0(void *);
void PlayIntroCinematic();
void FUN_0040c5d0();


void CheckDebug();
void ClearMessageLog();
void CreateGameObject_1();
void InitWorkBuffer(int, int);
void SetStateFlag(int, int);
void SetCursorVisible(int);
int FileExists(const char *);
void ShowError(const char *, ...);
void SetErrorCode(unsigned int);
void ParsePath(const char *, char *, char *, char *, char *);
int _chdir(const char *);
}


void FUN_0041e310();
void FUN_00421840();


/* Function start: 0x4192F0 */
int ProcessMessages() {
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
void FUN_00419170(char *);
void FUN_00423aac();
void FUN_00423a54();
void FUN_00422d98(int);
}



/* Function start: 0x430310 */
int GetFileAttributes_Wrapper(const char *param_1, char param_2) {
  int DVar1;

  DVar1 = GetFileAttributesA(param_1);
  if (DVar1 == -1) {
    DVar1 = GetLastError();
    SetErrorCode(DVar1);
    return -1;
  }
  if (((DVar1 & 1) != 0) && ((param_2 & 2) != 0)) {
    DAT_0043bdf0 = 0xd;
    DAT_0043bdf4 = 5;
    return -1;
  }
  return 0;
}

/* Function start: 0x42B300 */
void SetErrorCode(unsigned int errorCode) {
  int iVar1;
  unsigned int *puVar2;

  iVar1 = 0;
  puVar2 = DAT_0043c760;
  DAT_0043bdf4 = errorCode;
  do {
    if (*puVar2 == errorCode) {
      DAT_0043bdf0 = *(int *)(iVar1 * 8 + 0x43c764);
      return;
    }
    puVar2 = puVar2 + 2;
    iVar1 = iVar1 + 1;
  } while (puVar2 < (DAT_0043c760 + 90));
  if ((0x12 < errorCode) && (errorCode < 0x25)) {
    DAT_0043bdf0 = 0xd;
    return;
  }
  if ((0xbb < errorCode) && (errorCode < 0xcb)) {
    DAT_0043bdf0 = 8;
    return;
  }
  DAT_0043bdf0 = 0x16;
  return;
}

/* Function start: 0x4195A0 */
int FileExists(const char *filename) {
  return GetFileAttributesA(filename) != -1;
}

/* Function start: 0x4261C0 */
void ParsePath(const char *path, char *drive, char *dir, char *fname,
               char *ext) {
  const char *pbVar3;
  const char *_Source = 0;
  const char *local_4 = 0;
  size_t sVar2;

  if (path[1] == ':') {
    if (drive) {
      _mbsnbcpy((unsigned char *)drive, (unsigned char *)path, 2);
      drive[2] = '\0';
    }
    path += 2;
  } else if (drive) {
    *drive = '\0';
  }

  pbVar3 = path;
  while (*pbVar3 != 0) {
    char bVar1 = *pbVar3;
    if ((_mbctype[(unsigned char)bVar1 + 1] & 4) == 0) {
      if (bVar1 == '\\' || bVar1 == '/') {
        _Source = pbVar3 + 1;
      } else if (bVar1 == '.') {
        local_4 = pbVar3;
      }
    } else {
      pbVar3++;
    }
    pbVar3++;
  }

  if (_Source) {
    if (dir) {
      sVar2 = _Source - path;
      if (sVar2 > 255) {
        sVar2 = 255;
      }
      _mbsnbcpy((unsigned char *)dir, (unsigned char *)path, sVar2);
      dir[sVar2] = '\0';
    }
    path = _Source;
  } else if (dir) {
    *dir = '\0';
  }

  if (local_4 && local_4 > path) {
    if (fname) {
      sVar2 = local_4 - path;
      if (sVar2 > 255) {
        sVar2 = 255;
      }
      _mbsnbcpy((unsigned char *)fname, (unsigned char *)path, sVar2);
      fname[sVar2] = '\0';
    }
    if (ext) {
      strcpy(ext, local_4);
    }
  } else {
    if (fname) {
      strcpy(fname, path);
    }
    if (ext) {
      *ext = '\0';
    }
  }
}

/* Function start: 0x41A670 */
void CheckDebug(void) {
  char local_94[128];
  CDData *pvVar2;

  __try {
    pvVar2 = g_CDData_0043697c;
    if (g_CDData_0043697c == (CDData *)0x0) {
      CDData *local_14 = (CDData *)AllocateMemory(0x1e5);
      pvVar2 = (CDData *)0x0;
      if (local_14 != (CDData *)0x0) {
        pvVar2 = local_14->Init("cddata", DAT_0043d568);
      }
    }
    g_CDData_0043697c = pvVar2;
    if (DAT_0043d568[0] != '\0') {
      sprintf(local_94, "%s\\%s", DAT_0043d568, pvVar2->field_0x1c5);
      if (FileExists(local_94)) {
        g_CDData_0043697c->ChangeDirectory((unsigned char *)local_94);
      } else {
        ShowError("Invalid CD path specified on command line '%s'", local_94);
      }
    } else {
      if (FileExists(pvVar2->field_0x1c5) ||
          FileExists("Develop.___")) {
        DAT_0043d564 = 1;
        if (g_CDData_0043697c->ChangeDirectory(
                (unsigned char *)g_CDData_0043697c->field_0x1c5)) {
          ShowError("Invalid Development directory '%s'", g_CDData_0043697c->field_0x1c5);
        }
      } else {
        int i = 3;
        for (; i < 0x1a; i++) {
          if (g_CDData_0043697c->CheckFileOnDrive(i)) {
            if (g_CDData_0043697c->ChangeToDriveDirectory(i)) {
              ShowError("Invalid CD directory");
            }
            break;
          }
        }
        if (0x18 < i) {
          ShowError("Missing the Teacher CD-ROM");
        }
      }
    }
  } __except (EXCEPTION_EXECUTE_HANDLER) {
  }
}



/* Function start: 0x422E02 */
int CalculateBufferSize(int width, unsigned int height) {
  return (((width + 3) & ~3U) * height) + DAT_00437f4c;
}

/* Function start: 0x41A3D0 */
void InitGameSystems(void) {
  JoystickManager *pJoystick;
  JoystickManager *pJoystickInit;
  Sound *pSound;
  Sound *pSoundInit;
  AnimatedAsset *pTextManager;
  AnimatedAsset *pTextManagerInit;

  __try {
    g_Buffer_00436960 = (char *)AllocateMemory(0x100);
    g_Buffer_00436964 = AllocateMemory(CalculateBufferSize(0x280, 0x1e0));
    CheckDebug();
    ClearMessageLog();
    CreateGameObject_1();
    InitWorkBuffer(0x280, 0x1e0);
    pJoystick = (JoystickManager *)AllocateMemory(0x1b8);
    pJoystickInit = (JoystickManager *)0x0;
    if (pJoystick != (JoystickManager *)0x0) {
      pJoystickInit = pJoystick->Init(
          (unsigned int)*(unsigned char *)((int)g_Unknown_00436970 + 0x44));
    }
    g_JoystickManager_00436968 = pJoystickInit;
    pSound = (Sound *)AllocateMemory(0x3c);
    pSoundInit = (Sound *)0x0;
    if (pSound != (Sound *)0x0) {
      pSoundInit = (Sound *)pSound->Init(0x5622, 8, 1);
    }
    g_Sound_0043696c = pSoundInit;
    pTextManager = (AnimatedAsset *)AllocateMemory(0x38);
    pTextManagerInit = (AnimatedAsset *)0x0;
    if (pTextManager != (AnimatedAsset *)0x0) {
      pTextManagerInit = AnimatedAsset_Ctor(pTextManager);
    }
    g_TextManager_00436990 = pTextManagerInit;
    g_TextManager_00436990->LoadAnimatedAsset("elements\\barrel06.smk");
    SetStateFlag(0, 1);
    SetCursorVisible(0);
  } __except (EXCEPTION_EXECUTE_HANDLER) {
  }
}

/* Function start: 0x422520 */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,
                   int nCmdShow) {
  int uStack_14;
  g_GameWindow.CreateGameWindow(hInstance, (int)hPrevInstance, lpCmdLine, nCmdShow);
  if (g_GameWindow.InitGraphics() == 0) {
    return 0;
  }
  UpdateWindow(g_GameWindow.hWnd);
  if (DAT_0043d55c == 0) {
    FUN_0040c5d0();
  } else {
    PlayIntroCinematic();
  }
  g_GameWindow.FUN_00422430();
  return uStack_14;
}

/* Function start: 0x41A550 */
void ShutdownGameSystems(void) {
  if (g_TextManager_00436990 != 0) {
    FUN_00421010(g_TextManager_00436990);
    FreeMemory(g_TextManager_00436990);
    g_TextManager_00436990 = 0;
  }
  if (g_WorkBuffer_00436974 != 0) {
    g_WorkBuffer_00436974->~VBuffer();
    FreeMemory(g_WorkBuffer_00436974);
    g_WorkBuffer_00436974 = 0;
  }
  if (g_Sound_0043696c != 0) {
    AIL_shutdown();
    FreeMemory(g_Sound_0043696c);
    g_Sound_0043696c = 0;
  }
  if (g_JoystickManager_00436968 != 0) {
    FUN_00421840();
    FreeMemory(g_JoystickManager_00436968);
    g_JoystickManager_00436968 = 0;
  }
  if (g_Unknown_00436970 != 0) {
    FUN_004227a0(g_Unknown_00436970);
    FreeMemory(g_Unknown_00436970);
    g_Unknown_00436970 = 0;
  }
  if (g_CDData_0043697c != 0) {
    FUN_00421ea0(g_CDData_0043697c);
    FreeMemory(g_CDData_0043697c);
    g_CDData_0043697c = 0;
  }
  if (g_Buffer_00436964 != 0) {
    FreeMemory(g_Buffer_00436964);
    g_Buffer_00436964 = 0;
  }
  if (g_Buffer_00436960 != 0) {
    FreeMemory(g_Buffer_00436960);
    g_Buffer_00436960 = 0;
  }
}

/* Function start: 0x40d28c */
void CleanupCinematic(void) {}

/* Function start: 0x40d230 */
void PlayIntroCinematic(void) {
  InitGameSystems();
  Animation anim;
  anim.Play("cine\\cine0001.smk", 0x20);
  ShutdownGameSystems();
}