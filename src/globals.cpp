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

// --- 0x00468xxx combat parser state ---
int g_SpriteEntryCount_004686f4 = 0;                         // 0x004686f4 — sprite data entry counter
SpriteHashTable* g_CurrentSprite_004686f8 = 0;               // 0x004686f8 — current sprite hash table
int g_CurrentSpriteIndex_004686fc = 0;                       // 0x004686fc — current sprite table index
int g_StringTableCount_0046aa80 = 0;                         // 0x004366b4
char* g_Buffer_0046aa00 = 0;                                 // 0x0046aa00
char* g_Buffer_0046aa04 = 0;                                 // 0x0046aa04
CDData* g_PathResolver_0046aa1c = 0;                               // 0x0043697c
Timer* g_Timer_0046aa20 = 0;                                 // 0x00436980
GameState* g_StringTable_0046aa34 = 0;                        // 0x0043699c
GameState* g_StringState_0046aa38 = 0;                        // 0x004369a0
int g_SpriteTableInitialized_0046c91c = 0;                   // 0x0046c91c
unsigned int g_TimerCount_0046a5ac = 0;                      // 0x0046a5ac — active timer count

// --- 0x0046dxxx graphics state ---
HDC g_MainDC_0046d070 = 0;                                   // 0x0046d070
HPALETTE g_Palette_0046d074 = 0;                             // 0x0046d074
char g_TextColor_0046d078 = 0;                               // 0x0046d078
int g_FillColorDword_0046d079 = 0;                           // 0x0046d079
char g_ColorIndex_0046d07d = 0;                              // 0x0046d07d
HGDIOBJ g_StockFont_0046d07e = 0;                            // 0x0046d07e
int g_FontHeight_0046d082 = 0;                               // 0x0046d082
int g_FontExtLeading_0046d086 = 0;                           // 0x0046d086
int g_FontAvgWidth_0046d08a = 0;                             // 0x0046d08a
HPALETTE g_PreviousPalette_0046d096 = 0;                     // 0x0046d096
unsigned short g_StateFlags_0046d09a = 0;                    // 0x0046d09a
HDC g_SecondaryDC_0046d09c = 0;                              // 0x0046d09c
char g_TextAlignH_0046d0a8 = 0;                              // 0x0046d0a8
char g_TextAlignV_0046d0a9 = 0;                              // 0x0046d0a9
int g_DrawPosX_0046d0aa = 0;                                 // 0x0046d0aa
int g_VideoBufferWidth_0046d0ae = 0;                         // 0x0046d0ae
int g_VideoBufferHeightM1_0046d0b2 = 0;                      // 0x0046d0b2
int g_DrawPosY_0046d0b6 = 0;                                 // 0x0046d0b6
int g_VideoBufferHeight_0046d0ba = 0;                        // 0x0046d0ba
int g_LineWidthH_0046d0be = 0;                               // 0x0046d0be
int g_LineWidthV_0046d0c2 = 0;                               // 0x0046d0c2
int g_ClipLeft_0046d0c6 = 0;                                 // 0x0046d0c6
int g_ClipRight_0046d0ca = 0;                                // 0x0046d0ca
int g_ClipTop_0046d0ce = 0;                                  // 0x0046d0ce
int g_ClipBottom_0046d0d2 = 0;                               // 0x0046d0d2
HANDLE g_WinGFileHandle_0046d0d6 = 0;                        // 0x0046d0d6
char g_CursorVisible_0046d0ee = 0;                           // 0x0046d0ee
char g_CursorState_0046d0ef = 0;                             // 0x0046d0ef
int g_DblClickCX_0046d0f0 = 0;                               // 0x0046d0f0
int g_DblClickCY_0046d0f4 = 0;                               // 0x0046d0f4
int g_WindowWord_0046d0f8 = 0;                               // 0x0046d0f8
int g_GfxInitFlag_0046d0fc = 0;                              // 0x0046d0fc
char g_PaletteMap_0046d108[256] = {0};                       // 0x0046d108 — Palette identity map
unsigned char g_PaletteData_0046d208[256] = {0};  // 0x00437620 — Palette data / state flags
// LOGPALETTE: palVersion=0x0300, palNumEntries=0x0100
char g_LogPalette_0046d308[1028] = {0x00, 0x03, 0x00, 0x01}; // 0x00437720
char g_BgrPaletteData_0046d734[1024];                        // 0x0046d734
int g_BitmapHeaderSize_0046db34 = 0;                         // 0x0046db34
int g_DibModeFlag_0046db38 = 0;                              // 0x0046db38
int g_VideoBufferStride_0046db46 = 0;                        // 0x0046db46
int g_VideoBufferSize_0046db4a = 0;                          // 0x0046db4a
int g_VideoBufferBase_0046db4e = 0;                          // 0x0046db4e

// --- 0x00438xxx ---
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

// --- 0x0043dxxx ---
char g_PuzzleState_0043d0f8 = 0;                             // 0x0043d0f8
int g_DebugFlag_00472ddc = 0;                                // 0x0043d55c
int g_CmdLineAudioMode_00472dd8 = 0;                        // 0x0043d558
char g_CmdLineInputMode_00472de0 = 0;                       // 0x0043d560
int g_DirectSoundFlag_0046a87c = 1;                          // 0x0046a87c
char g_SpriteFilenameTable_0043d630[0x4000] = {0};           // 0x0043d630
HWND g_GameHWnd_0043de7c = 0;                                // 0x0043de7c

// --- 0x0043exxx - 0x0043fxxx ---
void* g_AtExitTableEnd_00475084 = 0;                         // 0x0043f100
void* g_AtExitTableStart_00475088 = 0;                       // 0x0043f104

// --- 0x00446xxx (SC_Wahoo / SC_Detention / SC_FireAlarm / SC_Question) ---
char s_WENT_TO_CLASS_00468078[14] = "WENT_TO_CLASS";        // 0x00468078
char s_ROOMINSTANCE_004680cc[13] = "ROOMINSTANCE";          // 0x004680cc
char s_PREVIOUSROOM_00468128[13] = "PREVIOUSROOM";          // 0x00468128
char s_PERIOD_00468138[7] = "PERIOD";                       // 0x00468138
char s_ddouble_palette_00468140[16] = "ddouble palette";    // 0x00468140
char s_NUM_ACTIONS_00468168[12] = "NUM_ACTIONS";            // 0x00468168
char s_INVENTORY_INFO_004681b4[16] = "INVENTORY_INFO";      // 0x004681b4
char s_queue_fault_0113_004681c4[17] = "queue fault 0113";  // 0x004681c4
char s_queue_fault_0112_004681d8[17] = "queue fault 0112";  // 0x004681d8
char s_SCI_SearchScreen_004681ec[17] = "SCI_SearchScreen";  // 0x004681ec
char s_VIDEO_RES_0046820c[12] = "VIDEO_RES";                // 0x0046820c
char s_NO_CHOICEMENU_00468218[16] = "NO_CHOICEMENU";        // 0x00468218
char s_MERCYBUTTON_0046823c[12] = "MERCYBUTTON";            // 0x0046823c
char s_NOACTIONINC_00468248[12] = "NOACTIONINC";            // 0x00468248
char s_NOICONBAR_00468254[12] = "NOICONBAR";                // 0x00468254
char s_FORCE_PARSE_00468260[12] = "FORCE_PARSE";            // 0x00468260
char s_SKIP_PERIOD_PARSE_0046826c[20] = "SKIP_PERIOD_PARSE"; // 0x0046826c
char s_BACKGROUNDSND_00468280[16] = "BACKGROUNDSND";        // 0x00468280
char s_LATE_HOTSPOT_00468290[16] = "LATE_HOTSPOT";          // 0x00468290
char s_DIALOG_10_004682a0[10] = "DIALOG 10";                // 0x004682a0
char s_INTRO_HOTSPOT_004682ac[16] = "INTRO_HOTSPOT";        // 0x004682ac
char s_PREMSG_004682bc[8] = "PREMSG";                       // 0x004682bc
char s_AVAILABLE_HOTSPOT_00468334[20] = "AVAILABLE_HOTSPOT"; // 0x00468334
char s_DLG_HOTSPOT_0046838c[12] = "DLG_HOTSPOT";            // 0x0046838c
char s_DELETE_HOTSPOT_004683bc[16] = "DELETE_HOTSPOT";      // 0x004683bc
char s_HOTSPOT_00468424[8] = "HOTSPOT";                     // 0x00468424
char s_AMBIENT_0046842c[8] = "AMBIENT";                     // 0x0046842c
char s_PALE_0046843c[8] = "PALE";                           // 0x0046843c
char s_END_00468444[4] = "END";                             // 0x00468444
char s_declaring_ZBuffer_00468450[18] = "declaring ZBuffer"; // 0x00468450
char s_queue_fault_0102_00468478[17] = "queue fault 0102";  // 0x00468478
char s_queue_fault_0101_0046848c[17] = "queue fault 0101";  // 0x0046848c
char s_SC_SelectHotSpot_004684bc[17] = "SC_SelectHotSpot";  // 0x004684bc
char s_RETURNMESSAGE_004684d0[16] = "RETURNMESSAGE";        // 0x004684d0
char s_SELECTED_004684e0[12] = "SELECTED";                  // 0x004684e0
char s_ROLLOFF_004684ec[8] = "ROLLOFF";                     // 0x004684ec
char s_ROLLON_004684f4[8] = "ROLLON";                       // 0x004684f4
char s_KEY_00468504[4] = "KEY";                             // 0x00468504
char s_LOC_00468518[4] = "LOC";                             // 0x00468518
char s_SPRITE_0046851c[8] = "SPRITE";                       // 0x0046851c
char s_MESSAGE_00468524[8] = "MESSAGE";                     // 0x00468524
char s_SC_Select_00468570[10] = "SC_Select";                // 0x00468570
char s_INTRO_0046857c[8] = "INTRO";                         // 0x0046857c
char s_PALETTE_0046858c[8] = "PALETTE";                     // 0x0046858c
char s_HANDLE_00468594[8] = "HANDLE";                       // 0x00468594

int g_FireAlarmTimerTarget_0046859c = 10;                      // 0x0046859c
int g_FireAlarmTimer_004685a0 = 5;                           // 0x004685a0
int g_FireAlarmFlag_004685a8 = 0;                            // 0x004685a8
int g_FireAlarmEngine_004685ac = 0;                          // 0x004685ac
char s_CB_FireAlarm_004685c8[13] = "CB_FireAlarm";          // 0x004685c8
char s_SCMI_INSERT_004685d8[12] = "SCMI_INSERT";            // 0x004685d8
char s_ROCKTHROWER2_00468604[16] = "ROCKTHROWER2";          // 0x00468604
char s_WEAPON_0046861c[8] = "WEAPON";                       // 0x0046861c
char s_SC_FireAlarm_00468624[13] = "SC_FireAlarm";          // 0x00468624
char s_SOUND_00468644[8] = "SOUND";                         // 0x00468644
char s_MAX_SOUNDS_0046864c[12] = "MAX_SOUNDS";              // 0x0046864c
char s_BG_SOUND_00468660[12] = "BG_SOUND";                  // 0x00468660
char s_STUDENT_HITS_ALLOWED_0046866c[24] = "STUDENT_HITS_ALLOWED"; // 0x0046866c
char s_CAUGHTS_SPRITE_00468684[16] = "CAUGHTS_SPRITE";      // 0x00468684
char s_ALARM_SPRITE_00468694[16] = "ALARM_SPRITE";          // 0x00468694
char s_HAND_SPRITE_004686a4[12] = "HAND_SPRITE";            // 0x004686a4
char s_PLANE_SPRITE_004686b0[16] = "PLANE_SPRITE";          // 0x004686b0
char s_TEACHER_SPRITE_004686c0[16] = "TEACHER_SPRITE";      // 0x004686c0
char s_CONSOLE_SPRITE_004686d0[16] = "CONSOLE_SPRITE";      // 0x004686d0
char s_BACKGROUND_SPRITE_004686e0[20] = "BACKGROUND_SPRITE"; // 0x004686e0
char s_CombatSprite_00468700[13] = "CombatSprite";          // 0x00468700
char s_PLAYED_ANNOUNCEMENT_00468768[20] = "PLAYED_ANNOUNCEMENT"; // 0x00468768
char s_COMBAT_POD_AVAILABLE_0046877c[21] = "COMBAT_POD_AVAILABLE"; // 0x0046877c
char s_IN_PRACTICEROOM_00468794[16] = "IN_PRACTICEROOM";    // 0x00468794
char s_PERIOD2_GYM_HACK_00468828[17] = "PERIOD2_GYM_HACK";  // 0x00468828
char s_D_IN_DETENTION_00468854[15] = "D_IN_DETENTION";      // 0x00468854
char s_S_IN_DETENTION_00468864[15] = "S_IN_DETENTION";      // 0x00468864
char s_P_IN_DETENTION_00468874[15] = "P_IN_DETENTION";      // 0x00468874
char s_end_of_Game_00468884[12] = "end of Game";            // 0x00468884
char s_STACY_ALIEN_00468890[12] = "STACY_ALIEN";            // 0x00468890
char s_JACK_ALIEN_0046889c[11] = "JACK_ALIEN";              // 0x0046889c
char s_WENDY_ALIEN_004688a8[12] = "WENDY_ALIEN";            // 0x004688a8
char s_MIKE_ALIEN_004688b4[11] = "MIKE_ALIEN";              // 0x004688b4
char s_LINSEY_ALIEN_004688c0[13] = "LINSEY_ALIEN";          // 0x004688c0
char s_AWARE_TEACHER_004688d0[14] = "AWARE_TEACHER";        // 0x004688d0
char s_D_AWARE_LINSEY_004688e0[15] = "D_AWARE_LINSEY";      // 0x004688e0
char s_D_AWARE_MIKE_004688f0[13] = "D_AWARE_MIKE";          // 0x004688f0
char s_S_AWARE_LINSEY_00468900[15] = "S_AWARE_LINSEY";      // 0x00468900
char s_S_AWARE_MIKE_00468910[13] = "S_AWARE_MIKE";          // 0x00468910
char s_P_AWARE_LINSEY_00468920[15] = "P_AWARE_LINSEY";      // 0x00468920
char s_P_AWARE_MIKE_00468930[13] = "P_AWARE_MIKE";          // 0x00468930
char s_MUST_SAVEGAME_00468964[14] = "MUST_SAVEGAME";        // 0x00468964
char s_DETENTION_INFO_004689d0[16] = "DETENTION_INFO";      // 0x004689d0
char s_ANNOUNCE_004689e0[12] = "ANNOUNCE";                  // 0x004689e0
char s_ENDPERIODANNOUNCE_004689f8[20] = "ENDPERIODANNOUNCE"; // 0x004689f8
char s_POSTMESSAGE_00468a0c[12] = "POSTMESSAGE";            // 0x00468a0c
char s_CB_Gauntlet_00468a34[12] = "CB_Gauntlet";            // 0x00468a34
char s_DERIVED_ENGINE_INFO_00468a7c[20] = "DERIVED_ENGINE_INFO"; // 0x00468a7c
char s_END_DERIVED_ENGINE_INFO_00468acc[24] = "END_DERIVED_ENGINE_INFO"; // 0x00468acc
char s_NUM_ENGINE_SHOTS_00468ae4[20] = "NUM_ENGINE_SHOTS";  // 0x00468ae4
char s_MAX_NUM_HITS_00468af8[16] = "MAX_NUM_HITS";          // 0x00468af8
char s_MASK_SPRITE_00468b08[12] = "MASK_SPRITE";            // 0x00468b08
char s_ENG_HIT_SPRITE_00468b14[16] = "ENG_HIT_SPRITE";      // 0x00468b14
char s_ENGINE_SPRITE_00468b24[16] = "ENGINE_SPRITE";        // 0x00468b24
char s_HEALTH_SPRITE_00468b34[16] = "HEALTH_SPRITE";        // 0x00468b34
char s_illegal_handle_00468b60[15] = "illegal handle";      // 0x00468b60
char s_T_Object_00468b7c[9] = "T_Object";                   // 0x00468b7c
char s_ILLEGAL_00468b88[8] = "ILLEGAL";                     // 0x00468b88
char s_DISCRIPTION_00468b90[12] = "DISCRIPTION";            // 0x00468b90
char s_INVENTORYSPR_00468ba8[16] = "INVENTORYSPR";          // 0x00468ba8
char s_CB_Slime_00468dbc[9] = "CB_Slime";                   // 0x00468dbc
char s_SC_Slime_00468de8[9] = "SC_Slime";                   // 0x00468de8
char s_TENTACLE_SHOTS_NEEDED_00468df4[24] = "TENTACLE_SHOTS_NEEDED"; // 0x00468df4
char s_OPENED_SHOTS_NEEDED_00468e0c[20] = "OPENED_SHOTS_NEEDED"; // 0x00468e0c
char s_CLOSED_SHOTS_NEEDED_00468e20[20] = "CLOSED_SHOTS_NEEDED"; // 0x00468e20
char s_SLIME_METER_SPRITE_00468e34[20] = "SLIME_METER_SPRITE"; // 0x00468e34
char s_ARM_MASK_SPRITE_00468e48[16] = "ARM_MASK_SPRITE";    // 0x00468e48
char s_RIGHT_SWITCH_SPRITE_00468e58[20] = "RIGHT_SWITCH_SPRITE"; // 0x00468e58
char s_LEFT_SWITCH_SPRITE_00468e6c[20] = "LEFT_SWITCH_SPRITE"; // 0x00468e6c
char s_RIGHT_ARM_SPRITE_00468e80[20] = "RIGHT_ARM_SPRITE";  // 0x00468e80
char s_LEFT_ARM_SPRITE_00468e94[16] = "LEFT_ARM_SPRITE";    // 0x00468e94
char s_COMBAT_SLIME_WON_00468edc[17] = "COMBAT_SLIME_WON";  // 0x00468edc
char s_CB_Fan_00468f04[7] = "CB_Fan";                       // 0x00468f04
char s_COMBAT_FAN_WON_00468f0c[15] = "COMBAT_FAN_WON";      // 0x00468f0c
char s_LOSE_MESSAGE_00468f1c[16] = "LOSE_MESSAGE";          // 0x00468f1c
char s_SC_Fan_00468f2c[7] = "SC_Fan";                       // 0x00468f2c
char s_SET_GAMESTATE_00468f4c[16] = "SET_GAMESTATE";        // 0x00468f4c
char s_NUM_BOLTS_REMAINING_00468f5c[20] = "NUM_BOLTS_REMAINING"; // 0x00468f5c
char s_NOISE_METER_VBUFFER_00468f84[20] = "NOISE_METER_VBUFFER"; // 0x00468f84
char s_BR_SPRITE_00468f98[12] = "BR_SPRITE";                // 0x00468f98
char s_TR_SPRITE_00468fa4[12] = "TR_SPRITE";                // 0x00468fa4
char s_BL_SPRITE_00468fb0[12] = "BL_SPRITE";                // 0x00468fb0
char s_TL_SPRITE_00468fbc[12] = "TL_SPRITE";                // 0x00468fbc
char s_STOP_SWITCH_SPRITE_00468fc8[20] = "STOP_SWITCH_SPRITE"; // 0x00468fc8
char g_QuestLevelKey_00468108[32] = {                          // 0x00468108
    'R','O','O','M', 0, 0, 0, 0,
    'P','R','E','V','I','O','U','S',
    'R','O','O','M','I','N','S','T',
    'A','N','C','E', 0, 0, 0, 0
};
int g_DetentionFlag_00468764 = 0;                            // 0x00468764
int g_DetentionKeyState_00468a18 = 0;                        // 0x00468a18
CombatWeapon* g_SlimeField_00468bbc = 0;                     // 0x00468bbc
int g_SlimeHitTable_00468bc0[122] = {                        // 0x00468bc0-0x00468da4
    0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 0, 0, 0, 0, 0, 0
};
SC_Combat* g_ActiveCombat_00468a1c = 0;                      // 0x00468a1c
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
char s_LABEL_00469374[8] = "LABEL";                         // 0x00469374
char s_GS_004695dc[4] = "GS";                               // 0x004695dc
char s_HALT__00469628[6] = "HALT_";                         // 0x00469628
char s_SET_MOUSE__00469780[11] = "SET_MOUSE_";              // 0x00469780
char s_SET_GAMESTATE__004697c8[15] = "SET_GAMESTATE_";      // 0x004697c8
char s__V__00469808[4] = "_V_";                             // 0x00469808
char s__HALT__0046980c[8] = "_HALT_";                       // 0x0046980c
char s__EXIT__00469814[8] = "_EXIT_";                       // 0x00469814
char s__RETURN__0046981c[12] = "_RETURN_";                  // 0x0046981c
char s__GOSUB__00469828[8] = "_GOSUB_";                     // 0x00469828
char s__GOTO__00469830[8] = "_GOTO_";                       // 0x00469830
char s__SET_MOUSE__00469838[12] = "_SET_MOUSE_";            // 0x00469838
char s__SET_GAMESTATE__00469844[16] = "_SET_GAMESTATE_";    // 0x00469844
char s__ENDIF__00469854[8] = "_ENDIF_";                     // 0x00469854
char s__ELSE__0046985c[8] = "_ELSE_";                       // 0x0046985c
char s__ELSEIF__00469864[12] = "_ELSEIF_";                  // 0x00469864
char s__IF__00469870[8] = "_IF_";                           // 0x00469870
char s_illegal_state_00469a60[14] = "illegal state";        // 0x00469a60
char s_HOLD_00469a70[8] = "HOLD";                           // 0x00469a70
char s_SINGLE_PLAY_00469a78[12] = "SINGLE_PLAY";            // 0x00469a78
char s_CONSTANT_00469a84[12] = "CONSTANT";                  // 0x00469a84
char s_PLAYSOUND_00469a90[12] = "PLAYSOUND";                // 0x00469a90
char s_SWITCHROOM_00469ad4[12] = "SWITCHROOM";              // 0x00469ad4
char s_GIVECUT_00469ae0[8] = "GIVECUT";                     // 0x00469ae0
char s_GIVELATE_00469ae8[12] = "GIVELATE";                  // 0x00469ae8
char s_SC_Question_in_gamestate_00469af4[25] = "SC_Question in gamestate"; // 0x00469af4
char s_GAMESTATE_00469b1c[12] = "GAMESTATE";                // 0x00469b1c
char g_QuestionBuffer_00469b28[] = "SPRITE%d";               // 0x00469b28
char s_DISABLESPRITE_00469b80[16] = "DISABLESPRITE";        // 0x00469b80
char s_DISABLEQ_00469b90[12] = "DISABLEQ";                  // 0x00469b90
char s_ENABLEQ_00469b9c[8] = "ENABLEQ";                     // 0x00469b9c
char s_DEACTIVATEQ_00469ba4[12] = "DEACTIVATEQ";            // 0x00469ba4
char s_ENABLEACTIVATEQ_00469bb0[16] = "ENABLEACTIVATEQ";    // 0x00469bb0
char s_ACTIVATEQ_00469bf8[12] = "ACTIVATEQ";                // 0x00469bf8
char s_SC_Question_00469c04[12] = "SC_Question";            // 0x00469c04
char s_TEXT_00469c10[8] = "TEXT";                           // 0x00469c10
char s_STANDARD_SPR_00469c30[16] = "STANDARD_SPR";          // 0x00469c30
char s_SPR_00469c40[4] = "SPR";                             // 0x00469c40
char s_OVERLAYS_00469c44[12] = "OVERLAYS";                  // 0x00469c44
char s_queue_fault_0103_00469cc8[17] = "queue fault 0103";  // 0x00469cc8
char s_SPRITE0_00469cdc[8] = "SPRITE0";                     // 0x00469cdc
char s_SCI_Dialog_00469ce4[11] = "SCI_Dialog";              // 0x00469ce4
char s_PLACEHOLDER_00469cfc[12] = "PLACEHOLDER";            // 0x00469cfc
char s_QUESTION_00469d08[12] = "QUESTION";                  // 0x00469d08
char s_Crystal_Piece_00469d30[14] = "Crystal_Piece";        // 0x00469d30
char s_PATTERN_00469d58[8] = "PATTERN";                     // 0x00469d58
char s_PZLOC_00469d60[8] = "PZLOC";                         // 0x00469d60
char s_CRYSTAL_00469dcc[8] = "CRYSTAL";                     // 0x00469dcc
char s_SOURCE_00469e0c[8] = "SOURCE";                       // 0x00469e0c
char s_SOURCEPICKUP_00469e28[16] = "SOURCEPICKUP";          // 0x00469e28
char s_BAR_SPRITE_00469e38[12] = "BAR_SPRITE";              // 0x00469e38
char s_CIRCLE_SPRITE_00469e44[16] = "CIRCLE_SPRITE";        // 0x00469e44
char s_REPORT_of_in_use_animations_00469e94[28] = "REPORT of in use animations"; // 0x00469e94
char s_illegal_message_15_00469ff0[19] = "illegal message 15"; // 0x00469ff0
char s_T_Simple_Hotspot_0046a004[17] = "T_Simple_Hotspot";  // 0x0046a004
char s_OBJECT_0046a018[8] = "OBJECT";                       // 0x0046a018
char s_ACTION_0046a020[8] = "ACTION";                       // 0x0046a020
char s_T_Simple_Hotspot_in_gamestate_0046a028[30] = "T_Simple_Hotspot in gamestate"; // 0x0046a028
char s_CHECKOBJECT_0046a048[12] = "CHECKOBJECT";            // 0x0046a048
char s_CHECKGAMESTATE_0046a054[16] = "CHECKGAMESTATE";      // 0x0046a054
char s_CHECKAVAILIABLE_0046a07c[16] = "CHECKAVAILIABLE";    // 0x0046a07c
char s_PLAYCINEMATIC_0046a08c[16] = "PLAYCINEMATIC";        // 0x0046a08c
char s_SWITCHPREVIOUSROOM_0046a09c[20] = "SWITCHPREVIOUSROOM"; // 0x0046a09c
char s_SWITCHFOCUS_0046a0bc[12] = "SWITCHFOCUS";            // 0x0046a0bc
char s_CHECKMSG_0046a0c8[12] = "CHECKMSG";                  // 0x0046a0c8
char s_INCORRECTMESSAGE_0046a0d4[20] = "INCORRECTMESSAGE";  // 0x0046a0d4
char s_INC_ACTIONS_0046a0e8[12] = "INC_ACTIONS";            // 0x0046a0e8
char s_INCORRECT_0046a0f4[12] = "INCORRECT";                // 0x0046a0f4
char s_CORRECT_0046a100[8] = "CORRECT";                     // 0x0046a100
char s_ROLLOVER_0046a110[12] = "ROLLOVER";                  // 0x0046a110

// --- 0x0046axxx (main game objects) ---
int g_SchoolMenuActive_0046a190 = 0;                         // 0x0046a190
char s_1_NOTALIEN_0046a194[11] = "1_NOTALIEN";              // 0x0046a194
char s_3_NOTALIEN_0046a1a0[11] = "3_NOTALIEN";              // 0x0046a1a0
char s_2_NOTALIEN_0046a1ac[11] = "2_NOTALIEN";              // 0x0046a1ac
char s_TEACHER_ALIEN_0046a1b8[14] = "TEACHER_ALIEN";        // 0x0046a1b8
char s_P_CAPTURED_0046a208[11] = "P_CAPTURED";              // 0x0046a208
char s_S_CAPTURED_0046a214[11] = "S_CAPTURED";              // 0x0046a214
char s_D_CAPTURED_0046a220[11] = "D_CAPTURED";              // 0x0046a220
char s_CLASS_0046a250[6] = "CLASS";                         // 0x0046a250
char s_CANCEL_0046a258[8] = "CANCEL";                       // 0x0046a258
char s_OKAY_0046a260[8] = "OKAY";                           // 0x0046a260
char s_OPTION_0046a268[7] = "OPTION";                       // 0x0046a268
char s_CHAR_0046a270[8] = "CHAR";                           // 0x0046a270
char s_BGSND_0046a278[8] = "BGSND";                         // 0x0046a278
char s_SCI_SchoolMenu_0046a280[15] = "SCI_SchoolMenu";      // 0x0046a280
char s_AVAILABILITY_0046a2a0[16] = "AVAILABILITY";          // 0x0046a2a0
char s_BACK_0046a2b0[8] = "BACK";                           // 0x0046a2b0
char s_IDI_ICON1_0046a328[10] = "IDI_ICON1";                // 0x0046a328
char s_FLAGARRAY_INFO_0046a514[16] = "FLAGARRAY_INFO";      // 0x0046a514
char s_T_MenuHotspot_0046a524[14] = "T_MenuHotspot";        // 0x0046a524
char s_HSOUND_0046a594[8] = "HSOUND";                       // 0x0046a594
char s_MOUSE_0046a59c[8] = "MOUSE";                         // 0x0046a59c
char s_RECT_0046a5a4[12] = "RECT";                          // 0x0046a5a4
char s_InitFiFile_0046a644[11] = "InitFiFile";              // 0x0046a644
char s_DISABLE_0046a650[8] = "DISABLE";                     // 0x0046a650
char s_TEST_STRINGS_0046a704[13] = "TEST_STRINGS";          // 0x0046a704
char s_CACHE_SIZE_0046a7dc[11] = "CACHE_SIZE";              // 0x0046a7dc
char s_MessagePoster_0046a830[14] = "MessagePoster";        // 0x0046a830
char s_Missing_the_Teacher_CD_ROM_0046a860[27] = "Missing the Teacher CD ROM"; // 0x0046a860
char s_workbuff_must_be_first_vb_create_0046aa58[39] = "workbuff must be first vb created '%d'"; // 0x0046aa58
char s_dest_string_too_small_0046aa84[22] = "dest string too small"; // 0x0046aa84
char s_missing_string_0046aab8[15] = "missing string";       // 0x0046aab8
char s_Error_0046aac8[6] = "Error";                         // 0x0046aac8
char s_Message_0046aad0[8] = "Message";                     // 0x0046aad0
char s_Error_Reading_CD_0046ab54[17] = "Error Reading CD";  // 0x0046ab54
char s_RockThrower_0046ac08[12] = "RockThrower";            // 0x0046ac08
char s_MAXROCKS_0046ac24[12] = "MAXROCKS";                  // 0x0046ac24
char s_CB_DGOrv_0046ac5c[9] = "CB_DGOrv";                   // 0x0046ac5c
char s_DODGE_COMBAT_AVAILABLE_0046ac80[23] = "DODGE_COMBAT_AVAILABLE"; // 0x0046ac80
char s_MAX_HITS_BY_STINK_BOMBS_0046ace4[24] = "MAX_HITS_BY_STINK_BOMBS"; // 0x0046ace4
char s_SC_DodgeOrville_0046acfc[16] = "SC_DodgeOrville";    // 0x0046acfc
char s_ANIM_0046ad0c[8] = "ANIM";                           // 0x0046ad0c
char s_STINK_METER_SPRITE_0046ad14[20] = "STINK_METER_SPRITE"; // 0x0046ad14
char s_STINK_METER_BASE_SPRITE_0046ad28[24] = "STINK_METER_BASE_SPRITE"; // 0x0046ad28
char s_NUMBER_OF_THROWS_0046ad40[20] = "NUMBER_OF_THROWS";  // 0x0046ad40
char s_SC_PRHotSpot_0046ad88[13] = "SC_PRHotSpot";          // 0x0046ad88
char s_SC_Simple_Hotspot_in_gamestate_0046ad98[31] = "SC_Simple_Hotspot in gamestate"; // 0x0046ad98
char s_FINAL_PRACTICEROOM_0046ade4[19] = "FINAL_PRACTICEROOM"; // 0x0046ade4
char s_SCI_PracticeRoom_0046ae38[17] = "SCI_PracticeRoom";  // 0x0046ae38
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
TimedEventPool*   g_HotspotPool_0046ae74 = 0;               // 0x0046ae74
SC_CombatBase* g_CombatEngine_0046ae78 = 0;                  // 0x0046ae78
char s_CONSOLE_0046ae90[8] = "CONSOLE";                     // 0x0046ae90
char s_Engine_0046ae98[7] = "Engine";                       // 0x0046ae98
char s_NONE_0046aea0[8] = "NONE";                           // 0x0046aea0
char s_RAY_GUN_0046aea8[8] = "RAY_GUN";                     // 0x0046aea8
char s_ROCKTHROWER_0046aeb0[12] = "ROCKTHROWER";            // 0x0046aeb0
char s_SC_IconBarModule_0046af58[17] = "SC_IconBarModule";  // 0x0046af58
char s_ICON_SOUND_0046af8c[12] = "ICON_SOUND";              // 0x0046af8c
char s_ICON_SPRITE_0046afa4[12] = "ICON_SPRITE";            // 0x0046afa4
char s_ICONBAR_SPRITE_0046afb0[16] = "ICONBAR_SPRITE";      // 0x0046afb0
char s_puz_roach_0046afd4[10] = "puz_roach";                // 0x0046afd4
char s_ROACH_SPRITE_0046b030[16] = "ROACH_SPRITE";          // 0x0046b030
char s_MAX_TRIES_0046b040[12] = "MAX_TRIES";                // 0x0046b040
char s_RULES_SPRITE_0046b04c[16] = "RULES_SPRITE";          // 0x0046b04c
char s_TREATS_SPRITE_0046b05c[16] = "TREATS_SPRITE";        // 0x0046b05c
char s_CINE_SUMMARY_0046b084[13] = "CINE_SUMMARY";          // 0x0046b084
char s_Summary_0046b094[8] = "Summary";                     // 0x0046b094
char s_illegal_modual_insertion_double_0046b42c[32] = "illegal modual insertion double"; // 0x0046b42c
char s_illegal_modual_insertion_0046b44c[25] = "illegal modual insertion"; // 0x0046b44c
char s_GAME_INFO_0046b4a0[12] = "GAME_INFO";                // 0x0046b4a0
char s_MouseControl_0046b4d0[13] = "MouseControl";          // 0x0046b4d0
char s_HOTPIXEL_0046b4e0[12] = "HOTPIXEL";                  // 0x0046b4e0
char s_MAXMICE_0046b4ec[8] = "MAXMICE";                     // 0x0046b4ec
char s_AUDIO_0046b4f4[8] = "AUDIO";                         // 0x0046b4f4
char s_GameDirectory_0046b600[14] = "GameDirectory";        // 0x0046b600
char s_COPY_TO_HD_0046b610[12] = "COPY_TO_HD";              // 0x0046b610
char s_GAMESTATE_INFO_0046b67c[16] = "GAMESTATE_INFO";      // 0x0046b67c
char s_GameState_0046b6b8[10] = "GameState";                // 0x0046b6b8
char s_MAXSTATES_0046b6d0[12] = "MAXSTATES";                // 0x0046b6d0
char s_illegal_message12b_0046b710[19] = "illegal message12b"; // 0x0046b710
char s_illegal_message12_0046b724[18] = "illegal message12"; // 0x0046b724
char s_illegal_message13_0046b738[18] = "illegal message13"; // 0x0046b738
char s_HD_File_Cache_0046b80c[14] = "HD File Cache";        // 0x0046b80c
char s_FSIZE_0046b8b8[6] = "FSIZE";                         // 0x0046b8b8
char s_TIME_0046b8c0[8] = "TIME";                           // 0x0046b8c0
char s_MapScene_0046b8f8[9] = "MapScene";                   // 0x0046b8f8
char s_SCI_Schedule_0046baa0[13] = "SCI_Schedule";          // 0x0046baa0
char s_EXIT_0046bab0[8] = "EXIT";                           // 0x0046bab0
char s_space_0046bad0[4] = " ";                             // 0x0046bad0
char s_Eye_Wand_Two_Believe_0046bad4[24] = " Eye Wand Two Believe"; // 0x0046bad4
char s_PUZ_WAHOO_WON_0046baec[14] = "PUZ_WAHOO_WON";        // 0x0046baec
char s_Nuts_0046bafc[8] = "Nuts";                           // 0x0046bafc
char s_Probe_0046bb04[8] = "Probe";                         // 0x0046bb04
char s_Conspiracy_0046bb0c[12] = "Conspiracy";              // 0x0046bb0c
char s_Bogie_0046bb18[8] = "Bogie";                         // 0x0046bb18
char s_Cow_0046bb20[4] = "Cow";                             // 0x0046bb20
char s_UFO_0046bb30[4] = "UFO";                             // 0x0046bb30
char s_Information_0046bb44[12] = "Information";            // 0x0046bb44
char s_Area_51_0046bb50[8] = "Area 51";                     // 0x0046bb50
char s_Sighting_0046bb64[12] = "Sighting";                  // 0x0046bb64
char s_Believe_0046bb70[8] = "Believe";                     // 0x0046bb70
char s_Abduction_0046bb78[12] = "Abduction";                // 0x0046bb78
char s_Two_0046bb84[4] = "Two";                             // 0x0046bb84
char s_Roswell_0046bb88[8] = "Roswell";                     // 0x0046bb88
char s_Wand_0046bb90[8] = "Wand";                           // 0x0046bb90
char s_Spaceship_0046bb98[12] = "Spaceship";                // 0x0046bb98
char s_Eye_0046bba4[4] = "Eye";                             // 0x0046bba4
char s_Encounter_0046bba8[12] = "Encounter";                // 0x0046bba8
char s_SC_Wahoo_0046bbb4[9] = "SC_Wahoo";                   // 0x0046bbb4
char s_WEBINVAL_SPRITE_0046bbc0[16] = "WEBINVAL_SPRITE";    // 0x0046bbc0
char s_WEBVAL_SPRITE_0046bbd0[16] = "WEBVAL_SPRITE";        // 0x0046bbd0
char s_KIDFACE_SPRITE_0046bbe0[16] = "KIDFACE_SPRITE";      // 0x0046bbe0
char s_HINT_SPRITE_0046bbf0[12] = "HINT_SPRITE";            // 0x0046bbf0
char s_PLAY_RIGHT_BRIDGE_0046bc14[18] = "PLAY_RIGHT_BRIDGE"; // 0x0046bc14
char s_CB_Bridge_0046bc28[10] = "CB_Bridge";                // 0x0046bc28
char s_COMBAT_BRIDGE_STATE_0046bc34[20] = "COMBAT_BRIDGE_STATE"; // 0x0046bc34
char s_SC_ExtBridge_0046bc48[13] = "SC_ExtBridge";          // 0x0046bc48
char s_OUTER_SPRITE_0046bc58[16] = "OUTER_SPRITE";          // 0x0046bc58
char s_MIDDLE_SPRITE_0046bc68[16] = "MIDDLE_SPRITE";        // 0x0046bc68
char s_INNER_SPRITE_0046bc78[16] = "INNER_SPRITE";          // 0x0046bc78
char s_RESET_SWITCH_SPRITE_0046bc88[20] = "RESET_SWITCH_SPRITE"; // 0x0046bc88
char s_DETECTION_MASK_0046bc9c[16] = "DETECTION_MASK";      // 0x0046bc9c
char s_GS_MUSICOFF_0046bcc4[12] = "GS_MUSICOFF";            // 0x0046bcc4
char s_SC_DuctNav_0046bcf0[11] = "SC_DuctNav";              // 0x0046bcf0
char s_SET_WORKBUFF_0046bd08[16] = "SET_WORKBUFF";          // 0x0046bd08
char s_NAVIGATION_0046bd18[12] = "NAVIGATION";              // 0x0046bd18
char s_DUCT_SWITCH_0046bd24[12] = "DUCT_SWITCH";            // 0x0046bd24
char s_NO_RESUME_NO_SAVE_0046bdac[18] = "NO_RESUME_NO_SAVE"; // 0x0046bdac
char s_SLOT_0046bde0[8] = "SLOT";                           // 0x0046bde0
char s_CHOICE_FOCUS_SPRITE_0046bde8[20] = "CHOICE_FOCUS_SPRITE"; // 0x0046bde8
char s_EDIT_FOCUS_SPRITE_0046bdfc[20] = "EDIT_FOCUS_SPRITE"; // 0x0046bdfc
char s_SCROLLDOWN_0046be10[12] = "SCROLLDOWN";              // 0x0046be10
char s_SCROLLUP_0046be1c[12] = "SCROLLUP";                  // 0x0046be1c
char s_DELETE_0046be28[8] = "DELETE";                       // 0x0046be28
char s_EDITBOX_0046be30[8] = "EDITBOX";                     // 0x0046be30
char s_LOAD_0046be38[8] = "LOAD";                           // 0x0046be38
char s_SAVE_0046be40[8] = "SAVE";                           // 0x0046be40
char s_SCI_Inventory_0046beb8[14] = "SCI_Inventory";        // 0x0046beb8
char s_AUTOCOMBINE_0046bef8[12] = "AUTOCOMBINE";            // 0x0046bef8
char s_OK_0046bf04[4] = "OK";                               // 0x0046bf04
char s_BACKGROUND_0046bf1c[12] = "BACKGROUND";              // 0x0046bf1c
char s_OBJ011_0046bf34[7] = "OBJ011";                       // 0x0046bf34
char s_EnginePodCombat_0046bf3c[16] = "EnginePodCombat";    // 0x0046bf3c
char s_CINEMATIC_0046bf4c[12] = "CINEMATIC";                // 0x0046bf4c
char s_STUDENT_PALETTE_0046bf58[16] = "STUDENT_PALETTE";    // 0x0046bf58
char s_STUDENT_SPRITE_0046bf68[16] = "STUDENT_SPRITE";      // 0x0046bf68
char s_CB_Pods_0046bf8c[8] = "CB_Pods";                     // 0x0046bf8c
char s_COMBAT_POD_RESCUED_0046bf94[19] = "COMBAT_POD_RESCUED"; // 0x0046bf94
char s_ORVILLE_ALIEN_0046bfa8[14] = "ORVILLE_ALIEN";        // 0x0046bfa8
char s_SC_Pods_0046bfe0[8] = "SC_Pods";                     // 0x0046bfe0
char s_BGSOUND_0046bfe8[8] = "BGSOUND";                     // 0x0046bfe8
char s_Target_0046c03c[7] = "Target";                       // 0x0046c03c
char s_TargetList_0046c048[11] = "TargetList";              // 0x0046c048
char s_MMPlayer_0046c05c[9] = "MMPlayer";                   // 0x0046c05c
char message_0046c0b8[17] = {
    '\t', '\t', 'E', 'N', 'D', '\t', '\t', '/', '/',
    'm', 'e', 's', 's', 'a', 'g', 'e', 0
};                                                          // 0x0046c0b8
char s_MESSAGE_INFO_0046c1d0[16] = "MESSAGE_INFO";          // 0x0046c1d0
char s_SC_Message_0046c1e0[11] = "SC_Message";              // 0x0046c1e0
char s_EXTRA2_0046c1ec[8] = "EXTRA2";                       // 0x0046c1ec
char s_EXTRA1_0046c1f4[8] = "EXTRA1";                       // 0x0046c1f4
char s_LASTKEY_0046c204[8] = "LASTKEY";                     // 0x0046c204
char s_BUTTON2_0046c20c[8] = "BUTTON2";                     // 0x0046c20c
char s_BUTTON1_0046c214[8] = "BUTTON1";                     // 0x0046c214
char s_INSTRUCTION_0046c270[12] = "INSTRUCTION";            // 0x0046c270
char s_FROM_0046c298[8] = "FROM";                           // 0x0046c298
char s_ADDRESS_0046c2e0[8] = "ADDRESS";                     // 0x0046c2e0
char s_T_Hotspot_0046c2e8[10] = "T_Hotspot";                // 0x0046c2e8
char s_LEVEL_0046c308[8] = "LEVEL";                         // 0x0046c308
char s_DANIELS_0046c310[8] = "DANIELS";                     // 0x0046c310
char s_MARSH_0046c318[8] = "MARSH";                         // 0x0046c318
char s_TUCKER_0046c320[8] = "TUCKER";                       // 0x0046c320
char s_CONTROLCAPTUREOFF_0046c328[20] = "CONTROLCAPTUREOFF"; // 0x0046c328
char s_DIALOG_0046c33c[8] = "DIALOG";                       // 0x0046c33c
char s_RIGHT_0046c344[6] = "RIGHT";                         // 0x0046c344
char s_LOUNGE_ALIEN_CINEMATIC_0046c3ac[23] = "LOUNGE_ALIEN_CINEMATIC"; // 0x0046c3ac
char s_TESTED_A_TEACHER_0046c3c4[17] = "TESTED_A_TEACHER";  // 0x0046c3c4
char s_WATER_TEST_AVAILABLE_0046c3d8[21] = "WATER_TEST_AVAILABLE"; // 0x0046c3d8
char s_CB_ShipNav_0046c410[11] = "CB_ShipNav";              // 0x0046c410
char s_windir_0046c4b0[7] = "windir";                       // 0x0046c4b0
char s_MAX_ACTIONS_0046c4e4[12] = "MAX_ACTIONS";            // 0x0046c4e4
char s_SC_Combat_0046c4f0[10] = "SC_Combat";                // 0x0046c4f0
char s_LURD_0046c4fc[8] = "LURD*";                          // 0x0046c4fc
char s_NESWUD_0046c504[8] = "NESWUD";                       // 0x0046c504
char* PTR_DAT_0046c50c = s_NESWUD_0046c504;                 // 0x0046c50c
char* PTR_DAT_0046c510 = s_LURD_0046c4fc;                   // 0x0046c510
char s_OnDir_SubNode_0046c59c[14] = "OnDir_SubNode";        // 0x0046c59c
char s_AND_ON_0046c5ac[7] = "AND_ON";                       // 0x0046c5ac
char s_PostMessage_SubNode_0046c660[20] = "PostMessage_SubNode"; // 0x0046c660
char s_FALSE_0046c684[6] = "FALSE";                         // 0x0046c684
char s_CheckMessage_SubNode_0046c68c[21] = "CheckMessage_SubNode"; // 0x0046c68c
char s_LogicFnc_SubNode_0046c6cc[17] = "LogicFnc_SubNode";  // 0x0046c6cc
char s_THEN_GOTO_0046c6fc[10] = "THEN_GOTO";                // 0x0046c6fc
char s_AUTO_MOUSE_0046c728[12] = "AUTO_MOUSE";              // 0x0046c728
char s_NO_MOUSE_0046c734[12] = "NO_MOUSE";                  // 0x0046c734
char s_NavNode_0046c74c[8] = "NavNode";                     // 0x0046c74c
char s_DO_MOUSE_0046c754[9] = "DO_MOUSE";                   // 0x0046c754
char s_DO_ONDIR_0046c760[9] = "DO_ONDIR";                   // 0x0046c760
char s_DO_BG_0046c76c[6] = "DO_BG";                         // 0x0046c76c
char s_DO_LOGICFNC_0046c774[12] = "DO_LOGICFNC";            // 0x0046c774
char s_DO_CHECKMESSAGE_0046c780[16] = "DO_CHECKMESSAGE";    // 0x0046c780
char s_DO_POSTMESSAGE_0046c790[15] = "DO_POSTMESSAGE";      // 0x0046c790
char s_DO_ANIM_0046c7a0[8] = "DO_ANIM";                     // 0x0046c7a0
char s_DO_EXIT_0046c7a8[8] = "DO_EXIT";                     // 0x0046c7a8
char s_DO_NOTHING_0046c7b0[11] = "DO_NOTHING";              // 0x0046c7b0
char s_FOR_BEARING_0046c7bc[12] = "FOR_BEARING";            // 0x0046c7bc
char s_Navigator_0046c820[10] = "Navigator";                // 0x0046c820
char s_STARTING_NODE_0046c834[14] = "STARTING_NODE";        // 0x0046c834
char s_BEARING_0046c88c[8] = "BEARING";                     // 0x0046c88c
char s_range_error_0046c974[12] = "range error";            // 0x0046c974
char s_EQUAL_0046ca1c[6] = "EQUAL";                         // 0x0046ca1c
char s_NEQUAL_0046ca24[7] = "NEQUAL";                       // 0x0046ca24
char s_FOLLOW_0046ca2c[7] = "FOLLOW";                       // 0x0046ca2c
char s_SPR_TRANS_P20_0046ca60[16] = "SPR_TRANS_P20";        // 0x0046ca60
char s_SPR_TRANS_LOOP_0046ca70[16] = "SPR_TRANS_LOOP";      // 0x0046ca70
char s_SPR_LOOP_0046ca80[12] = "SPR_LOOP";                  // 0x0046ca80
char s_SPR_TRANS_0046ca8c[12] = "SPR_TRANS";                // 0x0046ca8c
char s_Sprite_0046ca98[7] = "Sprite";                       // 0x0046ca98
char s_SET_DEFAULT_STATES_0046cabc[20] = "SET_DEFAULT_STATES"; // 0x0046cabc
char s_ONCD_0046cad0[8] = "ONCD";                           // 0x0046cad0
char s_NOGRAPHIC_0046cad8[12] = "NOGRAPHIC";                // 0x0046cad8
char s_NOAUTOWRAP_0046cae4[12] = "NOAUTOWRAP";              // 0x0046cae4
char s_LOOP_0046caf0[8] = "LOOP";                           // 0x0046caf0
char s_SCALE_0046caf8[8] = "SCALE";                         // 0x0046caf8
char s_INVERT_0046cb00[8] = "INVERT";                       // 0x0046cb00
char s_KEEPOFFSET_0046cb08[12] = "KEEPOFFSET";              // 0x0046cb08
char s_TOPLEFT_0046cb14[8] = "TOPLEFT";                     // 0x0046cb14
char s_BOTTOMLEFT_0046cb1c[12] = "BOTTOMLEFT";              // 0x0046cb1c
char s_TRANSPARENT_0046cb28[12] = "TRANSPARENT";            // 0x0046cb28
char s_SET_STATE_0046cb34[12] = "SET_STATE";                // 0x0046cb34
char s_PRIORITY_0046cb40[12] = "PRIORITY";                  // 0x0046cb40
char s_RANGE_0046cb4c[8] = "RANGE";                         // 0x0046cb4c
char s_STATES_0046cb54[8] = "STATES";                       // 0x0046cb54
char s_LOGIC_0046cb5c[8] = "LOGIC";                         // 0x0046cb5c
char s_MAXLOGIC_0046cb64[12] = "MAXLOGIC";                  // 0x0046cb64
char s_MAKENAME_0046cb70[12] = "MAKENAME";                  // 0x0046cb70
char s_HNAME_0046cb7c[8] = "HNAME";                         // 0x0046cb7c
char s_FNAME_0046cb84[8] = "FNAME";                         // 0x0046cb84
char s_ATTEMPTED_RESCUE_0046ce8c[17] = "ATTEMPTED_RESCUE";  // 0x0046ce8c
char s_MAX_NOISE_ALLOWED_RATS_0046cfe4[23] = "MAX_NOISE_ALLOWED_RATS"; // 0x0046cfe4
char s_CB_Rats_0046d020[8] = "CB_Rats";                     // 0x0046d020
char s_RAT_COMBAT_AVAILABLE_0046d028[21] = "RAT_COMBAT_AVAILABLE"; // 0x0046d028
char s_SC_Rats_0046d040[8] = "SC_Rats";                     // 0x0046d040
char DAT_0046d054[28] = "Fastgraph for Windows V1.01";      // 0x0046d054
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
Sprite* g_NavSprite_0046c514 = 0;                            // 0x0046c514 — BG_SubNode cached navigation sprite
int g_PeriodStateIdx_0046cb90 = 99;                          // 0x0046cb90
static char s_PeriodChars[] = "PSD";                         // 0x0046cb8c
char* g_PeriodCharTable_0046cb94 = s_PeriodChars;            // 0x0046cb94
int g_PuzzleSolutions_0046cc98[36] = {                        // 0x0046cc98
    1,4,6,0, 2,4,6,0, 0,5,8,1, 1,3,8,1, 2,3,8,1, 0,4,0,2, 0,4,6,2, 0,4,7,2, 0,4,8,2
};
// g_PuzzleResultIdx_0046cca4 = &g_PuzzleSolutions_0046cc98[3] (overlap, now a macro in globals.h)

// --- 0x00471xxx - 0x00473xxx ---
int g_FireAlarmField1_00472bd8 = g_FireAlarmTimerTarget_0046859c; // 0x00472bd8 - teacher-hit scold timer increment
int g_FireAlarmField2_00472bdc = g_FireAlarmTimerTarget_0046859c; // 0x00472bdc - plane wall-hit scold timer increment
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
PCMWAVEFORMAT g_PcmWaveFormat_00472dc8;                      // 0x00472dc8
int g_DevelopFlag_00472de4 = 0;                              // 0x00472de4
char g_LogEnabled_00472e28 = 0;                              // 0x00472e28
int g_StartBlock_00472e2c = 0;                               // 0x00472e2c
char g_StringTable_00472e30[640] = {0};                      // 0x00472e30 — 20 command-line string slots
int g_ProjectileHits_0047325c = 0;                           // 0x0047325c
HitBounds g_HitBounds_00473260[3] = {{0, 0}, {0, 0}, {0, 0}}; // 0x00473260
BombData g_BombData_00473278[6];                             // 0x00473278 (BSS, initialized by CRT startup)
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
int g_PodResults_004734a0[3] = {0, 0, 0};                    // 0x004734a0-0x004734a8

// --- Misc (not address-mapped or shared) ---
short _param_3 = 0;                                          // no original address: Sound.obj shim
MessageQueue* g_MessageQueue = 0;                            // no original address: Handler8 message queue
void* g_GameStruct2 = 0;                                     // no original address: shared shim
