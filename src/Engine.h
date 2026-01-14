#ifndef ENGINE_H
#define ENGINE_H

#include "Parser.h"

// Forward declarations for Engine subsystems
class mCNavigator;
class SoundList;

class Engine : public Parser {
public:
  // Parser fields end at 0x88
  // Engine fields start at 0x88

  void* m_sceneManager;       // 0x88 - scene/level manager (size 0x1c8)
  void* m_spriteManager;      // 0x8c - sprite manager (size 0xb8)
  int field_0x90;             // 0x90
  void* m_dialogManager;      // 0x94 - dialog/text manager (size 0x2c)
  void* m_inputHandler;       // 0x98 - input handler (size 0x98)
  Parser* m_subParser;        // 0x9c - sub-parser object (size 0xa8)
  void* m_timerManager;       // 0xa0 - timer manager (size 0x8)
  SoundList* m_soundList;     // 0xa4 - sound list (size 0x10)
  void* m_cursorManager;      // 0xa8 - cursor manager (size 0x24)
  mCNavigator* m_navigator;   // 0xac - navigation manager
  void* m_stateManager;       // 0xb0 - state manager (size 0x4)
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

  // Virtual function overrides
  int LBLParse(char* line);
  void OnProcessStart();
  void OnProcessEnd();

  void DisplayFrameRate();
  void Initialize();
};

#endif // ENGINE_H
