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
#include "Message.h"
#include "TimedEvent.h"
#include "string.h"
#include <mbctype.h>
#include <mbstring.h>
#include <mss.h>
#include <windows.h>
#include "GameConfig.h"

#include "globals.h"

static char s_TeacherDemo[] = "Teacher v(0.950)";

GameWindow g_GameWindow;


extern "C" {
void ClearMessageLog();
void SetVideoRes(int, int);
void SetStateFlag(int, int);
int SetCursorVisible(unsigned int);
int FileExists(const char *);
int* GetScreenWidth();
int* GetScreenHeight();
void ParsePath(const char *, char *, char *, char *, char *);
int ProcessMessages();

// Bridge globals (extern "C" linkage) — now in globals.h
}

// Bridge globals (C++ linkage, defined in stubs.cpp)
extern char* g_Buffer_0046aa00;   // = g_Buffer_0046aa00
extern InputManager* g_InputManager_0046aa08;   // = g_InputManager_0046aa08
extern ZBufferManager* g_ZBufferManager_0046aa24;   // ZBufferManager* (rendering manager, 0xAC bytes)

// Forward declarations for functions defined in this file
void InitGameSystems();
void ShutdownGameSystems();
void CreateGameObject_1();
void InitGameConfig();
int GetFileAttributes_Wrapper(const char *param_1, char param_2);
int IsAppReady();

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

extern void __cdecl FileCacheCleanup();
extern void __cdecl FileCacheEntryCleanup(void*, int);
extern "C" void SendGameMessage(int, int, int, int, int, int, int, int, int, int);

#include "GameEngine.h"
// g_GameEngine_0046a6ec, g_GameState_0046aa30 etc. — declared in globals.h
extern GameState* g_StringState_0046aa38;

static float g_PercentScale = 0.01f;
// Cache, SoundTracker, GameLoopHelper, MsgList, g_StartBlock_00472e2c — defined in globals.cpp
Timer DAT_00473448;

/* Function start: 0x4236F0 */
void RunGame() {
    InitGameSystems();
    SetVideoRes(0x280, 0x1e0);

    // Play splash cinematic if exists
    const char *splashPath = CDData_ResolvePath("cine\\\\splash.smk");
    if (FileExists(splashPath)) {
        Animation anim;
        anim.Play((char *)splashPath, 3);
    }

    // Pre-GameState manager allocation
    g_SoundTracker_0046928c = new SoundTracker(0xfa0);

    // Create 4 GameStates with inline ParseFile
    g_GameState_0046aa30 = new GameState("mis\\\\gamestat.mis", "[GAMESTATE%4.4d]", 1);

    g_GameState2_0046aa3c = new GameState("mis\\\\gamestat.mis", "[GAMESTATE%4.4d]", 2);

    g_StringTable_0046aa34 = new GameState("mis\\\\gamestat.mis", "[GAMESTATE%4.4d]", 3);
    g_StringTable_0046aa34 = g_StringTable_0046aa34;

    g_StringState_0046aa38 = new GameState("mis\\\\gamestat.mis", "[GAMESTATE%4.4d]", 4);
    g_StringState_0046aa38 = g_StringState_0046aa38;

    // Check CACHE_SIZE from gamestate
    int cacheIdx = g_GameState_0046aa30->FindState("CACHE_SIZE");
    if (cacheIdx < 0 || cacheIdx > g_GameState_0046aa30->maxStates - 1) {
        ShowError("Invalid gamestate %d", cacheIdx);
    }
    int cacheSize = g_GameState_0046aa30->stateValues[cacheIdx];
    if (cacheSize == 0) {
        cacheSize = 0xf;
    }

    int diskSpace = GetFreeDiskSpaceMB(0);
    if (diskSpace != -1 && (diskSpace / 1048576) < cacheSize) {
        ShowError("This game requires %lu MBytes of free disk space.\nYou have approximately %lu MBytes free.\nPlease free some space and rerun.", diskSpace, cacheSize);
    }
    InitMemoryCache(200, cacheSize, 50.0f);

    // Delete old mouse if exists, create new
    if (g_Mouse_0046aa18 != 0) {
        MouseControl* p = g_Mouse_0046aa18;
        p->~MouseControl();
        operator delete(p);
        g_Mouse_0046aa18 = 0;
    }

    g_Mouse_0046aa18 = new MouseControl();
    ParseFile(g_Mouse_0046aa18, "mis\\\\mouse1.mis", "[MICE]");

    // Linked list allocation
    g_MsgList_0046a6dc = new MsgList();

    g_StateString_0046aa2c = (char *)operator new(0x40);
    *g_StateString_0046aa2c = 0;

    g_Timer_0046aa20 = new Timer();

    g_FlagManager_0046a6e8 = new FlagArray("cfg\\\\question.dat", 10000);
    g_FlagManager_0046a6e8->ClearAllFlags();

    // Parse question init script (stack-allocated, immediately destroyed)
    {
        QuestionInit initQ("mis\\\\INIT_Q.mis");
    }

    g_Strings_0046a6e0 = new StringTable("mis\\\\strings.mis", 1);

    // Check TEST_STRINGS in gamestate
    int testIdx = g_GameState_0046aa30->FindState("TEST_STRINGS");
    if (testIdx < 0 || testIdx > g_GameState_0046aa30->maxStates - 1) {
        ShowError("Invalid gamestate %d", testIdx);
    }
    if (g_GameState_0046aa30->stateValues[testIdx] != 0) {
        int testIdx2 = g_GameState_0046aa30->FindState("TEST_STRINGS");
        if (testIdx2 < 0 || testIdx2 > g_GameState_0046aa30->maxStates - 1) {
            ShowError("Invalid gamestate %d", testIdx2);
        }
        g_Strings_0046a6e0->TestStrings(g_GlyphFont_0046aa28, g_GameState_0046aa30->stateValues[testIdx2]);
    }

    g_TimedEventPool1_00436984 = new TimedEventPool();

    // Original creates TWO objects (assembly lines 421-452 of FUN_4236F0):
    // 1. g_ZBufferManager_0046aa24 = new ZBufferManager (0xAC bytes, constructor 0x403910) — rendering
    // 2. [0x0046a6ec] = new GameEngine (0x28 bytes, constructor 0x430A00) — game loop
    g_ZBufferManager_0046aa24 = new ZBufferManager();
    GameEngine* gameEngine = new GameEngine();
    g_GameEngine_0046a6ec = gameEngine;

    g_Mouse_0046aa18->DrawCursor();
    g_GlyphFont_0046aa28->LoadFont("elements\\\\text1.smk");
    g_GlyphFont_0046aa28->char_adv.advance = 2;
    g_GlyphFont_0046aa28->spaceWidth = 5;
    g_GlyphFont_0046aa28->tabWidth = 0x14;
    g_Timer_0046aa20->Reset();

    SendGameMessage(1, 0x2c, 0, 0, 0x17, 0, 0, 0, 0, 0);
    SendGameMessage(1, 0x1e, 0, 0, 0x17, 0, 0, 0, 0, 0);

    // GameLoop (stack-allocated)
    GameLoop gameLoop;
    ParseFile(&gameLoop, "mis\\\\start.mis", "[BLOCK%4.4d]", g_StartBlock_00472e2c);

    // Post-GameLoop
    if (g_GameLoopHelper_0046a6f0 != 0) {
        g_GameLoopHelper_0046a6f0->PostProcess();
    }

    // Game loop — GameEngine::RunGameLoop (0x430CD0)
    gameEngine->RunGameLoop();

    // Cleanup: GameEngine destructor + delete (assembly lines 509-514)
    if (gameEngine != 0) {
        gameEngine->~GameEngine();
        operator delete(gameEngine);
    }

    // Cleanup: ZBufferManager Cleanup + delete (assembly lines 515-528)
    if (g_ZBufferManager_0046aa24 != 0) {
        (g_ZBufferManager_0046aa24)->Cleanup();
        operator delete(g_ZBufferManager_0046aa24);
        g_ZBufferManager_0046aa24 = 0;
    }

    if (g_TimedEventPool1_00436984 != 0) {
        TimedEventPool* p = g_TimedEventPool1_00436984;
        p->~TimedEventPool();
        operator delete(p);
        g_TimedEventPool1_00436984 = 0;
    }

    if (g_Strings_0046a6e0 != 0) {
        StringTable* p = g_Strings_0046a6e0;
        p->~StringTable();
        operator delete(p);
        g_Strings_0046a6e0 = 0;
    }

    if (g_StringState_0046aa38 != 0) {
        GameState* p = g_StringState_0046aa38;
        p->~GameState();
        operator delete(p);
        g_StringState_0046aa38 = 0;
    }

    if (g_StringTable_0046aa34 != 0) {
        GameState* p = g_StringTable_0046aa34;
        p->~GameState();
        operator delete(p);
        g_StringTable_0046aa34 = 0;
    }

    if (g_GameState2_0046aa3c != 0) {
        GameState* p = g_GameState2_0046aa3c;
        p->~GameState();
        operator delete(p);
        g_GameState2_0046aa3c = 0;
    }

    if (g_GameState_0046aa30 != 0) {
        GameState* p = g_GameState_0046aa30;
        p->~GameState();
        operator delete(p);
        g_GameState_0046aa30 = 0;
    }

    if (g_Timer_0046aa20 != 0) {
        Timer* p = g_Timer_0046aa20;
        p->~Timer();
        operator delete(p);
        g_Timer_0046aa20 = 0;
    }

    if (g_StateString_0046aa2c != 0) {
        operator delete(g_StateString_0046aa2c);
        g_StateString_0046aa2c = 0;
    }

    if (g_Mouse_0046aa18 != 0) {
        MouseControl* p = g_Mouse_0046aa18;
        p->~MouseControl();
        operator delete(p);
        g_Mouse_0046aa18 = 0;
    }

    if (g_FlagManager_0046a6e8 != 0) {
        FlagArray* p = g_FlagManager_0046a6e8;
        p->~FlagArray();
        operator delete(p);
        g_FlagManager_0046a6e8 = 0;
    }

    if (g_GameLoopHelper_0046a6f0 != 0) {
        GameLoopHelper* p = g_GameLoopHelper_0046a6f0;
        p->~GameLoopHelper();
        operator delete(p);
        g_GameLoopHelper_0046a6f0 = 0;
    }

    if (g_SoundTracker_0046928c != 0) {
        SoundTracker* p = g_SoundTracker_0046928c;
        p->~SoundTracker();
        operator delete(p);
        g_SoundTracker_0046928c = 0;
    }

    CleanupMemoryCache();
    ShutdownGameSystems();
}


/* Function start: 0x424C40 */
void PlayIntroCinematic(void) {
  InitGameSystems();
  ShutdownGameSystems();
}

/* SEH funclet at 0x411325 — not a standalone function */
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
        g_WaitForInputValue_0046ac04 = wParam;
      }
      DispatchMessageA(&local_1c);
      iVar1 = PeekMessageA(&local_1c, NULL, 0, 0, PM_REMOVE);
    }
    iVar1 = IsAppReady();
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


/* Function start: 0x425FF0 */
extern "C" void TouchFileTimestamp(const char* filename) {
    SYSTEMTIME sysTime;
    FILETIME fileTime;
    GetSystemTime(&sysTime);
    SystemTimeToFileTime(&sysTime, &fileTime);
    HANDLE hFile = CreateFileA(filename, 0xC0000000, 1, 0, 3, 0x80, 0);
    if (hFile != (HANDLE)0xFFFFFFFF) {
        SetFileTime(hFile, 0, 0, &fileTime);
        CloseHandle(hFile);
    }
}

/* Function start: 0x425720 */
void InitGameSystems(void) {
    g_Buffer_0046aa00 = new char[0x100];
    g_Buffer_0046aa04 = new char[CalculateBufferSize(0x280, 0x1e0)];
    CreateGameObject_1();
    ClearMessageLog();
    InitGameConfig();
    SetVideoRes(0x280, 0x1e0);
    g_InputManager_0046aa08 = new InputManager((unsigned int)g_GameConfig2_0046aa10->data.rawData[0]);
    g_EngineSound_0046aa0c = new Sound(0x5622, 8, 1);
    g_GlyphFont_0046aa28 = new AnimatedAsset();
    g_GlyphFont_0046aa28->LoadFont("elements\\barrel06.smk");
    SetStateFlag(0, 1);
    SetCursorVisible(0);
}

// g_PathResolver_0046aa1c declared in globals.h

/* Function start: 0x4258C0 */ /* ~97% match */
void ShutdownGameSystems(void) {
  if (g_GlyphFont_0046aa28 != 0) {
    delete g_GlyphFont_0046aa28;
    g_GlyphFont_0046aa28 = 0;
  }
  if (g_BackBuffer_0046aa14 != 0) {
    delete g_BackBuffer_0046aa14;
    g_BackBuffer_0046aa14 = 0;
    g_BackBuffer_0046aa14 = 0;
  }
  if (g_EngineSound_0046aa0c != 0) {
    Sound* p = g_EngineSound_0046aa0c;
    p->~Sound();
    operator delete(p);
    g_EngineSound_0046aa0c = 0;
  }
  if (g_InputManager_0046aa08 != 0) {
    delete g_InputManager_0046aa08;
    g_InputManager_0046aa08 = 0;
  }

  if (g_PathResolver_0046aa1c != 0) {
    CDData* p = g_PathResolver_0046aa1c;
    p->~CDData();
    operator delete(p);
    g_PathResolver_0046aa1c = 0;
    g_PathResolver_0046aa1c = 0;
  }
  if (g_GameConfig2_0046aa10 != 0) {
     delete g_GameConfig2_0046aa10;
     g_GameConfig2_0046aa10 = 0;
  }
  if (g_Buffer_0046aa04 != 0) {
    delete[] g_Buffer_0046aa04;
    g_Buffer_0046aa04 = 0;
  }
  if (g_Buffer_0046aa00 != 0) {
    delete g_Buffer_0046aa00;
    g_Buffer_0046aa00 = 0;
  }
}

/* Function start: 0x424BB0 */
void CreateGameObject_1() {
  static char s_pathBuffer[260]; // DAT_00472de8
  CDData* cd = new CDData(s_pathBuffer, "teacher.id", "Missing the Teacher CD ROM");
  g_PathResolver_0046aa1c = cd;
  g_PathResolver_0046aa1c = cd;
}

/* Function start: 0x4259E0 */
void InitGameConfig() {
  GameConfig* cfg = new GameConfig();
  g_GameConfig2_0046aa10 = cfg;

  if (g_CmdLineAudioMode_00472dd8 != 0) {
      g_GameConfig2_0046aa10->data.rawData[2] = (unsigned char)g_CmdLineAudioMode_00472dd8;
  }

  if (g_CmdLineInputMode_00472de0 != 0) {
      g_GameConfig2_0046aa10->data.rawData[0] = (unsigned char)g_CmdLineInputMode_00472de0;
  }

  g_GameConfig2_0046aa10->LoadConfig();
}

/* Function start: 0x426AC0 */
int WaitForInput() {
  while (g_WaitForInputValue_0046ac04 == 0) {
    ProcessMessages();
  }
  int iVar1 = g_WaitForInputValue_0046ac04;
  g_WaitForInputValue_0046ac04 = 0;
  return iVar1;
}

/* Function start: 0x420610 */
int IsAppReady() {
  return g_ActivateAppState_00472d14;
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
  if (g_DebugFlag_00472ddc == 0) {
    RunGame();
  } else {
    PlayIntroCinematic();
  }
  g_GameWindow.Shutdown();
  return uStack_14;
}

/* Function start: 0x453BFF */
int __cdecl CalculateBufferSize(int width, unsigned int height) {
  return (((width + 3) & ~3U) * height) + g_BitmapHeaderSize_0046db34;
}

// 0x4260F0 = FormatAssetPath — in CDData.cpp
const char* __cdecl CDData_ResolvePath(const char *format, ...) {
    char local_104[260];
    char *args = (char *)(&format + 1);

    vsprintf(local_104, format, args);
    if (FileExists(local_104)) {
        strcpy(g_PathResolver_0046aa1c->cdPath + 5, local_104);
    } else {
        sprintf(g_PathResolver_0046aa1c->cdPath + 5, "%s\\%s",
                g_PathResolver_0046aa1c->cdPath, local_104);
    }
    return g_PathResolver_0046aa1c->cdPath + 5;
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
    const char* path;
    int result;

    result = -1;
    if (param_1 != 0) {
        sprintf(local_8, "%c:\\", param_1 + 0x40);
        path = local_8;
    } else {
        path = 0;
    }
    if (GetDiskFreeSpaceA(path, &local_10, &local_14, &local_18, &local_c) != 0) {
        result = local_18 * local_14 * local_10;
    }
    return result;
}

/* Function start: 0x4340A0 */
void __cdecl InitMemoryCache(int param_1, int param_2, float param_3) {
    CleanupMemoryCache();
    g_CacheTotalSize_00473440 = 0;
    g_CacheSizeLimit_00473444 = param_2 << 20;
    g_CacheEntryCount_0046b780 = param_1;
    g_CacheEvictThreshold_0046b790 = g_CacheSizeLimit_00473444 - (int)((float)g_CacheSizeLimit_00473444 * param_3 * g_PercentScale);
    MemoryCache* ptr = new MemoryCache(g_CacheEntryCount_0046b780);
    g_FileCache_0046b78c = ptr;
    DAT_00473448.Reset();
}

/* Function start: 0x434170 */
void CleanupMemoryCache() {
    FileCacheCleanup();
    if (g_FileCache_0046b78c != 0) {
        MemoryCache* cache = g_FileCache_0046b78c;
        CacheNode* node = cache->head;
        while (node != 0) {
            FileCacheEntryCleanup((void*)&node->entry, 1);
            node = node->next;
        }
        cache->count = 0;
        cache->freeList = 0;
        cache->tail = 0;
        cache->head = 0;
        int* freeNode = cache->blockList;
        while (freeNode != 0) {
            int* next = (int*)freeNode[0];
            operator delete(freeNode);
            freeNode = next;
        }
        cache->blockList = 0;
        operator delete(cache);
        g_FileCache_0046b78c = 0;
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

  g_ErrorCode_004719c4 = errorCode;
  iVar1 = 0;
  puVar2 = DAT_00471d38;
  do {
    if (*puVar2 == errorCode) {
      g_CrtField_0043bdf0 = *(int *)(iVar1 * 8 + 0x43c764);
      return;
    }
    puVar2 = puVar2 + 2;
    iVar1 = iVar1 + 1;
  } while ((unsigned int)puVar2 < 0x43c8c8);
  if (errorCode >= 0x13 && errorCode <= 0x24) {
    g_CrtField_0043bdf0 = 0xd;
    return;
  }
  if (errorCode >= 0xbc && errorCode <= 0xca) {
    g_CrtField_0043bdf0 = 8;
    return;
  }
  g_CrtField_0043bdf0 = 0x16;
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
    g_CrtField_0043bdf0 = 0xd;
    g_ErrorCode_004719c4 = 5;
    return -1;
  }
  return 0;
}
