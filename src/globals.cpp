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
#include "MemoryCache.h"
#include "SoundTracker.h"
#include "GameLoopHelper.h"
#include "MsgList.h"
#include "FlagArray.h"
#include "StringTable.h"
#include "RockThrower.h"
#include "GameOutcome.h"
#include "CursorState.h"
#include "Target.h"
#include "SpriteAction.h"
#include "IconBar.h"
#include "GameConfig.h"
#include "GameEngine.h"
#include "AnimatedAsset.h"
#include "T_Object.h"
#include "Palette.h"

// ============================================================================
// Global variables sorted by address
// ============================================================================

// --- 0x00435xxx ---
int DAT_00435030 = 0;                                       // 0x00435030 — CRT initializer table start
int DAT_00435038 = 0;                                       // 0x00435038 — CRT initializer table end
int DAT_0043503c = 0;                                       // 0x0043503c — CRT terminator table start
int DAT_00435040 = 0;                                       // 0x00435040 — CRT terminator table end
SolutionEntry g_SolutionData_00435b88[9] = {                 // 0x00435b88
    {{1, 4, 6}, 0}, {{2, 4, 6}, 0}, {{0, 5, 8}, 1},
    {{1, 3, 8}, 1}, {{2, 3, 8}, 1}, {{0, 4, 0}, 2},
    {{0, 4, 6}, 2}, {{0, 4, 7}, 2}, {{0, 4, 8}, 2}
};
SC_CombatBase* g_CombatEngine_00435eb0 = 0;                  // 0x00435eb0 — Combat engine (EngineA or EngineB)
EngineInfoParser* g_EngineInfoParser_00435f00 = 0;           // 0x00435f00 — ENGINE_INFO parser
Sprite* g_ConsoleSprite_00435f04 = 0;                        // 0x00435f04 — Console sprite
Viewport* g_EngineViewport_00435f08 = 0;                     // 0x00435f08
TargetList* g_TargetList_00435f0c = 0;                       // 0x00435f0c — TARGETS parser
CombatSprite* g_SpriteList_00435f10 = 0;                     // 0x00435f10 — SPRITELIST parser
Weapon* g_Weapon_00435f14 = 0;                               // 0x00435f14 — Weapon parser (RockThrower)
Palette* g_EnginePalette_00435f18 = 0;                       // 0x00435f18
SoundList* g_SoundList_00435f1c = 0;                          // 0x00435f1c
CursorState* g_ScoreManager_00435f20 = 0;                    // 0x00435f20
mCNavigator* g_Navigator_00435f24 = 0;                       // 0x00435f24 — NAVIGATION parser
GameOutcome* g_GameOutcome_00435f28 = 0;                     // 0x00435f28 — Game outcome state

// --- 0x00436xxx ---
int g_SpriteEntryCount_00436344 = 0;                         // 0x00436344 — sprite data entry counter
SpriteHashTable* g_CurrentSprite_004686f8 = 0;               // 0x004686f8 — current sprite hash table
int g_CurrentSpriteIndex_004686fc = 0;                       // 0x004686fc — current sprite table index
Sprite* g_NavCursorSprite_004360a0 = 0;                      // 0x004360a0 — navigator cursor sprite
Sprite* g_NavSprite_004360a4 = 0;                            // 0x004360a4 — navigation sprite
int g_TargetBearingValue_004362c8 = 0;                       // 0x004362c8
int g_TargetRangeCounter_004362cc = 0;                       // 0x004362cc
int g_StringTableCount_0046aa80 = 0;                         // 0x004366b4
char* g_Buffer_0046aa00 = 0;                                 // 0x0046aa00
char* g_Buffer_0046aa04 = 0;                                 // 0x0046aa04
Sound* g_Sound_0043696c = 0;                                 // 0x0043696c
GameConfig* g_GameConfig_00436970 = 0;                       // 0x00436970
VBuffer* g_WorkBuffer_00436974 = 0;                          // 0x00436974
CDData* g_PathResolver_0046aa1c = 0;                               // 0x0043697c
Timer* g_Timer_0046aa20 = 0;                                 // 0x00436980
TimedEventPool* g_TimedEventPool1_00436984 = 0;              // 0x00436984
TimedEventPool* g_TimedEventPool2_00436988 = 0;              // 0x00436988
AnimatedAsset* g_TextManager_00436990 = 0;                   // 0x00436990
GameState* g_StringTable_0046aa34 = 0;                        // 0x0043699c
GameState* g_StringState_0046aa38 = 0;                        // 0x004369a0
int g_SpriteTableInitialized_0046c91c = 0;                   // 0x0046c91c
unsigned int g_TimerCount_00436b94 = 0;                      // 0x00436b94 — active timer count

// --- 0x00437xxx ---
int g_PaletteColorTable_00437608[256];                       // 0x00437608
HDC g_MainDC_0046d070 = 0;                                   // 0x0046d070
char g_TextColor_0046d078 = 0;                               // 0x00437490
int g_FillColorDword_0046d079 = 0;                           // 0x00437491
HGDIOBJ g_StockFont_00437496 = 0;                            // 0x00437496
int g_FontHeight_0046d082 = 0;                               // 0x0043749a
int g_FontExtLeading_0043749e = 0;                           // 0x0043749e
int g_FontAvgWidth_004374a2 = 0;                             // 0x004374a2
HPALETTE g_PreviousPalette_0046d096 = 0;                     // 0x004374ae
HPALETTE g_Palette_0046d074 = 0;                             // 0x0043748c
unsigned short g_StateFlags_004374b2 = 0;         // 0x004374b2
HDC g_SecondaryDC_0046d09c = 0;                              // 0x004374b4
char g_TextAlignH_0046d0a8 = 0;                              // 0x004374c0
char g_TextAlignV_0046d0a9 = 0;                              // 0x004374c1
int g_DrawPosX_0046d0aa = 0;                                 // 0x004374c2
int g_VideoBufferWidth_0046d0ae = 0;                         // 0x004374c6
int g_VideoBufferHeightM1_0046d0b2 = 0;                      // 0x004374ca
int g_DrawPosY_0046d0b6 = 0;                                 // 0x004374ce
int g_VideoBufferHeight_0046d0ba = 0;                        // 0x004374d2
int g_LineWidthH_0046d0be = 0;                               // 0x004374d6
int g_LineWidthV_0046d0c2 = 0;                               // 0x004374da
int g_ClipLeft_0046d0c6 = 0;                                 // 0x004374de
int g_ClipRight_0046d0ca = 0;                                // 0x004374e2
int g_ClipTop_0046d0ce = 0;                                  // 0x004374e6
int g_ClipBottom_0046d0d2 = 0;                               // 0x004374ea
HANDLE g_WinGFileHandle_0046d0d6 = 0;                        // 0x004374ee
char g_CursorVisible_0046d0ee = 0;                           // 0x00437506
char g_CursorState_0046d0ef = 0;                             // 0x00437507
int g_DblClickCX_00437508 = 0;                               // 0x00437508
int g_DblClickCY_0043750c = 0;                               // 0x0043750c
int g_WindowWord_00437510 = 0;                               // 0x00437510
int g_GfxInitFlag_00437514 = 0;                              // 0x00437514
int g_GfxField1_00437518 = 0;                                // 0x00437518
int g_GfxField2_0043751c = 0;                                // 0x0043751c
char g_PaletteMap_0046d108[256] = {0};                       // 0x00437520 — Palette identity map
unsigned char g_PaletteData_0046d208[256] = {0};  // 0x00437620 — Palette data / state flags
// LOGPALETTE: palVersion=0x0300, palNumEntries=0x0100
char g_LogPalette_0046d308[1028] = {0x00, 0x03, 0x00, 0x01}; // 0x00437720
char g_BgrPalette_00437b48[1028];                            // 0x00437b48
int g_BitmapHeaderSize_0046db34 = 0;                         // 0x00437f4c
int g_DibModeFlag_0046db38 = 0;                              // 0x00437f50
int g_VBufField1_00437f56 = 0;                               // 0x00437f56
int g_VBufField2_00437f5a = 0;                               // 0x00437f5a
int g_VideoBufferStride_0046db46 = 0;                        // 0x00437f5e
int g_VideoBufferSize_0046db4a = 0;                          // 0x00437f62
int g_VideoBufferBase_0046db4e = 0;                          // 0x00437f66
short g_VideoBufferDS_00437f6a = 0;                          // 0x00437f6a
int g_VBufMemHandles_00437f6c[32];                           // 0x00437f6c
int g_VBufDibHandles_00437fec[32];                           // 0x00437fec

// --- 0x00438xxx ---
int g_VBufMaxX_0043806c[32];                                 // 0x0043806c
int g_VBufMaxY_004380ec[32];                                 // 0x004380ec
int g_VBufClipLeft_0043816c[32];                             // 0x0043816c
int g_VBufClipTop_004381ec[32];                              // 0x004381ec
int g_VBufHeights_004382ec[32];                              // 0x004382ec
int g_VBufWidths_0043836c[32];                               // 0x0043836c
int g_GfxField3_004383ec = 0;                                // 0x004383ec
int g_GfxField4_004383f4 = 0;                                // 0x004383f4
int g_GfxField5_00438404 = 0;                                // 0x00438404
int g_GfxField6_0043840c = 0;                                // 0x0043840c
HDC g_WinGDC_0046e004 = 0;                                   // 0x0043841c
HMODULE g_WinGModule_0046e008 = 0;                           // 0x00438420
HGDIOBJ g_WinGBitmap_0046e00c = 0;                           // 0x00438424
void* g_WinGCreateDIB_0046e010 = 0;                          // 0x00438428
void* g_WinGSetDIBColorTable_0046e014 = 0;                   // 0x0043842c
void* g_WinGRecommendDIBFormat_0046e018 = 0;                 // 0x00438430
void* g_WinGBitBlt_0046e01c = 0;                             // 0x00438434
void* g_WinGStretchBlt_0046e020 = 0;                         // 0x00438438
char* g_SysDirPtr_0046e024 = g_SystemDirPath_0046e02e;       // 0x0043843c
short g_SegmentReg_0046e028 = 0;                             // 0x00438440
int g_WinGBufSize_0046e02a = 0x1000;                         // 0x00438442
char g_SystemDirPath_0046e02e[256] = {0};                    // 0x00438446
char g_TextMetric_0046f02e[256] = {0};                       // 0x00439446

// --- 0x0043bxxx - 0x0043cxxx (CRT / misc) ---
char g_ExitCode_0043be30 = 0;                                // 0x0043be30
int g_ExitInProgress_0043be34 = 0;                           // 0x0043be34
int g_CrtField_0043bdf0 = 0;                                 // 0x0043bdf0
int g_ErrorCode_004719c4 = 0;                                // 0x0043bdf4
unsigned int DAT_00471d38[90] = {                            // 0x00471d38 - CRT error code mapping
    0x1, 0x16, 0x2, 0x2, 0x3, 0x2, 0x4, 0x18, 0x5, 0xd,
    0x6, 0x9, 0x7, 0xc, 0x8, 0xc, 0x9, 0xc, 0xa, 0x7,
    0xb, 0x8, 0xc, 0x16, 0xd, 0x16, 0xf, 0x2, 0x10, 0xd,
    0x11, 0x12, 0x12, 0x2, 0x21, 0xd, 0x35, 0x2, 0x41, 0xd,
    0x43, 0x2, 0x50, 0x11, 0x52, 0xd, 0x53, 0xd, 0x57, 0x16,
    0x59, 0xb, 0x6c, 0xd, 0x6d, 0x20, 0x70, 0x1c, 0x72, 0x9,
    0x6, 0x16, 0x80, 0xa, 0x81, 0xa, 0x82, 0x9, 0x83, 0x16,
    0x84, 0xd, 0x91, 0x29, 0x9e, 0xd, 0xa1, 0x2, 0xa4, 0xb,
    0xa7, 0xd, 0xb7, 0x11, 0xce, 0x2, 0xd7, 0xb, 0x718, 0xc
};

// --- 0x0043dxxx ---
char g_PuzzleState_0043d0f8 = 0;                             // 0x0043d0f8
GlyphRect g_PuzzleRects1_0043d068[9] = {                     // 0x0043d068
    GlyphRect(0x1de, 0x147, 0x205, 0x16d),
    GlyphRect(0x209, 0x147, 0x230, 0x16d),
    GlyphRect(0x234, 0x147, 0x25b, 0x16d),
    GlyphRect(0x1de, 0x11c, 0x205, 0x143),
    GlyphRect(0x209, 0x11c, 0x230, 0x143),
    GlyphRect(0x234, 0x11c, 0x25b, 0x143),
    GlyphRect(0x1de, 0x0f1, 0x205, 0x118),
    GlyphRect(0x209, 0x0f1, 0x230, 0x118),
    GlyphRect(0x234, 0x0f1, 0x25b, 0x118),
};
GlyphRect g_PuzzleRects2_0043d100[3] = {                     // 0x0043d100
    GlyphRect(0x014, 0x0a1, 0x046, 0x137),
    GlyphRect(0x0b2, 0x0ac, 0x0ef, 0x118),
    GlyphRect(0x15d, 0x0a1, 0x18f, 0x137),
};
int g_ProjectileHits_0043d150 = 0;                           // 0x0043d150
int g_SaveState1_0043d134 = 0;                               // 0x0043d134
int g_SaveState2_0043d138 = 0;                               // 0x0043d138
int g_SaveState3_0043d13c = 0;                               // 0x0043d13c
char g_StringTable_0043d158[16384] = {0};                    // 0x0043d158 — string lookup table
int g_DebugFlag_00472ddc = 0;                                // 0x0043d55c
int g_CmdLineAudioMode_00472dd8 = 0;                        // 0x0043d558
char g_CmdLineInputMode_00472de0 = 0;                       // 0x0043d560
int g_DirectSoundFlag_0046a87c = 1;                          // 0x0046a87c
int g_DevModeFlag_0043d564 = 0;                              // 0x0043d564
char g_CmdLineDataPath_0043d568[260] = {0};                  // 0x0043d568
char g_SpriteFilenameTable_0043d630[0x4000] = {0};           // 0x0043d630
HWND g_GameHWnd_0043de7c = 0;                                // 0x0043de7c
int g_WindowedModeFlag_0043de90;                             // 0x0043de90

// --- 0x0043exxx - 0x0043fxxx ---
void* g_AtExitTableEnd_00475084 = 0;                         // 0x0043f100
void* g_AtExitTableStart_00475088 = 0;                       // 0x0043f104

// --- 0x00440xxx (WinG entry point) ---
void* g_SmackSoundCheck_0044053c = 0;                        // 0x0044053c

// --- 0x00446xxx (SC_Wahoo / SC_Detention / SC_FireAlarm / SC_Question) ---
int g_FireAlarmTimerTarget_0046859c = 10;                      // 0x0046859c
int g_FireAlarmTimer_004685a0 = 5;                           // 0x004685a0
int g_FireAlarmFlag_004685a8 = 0;                            // 0x004685a8
int g_FireAlarmEngine_004685ac = 0;                          // 0x004685ac
char g_QuestLevelKey_00468108[32] = {                          // 0x00468108
    'R','O','O','M', 0, 0, 0, 0,
    'P','R','E','V','I','O','U','S',
    'R','O','O','M','I','N','S','T',
    'A','N','C','E', 0, 0, 0, 0
};
int g_DetentionFlag_00468764 = 0;                            // 0x00468764
int g_DetentionKeyState_00468a18 = 0;                        // 0x00468a18
int g_SlimeField_00468bbc = 0;                               // 0x00468bbc
Weapon* g_ActiveWeapon_00468ef0 = 0;                         // 0x00468ef0
char g_QuestFormatStr_004690e4[4] = "%d";                     // 0x004690e4

// --- 0x00469xxx (SoundTracker / FilePosCache / Parser) ---
int g_SoundTrackerField1_00469128 = 0;                       // 0x00469128
int g_CacheHitCount_0046912c = 0;                            // 0x0046912c
int g_CacheMissCount_00469130 = 0;                           // 0x00469130
SoundPool* g_SoundPool_00469134 = 0;                         // 0x00469134
void* g_CacheLRUNode_00469138 = 0;                           // 0x00469138
int g_SoundTrackerField2_0046913c = 0;                       // 0x0046913c
int g_SoundTrackerField3_00469140 = 0;                       // 0x00469140
TimedEventPool* g_FilePosCache = 0;                          // 0x00469144
int g_ProcessFileCount_00469148 = 0;                         // 0x00469148
int g_LBLParseCount_0046914c = 0;                            // 0x0046914c
int g_LBLParseTime_00469150 = 0;                             // 0x00469150
int g_ReadLineTime_00469154 = 0;                             // 0x00469154
int g_FindKeyTime_00469158 = 0;                              // 0x00469158
int g_TotalProcessTime_0046915c = 0;                         // 0x0046915c
int g_VarSubstFlag_00469160 = 0;                             // 0x00469160
char g_VarSubstBuffer_00469168[160] = {0};                   // 0x00469168
int g_ParserCount = 0;                                       // 0x00469288
SoundTracker* g_SoundTracker_0046928c = 0;                   // 0x0046928c
char g_QuestionBuffer_00469b28[] = "SPRITE%d";               // 0x00469b28

// --- 0x0046axxx (main game objects) ---
int g_SchoolMenuActive_0046a190 = 0;                         // 0x0046a190
Sprite* g_SchoolMenuSprite_0046af08 = 0;                     // 0x0046af08
StringTable* g_Strings_0046a6e0 = 0;                         // 0x0046a6e0
T_Object* g_SelectedItem_0046a6e4 = 0;                       // 0x0046a6e4
FlagArray* g_FlagManager_0046a6e8 = 0;                       // 0x0046a6e8
GameEngine* g_GameEngine_0046a6ec = 0;             // 0x0046a6ec
GameLoopHelper* g_GameLoopHelper_0046a6f0 = 0;               // 0x0046a6f0
InputManager* g_InputManager_0046aa08 = 0;                   // 0x0046aa08
Sound* g_EngineSound_0046aa0c = 0;                           // 0x0046aa0c
GameConfig* g_GameConfig2_0046aa10 = 0;            // 0x0046aa10 — actually GameConfig*
VBuffer* g_BackBuffer_0046aa14 = 0;               // 0x0046aa14
MouseControl* g_Mouse_0046aa18 = 0;                          // 0x0046aa18
ZBufferManager* g_ZBufferManager_0046aa24 = 0;               // 0x0046aa24
AnimatedAsset* g_GlyphFont_0046aa28 = 0;                     // 0x0046aa28
char* g_StateString_0046aa2c = 0;                            // 0x0046aa2c
GameState* g_GameState_0046aa30 = 0;              // 0x0046aa30
GameState* g_GameState2_0046aa3c = 0;                        // 0x0046aa3c
int g_DoubleClickTime_0046ac00 = 0;                          // 0x0046ac00
int g_WaitForInputValue_0046ac04 = 0;                        // 0x0046ac04
int g_SlimeTableInit_0046ad6c = 1;                           // 0x0046ad6c
MsgList* g_MsgList_0046a6dc = 0;                             // 0x0046a6dc

// --- 0x0046bxxx - 0x0046cxxx (combat / engine) ---
EngineInfoParser* g_WeaponParser_0046ae4c = 0;               // 0x0046ae4c
Sprite*           g_BgSprite_0046ae50 = 0;                   // 0x0046ae50
Viewport*         g_Viewport_0046ae54 = 0;                   // 0x0046ae54
TargetList*       g_TargetList_0046ae58 = 0;                 // 0x0046ae58
CombatSprite*     g_CombatSprite_0046ae5c = 0;               // 0x0046ae5c
Weapon*           g_CombatWeapon_0046ae60 = 0;               // 0x0046ae60
Palette*          g_Palette_0046ae64 = 0;                    // 0x0046ae64
SoundList*        g_SoundList_0046ae68 = 0;                  // 0x0046ae68
ScoreDisplay*     g_ScoreDisplay_0046ae6c = 0;               // 0x0046ae6c
mCNavigator*      g_Navigator_0046ae70 = 0;                  // 0x0046ae70
HotspotListData*  g_HotspotPool_0046ae74 = 0;               // 0x0046ae74
SC_CombatBase* g_CombatEngine_0046ae78 = 0;                  // 0x0046ae78
int g_DodgeAnimStates_0046bcd0[3] = {4, 5, 3};               // 0x0046bcd0
int g_PathCacheHits_0046b784 = 0;                            // 0x0046b784
int g_PathCacheMisses_0046b788 = 0;                          // 0x0046b788
int g_CacheEntryCount_0046b780 = 0;                          // 0x0046b780
MemoryCache* g_FileCache_0046b78c = 0;                       // 0x0046b78c
int g_CacheEvictThreshold_0046b790 = 0;                      // 0x0046b790
int g_CachePreloadTime_0046b794 = 0;                         // 0x0046b794
char s_mis_pz_Wahoo_mis_0046bab8[] = "mis\\pz_Wahoo.mis";    // 0x0046bab8
char* g_MissionFilePath_0046bacc = s_mis_pz_Wahoo_mis_0046bab8; // 0x0046bacc
SC_CombatBase* g_WahooEngine_0046bbfc = 0;                    // 0x0046bbfc
char g_CDDriveLetter_0046bd74 = 'A';                         // 0x0046bd74
char* g_FontFilename_0046bd78 = (char*)0xdc;                 // 0x0046bd78
int g_FontField_0046bd7c = 0;                                // 0x0046bd7c
SlimeTable* g_SlimeTable_0046bf28 = 0;                       // 0x0046bf28
Palette* g_PodsPalette_0046bf30 = 0;              // 0x0046bf30
int g_TargetBearingValue_0046bff0 = 0;                       // 0x0046bff0
int g_TargetRangeCounter_0046bff4 = 0;                       // 0x0046bff4

// --- Full-game video buffer globals (no demo equivalent) ---
char g_CurrentVideoBuffer_0046db3c = (char)0xff;             // 0x0046db3c
short g_VideoBufferDS_0046db52 = 0;                          // 0x0046db52
int g_VBufMemHandles_0046db54[32];                           // 0x0046db54
int g_VBufDibHandles_0046dbd4[32];                           // 0x0046dbd4
int g_VBufMaxX_0046dc54[32];                                 // 0x0046dc54
int g_VBufMaxY_0046dcd4[32];                                 // 0x0046dcd4
int g_VBufClipLeft_0046dd54[32];                             // 0x0046dd54
int g_VBufClipTop_0046ddd4[32];                              // 0x0046ddd4
int g_VBufDataPtrs_0046de54[32];                             // 0x0046de54
int g_VBufHeights_0046ded4[32];                              // 0x0046ded4
int g_VBufWidths_0046df54[32];                               // 0x0046df54

int g_SchedulePositionTable_0046b940[42] = {                  // 0x0046b940 — x,y pairs, index 0 unused
    0x3e3d2065, 0x00732520, // [0] unused; overlaps prior string tail in original
    100, 108,       // [1]
    100, 182,       // [2]
    100, 257,       // [3]
    100, 304,       // [4]
    202, 108,       // [5]
    202, 182,       // [6]
    202, 257,       // [7]
    202, 304,       // [8]
    305, 108,       // [9]
    305, 182,       // [10]
    305, 257,       // [11]
    305, 304,       // [12]
    408, 108,       // [13]
    408, 182,       // [14]
    408, 257,       // [15]
    408, 304,       // [16]
    510, 108,       // [17]
    510, 182,       // [18]
    510, 257,       // [19]
    510, 304        // [20]
};
int g_SpaceNavStates_0046c3f0[3] = {4, 5, 3};               // 0x0046c3f0
int g_PeriodStateIdx_0046cb90 = 99;                          // 0x0046cb90
static char s_PeriodChars[] = "PSD";                         // 0x0046cb8c
char* g_PeriodCharTable_0046cb94 = s_PeriodChars;            // 0x0046cb94
int g_PuzzleSolutions_0046cc98[36] = {                        // 0x0046cc98
    1,4,6,0, 2,4,6,0, 0,5,8,1, 1,3,8,1, 2,3,8,1, 0,4,0,2, 0,4,6,2, 0,4,7,2, 0,4,8,2
};
// g_PuzzleResultIdx_0046cca4 = &g_PuzzleSolutions_0046cc98[3] (overlap, now a macro in globals.h)

// --- 0x00471xxx - 0x00473xxx ---
int g_FileDeleteError_004719c0 = 0;                          // 0x004719c0
int g_FireAlarmField1_00472bd8 = 0;                          // 0x00472bd8
int g_FireAlarmField2_00472bdc = 0;                          // 0x00472bdc
int g_FanField1_00472be0 = 0;                                // 0x00472be0
int g_FanField2_00472be4 = 0;                                // 0x00472be4
int g_FanField3_00472be8 = 0;                                // 0x00472be8
int g_FanField4_00472bec = 0;                                // 0x00472bec
char g_AnimFilename_00472c70[64] = {0};                      // 0x00472c70
char g_AnimFilename2_00472cb0[64] = {0};                     // 0x00472cb0
HDC g_WindowDC_00472d00 = 0;                                 // 0x00472d00
HPALETTE g_WindowPalette_00472d04 = 0;                       // 0x00472d04
int g_ScreenWidth_00472d08 = 0;                              // 0x00472d08 (set at runtime)
int g_ScreenHeight_00472d0c = 0;                             // 0x00472d0c (set at runtime)
int g_WindowWidth_00472d10 = 0;                              // 0x00472d10 (windowed mode flag: 0=fullscreen, 1=windowed)
int g_ActivateAppState_00472d14 = 0;                         // 0x00472d14 (set at runtime)
SpriteAction g_IconBarAction_00472d20;                       // 0x00472d20
SpriteAction g_PendingAction_00472d58;                       // 0x00472d58
SpriteAction g_HotspotAction_00472d90;                       // 0x00472d90
int g_DevelopFlag_00472de4 = 0;                              // 0x00472de4
char g_LogEnabled_00472e28 = 0;                              // 0x00472e28
int g_StartBlock_00472e2c = 0;                               // 0x00472e2c
int g_ProjectileHits_0047325c = 0;                           // 0x0047325c
HitBounds g_HitBounds_00473260[3] = {{0, 0}, {0, 0}, {0, 0}}; // 0x00473260
BombData g_BombData_00473278[6];                             // 0x00473278 (BSS, initialized at runtime)
POINT g_CursorPos_00473308;                                  // 0x00473308
// IconBar globals
int g_IconBarLeft_00473310 = 0;                              // 0x00473310
int g_IconBarTop_00473314 = 0;                               // 0x00473314 (set by CRT init via SetIconBarRect)
int g_IconBarRight_00473318 = 0;                             // 0x00473318 (set by CRT init via SetIconBarRect)
int g_IconBarBottom_0047331c = 0;                            // 0x0047331C (set by CRT init via SetIconBarRect)
IconBarEntry g_IconBarEntries_00473320[6];                   // 0x00473320-0x4733F8
int g_IconBarRefCount_0046af0c = 0;                          // 0x0046af0c
GlyphRect g_PuzzleButtonRects_00473d30[9];                   // 0x00473d30
// g_IconBarState_00473334, g_SchoolMenuField1_00473358, g_SchoolMenuField2_0047337c
// are now macros in globals.h — they alias IconBarEntry[0/1/2].field_14
char g_CinematicDebugStr_00473400 = 0;           // 0x00473400
int g_CacheTotalSize_00473440 = 0;                           // 0x00473440
int g_CacheSizeLimit_00473444 = 0;                           // 0x00473444
int g_AnimStates_0046ac30[5] = {0, 3, 1, 5, 2};             // 0x0046ac30
int g_LastBombDir_0046ac44 = -1;                             // 0x0046ac44
GlyphRect g_DoorRects_00473dc8[3];                           // 0x00473dc8
int g_State4Phase_00473df8 = 0;                              // 0x00473df8
int g_RatsField_00473e00 = 0;                                // 0x00473e00
int g_State0Phase_00473e14 = 0;                              // 0x00473e14
int g_RatsState_00473e18 = 0;                                // 0x00473e18
int g_State3Phase_00473e1c = 0;                              // 0x00473e1c
int g_State2Phase_00473e20 = 0;                              // 0x00473e20
// g_InventoryState_004733e8 is now a macro in globals.h — aliases IconBarEntry[5].field_14
int g_GameField_004734a4 = 0;                                // 0x004734a4

// --- Misc (not address-mapped or shared) ---
int (*g_OutOfMemoryCallback)(unsigned int) = NULL;
short _param_3 = 0;                                          // no original address: Sound.obj shim
MessageQueue* g_MessageQueue = 0;                            // no original address: Handler8 message queue
void* g_GameStruct2 = 0;                                     // no original address: shared shim
