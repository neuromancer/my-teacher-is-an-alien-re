#ifndef ENGINE_H
#define ENGINE_H

#include "Parser.h"

// Forward declarations for Engine subsystems
class mCNavigator;
class SoundList;
class Palette;
class EngineInfoParser;
class TargetList;
class CombatSprite;
class Viewport;
class CursorState;
class GameOutcome;

// Engine - Base class for game engine variants (vtable 0x431340, size 0xe8)
//
// Class hierarchy:
//   Parser
//     └── Engine (base, vtable 0x431340, size 0xe8)
//           ├── EngineA (exploration, vtable 0x431410, size 0xe8)
//           └── EngineB (combat, vtable 0x4313c0, size 0x168)
//
// vtable layout (18 entries):
//   [0] LBLParse        [1] OnProcessStart   [2] OnProcessEnd    [3] destructor
//   [4] Initialize      [5] CleanupSubsystems [6] VirtCleanup    [7] DisplayFrameRate
//   [8] VirtUpdate      [9] VirtCheck1       [10] VirtCheck2     [11] VirtProcess
//   [12] VirtDraw       [13] Virt13          [14] UpdateAndCheck [15] Virt15
//   [16] Virt16         [17] Virt17
class Engine : public Parser {
public:
  // Parser fields end at 0x88
  // Engine fields start at 0x88

  TargetList* m_targetList;         // 0x88 - target list manager (size 0x1c8)
  EngineInfoParser* m_engineInfoParser; // 0x8c - engine info parser (size 0xb8)
  int field_0x90;                   // 0x90
  Viewport* m_viewport;             // 0x94 - viewport (size 0x2c)
  CombatSprite* m_combatSprite;     // 0x98 - combat sprite manager (size 0x98)
  Parser* m_subParser;              // 0x9c - sub-parser object (can be Console, Weapon, etc)
  Palette* m_timerManager;          // 0xa0 - timer manager (size 0x8)
  SoundList* m_soundList;           // 0xa4 - sound list (size 0x10)
  CursorState* m_cursorState;       // 0xa8 - cursor state data (size 0x24)
  mCNavigator* m_navigator;         // 0xac - navigation manager
  GameOutcome* m_stateManager;     // 0xb0 - state manager (size 0x4)
  int field_0xb4;             // 0xb4
  int field_0xb8;             // 0xb8
  int field_0xbc;             // 0xbc
  int field_0xc0;             // 0xc0
  int field_0xc4;             // 0xc4
  int field_0xc8;             // 0xc8
  int field_0xcc;             // 0xcc
  int field_0xd0;             // 0xd0
  int m_framesA;              // 0xd4 - frame counter A
  int field_0xd8;             // 0xd8
  int m_framesL;              // 0xdc - frame counter L
  int field_0xe0;             // 0xe0
  int field_0xe4;             // 0xe4

  Engine();

  // Virtual function overrides from Parser (vtable[0-2])
  virtual int LBLParse(char* line);
  virtual void OnProcessStart();
  virtual void OnProcessEnd();

  // Engine virtual destructor (vtable[3])
  virtual ~Engine();

  // Engine virtual methods (vtable[4-17])
  virtual void Initialize();          // vtable[4] 0x411550
  virtual void CleanupSubsystems();   // vtable[5] 0x411a40
  virtual void VirtCleanup();         // vtable[6] 0x411540 - called by StopAndCleanup
  virtual void DisplayFrameRate();    // vtable[7] 0x411d60
  virtual void VirtUpdate();          // vtable[8] 0x411460
  virtual int VirtCheck1();           // vtable[9] 0x411dd0
  virtual int VirtCheck2();           // vtable[10] 0x411440
  virtual void ProcessTargets();      // vtable[11] 0x4113a0
  virtual void Draw();                // vtable[12] 0x411510
  virtual void UpdateMeter();         // vtable[13] 0x411190
  virtual int UpdateAndCheck();       // vtable[14] 0x411340
  virtual void Virt15();              // vtable[15] 0x411ca0
  virtual void Virt16();              // vtable[16] 0x411d20
  virtual void PlayCompletionSound(); // vtable[17] 0x4111a0

  // Non-virtual methods
  void StopAndCleanup();  // 0x411320
  void SetupViewport();   // 0x411230
};

#endif // ENGINE_H
