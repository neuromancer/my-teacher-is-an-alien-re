#ifndef ENGINEB_H
#define ENGINEB_H

#include "Engine.h"

struct SubObj16 {
  int field_0;
  int field_4;
  int field_8;
  int field_c;
};

struct SubObj8 {
  int field_0;
  int field_4;
};

// EngineB inherits from Engine (vtable 0x4313c0, size 0x168)
// Used for combat/action scenes (COMBAT1.MIS)
//
// Overrides from Engine:
//   [0] LBLParse: 0x004129a0
//   [2] OnProcessEnd: 0x00412690
//   [3] destructor: 0x004121f0
//   [11] ProcessTargets: 0x00412610
//   [12] Draw: 0x00412300
//   [13] UpdateMeter: 0x00412490
//   [17] PlayCompletionSound: 0x00412640
class EngineB : public Engine {
public:
  // EngineB-specific fields (Engine ends at 0xe8)
  void* field_0xe8;     // 0xe8 - SoundList*
  int field_0xec;       // 0xec
  int field_0xf0[4];    // 0xf0-0xff
  int field_0x100;      // 0x100
  int field_0x104;      // 0x104
  int field_0x108[4];   // 0x108-0x117
  int field_0x118;      // 0x118
  int field_0x11c;      // 0x11c
  int field_0x120;      // 0x120
  int field_0x124;      // 0x124
  void* field_0x128;    // 0x128 - pointer to object with vtable
  int field_0x12c;      // 0x12c
  SubObj16 field_0x130; // 0x130-0x13f
  SubObj16 field_0x140; // 0x140-0x14f
  SubObj8 field_0x150;  // 0x150-0x157
  SubObj8 field_0x158;  // 0x158-0x15f
  int field_0x160;      // 0x160
  int field_0x164;      // 0x164

  EngineB();
  virtual ~EngineB();

  // Virtual method overrides
  virtual int LBLParse(char* line);       // vtable[0] 0x4129a0
  virtual void OnProcessEnd();            // vtable[2] 0x412690
  virtual void ProcessTargets();          // vtable[11] 0x412610
  virtual void Draw();                    // vtable[12] 0x412300
  virtual void UpdateMeter();             // vtable[13] 0x412490
  virtual void PlayCompletionSound();     // vtable[17] 0x412640

  void DestructorHelper();  // 0x412210
};

#endif // ENGINEB_H
