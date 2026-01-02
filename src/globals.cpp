#include "globals.h"
#include "GameState.h"
#include "InputManager.h"
#include "VBuffer.h"
#include "AnimatedAsset.h"
#include "Sound.h"
#include "GameWindow.h"
#include "Sprite.h"
#include "CDData.h"
#include "Mouse.h"
#include "Parser.h"
#include "Parser.h"
#include "Timer.h"
//#include "Manager.h"
#include "FlagArray.h"
#include "StringTable.h"

// Pointers initialized to 0 to avoid crash in static CRT initialization or early access to invalid memory.
// Original code had hardcoded addresses here which are invalid in the new build.
char* g_Buffer_00436960 = 0;
void* g_Buffer_00436964 = 0;
InputManager* g_InputManager_00436968 = 0;
Sound* g_Sound_0043696c = 0;
#include "GameConfig.h"
GameConfig* g_GameConfig_00436970 = 0;
VBuffer* g_WorkBuffer_00436974 = 0;
CDData* g_CDData_0043697c = 0;
Mouse* g_Mouse_00436978 = 0;
Timer* g_Timer_00436980 = 0;
Timer g_leftClickTimer;  // 0x43de40
Timer g_rightClickTimer; // 0x43de58
int DAT_004373b8 = 0; // DoubleClickTime
FlagArray* g_Manager_00435a84 = 0;
StringTable* g_Strings_00435a70 = 0;
char* g_Unknown_00436994 = 0;
AnimatedAsset* g_TextManager_00436990 = 0;

// String pointers for config files
char s_Setup_cfg[] = "Setup.cfg";
extern "C" char* PTR_s_Setup_cfg_00437454 = s_Setup_cfg;
extern "C" char DAT_004371a8[] = "rb";

GameState* g_GameState_00436998 = 0;
GameState* g_GameState2_004369a4 = 0;
GameState* g_GameState3_0043699c = 0;
GameState* g_GameState4_004369a0 = 0;
Sprite* g_Sprite_004360a0 = 0;

// Regular variables initialized to 0
int DAT_004373bc = 0;
int DAT_00437f4c = 0;
int DAT_0043bdf4 = 0;
unsigned int DAT_0043c760[90] = {0}; // Array of size 0x168 (360 bytes) / 4 = 90
int DAT_0043d55c = 0;
char DAT_0043d568[260] = {0}; // Buffer
HWND DAT_0043de7c = 0;
int DAT_0043bdf0 = 0; 
char DAT_00435ef4 = 0;
char DAT_004374c0 = 0;
char DAT_004374c1 = 0;
int DAT_004374c2 = 0;
int DAT_004374ce = 0;
char DAT_00437506 = 0;
char DAT_00437507 = 0;
char DAT_00437720[1024];
char DAT_00437b48[1024];

int DAT_00437f50 = 0;
int DAT_00437f54 = 0;
int DAT_00437f66 = 0; 
short DAT_00437f6a = 0;
HDC h_0043841c = 0;
void* DAT_0043842c = 0;
HPALETTE hPal_0043748c = 0;
HPALETTE DAT_004374ae = 0;  // Previously selected palette
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
int DAT_0043503c = 0;
int DAT_00435040 = 0;
int DAT_00435030 = 0;
int DAT_00435038 = 0;
void* DAT_0043f104 = 0;
void* DAT_0043f100 = 0;
char DAT_0043be30 = 0;
int DAT_0043be34 = 0;

int* DAT_00436984 = 0;
int* DAT_00436988 = 0;
int* DAT_0043698c = 0;
void* DAT_00435a74 = 0;
void* DAT_00435a78 = 0;
void* DAT_00435a7c = 0;
int DAT_00435a80 = 0;
void* DAT_0043eff0 = 0;

typedef void (__cdecl *SmackSoundCheckFn)();
// Initialize to 0 instead of reading from hardcoded address
SmackSoundCheckFn g_SmackSoundCheck = 0;

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
short DAT_004374b2 = 0;
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
int DAT_0043841c = 0;  // WinG DC or similar
int DAT_004374ca = 0;  // Current buffer height - 1

char DAT_00437520[256] = {0};  // Palette identity map
int DAT_00437620[64] = {0};    // Palette data

// Far pointer storage for WinG
char* PTR_DAT_0043843c = 0;
char DAT_00438446[256] = {0};
int DAT_00438442 = 0;
short DAT_00438440 = 0;
HANDLE DAT_004374ee = 0;
HMODULE DAT_00438420 = 0;

// WinG function pointers
void* DAT_00438430 = 0;
void* DAT_00438434 = 0;
void* DAT_00438438 = 0;

// Window procedure globals
HDC DAT_0043de80 = 0;       // Main window DC
HPALETTE DAT_0043de84 = 0;  // Main window palette
int DAT_0043de88 = 0;       // Window width
int DAT_0043de8c = 0;       // Window height
int DAT_0043de94 = 0;       // Activate app state