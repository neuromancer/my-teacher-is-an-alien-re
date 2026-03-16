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
int* GetScreenWidth();
int* GetScreenHeight();
void ParsePath(const char *, char *, char *, char *, char *);
int ProcessMessages();

// Bridge globals (extern "C" linkage)
extern void* DAT_0046aa10;   // = g_GameConfig_00436970
extern void* DAT_0046aa14;   // = g_WorkBuffer_00436974
}

// Bridge globals (C++ linkage, defined in stubs.cpp)
extern char* DAT_0046aa00;   // = g_Buffer_00436960
extern InputManager* DAT_0046aa08;   // = g_InputManager_00436968
extern ZBufferManager* DAT_0046aa24;   // ZBufferManager* (rendering manager, 0xAC bytes)

// Forward declarations for functions defined in this file
void InitGameSystems();
void ShutdownGameSystems();
void CheckDebug();
void CreateGameObject_1();
void InitGameConfig();
int GetFileAttributes_Wrapper(const char *param_1, char param_2);
int IsAppDeactivated();

// Full-game-only functions (not in demo)
const char* __cdecl CDData_ResolvePath(const char *format, ...);
int __cdecl GetFreeDiskSpaceMB(int drive);
void __cdecl InitMemoryCache(int param_1, int param_2, float param_3);
void CleanupMemoryCache();

#include "MemoryCache.h"
#include "QuestionInit.h"
#include "SoundTracker.h"
#include "GameLoopHelper.h"
#include "MsgList.h"

extern void __cdecl FUN_004344b0();
extern void __cdecl FUN_00434030(void*, int);
extern "C" void SendGameMessage(int, int, int, int, int, int, int, int, int, int);

#include "GameEngine.h"
extern "C" int DAT_0046a6ec;
extern "C" extern GameState* DAT_0046aa30;
extern GameState* DAT_0046aa3c;
extern GameState* g_StringTable_0046aa34;
extern void* DAT_0046aa38;

static float g_PercentScale = 0.01f;
int DAT_00473440 = 0;
int DAT_00473444 = 0;
int DAT_0046b780 = 0;
int DAT_0046b790 = 0;
MemoryCache* DAT_0046b78c = 0;
Timer DAT_00473448;

SoundTracker* g_SoundTracker = 0;       // DAT_0046928c
GameLoopHelper* g_GameLoopHelper = 0;   // DAT_0046a6f0
MsgList* g_MsgList = 0;                 // DAT_0046a6dc
int g_StartBlock = 0;                    // DAT_00472e2c

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

    // Pre-GameState manager allocation
    g_SoundTracker = new SoundTracker(0xfa0);

    // Create 4 GameStates with inline ParseFile
    g_GameState_00436998 = new GameState("mis\\gamestat.mis", "[GAMESTATE%4.4d]", 1);
    DAT_0046aa30 = g_GameState_00436998;

    g_GameState2_004369a4 = new GameState("mis\\gamestat.mis", "[GAMESTATE%4.4d]", 2);
    DAT_0046aa3c = g_GameState2_004369a4;

    g_GameState3_0043699c = new GameState("mis\\gamestat.mis", "[GAMESTATE%4.4d]", 3);
    g_StringTable_0046aa34 = g_GameState3_0043699c;

    g_GameState4_004369a0 = new GameState("mis\\gamestat.mis", "[GAMESTATE%4.4d]", 4);
    DAT_0046aa38 = g_GameState4_004369a0;

    // Check CACHE_SIZE from gamestate
    int cacheIdx = g_GameState_00436998->FindState("CACHE_SIZE");
    if (cacheIdx < 0 || cacheIdx > g_GameState_00436998->maxStates - 1) {
        ShowError("0. Invalid gamestate %d", cacheIdx);
    }
    int cacheSize = g_GameState_00436998->stateValues[cacheIdx];
    if (cacheSize == 0) {
        cacheSize = 0xf;
    }

    int diskSpace = GetFreeDiskSpaceMB(0);
    if (diskSpace != -1 && (diskSpace / 1048576) < cacheSize) {
        ShowError("This game requires %lu MBytes of free disk space.\nPlease free up some disk space and try again.", diskSpace, cacheSize);
    }
    InitMemoryCache(200, cacheSize, 50.0f);

    // Delete old mouse if exists, create new
    if (g_Mouse_00436978 != 0) {
        MouseControl* p = g_Mouse_00436978;
        p->~MouseControl();
        operator delete(p);
        g_Mouse_00436978 = 0;
    }

    g_Mouse_00436978 = new MouseControl();
    ParseFile(g_Mouse_00436978, "mis\\mouse1.mis", "[MICE]");

    // Linked list allocation
    g_MsgList = new MsgList();

    g_StateString_00436994 = (char *)operator new(0x40);
    *g_StateString_00436994 = 0;

    g_Timer_00436980 = new Timer();

    g_FlagManager_00435a84 = new FlagArray("cfg\\question.dat", 10000);
    g_FlagManager_00435a84->ClearAllFlags();

    // Parse question init script (stack-allocated, immediately destroyed)
    {
        QuestionInit initQ("mis\\INIT_Q.mis");
    }

    g_Strings_00435a70 = new StringTable("mis\\strings.mis", 1);

    // Check TEST_STRINGS in gamestate
    int testIdx = g_GameState_00436998->FindState("TEST_STRINGS");
    if (testIdx < 0 || testIdx > g_GameState_00436998->maxStates - 1) {
        ShowError("1. Invalid gamestate %d", testIdx);
    }
    if (g_GameState_00436998->stateValues[testIdx] != 0) {
        int testIdx2 = g_GameState_00436998->FindState("TEST_STRINGS");
        if (testIdx2 < 0 || testIdx2 > g_GameState_00436998->maxStates - 1) {
            ShowError("2. Invalid gamestate %d", testIdx2);
        }
        g_Strings_00435a70->TestStrings(g_TextManager_00436990, g_GameState_00436998->stateValues[testIdx2]);
    }

    g_TimedEventPool1_00436984 = new TimedEventPool();

    // Original creates TWO objects (assembly lines 421-452 of FUN_4236F0):
    // 1. DAT_0046aa24 = new ZBufferManager (0xAC bytes, constructor 0x403910) — rendering
    // 2. [0x0046a6ec] = new GameEngine (0x28 bytes, constructor 0x430A00) — game loop
    DAT_0046aa24 = new ZBufferManager();
    g_ZBufferManager_0043698c = DAT_0046aa24;
    GameEngine* gameEngine = new GameEngine();
    DAT_0046a6ec = (int)gameEngine;

    g_Mouse_00436978->DrawCursor();
    g_TextManager_00436990->LoadAnimatedAsset("elements\\text1.smk");
    g_TextManager_00436990->char_adv.advance = 2;
    g_TextManager_00436990->spaceWidth = 5;
    g_TextManager_00436990->tabWidth = 0x14;
    g_Timer_00436980->Reset();

    SendGameMessage(1, 0x2c, 0, 0, 0x17, 0, 0, 0, 0, 0);
    SendGameMessage(1, 0x1e, 0, 0, 0x17, 0, 0, 0, 0, 0);

    // GameLoop (stack-allocated)
    GameLoop gameLoop;
    ParseFile(&gameLoop, "mis\\start.mis", "[BLOCK%4.4d]", g_StartBlock);

    // Post-GameLoop
    if (g_GameLoopHelper != 0) {
        g_GameLoopHelper->PostProcess();
    }

    // Game loop — GameEngine::RunGameLoop (0x430CD0)
    gameEngine->RunGameLoop();

    // Cleanup: GameEngine destructor + delete (assembly lines 509-514)
    if (gameEngine != 0) {
        gameEngine->~GameEngine();
        operator delete(gameEngine);
    }

    // Cleanup: ZBufferManager Cleanup + delete (assembly lines 515-528)
    if (DAT_0046aa24 != 0) {
        (DAT_0046aa24)->Cleanup();
        operator delete(DAT_0046aa24);
        DAT_0046aa24 = 0;
        g_ZBufferManager_0043698c = 0;
    }

    if (g_TimedEventPool1_00436984 != 0) {
        TimedEventPool* p = g_TimedEventPool1_00436984;
        p->~TimedEventPool();
        operator delete(p);
        g_TimedEventPool1_00436984 = 0;
    }

    if (g_Strings_00435a70 != 0) {
        StringTable* p = g_Strings_00435a70;
        p->~StringTable();
        operator delete(p);
        g_Strings_00435a70 = 0;
    }

    if (g_GameState4_004369a0 != 0) {
        GameState* p = g_GameState4_004369a0;
        p->~GameState();
        operator delete(p);
        g_GameState4_004369a0 = 0;
    }

    if (g_GameState3_0043699c != 0) {
        GameState* p = g_GameState3_0043699c;
        p->~GameState();
        operator delete(p);
        g_GameState3_0043699c = 0;
    }

    if (g_GameState2_004369a4 != 0) {
        GameState* p = g_GameState2_004369a4;
        p->~GameState();
        operator delete(p);
        g_GameState2_004369a4 = 0;
    }

    if (g_GameState_00436998 != 0) {
        GameState* p = g_GameState_00436998;
        p->~GameState();
        operator delete(p);
        g_GameState_00436998 = 0;
    }

    if (g_Timer_00436980 != 0) {
        Timer* p = g_Timer_00436980;
        p->~Timer();
        operator delete(p);
        g_Timer_00436980 = 0;
    }

    if (g_StateString_00436994 != 0) {
        operator delete(g_StateString_00436994);
        g_StateString_00436994 = 0;
    }

    if (g_Mouse_00436978 != 0) {
        MouseControl* p = g_Mouse_00436978;
        p->~MouseControl();
        operator delete(p);
        g_Mouse_00436978 = 0;
    }

    if (g_FlagManager_00435a84 != 0) {
        FlagArray* p = g_FlagManager_00435a84;
        p->~FlagArray();
        operator delete(p);
        g_FlagManager_00435a84 = 0;
    }

    if (g_GameLoopHelper != 0) {
        GameLoopHelper* p = g_GameLoopHelper;
        p->~GameLoopHelper();
        operator delete(p);
        g_GameLoopHelper = 0;
    }

    if (g_SoundTracker != 0) {
        SoundTracker* p = g_SoundTracker;
        p->~SoundTracker();
        operator delete(p);
        g_SoundTracker = 0;
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


/* Function start: 0x425E90 */
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
    DAT_0046aa00 = g_Buffer_00436960;
    g_Buffer_00436964 = new char[CalculateBufferSize(0x280, 0x1e0)];
    CheckDebug();
    ClearMessageLog();
    CreateGameObject_1();
    InitGameConfig();
    InitWorkBuffer(0x280, 0x1e0);
    g_InputManager_00436968 = new InputManager((unsigned int)g_GameConfig_00436970->data.rawData[0]);
    DAT_0046aa08 = g_InputManager_00436968;
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
    DAT_0046aa14 = 0;
  }
  if (g_Sound_0043696c != 0) {
    Sound* p = g_Sound_0043696c;
    p->~Sound();
    operator delete(p);
    g_Sound_0043696c = 0;
  }
  if (g_InputManager_00436968 != 0) {
    delete g_InputManager_00436968;
    g_InputManager_00436968 = 0;
    DAT_0046aa08 = 0;
  }

  if (g_CDData_0043697c != 0) {
    CDData* p = g_CDData_0043697c;
    p->~CDData();
    operator delete(p);
    g_CDData_0043697c = 0;
  }
  if (g_GameConfig_00436970 != 0) {
     delete g_GameConfig_00436970;
     g_GameConfig_00436970 = 0;
     DAT_0046aa10 = 0;
  }
  if (g_Buffer_00436964 != 0) {
    delete[] g_Buffer_00436964;
    g_Buffer_00436964 = 0;
  }
  if (g_Buffer_00436960 != 0) {
    delete g_Buffer_00436960;
    g_Buffer_00436960 = 0;
    DAT_0046aa00 = 0;
  }
}

/* Function start: 0x424BB0 */
void CheckDebug(void) {
  g_CDData_0043697c = new CDData(g_GameWindow.baseDir, "teacher_id", "Missing the Teacher CD ROM");
}

/* Function start: 0x4259E0 */
extern void* DAT_0046aa1c;

/* Function start: 0x424BB0 */
void CreateGameObject_1() {
  static char s_pathBuffer[260]; // DAT_00472de8
  CDData* cd = new CDData(s_pathBuffer, "teacher_id", "Missing the Teacher CD ROM");
  DAT_0046aa1c = cd;
}

/* Function start: 0x4259E0 */
void InitGameConfig() {
  GameConfig* cfg = new GameConfig();
  DAT_0046aa10 = cfg;
  g_GameConfig_00436970 = cfg;

  if (g_CmdLineAudioMode_0043d558 != 0) {
      ((GameConfig*)DAT_0046aa10)->data.rawData[2] = (unsigned char)g_CmdLineAudioMode_0043d558;
  }

  if (g_CmdLineInputMode_0043d560 != 0) {
      ((GameConfig*)DAT_0046aa10)->data.rawData[0] = (unsigned char)g_CmdLineInputMode_0043d560;
  }

  ((GameConfig*)DAT_0046aa10)->LoadConfig();
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
// Original returns [0x472d14] - a .data initialized global (always 1).
// ProcessMessages loops until this returns non-zero, making it non-blocking.
static int g_AppReady_00472d14 = 1; // DAT_00472d14

int IsAppDeactivated() {
  return g_AppReady_00472d14;
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
    if (FileExists(local_104)) {
        strcpy(g_CDData_0043697c->field_190 + 5, local_104);
    } else {
        sprintf(g_CDData_0043697c->field_190 + 5, "%s\\%s",
                g_CDData_0043697c->field_190, local_104);
    }
    return g_CDData_0043697c->field_190 + 5;
}

/* Function start: 0x4265A0 */
extern "C" void ShowLoadingScreen(void) {
    const char* path;
    VBuffer* buffer;
    int x;
    int* screen;

    path = CDData_ResolvePath("elements\\loading.smk");
    if (FileExists(path) != 0) {
        buffer = new VBuffer((char*)path, 0);
        screen = GetScreenWidth();
        x = (*screen - buffer->width) / 2;
        screen = GetScreenHeight();
        buffer->CallBlitter4(buffer->clip_x1, buffer->clip_x2, buffer->clip_y1,
                             buffer->clip_y2, x,
                             (*screen - buffer->height) / 2 + buffer->height);
        delete buffer;
    }
}

/* Function start: 0x426690 */
int __cdecl GetFreeDiskSpaceMB(int param_1) {
    DWORD local_18;
    DWORD local_14;
    DWORD local_10;
    DWORD local_c;
    char local_8[8];
    int result;

    result = -1;
    if (param_1 != 0) {
        sprintf(local_8, "%c:\\", param_1 + 0x40);
        if (GetDiskFreeSpaceA(local_8, &local_10, &local_14, &local_18, &local_c) != 0) {
            result = local_18 * local_14 * local_10;
        }
    } else {
        if (GetDiskFreeSpaceA(0, &local_10, &local_14, &local_18, &local_c) != 0) {
            result = local_18 * local_14 * local_10;
        }
    }
    return result;
}

/* Function start: 0x4340A0 */
void __cdecl InitMemoryCache(int param_1, int param_2, float param_3) {
    CleanupMemoryCache();
    DAT_00473440 = 0;
    DAT_00473444 = param_2 << 20;
    DAT_0046b780 = param_1;
    DAT_0046b790 = DAT_00473444 - (int)((float)DAT_00473444 * param_3 * g_PercentScale);
    MemoryCache* ptr = new MemoryCache(DAT_0046b780);
    DAT_0046b78c = ptr;
    DAT_00473448.Reset();
}

/* Function start: 0x434170 */
void CleanupMemoryCache() {
    FUN_004344b0();
    if (DAT_0046b78c != 0) {
        MemoryCache* cache = DAT_0046b78c;
        int* node = (int*)cache->field_0;
        while (node != 0) {
            FUN_00434030((void*)&node[2], 1);
            node = (int*)node[0];
        }
        cache->field_8 = 0;
        cache->field_c = 0;
        cache->field_4 = 0;
        cache->field_0 = 0;
        int* freeNode = (int*)cache->field_10;
        while (freeNode != 0) {
            int* next = (int*)freeNode[0];
            free(freeNode);
            freeNode = next;
        }
        cache->field_10 = 0;
        free(cache);
        DAT_0046b78c = 0;
    }
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
