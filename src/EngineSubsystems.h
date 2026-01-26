#ifndef ENGINE_SUBSYSTEMS_H
#define ENGINE_SUBSYSTEMS_H

#include "Parser.h"
#include "HashTable.h"
#include "Sprite.h"

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

  void ParseOffset(char* line, int arg2); // 0x416F70
  void ParseAnchor(char* line); // 0x416FD0
  void ParseDimensions(char* line); // 0x417000
  void ParseSound(char* line, int index); // 0x417030
  void ParsePalette(char* line); // 0x417130
  void ParseText(char* line); // 0x417170
};

class Viewport {
public:
    int x1, y1, x2, y2; // 0x00, 0x04, 0x08, 0x0c
    int width, height;  // 0x10, 0x14
    int centerX;        // 0x18
    int centerY;        // 0x1c
    int scrollX;        // 0x20
    int scrollY;        // 0x24
    int anchorOffsetY;  // 0x28

    void SetDimensions(int w, int h); // 0x412C30
    void SetAnchor(int x, int y);     // 0x412C50
    void SetDimensions2(int w, int h); // 0x412C90
    void SetCenter();                 // 0x412CB0
    void SetCenterX(int x);           // 0x412CE0
    void SetCenterY(int y);           // 0x412D00
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

class Target : public Sprite {
public:
    // Target-specific fields (0xD8 - 0x157)
    // Sprite ends at 0xD8, so Target fields start here
    int field_0xd8;       // 0xd8
    int field_0xdc;       // 0xdc - flags
    char* field_0xe0;     // 0xe0 - string pointer (INIT)
    char* field_0xe4;     // 0xe4 - string pointer (I case)
    int id;               // 0xe8
    int field_0xec;       // 0xec
    int field_0xf0;       // 0xf0
    int field_0xf4;       // 0xf4
    int field_0xf8;       // 0xf8
    int field_0xfc;       // 0xfc
    int field_0x100;      // 0x100
    int field_0x104;      // 0x104
    int field_0x108;      // 0x108
    int field_0x10c;      // 0x10c
    int field_0x110;      // 0x110
    int field_0x114;      // 0x114
    int field_0x118;      // 0x118
    int field_0x11c;      // 0x11c
    int field_0x120;      // 0x120
    int field_0x124;      // 0x124
    int field_0x128;      // 0x128
    void* field_0x12c;    // 0x12c - list structure
    void* field_0x130;    // 0x130
    int field_0x134;      // 0x134
    int field_0x138;      // 0x138
    int field_0x13c;      // 0x13c
    int field_0x140;      // 0x140
    int field_0x144;      // 0x144
    int field_0x148;      // 0x148
    int field_0x14c;      // 0x14c
    int field_0x150;      // 0x150
    int field_0x154;      // 0x154

    Target(); // 0x413DC0
    virtual ~Target(); // 0x413F10
    virtual int LBLParse(char* line); // 0x414930

    void Deactivate(); // 0x414230
    int CheckTimeInRange(); // 0x4142C0
    int CheckTimeInRangeParam(int* coords); // 0x414310
    int AdvanceHotspot(); // 0x414350
    void UpdateProgress(int delta); // 0x4143B0
    int Update(); // 0x414410

    void Init(char* line); // 0x4145E0
    void ParseSound(char* line); // 0x414690
};

class TargetList : public Parser {
public:
    int count; // 0x88
    Target* targets[73]; // 0x8c
    HashTable* hashTable; // 0x1B0
    int field_0x1B4[5]; // 0x1B4-0x1C7

    TargetList(); // 0x414D80
    virtual ~TargetList(); // 0x414DF0
    virtual int LBLParse(char* line); // 0x414FD0
    virtual void OnProcessEnd(); // 0x414F30
};

#endif // ENGINE_SUBSYSTEMS_H
