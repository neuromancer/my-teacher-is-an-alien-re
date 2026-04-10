#ifndef SC_FIREALARM_H
#define SC_FIREALARM_H

#include "Handler.h"
#include "SpriteAction.h"
#include "InvSlotItem.h"

class Sprite;
class Palette;
class SoundList;

// SC_FireAlarm - Fire alarm puzzle handler (case 64/0x40)
// Constructor: 0x407290
// Size: 0x150
// Vtable: 0x4610C8
// References: "CB_FireAlarm.mis", "SC_FireAlarm"
class SC_FireAlarm : public Handler {
public:
    SC_FireAlarm();
    ~SC_FireAlarm();

    void Init(SC_Message* msg);
    int AddMessage(SC_Message* msg);
    int ShutDown(SC_Message* msg);
    int Exit(SC_Message* msg);
    void SendResultMessage();       // 0x4079E0
    void ResetState();              // 0x407BB0
    int HandleClick(int* param);    // 0x407C20
    void Render();                  // 0x407E50
    void ProcessFrame();            // 0x407EE0

    SpriteAction* spriteAction;       // 0xA8 - SpriteAction*
    int stateFlags;                   // 0xAC - bit flags: 1=animating, 2=alarm_pulled, 4=caught, 8=intro
    int gamePhase;                    // 0xB0 - 0=idle, 1=playing, 2=alarm_win, 3=caught_lose, 4=escape
    SlimeDim screenSize;              // 0xB4-0xBB - screen dimensions (320x240)
    Sprite* consoleSprite;            // 0xBC - CONSOLE_SPRITE
    int paletteDummy;                 // 0xC0 - Palette* (allocated as Palette, cleaned in ShutDown)
    Sprite* bgSprite;                 // 0xC4 - BACKGROUND_SPRITE
    Sprite* alarmSprite;              // 0xC8 - ALARM_SPRITE
    Rect alarmSlotRect;               // 0xCC-0xDB - alarm pull area rectangle
    Sprite* handSprite;               // 0xDC - HAND_SPRITE
    SlimeDim handIdleDelay;           // 0xE0-0xE7 - hand idle counter/target
    SlimeDim waterHitRange;           // 0xE8-0xEF - water hit color range
    SlimeDim waterHitCounter;         // 0xF0-0xF7 - water hit count/max
    Sprite* planeSprite;              // 0xF8 - PLANE_SPRITE
    SlimeDim planeClickRange;         // 0xFC-0x103 - plane click color range
    SlimeDim planeHomePos;            // 0x104-0x10B - plane home position (initial loc_x/loc_y)
    Sprite* caughtsSprite;            // 0x10C - CAUGHTS_SPRITE
    Sprite* teacherSprite;            // 0x110 - TEACHER_SPRITE
    SlimeDim bellHitRange;            // 0x114-0x11B - bell hit color range
    Rect bellSlotRect;                // 0x11C-0x12B - unused slot rect
    SlimeDim teacherHomePos;          // 0x12C-0x133 - teacher home position (initial loc_x/loc_y)
    int animDistance;                 // 0x134 - animation frame distance
    SlimeDim timerCounter;            // 0x138-0x13F - timer counter/target
    int roundCount;                   // 0x140 - number of rounds completed
    int maxRounds;                    // 0x144 - max rounds (STUDENT_HITS_ALLOWED)
    SoundList* soundList;              // 0x148 - SoundList* (was misidentified as SlimeTable/Palette)
    int bgSoundId;                    // 0x14C - BG_SOUND id
};

#endif // SC_FIREALARM_H
