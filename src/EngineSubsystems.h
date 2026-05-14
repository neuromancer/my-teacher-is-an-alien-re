#ifndef ENGINE_SUBSYSTEMS_H
#define ENGINE_SUBSYSTEMS_H

#include "Parser.h"
#include "HashTable.h"
#include "Sprite.h"
#include "SpriteAction.h"
#include "GlyphRect.h"
#include "Sample.h"
#include "Target.h"
#include "Viewport.h"
#include "CursorState.h"
#include "GameOutcome.h"

// EngineInfoParser - Full game version
// Constructor: 0x434660, Destructor: 0x434740
// Vtable: 0x4616E8
// Parser is 0x90 bytes in full game, so EngineInfoParser fields start at 0x90
class EngineInfoParser : public Parser {
public:
  SlimeDim anchor;          // 0x90-0x97: parsed by 'A' (x, y position)
  SlimeDim dimensions;      // 0x98-0x9F: parsed by 'V' (width, height)
  SlimeDim paletteStart;    // 0xA0-0xA7: parsed by 'P' (start x, y)
  SlimeDim paletteEnd;      // 0xA8-0xAF: parsed by 'Q' (end x, y)
  int scrollSpeed;          // 0xB0: parsed by 'C'
  int animSpeed;            // 0xB4: parsed by 'D'
  int field_0xB8;           // 0xB8
  int field_0xBC;           // 0xBC

  EngineInfoParser();                       // 0x434660
  virtual ~EngineInfoParser();              // 0x434740
  virtual void OnProcessEnd();              // 0x434800
  virtual int LBLParse(char* line);         // 0x434820

  void ParseOffset(char* line, int arg2);   // 0x434A00
  void ParseAnchor(char* line);             // 0x434A60
  void ParseDimensions(char* line);         // 0x434A90
  void ParseSound(char* line, int index);   // 0x434AC0
  void ParsePalette(char* line);            // 0x434BD0
};

#endif // ENGINE_SUBSYSTEMS_H
