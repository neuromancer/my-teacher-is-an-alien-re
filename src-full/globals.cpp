#include "globals.h"
#include "ZBufferManager.h"
#include "SC_CombatBase.h"
#include "GameState.h"
#include "InputManager.h"
#include "VBuffer.h"
#include "AnimatedAsset.h"
#include "Sound.h"
#include "GameWindow.h"
#include "Sprite.h"
#include "CDData.h"
#include "MouseControl.h"
#include "Parser.h"
#include "Timer.h"
//#include "Manager.h"
#include "FlagArray.h"
#include "StringTable.h"
#include "RockThrower.h"
#include "GameOutcome.h"
#include "CursorState.h"
#include "Target.h"
#include "SpriteAction.h"

// Pointers initialized to 0 to avoid crash
// Sorted by address 0x00436960 - 0x004369A4

SC_CombatBase* g_CombatEngine_00435eb0 = 0; // Combat engine (EngineA or EngineB)
char* g_Buffer_0046aa00 = 0;
char* g_Buffer_00436964 = 0;
InputManager* g_InputManager_0046aa08 = 0;
Sound* g_Sound_0043696c = 0;
#include "GameConfig.h"
GameConfig* g_GameConfig_00436970 = 0;
VBuffer* g_WorkBuffer_00436974 = 0;
MouseControl* g_Mouse_0046aa18 = 0;
CDData* g_CDData_0043697c = 0;
Timer* g_Timer_00436980 = 0;
TimedEventPool* g_TimedEventPool1_00436984 = 0;
TimedEventPool* g_TimedEventPool2_00436988 = 0;
ZBufferManager* g_ZBufferManager_0046aa24 = 0;
AnimatedAsset* g_TextManager_00436990 = 0;
char* g_StateString_0046aa2c = 0;
extern "C" GameState* g_GameState_0046aa30 = 0;
GameState* g_GameState3_0043699c = 0;
GameState* g_GameState4_004369a0 = 0;
GameState* g_GameState2_0046aa3c = 0;

// Other globals (Timer globals defined in InputManager.cpp)
int g_DoubleClickTime_0046ac00 = 0;

int g_WaitForInputValue_0046ac04 = 0;

// String constants (Setup.cfg, "rb", engine types) inlined at call sites

// SC_CrystalPuzzle data tables
// TODO: Extract actual values from original binary .data section
// g_PuzzleSolutions_0046cc98 (0x46cc98-0x46cd28): 9 puzzle solutions, 4 ints each {btn0,btn1,btn2,pad}
// g_PuzzleResultIdx_0046cca4 (0x46cca4): result indices per solution
// g_PuzzleButtonRects_00473d30 (0x473d30): 9 button hit-test GlyphRects
// g_DoorRects_00473dc8 (0x473dc8): 3 floor door hit-test GlyphRects
int g_PuzzleSolutions_0046cc98[36] = {0};
int g_PuzzleResultIdx_0046cca4[36] = {0};
GlyphRect g_PuzzleButtonRects_00473d30[9];
GlyphRect g_DoorRects_00473dc8[3];

// ...

int g_BitmapHeaderSize_00437f4c = 0;  // Set at runtime by InitVideoSystem based on color depth
int g_ErrorCode_0043bdf4 = 0;
unsigned int DAT_0043c760[90] = {0}; 
int g_DebugFlag_0043d55c = 0;
char g_CmdLineDataPath_0043d568[260] = {0}; 
HWND g_GameHWnd_0043de7c = 0;
int g_CrtField_0043bdf0 = 0; 
// Engine type strings "A"/"B" inlined at call sites (were g_EngineTypeExplore/Combat)

SolutionEntry g_SolutionData_00435b88[9] = {
    {{1, 4, 6}, 0},
    {{2, 4, 6}, 0},
    {{0, 5, 8}, 1},
    {{1, 3, 8}, 1},
    {{2, 3, 8}, 1},
    {{0, 4, 0}, 2},
    {{0, 4, 6}, 2},
    {{0, 4, 7}, 2},
    {{0, 4, 8}, 2}
};

// Engine parser subsystem globals (0x435f00 - 0x435f28)
EngineInfoParser* g_EngineInfoParser_00435f00 = 0;  // ENGINE_INFO parser
Sprite* g_ConsoleSprite_00435f04 = 0; // Console sprite
Viewport* g_EngineViewport_00435f08 = 0;
TargetList* g_TargetList_00435f0c = 0; // TARGETS parser
CombatSprite* g_SpriteList_00435f10 = 0;  // SPRITELIST parser
Weapon* g_Weapon_00435f14 = 0; // Weapon parser (RockThrower)
Palette* g_EnginePalette_00435f18 = 0;
SoundList* g_SoundList_00435f1c = 0;
CursorState* g_ScoreManager_00435f20 = 0;
mCNavigator* g_Navigator_00435f24 = 0;  // NAVIGATION parser
GameOutcome* g_GameOutcome_00435f28 = 0; // Game outcome state

char g_TextAlignH_004374c0 = 0;
char g_TextAlignV_004374c1 = 0;
int g_DrawPosX_004374c2 = 0;
int g_DrawPosY_004374ce = 0;
char g_CursorVisible_00437506 = 0;
char g_CursorState_00437507 = 0;
// LOGPALETTE structure: palVersion (WORD) = 0x0300, palNumEntries (WORD) = 0x0100 (256)
char g_LogPalette_00437720[1028] = {0x00, 0x03, 0x00, 0x01};
char g_BgrPalette_00437b48[1028];

int g_DibModeFlag_00437f50 = 0;
char g_CurrentVideoBuffer_00437f54 = (char)0xff;  // Current video buffer index, 0xff = no selection
int g_VideoBufferBase_00437f66 = 0;
short g_VideoBufferDS_00437f6a = 0;
HDC g_WinGDC_0043841c = 0;
void* g_WinGSetDIBColorTable_0043842c = 0;
HPALETTE g_Palette_0043748c = 0;
HPALETTE g_PreviousPalette_004374ae = 0;  
HGDIOBJ g_WinGBitmap_00438424 = 0;
int g_VBufDataPtrs_0043826c[32];
int g_VBufDibHandles_00437fec[32];
int g_VBufMemHandles_00437f6c[32];
int g_VBufWidths_0043836c[32];
int g_VBufHeights_004382ec[32];
int g_VBufMaxX_0043806c[32];
int g_VBufMaxY_004380ec[32];
int g_VBufClipLeft_0043816c[32];
int g_VBufClipTop_004381ec[32];

void* g_WinGCreateDIB_00438428 = 0;
int g_VideoBufferSize_00437f62 = 0;
int g_FillColorDword_00437491 = 0;
char g_SpriteFilenameTable_0043d630[0x4000] = {0};
int g_SpriteTableInitialized_00436b9c = 0;
void* g_GameStruct2 = 0;
int g_DevModeFlag_0043d564 = 0;
int g_CmdLineAudioMode_0043d558 = 0;
char g_CmdLineInputMode_0043d560 = 0;
int g_RandomSeed_0043bc88 = 1;  // Time seed (original has 1)
GlyphRect g_PuzzleRects1_0043d068[9] = {
    GlyphRect(0x1de, 0x147, 0x205, 0x16d),  // [0] bottom-left
    GlyphRect(0x209, 0x147, 0x230, 0x16d),  // [1] bottom-center
    GlyphRect(0x234, 0x147, 0x25b, 0x16d),  // [2] bottom-right
    GlyphRect(0x1de, 0x11c, 0x205, 0x143),  // [3] middle-left
    GlyphRect(0x209, 0x11c, 0x230, 0x143),  // [4] middle-center
    GlyphRect(0x234, 0x11c, 0x25b, 0x143),  // [5] middle-right
    GlyphRect(0x1de, 0x0f1, 0x205, 0x118),  // [6] top-left
    GlyphRect(0x209, 0x0f1, 0x230, 0x118),  // [7] top-center
    GlyphRect(0x234, 0x0f1, 0x25b, 0x118),  // [8] top-right
};
char g_PuzzleState_0043d0f8 = 0;
GlyphRect g_PuzzleRects2_0043d100[3] = {
    GlyphRect(0x014, 0x0a1, 0x046, 0x137),  // [0] left floor
    GlyphRect(0x0b2, 0x0ac, 0x0ef, 0x118),  // [1] middle floor
    GlyphRect(0x15d, 0x0a1, 0x18f, 0x137),  // [2] right floor
};
int g_SaveState0_0043d130 = 0;  // Game state save area
int g_SaveState1_0043d134 = 0;  // Game state save area
int g_SaveState2_0043d138 = 0;  // Game state save area
int g_SaveState3_0043d13c = 0;  // Game state save area
int DAT_0043503c = 0;
int DAT_00435040 = 0;
int DAT_00435030 = 0;
int DAT_00435038 = 0;
void* g_AtExitTableStart_0043f104 = 0;
void* g_AtExitTableEnd_0043f100 = 0;
char g_ExitCode_0043be30 = 0;
int g_ExitInProgress_0043be34 = 0;

void* g_ProcessHeap_0043eff0 = 0;  // Process heap handle (GetProcessHeap) - used by Memory.cpp

// Graphics system globals (for Graphics.cpp)
HDC g_MainDC_00437488 = 0;      // Main device context
HDC g_SecondaryDC_004374b4 = 0;      // Secondary device context
int g_DblClickCX_00437508 = 0;      // SM_CXDOUBLECLK
int g_DblClickCY_0043750c = 0;      // SM_CYDOUBLECLK
int g_WindowWord_00437510 = 0;      // Window word

HGDIOBJ g_StockFont_00437496 = 0;  // Stock font object
int g_FontHeight_0043749a = 0;      // Text height + external leading
int g_FontExtLeading_0043749e = 0;      // External leading
int g_FontAvgWidth_004374a2 = 0;      // Average char width
char g_TextMetric_00439446[256] = {0}; // TEXTMETRIC struct buffer

char g_TextColor_00437490 = 0;
extern "C" unsigned short g_StateFlags_004374b2 = 0;  // State flags bitfield (C linkage for StateFlag.cpp)
int g_GfxField1_00437518 = 0;
int g_GfxField2_0043751c = 0;
int g_GfxField3_004383ec = 0;
int g_GfxField4_004383f4 = 0;
int g_GfxField5_00438404 = 0;
int g_GfxField6_0043840c = 0;
int g_LineWidthH_004374d6 = 0;
int g_LineWidthV_004374da = 0;
int g_GfxInitFlag_00437514 = 0;
int g_VBufField1_00437f56 = 0;
int g_VBufField2_00437f5a = 0;
// DAT_0043841c is defined above as HDC g_WinGDC_0043841c
int g_VideoBufferHeightM1_004374ca = 0;  // Height - 1

char g_PaletteMap_00437520[256] = {0};  // Palette identity map
// This memory is used both as palette data (int[64]) and state flags (BYTE[256])
// Both representations are 256 bytes
extern "C" unsigned char g_PaletteData_00437620[256] = {0};  // (C linkage for StateFlag.cpp)

// Far pointer storage for WinG
char g_SystemDirPath_00438446[256] = {0};
char* g_SysDirPtr_0043843c = g_SystemDirPath_00438446;  // Points to g_SystemDirPath_00438446
short g_SegmentReg_00438440 = 0;
int g_WinGBufSize_00438442 = 0x1000;  // Original has 4096
HANDLE g_WinGFileHandle_004374ee = 0;
HMODULE g_WinGModule_00438420 = 0;

// WinG function pointers
void* g_WinGRecommendDIBFormat_00438430 = 0;
void* g_WinGBitBlt_00438434 = 0;
void* g_WinGStretchBlt_00438438 = 0;

// Window procedure globals
HDC g_WindowDC_00472d00 = 0;       // Main window DC
HPALETTE g_WindowPalette_00472d04 = 0;  // Main window palette
int g_WindowWidth_00472d10 = 640;   // Window width (set at runtime in WM_SIZE)
int g_WindowHeight_0043de8c = 480;   // Window height (set at runtime in WM_SIZE)
int g_WindowedModeFlag_0043de90;   // Windowed mode flag (0=fullscreen, 1=windowed)
int g_ActivateAppState_00472d14 = 1;       // Activate app state (1 = app active/ready)

// Video table globals (from VideoTable.cpp)
int g_VideoBufferWidth_004374c6 = 0;
int g_VideoBufferStride_00437f5e = 0;  // Video buffer width copy
int g_VideoBufferHeight_004374d2 = 0;
int g_ClipLeft_004374de = 0;
int g_ClipRight_004374e2 = 0;
int g_ClipTop_004374e6 = 0;
int g_ClipBottom_004374ea = 0;

int g_TimeSeed_0043cb64 = 1;

// Added for GameLoop::Run assembly match
void* g_SmackSoundCheck_0044053c = 0;

// Target/Sprite globals
int g_TargetRangeCounter_004362cc = 0;
int g_TargetBearingValue_004362c8 = 0;

// CDData globals
int g_DevelopFlag_00472de4 = 0;  // 1 = "Develop.___" file found

// Detention/Period globals
int g_PeriodStateIdx_0046cb90 = 60;
static char s_PeriodChars[] = " DSP";
char* g_PeriodCharTable_0046cb94 = s_PeriodChars;

// ============================================================================
// Globals moved from stubs.cpp (sorted by address)
// ============================================================================

int (*g_OutOfMemoryCallback)(unsigned int) = NULL;     // Memory callback
short _param_3 = 0;                                     // Sound.obj ?_param_3@@3FA
MessageQueue* g_MessageQueue = 0;                        // Handler8 message queue

// EngineB globals
void* g_EngineSound_00435f1c = 0;                                 // 0x00435f1c

// Graphics globals
int g_PaletteColorTable_00437608[256];                                   // 0x00437608 palette color table

// Projectile globals
int g_ProjectileHits_0043d150 = 0;                       // 0x0043d150

// SC_Question globals
char g_QuestLevelKey_00468108[32] = {0};                             // 0x00468108
char g_QuestFormatStr_004690e4[32] = {0};                             // 0x004690e4

// SCI_IconBarModule / SC_Detention globals
int g_DetentionFlag_00468764 = 0;                                    // 0x00468764

// SC_Detention globals
int g_DetentionKeyState_00468a18 = 0;                                    // 0x00468a18

// SC_FireAlarm / SC_Pods globals
int g_FireAlarmTimer_004685a0 = 0;                                    // 0x004685a0
int g_FireAlarmEngine_004685ac = 0;                                    // 0x004685ac

// SC_Wahoo / SC_WordSearch / SCI_PracticeRoom globals
Weapon* g_ActiveWeapon_00468ef0 = 0;                                // 0x00468ef0

// SoundTracker globals
int g_SoundTrackerField1_00469128 = 0;                                    // 0x00469128
int g_CacheHitCount_0046912c = 0;                                    // 0x0046912c FilePosCache hit counter
int g_CacheMissCount_00469130 = 0;                                    // 0x00469130 FilePosCache miss counter
SoundPool* g_SoundPool_00469134 = 0;                             // 0x00469134
void* g_CacheLRUNode_00469138 = 0;                                  // 0x00469138 FilePosCache LRU node
int g_SoundTrackerField2_0046913c = 0;                                    // 0x0046913c
int g_SoundTrackerField3_00469140 = 0;                                    // 0x00469140

// Parser globals
void* g_FilePosCache = 0;                                // 0x00469144 DAT_00469144
int g_ProcessFileCount_00469148 = 0;                                    // 0x00469148
int g_LBLParseCount_0046914c = 0;                                    // 0x0046914c
int g_LBLParseTime_00469150 = 0;                                    // 0x00469150
int g_ReadLineTime_00469154 = 0;                                    // 0x00469154
int g_FindKeyTime_00469158 = 0;                                    // 0x00469158
int g_TotalProcessTime_0046915c = 0;                                    // 0x0046915c
int g_VarSubstFlag_00469160 = 0;                                    // 0x00469160 preprocessor variable substitution flag
char g_VarSubstBuffer_00469168[160] = {0};                            // 0x00469168 Parser buffer
int g_ParserCount = 0;                                   // 0x00469288

// SCI_SchoolMenu globals
extern "C" {
    int g_SchoolMenuActive_0046a190 = 0;                                // 0x0046a190
    Sprite* g_SchoolMenuSprite_0046af08 = 0;                             // 0x0046af08
}
int g_SchoolMenuField1_00473358 = 0;                                    // 0x00473358
int g_SchoolMenuField2_0047337c = 0;                                    // 0x0047337c

// SC_Question globals
StringTable* g_Strings_0046a6e0 = 0;                      // 0x0046a6e0
FlagArray* g_FlagManager_0046a6e8 = 0;                   // 0x0046a6e8
GameState* g_StringState_0046aa38 = 0;                             // 0x0046aa38

// SCI_Inventory globals
void* g_SelectedItem_0046a6e4 = 0;                                  // 0x0046a6e4
int g_IconBarState_00473334 = 0;                                    // 0x00473334
int g_InventoryState_004733e8 = 0;                                    // 0x004733e8

// Engine globals
Sound* g_EngineSound_0046aa0c = 0;                                 // 0x0046aa0c

// Full game globals
GameState* g_StringTable_0046aa34 = 0;                   // 0x0046aa34

// SC_Cinematic + SC_SelectHotSpot globals
extern "C" {
    int g_GameEngine_0046a6ec = 0;                                // 0x0046a6ec
    void* g_BackBuffer2_0046aa10 = 0;                              // 0x0046aa10
    VBuffer* g_BackBuffer_0046aa14 = 0;                           // 0x0046aa14
    char g_CinematicDebugStr_00473400 = 0;                               // 0x00473400
    char g_AnimFilename_00472c70[256] = {0};                        // 0x00472c70
    char g_AnimFilename2_00472cb0[256] = {0};                        // 0x00472cb0
}

// SC_Wahoo / SC_WordSearch / SCI_PracticeRoom globals
int g_SlimeTableInit_0046ad6c = 0;                                    // 0x0046ad6c
void* g_WahooEngine_0046bbfc = 0;                                  // 0x0046bbfc
char* g_MissionFilePath_0046bacc = 0;                                  // 0x0046bacc

// EngineB extern "C" globals
extern "C" {
    EngineInfoParser* g_WeaponParser_0046ae4c = 0;                  // 0x0046ae4c
    Sprite*           g_BgSprite_0046ae50 = 0;                  // 0x0046ae50
    Viewport*         g_Viewport_0046ae54 = 0;                  // 0x0046ae54
    TargetList*       g_TargetList_0046ae58 = 0;                  // 0x0046ae58
    CombatSprite*     g_CombatSprite_0046ae5c = 0;                  // 0x0046ae5c
    Weapon*           g_CombatWeapon_0046ae60 = 0;                  // 0x0046ae60
    Palette*          g_Palette_0046ae64 = 0;                  // 0x0046ae64
    SoundList*        g_SoundList_0046ae68 = 0;                  // 0x0046ae68
    ScoreDisplay*     g_ScoreDisplay_0046ae6c = 0;                  // 0x0046ae6c
    mCNavigator*      g_Navigator_0046ae70 = 0;                  // 0x0046ae70
    HotspotListData*  g_HotspotPool_0046ae74 = 0;                  // 0x0046ae74
    int g_CombatEngine_0046ae78 = 0;                     // 0x0046ae78 — active combat engine (SC_CombatBase*)
    int g_RatsState_00473e18 = 0;                                 // 0x00473e18 — SC_Rats main state (0-4)
    int g_State0Phase_00473e14 = 0;                               // 0x00473e14 — SC_Rats State0Handler sub-phase
    int g_State2Phase_00473e20 = 0;                               // 0x00473e20 — SC_Rats State2Handler sub-phase
    int g_State3Phase_00473e1c = 0;                               // 0x00473e1c — SC_Rats State3Handler sub-phase
    int g_State4Phase_00473df8 = 0;                               // 0x00473df8 — SC_Rats State4Handler sub-phase
    int g_RatsField_00473e00 = 0;                        // 0x00473e00 — SC_Rats reserved
}

// CombatSprite globals
int g_CurrentSpriteIndex_0043634c = 0;                   // 0x0043634c — current sprite table index
int g_SpriteEntryCount_00436344 = 0;                     // 0x00436344 — sprite data entry counter
SpriteHashTable* g_CurrentSprite_00436348 = 0;           // 0x00436348 — current sprite hash table

// NavSubNode / mCNavigator globals
Sprite* g_NavSprite_004360a4 = 0;                        // 0x004360a4 — navigation sprite
Sprite* g_NavCursorSprite_004360a0 = 0;                  // 0x004360a0 — navigator cursor sprite

// Timer globals
unsigned int g_TimerCount_00436b94 = 0;                  // 0x00436b94 — active timer count

// SC_SpaceShipNav globals
int g_SpaceNavStates_0046c3f0[3] = {0, 0, 0};           // 0x0046c3f0 — spaceship nav state array

// GameWindow globals
int g_ScreenWidth_00472d08 = 640;                        // 0x00472d08
int g_ScreenHeight_00472d0c = 480;                       // 0x00472d0c

// String table globals
int g_StringTableCount_004366b4 = 0;                     // 0x004366b4

// SC_Slime globals
int g_SlimeField_00468bbc = 0;                           // 0x00468bbc

// Path resolution globals
void* g_PathResolver_0046aa1c = 0;                                  // 0x0046aa1c
int g_PathCacheHits_0046b784 = 0;                                    // 0x0046b784 cache hit counter
int g_PathCacheMisses_0046b788 = 0;                                    // 0x0046b788 cache miss counter
int g_CachePreloadTime_0046b794 = 0;                                    // 0x0046b794 preload time ms

// SC_Detention globals
extern "C" void* g_PodsPalette_0046bf30 = 0;                      // 0x0046bf30 Pods palette pointer

// SC_FireAlarm / SC_Pods globals
SlimeTable* g_SlimeTable_0046bf28 = 0;                            // 0x0046bf28
int g_FireAlarmField1_00472bd8 = 0;                                    // 0x00472bd8
int g_FireAlarmField2_00472bdc = 0;                                    // 0x00472bdc

// SpriteAction globals
SpriteAction g_IconBarAction_00472d20;                               // 0x00472d20
SpriteAction g_PendingAction_00472d58;                               // 0x00472d58
SpriteAction g_HotspotAction_00472d90;                               // 0x00472d90

// SC_DodgeOrville globals
int g_AnimStates_0046ac30[5];                            // 0x0046ac30
int g_LastBombDir_0046ac44;                              // 0x0046ac44
HitBounds g_HitBounds_00473260[3];                       // 0x00473260
BombData g_BombData_00473278[6];                         // 0x00473278
POINT g_CursorPos_00473308;                              // 0x00473308
int g_FanField1_00472be0 = 0;                                    // 0x00472be0
int g_FanField2_00472be4 = 0;                                    // 0x00472be4
int g_FanField3_00472be8 = 0;                                    // 0x00472be8
int g_FanField4_00472bec = 0;                                    // 0x00472bec
int g_DodgeAnimStates_0046bcd0[3] = {0};                              // 0x0046bcd0
int g_FileDeleteError_004719c0 = 0;                                    // 0x004719c0

int g_GameField_004734a4 = 0;
void* g_GlyphFont_0046aa28 = 0;
char g_CDDriveLetter_0046bd74 = 'A';
char* g_FontFilename_0046bd78 = 0;
int g_FontField_0046bd7c = 0;
char g_LogEnabled_00472e28 = 0;
