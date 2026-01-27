#ifndef ENGINE_SUBSYSTEMS_H
#define ENGINE_SUBSYSTEMS_H

#include "Parser.h"
#include "HashTable.h"
#include "Sprite.h"
#include "GlyphRect.h"
#include "Sample.h"

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
    int active;           // 0xd8 - 0=inactive, 1=active, 3=hit
    int targetFlags;      // 0xdc - bit 0: use hit offset position
    char* animFilename;   // 0xe0 - animation filename from INIT
    char* identifier;     // 0xe4 - identifier string from 'I' label
    int id;               // 0xe8
    int field_0xec;       // 0xec
    int endAnimState;     // 0xf0 - last animation state (from 'B' label)
    int hitAnimOffset;    // 0xf4 - offset to hit animation state (from 'K' label)
    int hitEndState;      // 0xf8 - last hit animation state (from 'K' label)
    int timeMin;          // 0xfc - time range minimum (from 'P' label)
    int timeMax;          // 0x100 - time range maximum (from 'P' label)
    int progress;         // 0x104 - accumulated progress
    int progressMax;      // 0x108 - progress threshold (from 'C' label)
    int scoreIndex;       // 0x10c - score index (from 'V' label)
    int weight;           // 0x110 - weight value (from 'W' label)
    int hitPoints;        // 0x114 - points awarded on hit (from 'D' label)
    int missPoints;       // 0x118 - points deducted on miss
    int combatBonus1;     // 0x11c - added to combat engine field_0xb4
    int field_0x120;      // 0x120
    int combatBonus2;     // 0x124 - added to combat engine field_0xc4
    int field_0x128;      // 0x128
    void* hotspotList;    // 0x12c - hotspot list structure
    Sample* stopSound;    // 0x130 - sound stopped on hit
    Sample* progressSound; // 0x134 - sound played on progress
    Sample* hitSound;     // 0x138 - sound played on hit
    Sample* sound3;       // 0x13c - sound 3
    int animParam1;       // 0x140 - from 'A' label
    int animParam2;       // 0x144 - from 'A' label
    int hitX;             // 0x148 - hit offset X (from 'O' label)
    int hitY;             // 0x14c - hit offset Y (from 'O' label)
    int pendingAction;    // 0x150 - pending action (0=none, 1=miss, 3=hit)
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
    Target* targets[70]; // 0x8c - 0x1a3
    Target* currentTarget; // 0x1a4 (was targets[70])
    int field_0x1a8;  // 0x1a8
    int field_0x1ac;  // 0x1ac
    HashTable* hashTable; // 0x1B0
    int field_0x1b4;  // 0x1b4
    int field_0x1b8;  // 0x1b8
    int field_0x1bc;  // 0x1bc
    void* field_0x1c0; // 0x1c0 - some object pointer

    TargetList(); // 0x414D80
    virtual ~TargetList(); // 0x414DF0
    virtual int LBLParse(char* line); // 0x414FD0
    virtual void OnProcessEnd(); // 0x414F30

    Target* ProcessTargets(); // 0x4150F0
};

#endif // ENGINE_SUBSYSTEMS_H
