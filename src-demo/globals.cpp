#include "globals.h"
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

// Pointers initialized to 0 to avoid crash
// Sorted by address 0x00436960 - 0x004369A4

Engine* g_CombatEngine_00435eb0 = 0; // Combat engine (EngineB subclass)
char* g_Buffer_00436960 = 0;
char* g_Buffer_00436964 = 0;
InputManager* g_InputManager_00436968 = 0;
Sound* g_Sound_0043696c = 0;
#include "GameConfig.h"
GameConfig* g_GameConfig_00436970 = 0;
VBuffer* g_WorkBuffer_00436974 = 0;
MouseControl* g_Mouse_00436978 = 0;
CDData* g_CDData_0043697c = 0;
Timer* g_Timer_00436980 = 0;
TimedEventPool* g_TimedEventPool1_00436984 = 0;
TimedEventPool* g_TimedEventPool2_00436988 = 0;
ZBufferManager* g_ZBufferManager_0043698c = 0;
AnimatedAsset* g_TextManager_00436990 = 0;
char* g_StateString_00436994 = 0;
GameState* g_GameState_00436998 = 0;
GameState* g_GameState3_0043699c = 0;
GameState* g_GameState4_004369a0 = 0;
GameState* g_GameState2_004369a4 = 0;

// Sprite* g_Sprite_004360a0 = 0; // Moved/Removed to match address 0x4360A0 (EngineB/mCNavigator)

// Other globals (Timer globals defined in InputManager.cpp)
int g_DoubleClickTime_004373b8 = 0;

int g_WaitForInputValue_004373bc = 0;

// String pointers for config files
char s_Setup_cfg[] = "Setup.cfg";
extern "C" char* PTR_s_Setup_cfg_00437454 = s_Setup_cfg;
extern "C" char DAT_004371a8[] = "rb";

// ...

int g_BitmapHeaderSize_00437f4c = 0;  // Set at runtime by InitVideoSystem based on color depth
int g_ErrorCode_0043bdf4 = 0;
unsigned int DAT_0043c760[90] = {0}; 
int g_DebugFlag_0043d55c = 0;
char g_CmdLineDataPath_0043d568[260] = {0}; 
HWND DAT_0043de7c = 0;
int DAT_0043bdf0 = 0; 
char g_EngineTypeExplore_00435ef0[4] = "B"; // Engine type for EngineA (exploration)
char g_EngineTypeCombat_00435ef4[4] = "A"; // Engine type for EngineB (combat), also used for text metrics

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
HGDIOBJ DAT_00438424 = 0;
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
int DAT_0043bc88 = 1;  // Time seed (original has 1)
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
char DAT_0043d0f8 = 0;
GlyphRect g_PuzzleRects2_0043d100[3] = {
    GlyphRect(0x014, 0x0a1, 0x046, 0x137),  // [0] left floor
    GlyphRect(0x0b2, 0x0ac, 0x0ef, 0x118),  // [1] middle floor
    GlyphRect(0x15d, 0x0a1, 0x18f, 0x137),  // [2] right floor
};
int DAT_0043d130 = 0;  // Game state save area
int DAT_0043d134 = 0;  // Game state save area
int DAT_0043d138 = 0;  // Game state save area
int DAT_0043d13c = 0;  // Game state save area
int DAT_0043503c = 0;
int DAT_00435040 = 0;
int DAT_00435030 = 0;
int DAT_00435038 = 0;
void* g_AtExitTableStart_0043f104 = 0;
void* g_AtExitTableEnd_0043f100 = 0;
char g_ExitCode_0043be30 = 0;
int g_ExitInProgress_0043be34 = 0;

void* DAT_0043eff0 = 0;  // Process heap handle (GetProcessHeap) - used by Memory.cpp

// Graphics system globals (for Graphics.cpp)
HDC DAT_00437488 = 0;      // Main device context
HDC DAT_004374b4 = 0;      // Secondary device context
int DAT_00437508 = 0;      // SM_CXDOUBLECLK
int DAT_0043750c = 0;      // SM_CYDOUBLECLK
int DAT_00437510 = 0;      // Window word

HGDIOBJ DAT_00437496 = 0;  // Stock font object
int DAT_0043749a = 0;      // Text height + external leading
int DAT_0043749e = 0;      // External leading
int DAT_004374a2 = 0;      // Average char width
char DAT_00439446[256] = {0}; // TEXTMETRIC struct buffer

char DAT_00437490 = 0;
extern "C" unsigned short DAT_004374b2 = 0;  // State flags bitfield (C linkage for StateFlag.cpp)
int DAT_00437518 = 0;
int DAT_0043751c = 0;
int DAT_004383ec = 0;
int DAT_004383f4 = 0;
int DAT_00438404 = 0;
int DAT_0043840c = 0;
int g_LineWidthH_004374d6 = 0;
int g_LineWidthV_004374da = 0;
int DAT_00437514 = 0;
int DAT_00437f56 = 0;
int DAT_00437f5a = 0;
// DAT_0043841c is defined above as HDC g_WinGDC_0043841c
int g_VideoBufferHeightM1_004374ca = 0;  // Height - 1

char DAT_00437520[256] = {0};  // Palette identity map
// This memory is used both as palette data (int[64]) and state flags (BYTE[256])
// Both representations are 256 bytes
extern "C" unsigned char DAT_00437620[256] = {0};  // (C linkage for StateFlag.cpp)

// Far pointer storage for WinG
char DAT_00438446[256] = {0};
char* PTR_DAT_0043843c = DAT_00438446;  // Points to DAT_00438446
short DAT_00438440 = 0;
int DAT_00438442 = 0x1000;  // Original has 4096
HANDLE DAT_004374ee = 0;
HMODULE g_WinGModule_00438420 = 0;

// WinG function pointers
void* g_WinGRecommendDIBFormat_00438430 = 0;
void* g_WinGBitBlt_00438434 = 0;
void* g_WinGStretchBlt_00438438 = 0;

// Window procedure globals
HDC DAT_0043de80 = 0;       // Main window DC
HPALETTE DAT_0043de84 = 0;  // Main window palette
int DAT_0043de88;   // Window width (set at runtime in WM_SIZE)
int DAT_0043de8c;   // Window height (set at runtime in WM_SIZE)
int DAT_0043de90;   // Windowed mode flag (0=fullscreen, 1=windowed)
int DAT_0043de94 = 0;       // Activate app state

// Video table globals (from VideoTable.cpp)
int g_VideoBufferWidth_004374c6 = 0;
int g_VideoBufferStride_00437f5e = 0;  // Video buffer width copy
int g_VideoBufferHeight_004374d2 = 0;
int g_ClipLeft_004374de = 0;
int g_ClipRight_004374e2 = 0;
int g_ClipTop_004374e6 = 0;
int g_ClipBottom_004374ea = 0;

int DAT_0043cb64 = 1;

// Added for GameLoop::Run assembly match
void* g_SmackSoundCheck_0044053c = 0;

// Target/Sprite globals
int DAT_004362cc = 0;
int g_TargetBearingValue_004362c8 = 0;