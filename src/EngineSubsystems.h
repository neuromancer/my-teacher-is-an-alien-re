#ifndef ENGINE_SUBSYSTEMS_H
#define ENGINE_SUBSYSTEMS_H

#include "Parser.h"

// Stub classes for Engine subsystems (sizes match original)

class EngineInfoParser : public Parser {
public:
  int anchorX;      // 0x88
  int anchorY;      // 0x8c
  int width;        // 0x90
  int height;       // 0x94
  int paletteStart; // 0x98
  int paletteEnd;   // 0x9c
  int field_0xa0;   // 0xa0
  int field_0xa4;   // 0xa4
  int field_0xa8;   // 0xa8
  int field_0xac;   // 0xac
  
  EngineInfoParser(); // 0x416BD0
  virtual ~EngineInfoParser(); // 0x416CD0
  virtual int LBLParse(char* line); // 0x416D70

  void ParseAnchor(char* line); // 0x416FD0
  void ParseDimensions(char* line); // 0x417000
  void FUN_00416F70(char* line, int arg2); // 0x416F70
  void FUN_00417130(char* line); // 0x417130
  void FUN_00417030(char* line, int index); // 0x417030
  void FUN_00417170(char* line); // 0x417170
};

class Viewport {
public:
    int x1, y1, x2, y2; // 0x00, 0x04, 0x08, 0x0c
    int width, height;  // 0x10, 0x14
    int field_0x18;
    int field_0x1c;
    int field_0x20;
    int field_0x24;
    int field_0x28;

    void SetDimensions(int w, int h); // 0x412C30
    void SetAnchor(int x, int y);     // 0x412C50
    void SetDimensions2(int w, int h); // 0x412C90
    void SetCenter();                 // 0x412CB0
    void FUN_00412ce0(int x);         // 0x412CE0
    void FUN_00412d00(int y);         // 0x412D00
};

class SceneManager {
public:
  char data[0x1c8];
  SceneManager() {}
};

class SpriteManager {
public:
  char data[0xb8];
  SpriteManager() {}
};

class DialogManager {
public:
  char data[0x2c];
  DialogManager() {}
};

class InputHandler {
public:
  char data[0x98];
  InputHandler() {}
};

class CursorManager {
public:
  char data[0x24];
  CursorManager() {}
};

class StateManager {
public:
  char data[0x4];
  StateManager() {}
};

#endif // ENGINE_SUBSYSTEMS_H
