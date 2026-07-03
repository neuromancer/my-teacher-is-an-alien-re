#ifndef SC_FIREALARM_H
#define SC_FIREALARM_H

#include "Handler.h"
#include "SpriteAction.h"
#include "InvSlotItem.h"

class Sprite;
class Palette;
class SlimeTable;

// Destructor-less coordinate/rect pairs: the original SC_FireAlarm uses these
// (NOT SlimeDim/Rect) for counters and the unused teacher slot — proven by the
// destructor only unwinding 6 members (screenSize, alarmSlotRect, kidHitRange,
// planeClickRange, planeHomePos, teacherHitRange), not these.
struct FA_Dim { int x; int y; FA_Dim() { memset(this, 0, 8); } };

// SC_FireAlarm - Fire alarm puzzle handler (case 64/0x40)
// Constructor: 0x407290
// Size: 0x150
// Vtable: 0x4610C8
// References: "CB_FireAlarm.mis", "SC_FireAlarm"
class SC_FireAlarm : public Handler {
public:
    SC_FireAlarm();
    ~SC_FireAlarm();

    virtual int LBLParse(char* line);              // [0]  0x4086B0
    virtual void OnProcessEnd();                   // [2]  0x408530
    void Init(SC_MessageParser* msg);
    int AddMessage(SC_MessageParser* msg);
    void ShutDown(SC_MessageParser* msg);
    virtual void Update(int p1, int p2);           // [7]  0x4078A0
    int Exit(SC_MessageParser* msg);
    virtual int HandleClick(int* param);           // [11] 0x407C20
    virtual void ProcessClick();                   // [12] 0x407C10

    void SendResultMessage();       // 0x4079E0
    void ResetState();              // 0x407BB0
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
    FA_Dim handIdleDelay;             // 0xE0-0xE7 - hand idle counter/target (plain, no dtor)
    SlimeDim kidHitRange;             // 0xE8-0xEF - SMP_HitKid color range (B4-C1)
    FA_Dim kidHitCounter;             // 0xF0-0xF7 - kid hit count/max (plain, no dtor)
    Sprite* planeSprite;              // 0xF8 - PLANE_SPRITE
    SlimeDim planeClickRange;         // 0xFC-0x103 - plane click color range
    SlimeDim planeHomePos;            // 0x104-0x10B - plane home position (initial loc.x/loc.y)
    Sprite* caughtsSprite;            // 0x10C - CAUGHTS_SPRITE
    Sprite* teacherSprite;            // 0x110 - TEACHER_SPRITE
    SlimeDim teacherHitRange;         // 0x114-0x11B - SMP_HitTeacher color range (C2-CF)
    Rect teacherSlotRect;             // 0x11C-0x12B - original initializes this but HandleClick does not use it
    SlimeDim teacherHomePos;          // 0x12C-0x133 - teacher home position (initial loc.x/loc.y)
    int animDistance;                 // 0x134 - animation frame distance
    FA_Dim timerCounter;              // 0x138-0x13F - timer counter/target (plain, no dtor)
    int roundCount;                   // 0x140 - number of rounds completed
    int maxRounds;                    // 0x144 - max rounds (STUDENT_HITS_ALLOWED)
    SlimeTable* soundList;             // 0x148 - indexed sound table
    int bgSoundId;                    // 0x14C - BG_SOUND id
};

#endif // SC_FIREALARM_H
