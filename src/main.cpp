#include "AnimatedAsset.h"
#include "Manager.h"
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
#include "ZBufferManager.h"
#include "GameLoop.h"
#include "AssetList.h"
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
// void FUN_0040c5d0();


void CheckDebug();
void ClearMessageLog();
void CreateGameObject_1();
void InitWorkBuffer(int, int);
void SetStateFlag(int, int);
void SetCursorVisible(int);
int FileExists(const char *);
void SetErrorCode(unsigned int);
void ParsePath(const char *, char *, char *, char *, char *);
int _chdir(const char *);
}


void FUN_0041e310();
void FUN_00421840();
void ShutdownGameSystems();

extern "C" {
void* __fastcall FUN_00420140(void*, const char*, int);
void __fastcall FUN_00420430(void*);
void* __fastcall FUN_004209e0(void*, const char*, int);
void __fastcall FUN_00420a50(void*);
void __fastcall FUN_00420480(void*);
void __fastcall FUN_00418ee0(void*);
void __fastcall FUN_0041ee30(void*);
void __fastcall FUN_00420250(void*);
void __fastcall FUN_0041f200(void*);
void __fastcall FUN_00418ef0(void*);
void __fastcall FUN_004210d0(void*, const char*);
void __fastcall FUN_0041abf0(void*, int);
void __fastcall FUN_0041abc0(void*, int);
}
// Function Declarations
void InitGameSystems(void);
extern "C" {
  void FUN_0040cd15(); 
  void FUN_0040cd1d(); 
}

/* Function start: 0x40C5D0 */
void RunGame() {
  extern int* DAT_00436984;
  extern int* DAT_00436988;
  extern int* DAT_0043698c;
  extern void* DAT_00435a74;
  extern void* DAT_00435a78;
  extern void* DAT_00435a7c;

  __try {
    InitGameSystems();
    if (g_WorkBuffer_00436974 == 0) {
      ShowError("missing workbuff");
    }
    g_WorkBuffer_00436974->FUN_0041abf0(0); 
    g_WorkBuffer_00436974->FUN_0041abc0(0);

    void* puVar2 = g_Mouse_00436978;
    if (g_Mouse_00436978 != 0) {
        g_Mouse_00436978->FUN_0041ee30();
        FreeMemory(puVar2);
        g_Mouse_00436978 = 0;
    }
    
    puVar2 = AllocateMemory(0x1c0);
    Mouse* pMouse = 0;
    if (puVar2 != 0) {
        pMouse = ((Mouse*)puVar2);
        pMouse->Mouse::Mouse();
    }
    g_Mouse_00436978 = pMouse;
    ParseFile((Parser*)pMouse, "mis\\mouse1.mis", "[MICE]");

    g_Unknown_00436994 = AllocateMemory(0x40);

    puVar2 = AllocateMemory(0x14);
    void* pInit = 0;
    if (puVar2 != 0) {
        pInit = ((Timer*)puVar2)->Init();
    }
    g_Timer_00436980 = (Timer*)pInit;

    puVar2 = AllocateMemory(0xCC);
    pInit = 0;
    if (puVar2 != 0) {
        pInit = ((Manager*)puVar2)->FUN_00420140("question.sav", 1000); 
    }
    g_Manager_00435a84 = (Manager*)pInit;
    ((Manager*)pInit)->FUN_00420430();

    GameState* pGS = (GameState*)AllocateMemory(0x98);
    if (pGS != 0) {
        ((Parser*)pGS)->Parser::Parser();
        pGS->stateValues = 0;
        pGS->stateLabels = 0;
        pGS->maxStates = 0;
        pGS->reserved = 0;
        ParseFile((Parser*)pGS, "mis\\gamestat.mis", "[GAMESTATE%4.4d]", 1);
    }
    g_GameState_00436998 = pGS;

    pGS = (GameState*)AllocateMemory(0x98);
    if (pGS != 0) {
        ((Parser*)pGS)->Parser::Parser();
        pGS->stateValues = 0;
        pGS->stateLabels = 0;
        pGS->maxStates = 0;
        pGS->reserved = 0;
        ParseFile((Parser*)pGS, "mis\\gamestat.mis", "[GAMESTATE%4.4d]", 2);
    }
    g_GameState2_004369a4 = pGS;

    pGS = (GameState*)AllocateMemory(0x98);
    if (pGS != 0) {
        ((Parser*)pGS)->Parser::Parser();
        pGS->stateValues = 0;
        pGS->stateLabels = 0;
        pGS->maxStates = 0;
        pGS->reserved = 0;
        ParseFile((Parser*)pGS, "mis\\gamestat.mis", "[GAMESTATE%4.4d]", 3);
    }
    g_GameState3_0043699c = pGS;

    pGS = (GameState*)AllocateMemory(0x98);
    if (pGS != 0) {
        ((Parser*)pGS)->Parser::Parser();
        pGS->stateValues = 0;
        pGS->stateLabels = 0;
        pGS->maxStates = 0;
        pGS->reserved = 0;
        ParseFile((Parser*)pGS, "mis\\gamestat.mis", "[GAMESTATE%4.4d]", 4);
    }
    g_GameState4_004369a0 = pGS;

    puVar2 = AllocateMemory(0xC);
    pInit = 0;
    if (puVar2 != 0) {
        pInit = ((Parser*)puVar2)->FUN_004209e0("mis\\strings.mis", 1);
    }
    g_Strings_00435a70 = (Parser*)pInit;
    
    puVar2 = AllocateMemory(0x18);
    if (puVar2 != 0) {
        ((int*)puVar2)[2] = 0;
        ((int*)puVar2)[3] = 0;
        ((int*)puVar2)[1] = 0;
        ((int*)puVar2)[0] = 0;
        ((int*)puVar2)[4] = 0;
        ((int*)puVar2)[5] = 0x32;
    }
    DAT_00436984 = (int*)puVar2;

    puVar2 = AllocateMemory(0x18);
    if (puVar2 != 0) {
        ((int*)puVar2)[2] = 0;
        ((int*)puVar2)[3] = 0;
        ((int*)puVar2)[1] = 0;
        ((int*)puVar2)[0] = 0;
        ((int*)puVar2)[4] = 0;
        ((int*)puVar2)[5] = 0x32;
    }
    DAT_00436988 = (int*)puVar2;

    puVar2 = AllocateMemory(0xAC);
    ZBufferManager* pZBuffer = 0;
    if (puVar2 != 0) {
        pZBuffer = ((ZBufferManager*)puVar2)->Init();
    }
    DAT_0043698c = (int*)pZBuffer;

    puVar2 = AllocateMemory(0x1C);
    GameLoop* pGameLoop = 0;
    if (puVar2 != 0) {
        pGameLoop = ((GameLoop*)puVar2)->Init();
    }

    g_Mouse_00436978->FUN_0041f200();
    g_TextManager_00436990->FUN_004210d0("elements\\text1.smk");
    *(int*)((char*)g_TextManager_00436990 + 0x30) = 2;
    g_Timer_00436980->FUN_00418ef0();

    void FUN_0041a150(int, int, int, int, int, int, int, int, int, int);
    FUN_0041a150(8, 1, 1, 1, 5, 0, 0, 0, 0, 0);

    pGameLoop->Run();
    if (pGameLoop != 0) {
        pGameLoop->Cleanup();
        FreeMemory(pGameLoop);
    }

    puVar2 = (void*)DAT_0043698c;
    if (DAT_0043698c != 0) {
        ((ZBufferManager*)DAT_0043698c)->Cleanup();
        FreeMemory(puVar2);
        DAT_0043698c = 0;
    }

    puVar2 = (void*)DAT_00436988;
    if (DAT_00436988 != 0) {
        FUN_0040cd15(); 
        FreeMemory(puVar2);
        DAT_00436988 = 0;
    }

    puVar2 = (void*)DAT_00436984;
    if (DAT_00436984 != 0) {
        FUN_0040cd1d(); 
        FreeMemory(puVar2);
        DAT_00436984 = 0;
    }

    puVar2 = g_Strings_00435a70;
    if (g_Strings_00435a70 != 0) {
        g_Strings_00435a70->FUN_00420a50();
        FreeMemory(puVar2);
        g_Strings_00435a70 = 0;
    }

    puVar2 = g_GameState4_004369a0;
    if (g_GameState4_004369a0 != 0) {
        g_GameState4_004369a0->FUN_00420480();
        FreeMemory(puVar2);
        g_GameState4_004369a0 = 0;
    }
    
    puVar2 = g_GameState3_0043699c;
    if (g_GameState3_0043699c != 0) {
        g_GameState3_0043699c->FUN_00420480();
        FreeMemory(puVar2);
        g_GameState3_0043699c = 0;
    }

    puVar2 = g_GameState2_004369a4;
    if (g_GameState2_004369a4 != 0) {
        g_GameState2_004369a4->FUN_00420480();
        FreeMemory(puVar2);
        g_GameState2_004369a4 = 0;
    }

    puVar2 = g_GameState_00436998;
    if (g_GameState_00436998 != 0) {
        g_GameState_00436998->FUN_00420480();
        FreeMemory(puVar2);
        g_GameState_00436998 = 0;
    }

    puVar2 = g_Timer_00436980;
    if (g_Timer_00436980 != 0) {
        g_Timer_00436980->FUN_00418ee0();
        FreeMemory(puVar2);
        g_Timer_00436980 = 0;
    }

    if (g_Unknown_00436994 != 0) {
        FreeMemory(g_Unknown_00436994);
        g_Unknown_00436994 = 0;
    }

    puVar2 = g_Mouse_00436978;
    if (g_Mouse_00436978 != 0) {
        g_Mouse_00436978->FUN_0041ee30();
        FreeMemory(puVar2);
        g_Mouse_00436978 = 0;
    }

    puVar2 = DAT_00435a74;
    if (DAT_00435a74 != 0) {
        ((AssetList*)DAT_00435a74)->Cleanup();
        FreeMemory(puVar2);
        DAT_00435a74 = 0;
    }
    puVar2 = DAT_00435a78;
    if (DAT_00435a78 != 0) {
        ((AssetList*)DAT_00435a78)->Cleanup();
        FreeMemory(puVar2);
        DAT_00435a78 = 0;
    }
    puVar2 = DAT_00435a7c;
    if (DAT_00435a7c != 0) {
        ((AssetList*)DAT_00435a7c)->Cleanup();
        FreeMemory(puVar2);
        DAT_00435a7c = 0;
    }

    puVar2 = g_Manager_00435a84;
    if (g_Manager_00435a84 != 0) {
        g_Manager_00435a84->FUN_00420250();
        FreeMemory(puVar2);
        g_Manager_00435a84 = 0;
    }

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
/* Function start: 0x422E02 */
int __cdecl CalculateBufferSize(int width, unsigned int height) {
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

// Stubs and Wrappers for Linker
extern "C" {
void* __fastcall FUN_00420140(void* thisptr, const char* a, int b) { return thisptr; }
void __fastcall FUN_00420430(void* thisptr) {}
void* __fastcall FUN_004209e0(void* thisptr, const char* a, int b) { return thisptr; }
void __fastcall FUN_00420a50(void* thisptr) {}
void __fastcall FUN_00420480(void* thisptr) {}
void __fastcall FUN_00418ee0(void* thisptr) {}
void __fastcall FUN_0041ee30(void* thisptr) {}
void __fastcall FUN_00420250(void* thisptr) {}
void __fastcall FUN_0041f200(void* thisptr) {}
void __fastcall FUN_00418ef0(void* thisptr) {}
void __fastcall FUN_004210d0(void* thisptr, const char* a) {}
void __fastcall FUN_0041abf0(void* thisptr, int a) {}
void __fastcall FUN_0041abc0(void* thisptr, int a) {}

// Stubs for new class external implementations
void* __fastcall FUN_0041b760_impl(void* thisptr) { return thisptr; }
void __fastcall FUN_0041b8e0_impl(void* thisptr) {}
void* __fastcall FUN_00417200_impl(void* thisptr) { return thisptr; }
void __fastcall FUN_00417690_impl(void* thisptr) {}
void __fastcall FUN_00417320_impl(void* thisptr) {}
void __fastcall FUN_00404230_impl(void* thisptr) {}
}

// Class Wrappers
void* Manager::FUN_00420140(const char* a, int b) { return ::FUN_00420140(this, a, b); }
void Manager::FUN_00420250() { ::FUN_00420250(this); }
void Manager::FUN_00420430() { ::FUN_00420430(this); }

void* Parser::FUN_004209e0(const char* a, int b) { return ::FUN_004209e0(this, a, b); }
void Parser::FUN_00420a50() { ::FUN_00420a50(this); }

void GameState::FUN_00420480() { ::FUN_00420480(this); }

void Timer::FUN_00418ee0() { ::FUN_00418ee0(this); }
void Timer::FUN_00418ef0() { ::FUN_00418ef0(this); }

void Mouse::FUN_0041ee30() { ::FUN_0041ee30(this); }
void Mouse::FUN_0041f200() { ::FUN_0041f200(this); }

void AnimatedAsset::FUN_004210d0(const char* a) { ::FUN_004210d0(this, a); }

void VBuffer::FUN_0041abf0(int a) { ::FUN_0041abf0(this, a); }
void VBuffer::FUN_0041abc0(int a) { ::FUN_0041abc0(this, a); }