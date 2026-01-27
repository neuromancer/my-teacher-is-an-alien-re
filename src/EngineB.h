#ifndef ENGINEB_H
#define ENGINEB_H

#include "Engine.h"
#include "GlyphRect.h"

class Animation;

struct IntPair {
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
  SoundList* m_localSoundList;     // 0xe8 - local SoundList for combat sounds
  int m_missSound;                 // 0xec - Sample* "audio\\slingmis.wav"
  int field_0xf0[4];              // 0xf0-0xff
  int m_completionSound;          // 0x100 - Sample* "audio\\ldu013_1.wav"
  int m_ambientSound;             // 0x104 - Sample* "audio\\ldu005_1.wav"
  int field_0x108[4];             // 0x108-0x117
  int field_0x118;                // 0x118 - Sample* "audio\\ldu007_2.wav"
  int field_0x11c;                // 0x11c - Sample* "audio\\ldu010_1.wav"
  int field_0x120;                // 0x120 - Sample* "audio\\ldu011_1.wav"
  int field_0x124;                // 0x124 - Sample* "audio\\ldu006_1.wav"
  Animation* m_meterAnimation;    // 0x128 - "rat1\\nmeter.smk"
  int m_meterBuffer;              // 0x12c - VBuffer* for meter blitting
  GlyphRect m_meterEmptyRect;     // 0x130-0x13f - source rect for empty meter
  GlyphRect m_meterFullRect;      // 0x140-0x14f - source rect for full meter
  IntPair m_progress;             // 0x150-0x157 - {current, max}
  IntPair m_meterPosition;        // 0x158-0x15f - {x, y} destination
  int m_weaponParser;             // 0x160 - weapon/parser pointer
  int m_targetConfig;             // 0x164 - allocated config object

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
