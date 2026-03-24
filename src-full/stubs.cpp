#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <share.h>
#include <sys/stat.h>
#include <direct.h>
#include "string.h"
#include "Memory.h"
#include "OnScreenMessage.h"
#include "TimedEvent.h"
#include "globals.h"
#include "ZBufferManager.h"
#include "SC_Question.h"
#include "SC_OnScreenMessage.h"
#include "SCI_SearchScreen.h"
#include "InputManager.h"
#include "MouseControl.h"
#include "smack.h"
#include "mss.h"
#include "Parser.h"
#include "Engine.h"
#include "Timer.h"
#include "SCI_Dialog.h"
#include "Queue.h"
#include "Sound.h"
#include "SpriteAction.h"
#include "GameState.h"
#include "FlagArray.h"
#include "LinkedList.h"
#include "MemoryCache.h"

extern "C" void ShowLoadingScreen(void);
extern "C" void SetVideoRes(int width, int height);
extern "C" char* MakeAudioName(char* baseName);

// ============================================================================
// Global variables moved to globals.cpp
// ============================================================================

/* Function start: 0x426050 */
extern "C" FILE* __cdecl OpenSaveFile(char* path, char* mode) {
    char dir[64];
    dir[0] = 0;
    _splitpath(path, 0, dir, 0, 0);
    if (dir[0] != 0) {
        _mkdir(dir);
    }
    return fsopen(path, mode);
}

// SC_Detention stubs
// FUN_00447790 = SC_SpaceShipNav::ShutDown — implemented in SC_SpaceShipNav.cpp
// FUN_0044c660 = Sprite::Sprite(char*) — implemented in Sprite.cpp
// FUN_00426ac0 = WaitForInput — implemented in main.cpp, callers updated
/* Function start: 0x408EE0 */
void __cdecl FreeArrayItems(void** array, int count) {
    int remaining = count;
    count--;
    if (remaining == 0) {
        return;
    }
    do {
        if (*array != 0) {
            FreeMemory(*array);
            *array = 0;
        }
        array++;
        remaining = count;
        count--;
    } while (remaining != 0);
}

// ============================================================================
// EngineB proxy class stubs
// ============================================================================

// TargetListFull proxy — replaced with TargetList::ProcessTargets
// InputManagerFull proxy — replaced with InputManager::Refresh
// PaletteLoader proxy — replaced with Palette::LoadFile

// WeaponParser proxy removed — Weapon::UpdateProjectiles implemented in Weapon.cpp

// InputManager::Refresh → InputManager.cpp

// Engine COMDAT thunks (needed by SC_DodgeOrville)
// FUN_00449320 = SC_Combat::ShutDown — callers updated
// FUN_00449400 = SC_Combat::AddMessage — callers updated
// FUN_004494e0 = SC_Combat::ProcessLose (callers updated)

// ============================================================================
// AssetList helper stubs
// ============================================================================

extern "C" {
void __fastcall AssetCleanup(void* thisPtr) {}
void __fastcall AssetNodeDelete(void* thisPtr, int param) {}
}

// ============================================================================
// Remaining proxy classes (real class unknown)
// ============================================================================

// FUN_00420CE0 = T_MenuHotspot(int) ctor — implemented in T_MenuHotspot.cpp


// ============================================================================
// GameLoopHelper helper stubs
// ============================================================================
// FUN_0041A6D0 = CompareNodePriority — moved to GameLoopHelper.cpp
// FUN_0041A6F0 = FreeNodeData — moved to GameLoopHelper.cpp

// ============================================================================
// SoundTracker stubs and globals
// ============================================================================
#include "SoundTracker.h"
// SoundTracker globals moved to globals.cpp

// FUN_00455130 = remove() — callers updated
// FUN_00425fa0 = FileExists — callers updated
// FUN_00455110 = _fsopen — callers updated
// FUN_00426400 = internal_ReadLine — callers updated
// FUN_00426490 = EncryptAndWrite — callers updated
// FUN_00454850 = sscanf — callers updated
// FUN_00454D20 = _findfirst — callers updated
// FUN_00454E70 = _findnext — callers updated
// FUN_00455040 = fclose — callers updated
// FUN_004550B0 = fgetpos — callers updated
// FUN_00454510 = sprintf — callers updated
// FUN_00421880 = Timer::Timer — callers updated
// FUN_004218c0 = Timer::Reset — callers updated
// FUN_004218e0 = Timer::Update — callers updated
// FUN_00412210 = FilePosCache::Store — callers updated

// ============================================================================
// Implemented functions (with real code)
// ============================================================================

// WriteToLog — implemented in string.cpp (0x425E40)

extern "C" {
// FUN_0044e530 = GetSoundFilename in string.cpp (callers updated)
// FUN_004265a0 = ShowLoadingScreen — all callers already use the proper name

/* Function start: 0x449B60 */
static char* g_DirectionChars = "NSEWUD";
char GetDirectionChar(int dir) {
    if (dir < 0 || dir > 5) {
        ShowError("Invalid direction %d", dir);
    }
    return g_DirectionChars[dir];
}
} // extern "C"

// ============================================================================
// Remaining __fastcall stubs (unidentified functions)
// ============================================================================

// FUN_004148f0 = SC_Question::~SC_Question — callers updated to use dtor directly
// FUN_00418540 = LinkedList::RemoveCurrent COMDAT — callers updated
// FUN_00403620 = LinkedList::RemoveCurrent COMDAT — callers updated
// FUN_00401c80 = ZBuffer::CleanUpVBuffer — callers updated
// FUN_004061e0 = RenderEntry::~RenderEntry — callers updated to use dtor
// FUN_004218b0 = Timer dtor — callers updated to use delete
// FUN_00406cc0 = Queue::Add in Queue.cpp
// FUN_00406fd0 = Queue::InsertAtCurrent in Queue.cpp — callers updated
// FUN_004070a0 = Queue::Push in Queue.cpp — callers updated
// FUN_004308c0 = ClearActionList in SC_Cinematic.cpp
// FUN_00407180 = LinkedList::RemoveCurrent COMDAT — callers updated
// FUN_0041dbe0 = InitPalette in Palette.cpp
// FUN_0042be00 = InitCombatScreen in SC_Pods.cpp
// FUN_00404d70 = ListNode sdtor (callers: SC_ExtBridge, SC_Rats, ZBufferManager)
// FUN_00404d70 = ListNode sdtor — callers updated to use delete
// FUN_00404b80 = LinkedList::GetCurrentData non-inline (callers: SC_ExtBridge)
// FUN_00404b80 = LinkedList::GetCurrentData — callers updated
// FUN_0040b760 = VBuffer sdtor — callers updated to use delete
// FUN_0044bac0 = mCNavigator::SetNavParams in mCNavigator.cpp
// FUN_00450b10 = EngineB::EngineB — callers updated to use placement new
// FUN_0044c880 = Sprite::InitAnimation in Sprite.cpp — no remaining callers
// FUN_004407c0 = RemoveCurrent COMDAT — callers updated
// FUN_00429c10 = SC_PRHotSpot::~SC_PRHotSpot in SC_PRHotSpot.cpp — callers updated
// FUN_00429df0 = SC_PRHotSpot::Update in SC_PRHotSpot.cpp — callers updated
// FUN_00420f00 = T_MenuHotspot::Update in T_MenuHotspot.cpp — callers updated
// FUN_0042b100 = SavePracticeState — implemented in SCI_PracticeRoom.cpp, callers updated
// FUN_0042b270 = LoadPracticeState — implemented in SCI_PracticeRoom.cpp, callers updated
// FUN_0042bc50 = LinkedList::RemoveCurrent COMDAT — callers updated
// FUN_0042a010 = SC_PRHotSpot::AddMessage — callers updated to use method call
// FUN_00429b60 = SC_PRHotSpot ctor — callers updated to use new
// FUN_00420ce0 = T_MenuHotspot(int) ctor — callers updated to use new
// FUN_004274c0 = RockThrower ctor — callers updated to use new
// FUN_00410fb0 = VBuffer ctor — callers updated to use new
// FUN_00420d90 = T_MenuHotspot::~T_MenuHotspot — callers updated to use delete
// FUN_00425490 = SlimeTable::~SlimeTable — callers updated to use delete
// FUN_00425480 = SlimeTable::SlimeTable — no remaining callers
// FUN_00440860 = PodsEngine ctor — still called via __fastcall
// FUN_00421920 = InitTimeOut in TimeOut.cpp
// FUN_00427880 = Weapon::UpdateProjectiles — implemented in Weapon.cpp, callers updated
// FUN_00403fd0 = ZBufferManager::DrawVBufferRegion (callers updated to use method call)
// FUN_0044cb40 = Sprite::ResetAnimation (callers updated)
// FUN_004127c0 = Parser::Parser in Parser.cpp
// FUN_00407b60 = Parser SEH wrapper destructor (COMDAT, auto-generated by compiler)
// FUN_004047c0 = ZBufferManager::ProcessMessage in ZBufferManager.cpp
// Legacy alias for the remove-current wrapper used by older callers.
// FUN_004036a0 = LinkedList::RemoveCurrent — last caller updated
// FUN_00445970 = T_Hotspot::StopAll in Hotspot.cpp (removed stub, callers updated)
// FUN_004459a0 = T_Hotspot::DoItem in Hotspot.cpp — callers updated to use method call
// FUN_004279a0 = ZBuffer::ResetItems in ZBuffer.cpp
// FUN_00404350 = ZBufferManager::DrawRect — callers updated
// FUN_00404230 = ZBufferManager::DrawText — callers updated
// FUN_00404b90 = ZBufferManager::UpdateScreen — no callers

// ============================================================================
// Remaining __cdecl stubs
// ============================================================================

// FUN_00412a50 = Parser::LBLParse/ReportUnknownLabel in Parser.cpp (callers updated)
// FUN_00413e70 = Parser::ProcessFile in Parser.cpp (callers updated)
// FUN_00425a90 = SetVideoRes in VBuffer.cpp (callers updated)
// Legacy alias for the renamed enqueue wrapper in SpriteAction.cpp.
// FUN_00444e40 = EnqueueSpriteAction — all callers already use the proper name
// ParseSpriteAction (was FUN_00445450) moved to SC_Message.cpp
// FUN_00444a40 = SpriteAction::SpriteAction constructor in SpriteAction.cpp (callers updated to use placement new)
// FUN_00426190 = ResolveAssetPath in CDData.cpp

// Path resolution globals moved to globals.cpp
// DAT_0046b790, DAT_00473440, DAT_00473444 defined in main.cpp
// FUN_004341f0 = FileCacheLookup — moved to CDData.cpp
// FUN_004342d0 = FileCacheRegister — moved to CDData.cpp
// FUN_00433230 = CDData::ResolvePath — moved to CDData.cpp
// FUN_00434030 = FileCacheEntryCleanup — moved to CDData.cpp

// FUN_004560f0 = _splitpath — callers updated
// FUN_004603d0 = _chdir — callers updated
// FUN_004602d0 = _mkdir — callers updated
// FUN_00426220 = CopyFileContent — callers updated


// SC_Combat base method stubs
#include "SC_Combat.h"
// FUN_00432da0 = MouseControl::DrawCursor (removed, callers updated)

// ProcessEvents helper stubs
// FUN_0042d1a0 = TimedEventPool::Pop — callers updated
// FUN_00444a40/FUN_00444920/FUN_00444af0 = SpriteAction ctor/CopyFrom/dtor (callers updated)

// FUN_0044e3e0 = MakeAnimName — moved to string.cpp
// FUN_0044e320 = MakeSoundName — moved to string.cpp
// FUN_0044d420 = Sprite::SetSpriteLogic — moved to Sprite.cpp as class member

// FUN_0044ccf0 = Sprite::Do — callers updated to use method calls
// FUN_004344b0 = FileCacheCleanup — moved to CDData.cpp
// FUN_00434030 = cache entry cleanup — moved to CDData globals section above

// ============================================================================
// Remaining extern "C" stubs
// ============================================================================

extern "C" {
    // FUN_004205e0 = GetScreenWidth in GameWindow.cpp
    // FUN_004205f0 = GetScreenHeight in GameWindow.cpp
    // FUN_00425f10 = BlankScreen in Graphics.cpp
    // GetWindowHeight, GetWindowedModeFlag moved to GameWindow.cpp
    // FUN_00425fa0 = FileExists in main.cpp
    // FUN_004260f0 = FormatAssetPath in CDData.cpp
    // FUN_004307b0 = ShowSmackSummary in SC_Cinematic.cpp
    void __stdcall SmackSummary(void*, void*) {}
    // FUN_0044e3e0 = GetCinematicFilename in SC_Cinematic.cpp
    // SetFontPosition, SetFontColor, DrawFontText moved to Graphics.cpp
    // FUN_00413e10 = ParseFile in Parser.cpp (callers updated)
    // FUN_004309c0 = Handler::WriteMessageAddress in Handler.cpp (callers updated)
}

// ============================================================================
// Remaining IconBar.obj stubs
// ============================================================================

// FUN_00412a50 = Parser::LBLParse in Parser.cpp (callers updated)

#include "GameLoop.h"
// FUN_0044b7e0 = mCNavNode::CallActivateNeighbor in mCNavNode.cpp (callers updated)

// CombatViewport = Viewport class (SetClip=SetDimensions, SetOffset=SetDimensions2, Refresh=SetCenter, SetSize=SetAnchor)
// Methods implemented in Viewport.cpp
// CombatWeapon = Palette class (SetPriority = SetPalette) — implemented in Palette.cpp
// CombatSprite2 = Sample class (SetVolume = Play) — implemented in Sample.cpp

// ============================================================================
// Unresolved externals from new implementations
// ============================================================================

// Hotspot/T_Hotspot stubs moved to Hotspot.cpp

// EventList::InsertNode moved to EventList.cpp

// External function stubs (still referenced by other .cpp files)
// Extern stubs still referenced by callers using raw __fastcall convention
// FUN_0044c740 = Sprite::~Sprite — callers updated to use delete/dtor
// FUN_00444af0 = SpriteAction::~SpriteAction — callers updated to use delete/dtor
// PracticeRoomNotify moved to SCI_PracticeRoom.cpp
// FUN_00409730 = CombatSprite::ProcessFrame — callers updated
// FUN_0044be70 = mCNavigator::Update — callers updated
// FUN_00443360 = TargetList dtor — callers updated to use delete
// FUN_00434740 = EngineInfoParser dtor — callers updated to use delete
// FUN_00409020 = CombatSprite dtor — callers updated to use delete

// Not-yet-typed ctors (classes not fully defined)
// FUN_00421880 = Timer ctor — callers updated to use new Timer()
// FUN_00418690 = NavCrystal ctor — callers updated to use new NavCrystal()
// FUN_0041dbf0 = Palette(char*) ctor — callers updated to use new Palette(name)
// FUN_00421a50 = T_MenuHotspot(char*, int*) ctor — callers updated to use new
// FUN_0044d210 = Sprite::ConfigRange — callers updated to use method call

// SC_ExtBridge extern stubs
// FUN_0044c9d0 = Sprite::StopAnimationSound — callers updated
// FUN_00431030 = GameEngine::ProcessMessage — callers updated
// FUN_00444e40 already defined above as __cdecl (SpriteAction::Dispatch)
// FUN_0041dcc0 = Palette::LoadFile — no remaining callers

// FUN_00455040 = fclose (CRT)
// FUN_00455110 = fsopen (string.cpp) = _fsopen(filename, mode, _SH_DENYNO)

// FUN_00425FC0 = GetFileSize (moved to string.cpp)

// SC_DodgeOrville globals moved to globals.cpp
// rand_00454920 = rand() — callers updated to use rand() directly
// FUN_00433cb0 = GameState::SetFromAction — implemented in GameState.cpp, callers updated
// FUN_00425620 = SlimeTable::LoadEntry — callers updated
// FUN_004254a0 = SlimeTable::Allocate — callers updated
// FUN_00445450 = ParseSpriteAction — callers updated
// FUN_00449480 = SC_Combat::Update — callers updated to use SC_Combat::Update()
// FUN_004250e0 = Sample::~Sample — callers updated
extern MemoryCache* DAT_0046b78c;
extern int DAT_0046b780;
extern int DAT_0046b784;
extern int DAT_0046b788;
extern int DAT_0046b794;
extern int DAT_00473440;
extern int DAT_00473444;

extern "C" {
/* Function start: 0x4260A0 */
int __cdecl DeleteFileAndDir(char* path) {
    char dir[64];
    int result;

    result = remove(path);
    if (result == 0) {
        dir[0] = 0;
        _splitpath(path, 0, dir, 0, 0);
        if (dir[0] != 0) {
            _rmdir(dir);
        }
    }
    return result;
}

/* Function start: 0x434520 */
void LogCacheStats() {
    if (DAT_0046b78c == 0) {
        WriteToMessageLog("HDCache::LogStats() - HDCache not initialized");
        return;
    }
    WriteToMessageLog("***********************");
    WriteToMessageLog("HD File Cache");
    WriteToMessageLog("PreLoading Cache  = %d ms", DAT_0046b794);
    WriteToMessageLog("Cache Entries  = %d of %d (%d of %d Bytes)",
        DAT_0046b78c->field_8, DAT_0046b780, DAT_00473440, DAT_00473444);
    WriteToMessageLog("Hits=%d Misses = %d", DAT_0046b784, DAT_0046b788);
}

/* Function start: 0x4345B0 */
void LogCacheEntries() {
    int* node;
    int idx;

    if (DAT_0046b78c == 0) {
        WriteToMessageLog("HDCache::LogCache() - HDCache not initialized");
        return;
    }
    idx = 1;
    node = (int*)((int*)DAT_0046b78c)[0];
    if (node != 0) {
        WriteToMessageLog("     %-20.20s %6.6s %10.10s %4.4s",
            "FILE", "FSIZE", "BSIZE", "HITS");
    } else {
        WriteToMessageLog("HDCache::LogCache() - No items in Cache");
    }
    if (node != 0) {
        do {
            int* entry = (int*)node[2];
            node = (int*)node[0];
            WriteToMessageLog("%4d %-20.20s %6.6lu %10lu %4.4d",
                idx, entry, *(unsigned long*)((char*)entry + 0x20),
                *(unsigned long*)((char*)entry + 0x28),
                *(int*)((char*)entry + 0x24));
            idx++;
        } while (node != 0);
    }
}
}
// FUN_00427390 = Projectile::UpdateFull — implemented in Projectile.cpp, callers updated
void __fastcall FreePoolBlocks(void* blocks) {
    int* block = (int*)blocks;
    while (block != 0) {
        int* next = (int*)*block;
        FreeMemory(block);
        block = next;
    }
}

// Save/load system stubs
/* Function start: 0x43E2A0 */
int* __cdecl FUN_0043e2a0(int* pool, int prev, int next) {
    // Grow pool if free list empty
    if (pool[3] == 0) {
        int* block = (int*)AllocateMemory(pool[5] * 0x120 + 4);
        *block = pool[4];
        pool[4] = (int)block;
        int count = pool[5];
        int* entry = block + count * 0x48 - 0x47;
        while (count-- >= 0) {
            *entry = pool[3];
            pool[3] = (int)entry;
            entry -= 0x48;
        }
    }
    // Take from free list
    int* node = (int*)pool[3];
    pool[3] = *node;
    node[1] = prev;
    *node = next;
    pool[2]++;
    // Zero payload (0x46 dwords)
    int* payload = node + 2;
    int i;
    for (i = 0x46; i != 0; i--) {
        *payload = 0;
        payload++;
    }
    return node;
}
/* Function start: 0x43E320 */
void __cdecl FUN_0043e320(void* buf, int count) {
    unsigned int dwords = (unsigned int)(count * 0x118) >> 2;
    int* p = (int*)buf;
    for (; dwords != 0; dwords--) {
        *p = 0;
        p++;
    }
}
// SpriteAction::Serialize moved to SpriteAction.cpp
// GameEngine::Serialize moved to GameEngine.cpp
// FlagArray::Serialize moved to FlagArray.cpp

/* Function start: 0x443830 */
void __fastcall PoolAllocate16(int* pool) {
    if (pool[3] == 0) {
        int* block = (int*)AllocateMemory(pool[5] * 0x10 + 4);
        *block = pool[4];
        pool[4] = (int)block;
        int count = pool[5];
        int* entry = block + count * 4 - 3;
        while (count-- >= 0) {
            *entry = pool[3];
            pool[3] = (int)entry;
            entry -= 4;
        }
    }
    int* node = (int*)pool[3];
    pool[3] = *node;
    pool[2]++;
    node[2] = 0;
    int i = 0;
    do { i--; } while (i != 0);
}

/* Function start: 0x445680 */
void* __fastcall StringEntry_Init(void* thisPtr, char* name, int nameLen, int param3, int param4) {
    int* self = (int*)thisPtr;
    self[0] = 0;
    self[1] = 0;
    self[2] = 0;
    self[3] = 0;
    self[4] = 0;
    self[1] = (int)name;
    self[2] = nameLen;
    char* buf = (char*)AllocateMemory(nameLen + 1);
    self[0] = (int)buf;
    strcpy(buf, name);
    self[3] = param3;
    self[4] = param4;
    return thisPtr;
}

extern int GlyphFont_GetTextWidth(void*, char*);

/* Function start: 0x445710 */
int __fastcall TextInput_ProcessKey(void* thisPtr, int key) {
    int* self = (int*)thisPtr;
    char* editBuf = (char*)self[1];
    int editLen = strlen(editBuf);

    if (key == 8) {
        // Backspace
        int pos = editLen - 1;
        if (pos < 0) pos = 0;
        editBuf[pos] = 0;
        return 0;
    }
    if (key == 0xD) {
        // Enter - copy edit to display
        strcpy((char*)self[0], editBuf);
        return 1;
    }
    if (key == 0x1B) {
        // Escape - copy display to edit (revert)
        strcpy(editBuf, (char*)self[0]);
        return 1;
    }
    // Typing
    if (key == 0x2D) key = 0x5F; // '-' → '_'
    if ((key > 0x40 && key < 0x5B) || (key > 0x2F && key < 0x3A) ||
        key == 0x5F || key == 0x20) {
        editBuf[editLen - 1] = (char)key;
        int width = GlyphFont_GetTextWidth((void*)self[3], editBuf);
        if (self[4] < width) {
            editBuf[editLen - 1] = 0;
            return 0;
        }
        unsigned int maxLen = (unsigned int)self[2] - 1;
        if (maxLen < (unsigned int)editLen) {
            editLen = maxLen;
        }
        editBuf[editLen] = 0;
        return 0;
    }
    return 0;
}

// FUN_004256d0 = SoundList::IsSamplePlaying — moved to SoundList.cpp
// FUN_0040c600 = ScoreDisplay::AdjustScore — moved to ScoreDisplay.h (inline)
// FUN_004427c0 = Target::Deactivate — already in Target.cpp
// FUN_0042e4b0 = UpdateWordSearchCursor — moved to SC_WordSearch.cpp
// FUN_00449520 = Engine::OnProcessEnd — wrapper for cross-hierarchy calls
#include "Engine.h"
void __fastcall Handler_OnProcessEnd(int thisPtr) {
    ((Engine*)thisPtr)->OnProcessEnd();
}

/* Function start: 0x43AD50 */
int GlyphFont_GetTextWidth(void* thisPtr, char* str) {
    int width = 0;
    int* self = (int*)thisPtr;

    if (str == 0) return 0;

    while (*str != 0) {
        int ch = (int)(signed char)*str;
        if (ch == 0x20) {
            width = width + self[9]; // offset 0x24 = spaceWidth
        } else if (ch == 0x9) {
            width = width + self[10]; // offset 0x28 = tabWidth
        } else {
            int glyphIdx = ch - self[1]; // offset 0x04 = firstChar
            int* glyphData = (int*)(self[5] + glyphIdx * 16); // offset 0x14 = glyph table
            width = width - glyphData[0] + glyphData[2];
        }
        str++;
        width = width + self[12]; // offset 0x30 = letterSpacing
    }
    return width;
}
