#include "AnimatedAsset.h"

#include "Animation.h"
#include "GameState.h"
#include "GameWindow.h"
#include "InputManager.h"
#include "Memory.h"
#include "Sound.h"
#include "VBuffer.h"
#include "CDData.h"
#include "MouseControl.h"
#include "Timer.h"
#include "Parser.h"
#include "FlagArray.h"
#include "StringTable.h"
#include "ZBufferManager.h"
#include "GameLoop.h"
#include "AssetList.h"
#include "Character.h"
#include "Message.h"
#include "TimedEvent.h"
#include "string.h"
#include <mbctype.h>
#include <mbstring.h>
#include <mss.h>
#include <windows.h>
#include "GameConfig.h"







#include "globals.h"

// Globals moved from globals.cpp to match layout
StringTable* g_Strings_00435a70 = 0;
Character* g_PeterCharacter_00435a74 = 0;
Character* g_SusanCharacter_00435a78 = 0;
Character* g_DuncanCharacter_00435a7c = 0;
Character* g_SelectedCharacter_00435a80 = 0;
FlagArray* g_FlagManager_00435a84 = 0;

static char s_TeacherDemo[] = "Teacher v(0.950)";

GameWindow g_GameWindow;


extern "C" {
void __fastcall CDData_ChangeToBaseDir(void *);
void ClearMessageLog();
void InitWorkBuffer(int, int);
void SetStateFlag(int, int);
int SetCursorVisible(unsigned int);
int FileExists(const char *);
void ParsePath(const char *, char *, char *, char *, char *);
int ProcessMessages();
}

// Forward declarations for functions defined in this file
void InitGameSystems();
void ShutdownGameSystems();
void CheckDebug();
void CreateGameObject_1();
int GetFileAttributes_Wrapper(const char *param_1, char param_2);
int IsAppDeactivated();

// Full-game-only functions (not in demo)
const char* __cdecl CDData_ResolvePath(const char *format, ...);
int __cdecl GetFreeDiskSpaceMB(int drive);
void __cdecl InitMemoryCache(int param_1, int param_2, float param_3);
void CleanupMemoryCache();
void __cdecl ParseQuestionInit(void *dest, const char *filename);
void RunGame_Cleanup2();

/* Function start: 0x4236F0 */
void RunGame() {
    InitGameSystems();
    InitWorkBuffer(0x280, 0x1e0);

    // Play splash cinematic if exists
    const char *splashPath = CDData_ResolvePath("cine\\splash.smk");
    if (FileExists(splashPath)) {
        Animation anim;
        anim.Play((char *)splashPath, 3);
    }

    // Create 4 GameStates
    g_GameState_00436998 = new GameState();
    ParseFile(g_GameState_00436998, "mis\\gamestat.mis", "[GAMESTATE%4.4d]", 1);

    g_GameState2_004369a4 = new GameState();
    ParseFile(g_GameState2_004369a4, "mis\\gamestat.mis", "[GAMESTATE%4.4d]", 2);

    g_GameState3_0043699c = new GameState();
    ParseFile(g_GameState3_0043699c, "mis\\gamestat.mis", "[GAMESTATE%4.4d]", 3);

    g_GameState4_004369a0 = new GameState();
    ParseFile(g_GameState4_004369a0, "mis\\gamestat.mis", "[GAMESTATE%4.4d]", 4);

    // Check CACHE_SIZE from gamestate
    int cacheIdx = g_GameState_00436998->FindState("CACHE_SIZE");
    if (cacheIdx < 0 || cacheIdx >= g_GameState_00436998->maxStates - 1) {
        ShowError("Invalid gamestate %d", cacheIdx);
    }
    int cacheSize = g_GameState_00436998->stateValues[cacheIdx];
    if (cacheSize == 0) {
        cacheSize = 0xf;
    }

    int diskSpace = GetFreeDiskSpaceMB(0);
    if (diskSpace != -1 && ((diskSpace + (diskSpace >> 0x1f & 0xfffffU)) >> 0x14) < cacheSize) {
        ShowError("This game requires %lu MBytes of free disk space.\nPlease free up some disk space and try again.", diskSpace, cacheSize);
    }
    InitMemoryCache(200, cacheSize, 50.0f);

    // Delete old mouse if exists, create new
    if (g_Mouse_00436978 != 0) {
        delete g_Mouse_00436978;
        g_Mouse_00436978 = 0;
    }

    g_Mouse_00436978 = new MouseControl();
    ParseFile(g_Mouse_00436978, "mis\\mouse1.mis", "[MICE]");

    g_StateString_00436994 = new char[0x40];
    *g_StateString_00436994 = 0;

    g_Timer_00436980 = new Timer();

    g_FlagManager_00435a84 = new FlagArray("cfg\\question.dat", 10000);
    g_FlagManager_00435a84->ClearAllFlags();

    // Parse question init script
    char initQBuffer[140];
    ParseQuestionInit(initQBuffer, "mis\\INIT_Q.mis");

    RunGame_Cleanup2();

    g_Strings_00435a70 = new StringTable("mis\\strings.mis", 1);

    // Check TEST_STRINGS in gamestate
    int testIdx = g_GameState_00436998->FindState("TEST_STRINGS");
    if (testIdx < 0 || testIdx >= g_GameState_00436998->maxStates - 1) {
        ShowError("Invalid gamestate %d", testIdx);
    }
    if (g_GameState_00436998->stateValues[testIdx] != 0) {
        int testIdx2 = g_GameState_00436998->FindState("TEST_STRINGS");
        if (testIdx2 < 0 || testIdx2 >= g_GameState_00436998->maxStates - 1) {
            ShowError("Invalid gamestate %d", testIdx2);
        }
    }

    g_TimedEventPool1_00436984 = new TimedEventPool();

    ZBufferManager* pZBuffer = new ZBufferManager();
    g_ZBufferManager_0043698c = pZBuffer;

    g_Mouse_00436978->DrawCursor();
    g_TextManager_00436990->LoadAnimatedAsset("elements\\text1.smk");
    g_TextManager_00436990->char_adv.advance = 2;
    g_Timer_00436980->Reset();

    SC_Message_Send(1, 0x2c, 0, 0, 0x17, 0, 0, 0, 0, 0);
    SC_Message_Send(1, 0x1e, 0, 0, 0x17, 0, 0, 0, 0, 0);

    // GameLoop (stack-allocated in full game)
    GameLoop gameLoop;
    ParseFile(&gameLoop, "mis\\start.mis", "[BLOCK%4.4d]");

    // Cleanup
    if (g_ZBufferManager_0043698c != 0) {
        g_ZBufferManager_0043698c->Cleanup();
        operator delete(g_ZBufferManager_0043698c);
        g_ZBufferManager_0043698c = 0;
    }

    if (g_TimedEventPool1_00436984 != 0) {
        delete g_TimedEventPool1_00436984;
        g_TimedEventPool1_00436984 = 0;
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

    if (g_StateString_00436994 != 0) {
        delete g_StateString_00436994;
        g_StateString_00436994 = 0;
    }

    if (g_Mouse_00436978 != 0) {
        delete g_Mouse_00436978;
        g_Mouse_00436978 = 0;
    }

    if (g_FlagManager_00435a84 != 0) {
        g_FlagManager_00435a84->SafeClose();
        operator delete(g_FlagManager_00435a84);
        g_FlagManager_00435a84 = 0;
    }

    CleanupMemoryCache();
    ShutdownGameSystems();
}


/* Function start: 0x424C40 */
void PlayIntroCinematic(void) {
  InitGameSystems();
  ShutdownGameSystems();
}

/* Function start: 0x411325 */
void CleanupCinematic(void) {}


/* Function start: 0x425E90 */ /* ~86% match */
extern "C" int ProcessMessages() {
  MSG local_1c;
  int iVar1;

  do {
    iVar1 = PeekMessageA(&local_1c, NULL, 0, 0, PM_REMOVE);
    while (iVar1 != 0) {
      TranslateMessage(&local_1c);
      if (local_1c.message == WM_KEYDOWN) {
        int wParam = local_1c.wParam;
        if (local_1c.wParam > 0x7f) {
          wParam = wParam & 0x2f;
        }
        g_WaitForInputValue_004373bc = wParam;
        if (local_1c.wParam == VK_F12) {
          return 1;
        }
      }
      DispatchMessageA(&local_1c);
      iVar1 = PeekMessageA(&local_1c, NULL, 0, 0, PM_REMOVE);
    }
    iVar1 = IsAppDeactivated();
    if (iVar1 != 0) {
      return 0;
    }
  } while (1);
}

/* Function start: 0x425FA0 */
extern "C" int FileExists(const char *filename) {
  int iVar1;
  iVar1 = GetFileAttributes_Wrapper(filename, 0);
  return iVar1 == 0;
}

/* Function start: 0x425720 */
void InitGameSystems(void) {
    g_Buffer_00436960 = new char[0x100];
    g_Buffer_00436964 = new char[CalculateBufferSize(0x280, 0x1e0)];
    CheckDebug();
    ClearMessageLog();
    CreateGameObject_1();
    InitWorkBuffer(0x280, 0x1e0);
    g_InputManager_00436968 = new InputManager((unsigned int)g_GameConfig_00436970->data.rawData[0]);
    g_Sound_0043696c = new Sound(0x5622, 8, 1);
    g_TextManager_00436990 = new AnimatedAsset();
    g_TextManager_00436990->LoadAnimatedAsset("elements\\barrel06.smk");
    SetStateFlag(0, 1);
    SetCursorVisible(0);
}

/* Function start: 0x4258C0 */ /* ~97% match */
void ShutdownGameSystems(void) {
  if (g_TextManager_00436990 != 0) {
    delete g_TextManager_00436990;
    g_TextManager_00436990 = 0;
  }
  if (g_WorkBuffer_00436974 != 0) {
    delete g_WorkBuffer_00436974;
    g_WorkBuffer_00436974 = 0;
  }
  if (g_Sound_0043696c != 0) {
    AIL_shutdown();
    delete g_Sound_0043696c;
    g_Sound_0043696c = 0;
  }
  if (g_InputManager_00436968 != 0) {
    delete g_InputManager_00436968;
    g_InputManager_00436968 = 0;
  }

  if (g_CDData_0043697c != 0) {
    CDData_ChangeToBaseDir(g_CDData_0043697c);
    delete g_CDData_0043697c;
    g_CDData_0043697c = 0;
  }
  if (g_GameConfig_00436970 != 0) {
     delete g_GameConfig_00436970;
     g_GameConfig_00436970 = 0;
  }
  if (g_Buffer_00436964 != 0) {
    delete[] g_Buffer_00436964;
    g_Buffer_00436964 = 0;
  }
  if (g_Buffer_00436960 != 0) {
    delete g_Buffer_00436960;
    g_Buffer_00436960 = 0;
  }
}

/* Function start: 0x424BB0 */
void CheckDebug(void) {
  g_CDData_0043697c = new CDData(g_GameWindow.baseDir, "teacher_id", "Missing the Teacher CD ROM");
}

/* Function start: 0x4259E0 */
void CreateGameObject_1() {
  g_GameConfig_00436970 = new GameConfig();

  if (g_CmdLineAudioMode_0043d558 != 0) {
      g_GameConfig_00436970->data.rawData[2] = (unsigned char)g_CmdLineAudioMode_0043d558;
  }

  if (g_CmdLineInputMode_0043d560 != 0) {
      g_GameConfig_00436970->data.rawData[0] = (unsigned char)g_CmdLineInputMode_0043d560;
  }

  g_GameConfig_00436970->LoadConfig();
}

/* Function start: 0x426AC0 */
int WaitForInput() {
  while (g_WaitForInputValue_004373bc == 0) {
    ProcessMessages();
  }
  int iVar1 = g_WaitForInputValue_004373bc;
  g_WaitForInputValue_004373bc = 0;
  return iVar1;
}

/* Function start: 0x453CC8 */
int IsAppDeactivated() {
  return DAT_0043de94;
}

/* Function start: 0x420620 */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,
                   int nCmdShow) {
  int uStack_14;
  g_GameWindow.CreateGameWindow(hInstance, (int)hPrevInstance, lpCmdLine, nCmdShow);
  if (g_GameWindow.InitGraphics() == 0) {
    return 0;
  }
  UpdateWindow(g_GameWindow.hWnd);
  if (g_DebugFlag_0043d55c == 0) {
    RunGame();
  } else {
    PlayIntroCinematic();
  }
  g_GameWindow.Shutdown();
  return uStack_14;
}

/* Function start: 0x453BFF */
int __cdecl CalculateBufferSize(int width, unsigned int height) {
  return (((width + 3) & ~3U) * height) + g_BitmapHeaderSize_00437f4c;
}

/* Function start: 0x4260F0 */
const char* __cdecl CDData_ResolvePath(const char *format, ...) {
    char local_104[260];
    char *args = (char *)(&format + 1);

    vsprintf(local_104, format, args);
    if (FileExists(local_104) == 0) {
        sprintf(g_CDData_0043697c->pathBuffer + 0x45, "%s\\%s",
                g_CDData_0043697c->pathBuffer + 0x40, local_104);
    } else {
        strcpy(g_CDData_0043697c->pathBuffer + 0x45, local_104);
    }
    return g_CDData_0043697c->pathBuffer + 0x45;
}

/* Function start: 0x426690 */
int __cdecl GetFreeDiskSpaceMB(int param_1) {
    DWORD local_10;
    DWORD local_c;
    DWORD local_18;
    DWORD local_14;
    LPCSTR lpRootPathName;
    char local_8[8];
    int result;

    result = -1;
    if (param_1 == 0) {
        lpRootPathName = 0;
    } else {
        sprintf(local_8, "%c:\\", param_1 + 0x40);
        lpRootPathName = local_8;
    }
    if (GetDiskFreeSpaceA(lpRootPathName, &local_10, &local_14, &local_18, &local_c) != 0) {
        result = local_18 * local_14 * local_10;
    }
    return result;
}

/* Function start: 0x4340A0 */
void __cdecl InitMemoryCache(int param_1, int param_2, float param_3) {
}

/* Function start: 0x434170 */
void CleanupMemoryCache() {
}

// ParseQuestionInit and RunGame_Cleanup2 are compiler-generated thunks
// embedded within RunGame's code in the original binary
void __cdecl ParseQuestionInit(void *dest, const char *filename) {
}

void RunGame_Cleanup2() {
}

// ParsePath is a CRT library function in the full game (0x4560F0, in library range)
extern "C" void ParsePath(const char *fullPath, char *drive, char *dir, char *fname,
               char *ext) {
  unsigned char *lastSlash;
  unsigned char *dotPos;
  unsigned char *p;
  unsigned char ch;
  size_t len;

  lastSlash = 0;
  dotPos = 0;
  if (((const unsigned char *)fullPath)[1] == 0x3a) {
    if (drive != 0) {
      _mbsnbcpy((unsigned char *)drive, (const unsigned char *)fullPath, 2);
      drive[2] = '\0';
    }
    fullPath = fullPath + 2;
  }
  else if (drive != 0) {
    *drive = '\0';
  }
  lastSlash = 0;
  p = (unsigned char *)fullPath;
  if (*p != 0) {
    do {
      ch = *p;
      if ((_mbctype[ch + 1] & 4) == 0) {
        if (ch == 0x2f || ch == 0x5c) {
          lastSlash = p + 1;
        }
        else if (ch == 0x2e) {
          dotPos = p;
        }
      }
      else {
        p = p + 1;
      }
      p = p + 1;
    } while (*p != 0);
  }
  if (lastSlash != 0) {
    if (dir != 0) {
      len = (size_t)lastSlash - (size_t)fullPath;
      if (len >= 0xff) {
        len = 0xff;
      }
      _mbsnbcpy((unsigned char *)dir, (const unsigned char *)fullPath, len);
      dir[len] = '\0';
    }
    fullPath = (const char *)lastSlash;
  }
  else {
    if (dir != 0) {
      *dir = '\0';
    }
  }
  if (dotPos == 0 || (unsigned int)dotPos < (unsigned int)fullPath) {
    if (fname != 0) {
      len = (size_t)p - (size_t)fullPath;
      if (len >= 0xff) {
        len = 0xff;
      }
      _mbsnbcpy((unsigned char *)fname, (const unsigned char *)fullPath, len);
      fname[len] = '\0';
    }
    if (ext != 0) {
      *ext = '\0';
    }
  }
  else {
    if (fname != 0) {
      len = (size_t)dotPos - (size_t)fullPath;
      if (len >= 0xff) {
        len = 0xff;
      }
      _mbsnbcpy((unsigned char *)fname, (const unsigned char *)fullPath, len);
      fname[len] = '\0';
    }
    if (ext != 0) {
      len = (size_t)p - (size_t)dotPos;
      if (len >= 0xff) {
        len = 0xff;
      }
      _mbsnbcpy((unsigned char *)ext, dotPos, len);
      ext[len] = '\0';
    }
  }
}

// SetErrorCode is a CRT library function in the full game (0x45A860, in library range)
void SetErrorCode(unsigned int errorCode) {
  int iVar1;
  unsigned int *puVar2;

  g_ErrorCode_0043bdf4 = errorCode;
  iVar1 = 0;
  puVar2 = DAT_0043c760;
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

// GetFileAttributes_Wrapper is a CRT library function in the full game (0x456440, in library range)
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
    g_ErrorCode_0043bdf4 = 5;
    return -1;
  }
  return 0;
}

