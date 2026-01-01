#include "AnimatedAsset.h"

#include "Animation.h"
#include "GameState.h"
#include "GameWindow.h"
#include "InputManager.h"
#include "Memory.h"
#include "Sound.h"
#include "VBuffer.h"
#include "CDData.h"
#include "Mouse.h"
#include "Timer.h"
#include "Parser.h"
#include "FlagArray.h"
#include "StringTable.h"
#include "ZBufferManager.h"
#include "GameLoop.h"
#include "AssetList.h"
#include "Message.h"
#include "string.h"
#include <mbctype.h>
#include <mbstring.h>
#include <mss.h>
#include <windows.h>
#include "GameConfig.h"





extern int FUN_00422510();

#include "globals.h"

GameWindow g_GameWindow;


extern "C" {
void FUN_00421010(void *);
void FUN_004227a0(void *);
void FUN_00421ea0(void *);
void PlayIntroCinematic();


void CheckDebug();
void ClearMessageLog();
void InitWorkBuffer(int, int);
void SetStateFlag(int, int);
int SetCursorVisible(unsigned int);
int FileExists(const char *);
void SetErrorCode(unsigned int);
void ParsePath(const char *, char *, char *, char *, char *);
int _chdir(const char *);
}


void FUN_00421840();
void ShutdownGameSystems();
// Function Declarations
void InitGameSystems(void);
extern "C" {
  void FUN_0040cd15(); 
  void FUN_0040cd1d();
  void FUN_004227a0(void*); // Config Cleanup
}

/* Function start: 0x40C5D0 */
void RunGame() {
  extern int* DAT_00436984;
  extern int* DAT_00436988;
  extern int* DAT_0043698c;
  extern void* DAT_00435a74;
  extern void* DAT_00435a78;
  extern void* DAT_00435a7c;

  //__try {
    InitGameSystems();
    if (g_WorkBuffer_00436974 == 0) {
      ShowError("missing workbuff");
    }
    g_WorkBuffer_00436974->SetVideoMode(); 
    g_WorkBuffer_00436974->ClearScreen(0);

    void* puVar2; //= g_Mouse_00436978;
    if (g_Mouse_00436978 != 0) {
        delete g_Mouse_00436978;
        g_Mouse_00436978 = 0;
    }
    
    g_Mouse_00436978 = new Mouse(); //pMouse;
    ParseFile(g_Mouse_00436978, "mis\\mouse1.mis", "[MICE]");
    
    g_Unknown_00436994 = new char[0x40];

    g_Timer_00436980 = new Timer();

    g_Manager_00435a84 = new FlagArray("question.sav", 1000);
    g_Manager_00435a84->ClearAllFlags();

    g_GameState_00436998 = new GameState();
    ParseFile(g_GameState_00436998, "mis\\gamestat.mis", "[GAMESTATE%4.4d]", 1);

    g_GameState2_004369a4 = new GameState();
    ParseFile(g_GameState2_004369a4, "mis\\gamestat.mis", "[GAMESTATE%4.4d]", 2);

    g_GameState3_0043699c = new GameState();
    ParseFile(g_GameState3_0043699c, "mis\\gamestat.mis", "[GAMESTATE%4.4d]", 3);

    g_GameState4_004369a0 = new GameState();
    ParseFile(g_GameState4_004369a0, "mis\\gamestat.mis", "[GAMESTATE%4.4d]", 4);

    g_Strings_00435a70 = new StringTable("mis\\strings.mis", 1);
    
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

    ZBufferManager* pZBuffer = new ZBufferManager();
    DAT_0043698c = (int*)pZBuffer;

    GameLoop* pGameLoop = new GameLoop();

    g_Mouse_00436978->DrawCursor();
    g_TextManager_00436990->LoadAnimatedAsset("elements\\text1.smk");
    g_TextManager_00436990->charAdvance = 2;
    g_Timer_00436980->Reset();

    SC_Message_Send(8, 1, 1, 1, 5, 0, 0, 0, 0, 0);

    pGameLoop->Run();
    if (pGameLoop != 0) {
        delete pGameLoop;
    }

    if (DAT_0043698c != 0) {
        delete ((ZBufferManager*)DAT_0043698c);
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

    if (g_Strings_00435a70 != 0) {
        delete g_Strings_00435a70;
        g_Strings_00435a70 = 0;
    }

    if (g_GameState4_004369a0 != 0) {
        delete g_GameState4_004369a0;
        g_GameState4_004369a0 = 0;
    }
    
    if (g_GameState3_0043699c != 0) {
        delete g_GameState3_0043699c;
        g_GameState3_0043699c = 0;
    }

    if (g_GameState2_004369a4 != 0) {
        delete g_GameState2_004369a4;
        g_GameState2_004369a4 = 0;
    }

    if (g_GameState_00436998 != 0) {
        delete g_GameState_00436998;
        g_GameState_00436998 = 0;
    }

    if (g_Timer_00436980 != 0) {
        delete g_Timer_00436980;
        g_Timer_00436980 = 0;
    }

    if (g_Unknown_00436994 != 0) {
        FreeMemory(g_Unknown_00436994);
        g_Unknown_00436994 = 0;
    }

    if (g_Mouse_00436978 != 0) {
        delete g_Mouse_00436978;
        g_Mouse_00436978 = 0;
    }

    if (DAT_00435a74 != 0) {
        delete ((AssetList*)DAT_00435a74);
        DAT_00435a74 = 0;
    }
    if (DAT_00435a78 != 0) {
        delete ((AssetList*)DAT_00435a78);
        DAT_00435a78 = 0;
    }
    if (DAT_00435a7c != 0) {
        delete ((AssetList*)DAT_00435a7c);
        DAT_00435a7c = 0;
    }

    if (g_Manager_00435a84 != 0) {
        delete g_Manager_00435a84;
        g_Manager_00435a84 = 0;
    }

    ShutdownGameSystems();

  //} __except (EXCEPTION_EXECUTE_HANDLER) {
  //}
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

/* Function start: 0x421AF0 */
int WaitForInput() {
  while (DAT_004373bc == 0) {
    ProcessMessages();
  }
  int iVar1 = DAT_004373bc;
  DAT_004373bc = 0;
  return iVar1;
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
  } while ((unsigned int)puVar2 < 0x43c8c8);
  if (errorCode >= 0x13 && errorCode <= 0x24) {
    DAT_0043bdf0 = 0xd;
    return;
  }
  if (errorCode >= 0xbc && errorCode <= 0xca) {
    DAT_0043bdf0 = 8;
    return;
  }
  DAT_0043bdf0 = 0x16;
  return;
}

/* Function start: 0x4195A0 */
int FileExists(const char *filename) {
  int iVar1;
  iVar1 = GetFileAttributes_Wrapper(filename, 0);
  return iVar1 == 0;
}

/* Function start: 0x4261C0 */
void ParsePath(const char *param_1, char *param_2, char *param_3, char *param_4,
               char *param_5) {
  unsigned char bVar1;
  unsigned char *_Source;
  size_t sVar2;
  unsigned char *pbVar3;
  unsigned char *local_4;

  local_4 = (unsigned char *)0;
  if (((unsigned char *)param_1)[1] == 0x3a) {
    if (param_2 != (char *)0) {
      _mbsnbcpy((unsigned char *)param_2, (unsigned char *)param_1, 2);
      param_2[2] = '\0';
    }
    param_1 = param_1 + 2;
  }
  else if (param_2 != (char *)0) {
    *param_2 = '\0';
  }
  _Source = (unsigned char *)0;
  pbVar3 = (unsigned char *)param_1;
  if (*(unsigned char *)param_1 != 0) {
    do {
      bVar1 = *pbVar3;
      if ((_mbctype[bVar1 + 1] & 4) == 0) {
        if ((bVar1 == 0x2f) || (bVar1 == 0x5c)) {
          _Source = pbVar3 + 1;
        }
        else if (bVar1 == 0x2e) {
          local_4 = pbVar3;
        }
      }
      else {
        pbVar3 = pbVar3 + 1;
      }
      pbVar3 = pbVar3 + 1;
    } while (*pbVar3 != 0);
  }
  if (_Source == (unsigned char *)0) {
    _Source = (unsigned char *)param_1;
    if (param_3 != (char *)0) {
      *param_3 = '\0';
    }
  }
  else if (param_3 != (char *)0) {
    sVar2 = (int)_Source - (int)param_1;
    if (sVar2 >= 0xff) {
      sVar2 = 0xff;
    }
    _mbsnbcpy((unsigned char *)param_3, (unsigned char *)param_1, sVar2);
    param_3[sVar2] = '\0';
  }
  if ((local_4 == (unsigned char *)0) || (local_4 < _Source)) {
    if (param_4 != (char *)0) {
      sVar2 = (int)pbVar3 - (int)_Source;
      if (sVar2 >= 0xff) {
        sVar2 = 0xff;
      }
      _mbsnbcpy((unsigned char *)param_4, _Source, sVar2);
      param_4[sVar2] = '\0';
    }
    if (param_5 != (char *)0) {
      *param_5 = '\0';
    }
  }
  else {
    if (param_4 != (char *)0) {
      sVar2 = (int)local_4 - (int)_Source;
      if (sVar2 >= 0xff) {
        sVar2 = 0xff;
      }
      _mbsnbcpy((unsigned char *)param_4, _Source, sVar2);
      param_4[sVar2] = '\0';
    }
    if (param_5 != (char *)0) {
      sVar2 = (int)pbVar3 - (int)local_4;
      if (sVar2 >= 0xff) {
        sVar2 = 0xff;
      }
      _mbsnbcpy((unsigned char *)param_5, local_4, sVar2);
      param_5[sVar2] = '\0';
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

/* Function start: 0x41A810 */
void CreateGameObject_1() {
  GameConfig* ptr = (GameConfig*)AllocateMemory(sizeof(GameConfig)); // 0x94
  
  __try {
      if (ptr != 0) {
        ptr->Constructor();
      }
  } __except (EXCEPTION_EXECUTE_HANDLER) {
  }
  g_GameConfig_00436970 = ptr;

  if (DAT_0043d558 != 0) {
      g_GameConfig_00436970->data[2] = (unsigned char)DAT_0043d558;
  }

  if (DAT_0043d560 != 0) {
      g_GameConfig_00436970->data[0] = (unsigned char)DAT_0043d560;
  }

  g_GameConfig_00436970->LoadConfig();
}

/* Function start: 0x422E02 */
int __cdecl CalculateBufferSize(int width, unsigned int height) {
  return (((width + 3) & ~3U) * height) + DAT_00437f4c;
}

/* Function start: 0x41A3D0 */
void InitGameSystems(void) {
  void *pAlloc;
  void *pInit;

  __try {
    g_Buffer_00436960 = (char *)AllocateMemory(0x100);
    g_Buffer_00436964 = AllocateMemory(CalculateBufferSize(0x280, 0x1e0));
    CheckDebug();
    ClearMessageLog();
    CreateGameObject_1();
    InitWorkBuffer(0x280, 0x1e0);
    pAlloc = AllocateMemory(0x1b8);
    pInit = 0;
    if (pAlloc != 0) {
      pInit = ((InputManager *)pAlloc)->Init(
          (unsigned int)g_GameConfig_00436970->data[0]);
    }
    g_InputManager_00436968 = (InputManager *)pInit;
    pAlloc = AllocateMemory(0x3c);
    pInit = 0;
    if (pAlloc != 0) {
      pInit = ((Sound *)pAlloc)->Init(0x5622, 8, 1);
    }
    g_Sound_0043696c = (Sound *)pInit;
    pAlloc = AllocateMemory(0x38);
    pInit = 0;
    if (pAlloc != 0) {
      pInit = ((AnimatedAsset *)pAlloc)->Init();
    }
    (g_TextManager_00436990 = (AnimatedAsset *)pInit)->LoadAnimatedAsset("elements\\barrel06.smk");
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
  if (g_InputManager_00436968 != 0) {
    FUN_00421840();
    FreeMemory(g_InputManager_00436968);
    g_InputManager_00436968 = 0;
  }

  if (g_CDData_0043697c != 0) {
    FUN_00421ea0(g_CDData_0043697c);
    FreeMemory(g_CDData_0043697c);
    g_CDData_0043697c = 0;
  }
  if (g_GameConfig_00436970 != 0) {
     FUN_004227a0(g_GameConfig_00436970);
     FreeMemory(g_GameConfig_00436970);
     g_GameConfig_00436970 = 0;
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

// Stubs for new class external implementations
void* __fastcall FUN_0041b760_impl(void* thisptr) { return thisptr; }
void __fastcall FUN_0041b8e0_impl(void* thisptr) {}
void* __fastcall FUN_00417200_impl(void* thisptr) { return thisptr; }
void __fastcall FUN_00417690_impl(void* thisptr) {}
void __fastcall FUN_00404230_impl(void* thisptr) {}

}
