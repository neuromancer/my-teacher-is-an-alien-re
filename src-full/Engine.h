#ifndef ENGINE_H
#define ENGINE_H

#include "Parser.h"
#include "SpriteAction.h"

class Sprite;
class SoundList;
class Sample;
class Palette;

// Engine - Base class for game engine variants (vtable 0x461A90, size 0x118)
//
// Class hierarchy:
//   Parser (0x90 bytes)
//     └── Engine (base, vtable 0x461A90, size 0x118)
//
// vtable layout (13 entries):
//   [0] LBLParse           [1] OnProcessStart    [2] OnProcessEnd     [3] destructor
//   [4] Initialize         [5] CleanupSubsystems [6] VirtCleanup      [7] ProcessActions
//   [8] HandleInput        [9] CheckNavState     [10] fn10            [11] ProcessTargets
//   [12] fn12
class Engine : public Parser {
public:
  // Parser fields end at 0x90
  // Engine fields start at 0x90

  int field_0x90;        // 0x90 - engine type ID
  int field_0x94;        // 0x94
  int field_0x98;        // 0x98
  int field_0x9C;        // 0x9C
  int field_0xA0;        // 0xA0
  int field_0xA4;        // 0xA4
  int* field_0xA8;       // 0xA8 - action array (allocated, freed in VirtCleanup)
  int field_0xAC;        // 0xAC - action loop counter
  int field_0xB0;        // 0xB0
  int m_combatBonus1;    // 0xB4 - (demo: combatBonus1, full: action count)
  SlimeDim field_0xB8;   // 0xB8 - SlimeDim subobject (dtor 0x401120)
  int m_scrollOffsetX;   // 0xC0
  int m_combatBonus2;    // 0xC4
  int m_scrollOffsetY;   // 0xC8
  int field_0xCC;        // 0xCC
  int m_viewOffset1X;    // 0xD0
  int m_framesA;         // 0xD4
  int m_viewOffset1Y;    // 0xD8
  int m_framesL;         // 0xDC
  Sample* m_backgroundSample; // 0xE0
  int field_0xE4;        // 0xE4
  int field_0xE8;        // 0xE8
  int field_0xEC;        // 0xEC
  int field_0xF0;        // 0xF0
  int field_0xF4;        // 0xF4
  int field_0xF8;        // 0xF8
  int field_0xFC;        // 0xFC
  SpriteAction* field_0x100; // 0x100 - SpriteAction (0x444AF0 dtor)
  Palette* field_0x104;  // 0x104 - palette ptr (0x41DC10 dtor)
  Sprite* field_0x108;   // 0x108 - background sprite ptr (0x44C740 dtor)
  Sprite* field_0x10C;   // 0x10C - console sprite ptr (0x44C740 dtor)
  SoundList* field_0x110; // 0x110 - sound list ptr (0x425490 dtor)
  int field_0x114;       // 0x114

  Engine();

  // Full game virtual methods (vtable 0x461A90, entries 0-12)
  virtual int LBLParse(char* line);           // [0] 0x449600
  virtual void OnProcessStart();              // [1] 0x401140
  virtual void OnProcessEnd();                // [2] 0x449520
  virtual ~Engine();                          // [3] 0x4491C0/0x4491E0
  virtual void Initialize(int* param);        // [4] 0x449260
  virtual int CleanupSubsystems(int* param);  // [5] 0x449400
  virtual void VirtCleanup(int flag);         // [6] 0x449320
  virtual void ProcessActions(int p1, int p2);// [7] 0x449480
  virtual int HandleInput(int* param);        // [8] 0x449410
  virtual int CheckNavState(int* param);      // [9] 0x405D60
  virtual void fn10(int param);               // [10] 0x401160
  virtual void ProcessTargets();              // [11] 0x4494E0
  virtual void fn12(int index, int* action);  // [12] 0x4491B0

  // Demo-compat virtual stubs (not in full game vtable, needed for EngineB)
  virtual void Draw();
  virtual void UpdateMeter();
  virtual int UpdateAndCheck();
  virtual void CopyToGlobals();
  virtual void ClearGlobals();
  virtual void PlayCompletionSound();

  // Non-virtual methods
  void StopAndCleanup();                      // 0x42BF00
  void SetupViewport();
};

#endif // ENGINE_H
