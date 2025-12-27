#include "globals.h"
#include "GameState.h"
#include "JoystickManager.h"
#include "VBuffer.h"
#include "AnimatedAsset.h"
#include "Sound.h"
#include "GameWindow.h"
#include "Sprite.h"
#include "Sprite.h"
#include "CDData.h"
#include "Mouse.h"
#include "Parser.h"
#include "Timer.h"

// Pointers initialized to their specific values as requested by patterns in main.cpp
char* g_Buffer_00436960 = (char*)0x00436960;
void* g_Buffer_00436964 = (void*)0x00436964;
JoystickManager* g_JoystickManager_00436968 = (JoystickManager*)0x00436968;
Sound* g_Sound_0043696c = (Sound*)0x0043696c;
void* g_Unknown_00436970 = (void*)0x00436970;
VBuffer* g_WorkBuffer_00436974 = (VBuffer*)0x00436974;
CDData* g_CDData_0043697c = (CDData*)0x0043697c;
Mouse* g_Mouse_00436978 = (Mouse*)0x00436978;
Timer* g_Timer_00436980 = (Timer*)0x00436980;
void* g_Manager_00435a84 = (void*)0x00435a84;
Parser* g_Strings_00435a70 = (Parser*)0x00435a70;
void* g_Unknown_00436994 = (void*)0x00436994;
AnimatedAsset* g_TextManager_00436990 = (AnimatedAsset*)0x00436990;

GameState* g_GameState_00436998 = (GameState*)0x00436998;
GameState* g_GameState2_004369a4 = (GameState*)0x004369a4;
GameState* g_GameState3_0043699c = (GameState*)0x0043699c;
GameState* g_GameState4_004369a0 = (GameState*)0x004369a0;
Sprite* g_Sprite_004360a0 = (Sprite*)0;

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

int DAT_00438428 = 0;
int DAT_00437f62 = 0;
int DAT_00437491 = 0;
char DAT_0043d630[0x4000] = {0};
int DAT_00436b9c = 0;
void* g_GameStruct2 = 0;
int DAT_0043d564 = 0;
int DAT_0043503c = 0;
int DAT_00435040 = 0;
int DAT_00435030 = 0;
int DAT_00435038 = 0;
void* DAT_0043f104 = 0;
void* DAT_0043f100 = 0;
char DAT_0043be30 = 0;
int DAT_0043be34 = 0;