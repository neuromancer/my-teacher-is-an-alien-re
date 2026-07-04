#ifndef SC_COMBATBASE_H
#define SC_COMBATBASE_H

#include "Parser.h"
#include <string.h>

class TargetList;
class Viewport;
class Palette;
class Sprite;
class Sample;
class CombatSprite;
class mCNavigator;
class SoundList;
class ScoreDisplay;
class EngineInfoParser;
class Weapon;
class TimedEventPool;

// Combat globals — declared in globals.h

// CB_Dim - ctor-only 8-byte pair, no destructor.
// Same pattern as EC_Dim in EngineC.h / FA_Dim in SC_FireAlarm.h: the
// SC_CombatBase constructor at 0x42BCD0 zero-constructs four of these via
// lea+paired stores BEFORE the vtable store, proving they are members with
// inline default constructors (MSVC 4.20 emits the vfptr store after the
// member initializers).
struct CB_Dim { int x; int y; CB_Dim() { memset(this, 0, 8); } };

// SC_CombatBase - Base class for combat/exploration engines
// Constructor: 0x42BCD0, Destructor: 0x42BDA0
// Scalar deleting destructor: 0x42BD80
// Vtable: 0x461550 (18 entries)
// Size: 0xF0
//
// EngineA, EngineB, EngineC, PodsEngine all inherit from this class.
//
// vtable layout:
//   [0]  LBLParse           0x42CB10
//   [1]  OnProcessStart     0x401140 (inherited)
//   [2]  OnProcessEnd       0x401150 (inherited)
//   [3]  ~SC_CombatBase     0x42BD80 (sdtor)
//   [4]  Initialize         0x42C240
//   [5]  CleanupAll         0x42C630
//   [6]  ResetState         0x42C230
//   [7]  ProcessInput       0x42C960
//   [8]  BeginFrame         0x42C070
//   [9]  UpdateSprites      0x42C050
//   [10] ProcessFrame       0x42BFC0
//   [11] RenderBackground   0x42C120
//   [12] PostRender         0x40BC80
//   [13] HandleAction       0x42BD70
//   [14] StopAndCleanup     0x42BF20
//   [15] SetupViewport      0x42C8A0
//   [16] RenderState        0x42C920
//   [17] UpdateAndCheck     0x40BC90
class SC_CombatBase : public Parser {
public:
    // Sub-objects (allocated in Initialize, freed in CleanupAll)
    TargetList* targetList;          // 0x90 (0x1D0 bytes)
    EngineInfoParser* weaponParser;  // 0x94 (0xC0 bytes)
    Sprite* bgSprite;                // 0x98 (background console sprite)
    Viewport* viewport;              // 0x9C (0x2C bytes)
    CombatSprite* combatSprite;      // 0xA0 (0xA0 bytes)
    Weapon* combatDisplay;           // 0xA4 (0xB0 bytes, vtable 0x4615A0)
    Palette* palette;                // 0xA8 (0x08 bytes)
    SoundList* soundList;            // 0xAC (0x10 bytes)
    ScoreDisplay* scoreDisplay;      // 0xB0 (0x24 bytes)
    mCNavigator* navigator;          // 0xB4 (0xA8 bytes)
    TimedEventPool* hotspotPool;     // 0xB8 (0x18 bytes)

    // State fields (zero-constructed CB_Dim pair members, see CB_Dim above)
    CB_Dim hotspotPos;    // 0xBC x — hotspot creation x / 0xC0 y — hotspot creation y
    CB_Dim bonusOverlay;  // 0xC4 x — combat bonus (from Target::combatBonus2) / 0xC8 y — PodsEngine overlay sprite
    CB_Dim effectPair1;   // 0xCC x — PodsEngine effect sprite 1 / 0xD0 y — effect sprite 2
    CB_Dim effectPair2;   // 0xD4 x — PodsEngine effect sprite 3 / 0xD8 y — zeroed in ctor, purpose unknown

    // Runtime state
    int combatFlags;      // 0xDC — combat state flags (bitwise OR'd)
    int frameCount;       // 0xE0 — frame counter (incremented in StopAndCleanup)
    int currentFrame;     // 0xE4
    int spriteFrameCount; // 0xE8 — sprite frame counter (passed to CombatSprite::ProcessFrame)
    Sample* backgroundSound; // 0xEC — background sound

    SC_CombatBase();
    virtual ~SC_CombatBase();

    virtual int LBLParse(char* line);       // [0] 0x42CB10
    virtual void Initialize();              // [4] 0x42C240
    virtual void CleanupAll();              // [5] 0x42C630
    virtual void ResetState();              // [6] 0x42C230
    virtual void ProcessInput();            // [7] 0x42C960
    virtual void BeginFrame();              // [8] 0x42C070
    virtual int UpdateSprites();            // [9] 0x42C050
    virtual void ProcessFrame();            // [10] 0x42BFC0
    virtual void RenderBackground();        // [11] 0x42C120
    virtual void PostRender();              // [12] 0x40BC80
    virtual int HandleAction(int* param);   // [13] 0x42BD70
    virtual int StopAndCleanup();           // [14] 0x42BF20
    virtual void SetupViewport();           // [15] 0x42C8A0
    virtual void RenderState();             // [16] 0x42C920
    virtual void UpdateAndCheck();          // [17] 0x40BC90

    void StopSoundsAndReset();              // 0x42BF00
    int ProcessEvents();                    // 0x42C9D0
};

class SpriteAction;
void __stdcall EnqueueHotspotAction(SpriteAction* param);
#endif // SC_COMBATBASE_H
