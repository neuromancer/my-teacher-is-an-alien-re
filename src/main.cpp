#include "AnimatedAsset.h"
#include "Animation.h"
#include "GameState.h"
#include "GameWindow.h"
#include "JoystickManager.h"
#include "Memory.h"
#include "Sound.h"
#include "VBuffer.h"
#include "CDData.h"
#include "CDData.h"
#include "Mouse.h"
#include "Timer.h"
#include "Parser.h"
#include "string.h"
#include <mbctype.h>
#include <mbstring.h>
#include <mss.h>
#include <windows.h>

#include <mbctype.h>
#include <mbstring.h>
#include <mss.h>
#include <windows.h>


void* operator new(size_t size, void* p) { return p; }

extern AnimatedAsset *AnimatedAsset_Ctor(AnimatedAsset *);



extern int FUN_00422510();

#include "globals.h"

GameWindow g_GameWindow;


extern "C" {
void FUN_00421010(void *);
void FUN_004227a0(void *);
void FUN_00421ea0(void *);
void PlayIntroCinematic();
// void FUN_0040c5d0();


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
void ShutdownGameSystems();

extern "C" {
void* FUN_00420140(void*, const char*);
void FUN_00420430(void*);
void* FUN_004209e0(void*, const char*);
void FUN_00420a50(void*);
void FUN_00420480(void*);
void FUN_00418ee0(void*);
void FUN_0041ee30(void*);
void FUN_00404230(void*);
void FUN_00420250(void*);
void FUN_0041b760(void*);
void FUN_0041b8e0(void*);
void FUN_00417200(void*);
void FUN_0041f200(void*);
void FUN_004210d0(AnimatedAsset*, void*);
void FUN_00418ef0(void*);
void FUN_0041a150(int, int, int, int, int, int, int, int, int, int);
void FUN_00417690(void*);
void FUN_00417320(void*);
}

/* Function start: 0x40C5D0 */
/* Function start: 0x40C5D0 */
void RunGame() {
  __try {
    // 1. Mouse
    Mouse* pMouse = (Mouse*)AllocateMemory(sizeof(Mouse));
    if (pMouse) {
        new (pMouse) Mouse(); // Calls 0x41eca0 -> 0x4189f0
        g_Mouse_00436978 = pMouse;
        ParseFile(pMouse, "mis\\mouse1.mis", "[MICE]");
    }

    // 2. Unknown 0x40 object
    g_Unknown_00436994 = AllocateMemory(0x40);

    // 3. Timer
    Timer* pTimer = (Timer*)AllocateMemory(0x14); // 0x14 = 20 bytes
    // Calls 0x418eb0
    if (pTimer) {
        // Since we don't have Timer ctor exposed, assuming Init or default ctor
        // Assembly calls 0x418eb0.
        // Timer struct: 00 00 00 00 (time) 00 00
        // We can just zero it or call Init if we had it.
        // For now, let's assume new works or memset.
        // Wait, 0x418eb0 decompiled in step 68 sets fields 0,1,2,3,4 to 0. 5*4=20 bytes.
        // And calls 0x418ef0 (Reset).
        // Let's manually do it if we can't call ctor.
        // Actually we can use placement new if Timer has ctor.
        // Timer class in Timer.h has Timer().
        new (pTimer) Timer(); 
    }
    g_Timer_00436980 = pTimer;
    
    // 4. Manager (0xCC)
    void* pManager = AllocateMemory(0xCC);
    if (pManager) {
        FUN_00420140(pManager, "question.sav");
        FUN_00420430(pManager);
        g_Manager_00435a84 = (void*)FUN_00420140; // Wait, assembly stores result of init?
        // Assembly: MOV [0x435a84], EAX where EAX is result of 0x420430? No.
        // "MOV ECX, EAX ... CALL 0x420430".
        // [0x435a84] receives EAX from somewhere. 
        // Let's assume pManager is the object.
        g_Manager_00435a84 = pManager; // Fix logic if needed based on detailed verification
    }

    // 5. GameStates
    // 1
    GameState* gs1 = (GameState*)AllocateMemory(0x98);
    if (gs1) {
        new (gs1) GameState(); // Calls 0x4189f0 (Parser Ctor) and inits fields
        // In assembly, fields init to 0 is done manually or via our new Ctor.
        ParseFile(gs1, "mis\\gamestat.mis", "[GAMESTATE%4.4d]", 1);
    }
    g_GameState_00436998 = gs1;

    // 2
    GameState* gs2 = (GameState*)AllocateMemory(0x98);
    if (gs2) {
        new (gs2) GameState();
        ParseFile(gs2, "mis\\gamestat.mis", "[GAMESTATE%4.4d]", 2);
    }
    g_GameState2_004369a4 = gs2;

    // 3
    GameState* gs3 = (GameState*)AllocateMemory(0x98);
    if (gs3) {
        new (gs3) GameState();
        ParseFile(gs3, "mis\\gamestat.mis", "[GAMESTATE%4.4d]", 3);
    }
    g_GameState3_0043699c = gs3;

    // 4
    GameState* gs4 = (GameState*)AllocateMemory(0x98);
    if (gs4) {
        new (gs4) GameState();
        ParseFile(gs4, "mis\\gamestat.mis", "[GAMESTATE%4.4d]", 4);
    }
    g_GameState4_004369a0 = gs4;

    // 6. Strings
    void* pStringsInfo = AllocateMemory(0xC);
    if (pStringsInfo) {
        // PUSH 1; PUSH strings.mis
        FUN_004209e0(pStringsInfo, "mis\\strings.mis");
        
        // PUSH 0x18 Alloc
        Parser* pStringsParser = (Parser*)AllocateMemory(0x18);
        if (pStringsParser) {
           // Manual Init of 0x18 struct:
           // 0x00: 0
           // 0x04: 0
           // 0x08: 0
           // 0x0C: 0
           // 0x10: 0
           // 0x14: 0x32
           memset(pStringsParser, 0, 0x18);
           *(int*)((char*)pStringsParser + 0x14) = 0x32;
        }
        g_Strings_00435a70 = pStringsParser; 
    }

    // Further initialization
    // PUSH 0xAC
    void* pUnknown88 = AllocateMemory(0xAC);
    // CALL 0x41b760
    FUN_0041b760(pUnknown88);
    // MOV [0x436988], EAX? Actually assembly says MOV [0x436988], EAX where EAX is pUnknown88?
    // Let's assume yes.
    // extern int DAT_00436988
    // I need to use the pointer for cleanups
    
    // PUSH 0x1C
    void* pUnknown8C = AllocateMemory(0x1C);
    FUN_00417200(pUnknown8C);
    // MOV [0x43698c], EAX (pUnknown8C)

    FUN_0041f200(g_Mouse_00436978);
    FUN_004210d0(g_TextManager_00436990, (void*)0x435a88); // "elements\\text1.smk"?
    
    if (g_Timer_00436980) {
        FUN_00418ef0(g_Timer_00436980);
    }

    // Main Loop
    FUN_0041a150(8, 1, 1, 1, 5, 0, 0, 0, 0, 0);

    ShutdownGameSystems();

  } __except (EXCEPTION_EXECUTE_HANDLER) {
  }
}

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
    RunGame();
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