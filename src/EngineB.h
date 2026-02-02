#ifndef ENGINEB_H
#define ENGINEB_H

#include "Engine.h"
#include "GlyphRect.h"
#include "Range.h"

class Animation;
class Sample;

struct MeterPos {
  int x;
  int y;
  MeterPos() { x = 0; y = 0; }
  ~MeterPos() { }
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
  Sample* m_missSound;             // 0xec - "audio\\slingmis.wav"
  int field_0xf0;                 // 0xf0
  int m_prevHitCount;             // 0xf4 - tracks g_ScoreManager[5]
  int field_0xf8;                 // 0xf8
  int m_prevMissCount;            // 0xfc - tracks g_ScoreManager[3]
  Sample* m_completionSound;      // 0x100 - "audio\\ldu013_1.wav"
  Sample* m_ambientSound;         // 0x104 - "audio\\ldu005_1.wav"
  int field_0x108;                // 0x108
  int field_0x10c;                // 0x10c
  Sample* m_hitSound1;            // 0x110 - "audio\\ldu008_1.wav" (hit reaction, rand%3)
  Sample* m_hitSound2;            // 0x114 - "audio\\ldu009_1.wav" (hit reaction, rand%3)
  Sample* m_hitSound3;            // 0x118 - "audio\\ldu007_2.wav" (hit reaction, rand%3)
  Sample* m_tauntSound1;          // 0x11c - "audio\\ldu010_1.wav" (idle taunt, rand%2)
  Sample* m_tauntSound2;          // 0x120 - "audio\\ldu011_1.wav" (idle taunt, rand%2)
  Sample* m_milestoneSound;       // 0x124 - "audio\\ldu006_1.wav" (played at progress 0x13/0x25)
  Animation* m_meterAnimation;    // 0x128 - "rat1\\nmeter.smk"
  int m_meterBuffer;              // 0x12c - VBuffer* for meter blitting
  GlyphRect m_meterEmptyRect;     // 0x130-0x13f - source rect for empty meter
  GlyphRect m_meterFullRect;      // 0x140-0x14f - source rect for full meter
  IntPair m_progress;             // 0x150-0x157 - {current, max}
  MeterPos m_meterPosition;        // 0x158-0x15f - {x, y} destination
  Parser* m_weaponParser;         // 0x160 - weapon/parser pointer
  int* m_targetConfig;            // 0x164 - allocated config object

  EngineB();
  virtual ~EngineB();

  // Virtual method overrides
  virtual int LBLParse(char* line);       // vtable[0] 0x4129a0
  virtual void OnProcessEnd();            // vtable[2] 0x412690
  virtual void ProcessTargets();          // vtable[11] 0x412610
  virtual void Draw();                    // vtable[12] 0x412300
  virtual void UpdateMeter();             // vtable[13] 0x412490
  virtual void PlayCompletionSound();     // vtable[17] 0x412640
};

#endif // ENGINEB_H
