#ifndef ENGINE_H
#define ENGINE_H

#include "Parser.h"
#include "SpriteAction.h"

class Sprite;
class Sample;
class Palette;
class SlimeTable;

// Engine - Base class for combat engine handlers (full game)
// vtable: 0x461A90, size: 0x118
// Extends Parser (0x90)
//
// vtable layout (13 entries):
//   [0]  LBLParse          [1]  OnProcessStart   [2]  OnProcessEnd
//   [3]  destructor         [4]  Init             [5]  AddMessage
//   [6]  ShutDown           [7]  Update           [8]  Exit
//   [9]  CheckNavState      [10] LogHandler       [11] ProcessTargets
//   [12] ProcessAction
class Engine : public Parser {
public:
  int handlerId;             // 0x90 — engine/handler type ID
  int moduleParam;           // 0x94 — module parameter
  int savedCommand;          // 0x98 — saved from Init message
  int savedMsgData;          // 0x9C — saved from Init message
  int field_A0;              // 0xA0
  int field_A4;              // 0xA4
  int* actionArray;          // 0xA8 — allocated action ID array
  int actionIndex;           // 0xAC — current action loop index
  int field_B0;              // 0xB0
  int actionCount;           // 0xB4 — MAX_ACTIONS value
  SlimeDim videoDimensions;  // 0xB8 — video resolution (width, height)
  int field_C0;              // 0xC0
  int field_C4;              // 0xC4
  int field_C8;              // 0xC8
  int field_CC;              // 0xCC
  int field_D0;              // 0xD0
  int field_D4;              // 0xD4
  int field_D8;              // 0xD8
  int field_DC;              // 0xDC
  Sample* backgroundSample;  // 0xE0
  int field_E4;              // 0xE4
  int field_E8;              // 0xE8
  int field_EC;              // 0xEC
  int field_F0;              // 0xF0
  int field_F4;              // 0xF4
  int field_F8;              // 0xF8
  int field_FC;              // 0xFC
  SpriteAction* pendingAction; // 0x100 — queued action for ProcessTargets
  Palette* palette;          // 0x104 — scene palette
  Sprite* bgSprite;          // 0x108 — background sprite
  Sprite* consoleSprite;     // 0x10C — console/UI sprite
  SlimeTable* soundTable;    // 0x110 — indexed sound table
  int bgSoundHandle;         // 0x114 — background sound ID

  Engine();                                         // 0x449100
  virtual ~Engine();                                // 0x4491E0

  virtual int LBLParse(char* line);                 // [0]  0x449600
  virtual void OnProcessStart();                    // [1]
  virtual void OnProcessEnd();                      // [2]  0x449520
  virtual void Init(int* param);                    // [4]  0x449260
  virtual int AddMessage(int* param);               // [5]  0x449400
  virtual void ShutDown(int flag);                  // [6]  0x449320
  virtual void Update(int p1, int p2);              // [7]  0x449480
  virtual int Exit(int* param);                     // [8]  0x449410
  virtual void CheckNavState(int* param);           // [9]  0x405D60
  virtual void LogHandler(int param);               // [10] 0x401160
  virtual void ProcessTargets();                    // [11] 0x4494E0
  virtual void ProcessAction(int index, int* action); // [12] 0x4491B0

  void StopAndCleanup();                            // 0x42BF00
};

#endif // ENGINE_H
