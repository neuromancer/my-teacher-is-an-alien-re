#ifndef ENGINE_SUBSYSTEMS_H
#define ENGINE_SUBSYSTEMS_H

#include "Parser.h"
#include "HashTable.h"
#include "Sprite.h"
#include "GlyphRect.h"
#include "Sample.h"
#include "Target.h"
#include "Viewport.h"

// Stub classes for Engine subsystems (sizes match original)

class EngineInfoParser : public Parser {
public:
  GlyphRect anchorRect;   // 0x88: left=anchorX, top=anchorY, right=width, bottom=height
  GlyphRect paletteRect;  // 0x98: left=paletteStart, top=paletteEnd, right=field_0xa0, bottom=field_0xa4
  GlyphRect rect_0xa8;    // 0xa8: left=field_0xa8, top=field_0xac, right=field_0xb0, bottom=field_0xb4

  EngineInfoParser(); // 0x416BD0
  virtual ~EngineInfoParser(); // 0x416CD0
  virtual int LBLParse(char* line); // 0x416D70

  void ParseOffset(char* line, int arg2); // 0x416F70
  void ParseAnchor(char* line); // 0x416FD0
  void ParseDimensions(char* line); // 0x417000
  void ParseSound(char* line, int index); // 0x417030
  void ParsePalette(char* line); // 0x417130
  void ParseText(char* line); // 0x417170
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
