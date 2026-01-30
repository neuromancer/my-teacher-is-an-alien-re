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

// Pointers initialized to 0 to avoid crash
// Sorted by address 0x00436960 - 0x004369A4

char* g_Buffer_00436960 = 0;
void* g_Buffer_00436964 = 0;
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
int DAT_004373b8 = 0; // DoubleClickTime

int DAT_004373bc = 0; // WaitForInput var

// String pointers for config files
char s_Setup_cfg[] = "Setup.cfg";
extern "C" char* PTR_s_Setup_cfg_00437454 = s_Setup_cfg;
extern "C" char DAT_004371a8[] = "rb";

// ...

int DAT_00437f4c = 0;  // Set at runtime by InitVideoSystem based on color depth
int DAT_0043bdf4 = 0;
unsigned int DAT_0043c760[90] = {0}; 
int DAT_0043d55c = 0;
char DAT_0043d568[260] = {0}; 
HWND DAT_0043de7c = 0;
int DAT_0043bdf0 = 0; 
char DAT_00435ef4 = 'A';  // Original has 0x41

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
EngineInfoParser* DAT_00435f00 = 0;  // ENGINE_INFO parser
Parser* DAT_00435f04 = 0;  // Console parser
Viewport* g_EngineViewport = 0; // 0x435f08
Parser* DAT_00435f0c = 0;  // TARGETS parser
Parser* DAT_00435f10 = 0;  // SPRITELIST parser
Parser* DAT_00435f14 = 0;  // Weapon parser (RockThrower)
Palette* g_EnginePalette = 0; // 0x435f18
SoundList* g_SoundList_00435f1c;
int* g_ScoreManager = 0; // 0x435f20
Parser* DAT_00435f24 = 0;  // NAVIGATION parser
static int _navState = 0;  // Navigation state value
int* DAT_00435f28 = &_navState;  // Pointer to navigation state

char DAT_004374c0 = 0;
char DAT_004374c1 = 0;
int DAT_004374c2 = 0;
int DAT_004374ce = 0;
char DAT_00437506 = 0;
char DAT_00437507 = 0;
// LOGPALETTE structure: palVersion (WORD) = 0x0300, palNumEntries (WORD) = 0x0100 (256)
char DAT_00437720[1028] = {0x00, 0x03, 0x00, 0x01};
char DAT_00437b48[1028];

int DAT_00437f50 = 0;
int DAT_00437f54 = 0xff;  // Current video buffer index, 0xff = no selection
int DAT_00437f66 = 0; 
short DAT_00437f6a = 0;
HDC g_WinGDC_0043841c = 0;
void* DAT_0043842c = 0;
HPALETTE g_Palette_0043748c = 0;
HPALETTE DAT_004374ae = 0;  
HGDIOBJ DAT_00438424 = 0;
int DAT_0043826c[32];
int DAT_00437fec[32];
int DAT_00437f6c[32];
int DAT_0043836c[32];
int DAT_004382ec[32];
int DAT_0043806c[32]; 
int DAT_004380ec[32];
int DAT_0043816c[32];
int DAT_004381ec[32];

void* DAT_00438428 = 0;
int DAT_00437f62 = 0;
int DAT_00437491 = 0;
char DAT_0043d630[0x4000] = {0};
int DAT_00436b9c = 0;
void* g_GameStruct2 = 0;
int DAT_0043d564 = 0;
int DAT_0043d558 = 0;
char DAT_0043d560 = 0;
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
void* DAT_0043f104 = 0;
void* DAT_0043f100 = 0;
char DAT_0043be30 = 0;
int DAT_0043be34 = 0;

void* DAT_0043eff0 = 0;

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
int DAT_004374d6 = 0;
int DAT_004374da = 0;
int DAT_00437514 = 0;
int DAT_00437f56 = 0;
int DAT_00437f5a = 0;
// DAT_0043841c is defined above as HDC g_WinGDC_0043841c
int DAT_004374ca = 0;  // Current buffer height - 1

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
HMODULE DAT_00438420 = 0;

// WinG function pointers
void* DAT_00438430 = 0;
void* DAT_00438434 = 0;
void* DAT_00438438 = 0;

// Window procedure globals
HDC DAT_0043de80 = 0;       // Main window DC
HPALETTE DAT_0043de84 = 0;  // Main window palette
int DAT_0043de88;   // Window width (set at runtime in WM_SIZE)
int DAT_0043de8c;   // Window height (set at runtime in WM_SIZE)
int DAT_0043de90;   // Windowed mode flag (0=fullscreen, 1=windowed)
int DAT_0043de94 = 0;       // Activate app state

// Video table globals (from VideoTable.cpp)
int DAT_004374c6 = 0;  // Current video buffer width
int DAT_00437f5e = 0;  // Video buffer width copy
int DAT_004374d2 = 0;  // Current video buffer height
int DAT_004374de = 0;  // Clip left
int DAT_004374e2 = 0;  // Clip right
int DAT_004374e6 = 0;  // Clip top
int DAT_004374ea = 0;  // Clip bottom

// Added for GameLoop::Run assembly match
void* g_SmackSoundCheck_0044053c = 0;

// Target/Sprite globals
int DAT_004362cc = 0;
int DAT_004362c8 = 0;