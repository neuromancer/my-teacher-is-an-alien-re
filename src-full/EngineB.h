#ifndef ENGINEB_H
#define ENGINEB_H

#include "SC_CombatBase.h"
#include "GlyphRect.h"
#include "Range.h"

class Animation;
class Sample;
class SoundList;

struct MeterPos {
  int x;
  int y;
  MeterPos() {
    memset(this, 0, sizeof(MeterPos));
  }
  ~MeterPos() { }
};

// EngineB inherits from SC_CombatBase (full game vtable 0x461c90, size 0x178)
// Used for combat/action scenes (COMBAT1.MIS)
//
// Vtable layout (18 entries, SC_CombatBase base):
//   [0]  LBLParse           0x451690 (OVERRIDE)
//   [1]  OnProcessStart     0x401140 (inherited)
//   [2]  OnProcessEnd       0x451230 (OVERRIDE)
//   [3]  ~EngineB sdtor     0x450BF0 (OVERRIDE)
//   [4]  Initialize         0x42C240 (inherited)
//   [5]  CleanupAll         0x42C630 (inherited)
//   [6]  ResetState         0x42C230 (inherited)
//   [7]  ProcessInput       0x42C960 (inherited)
//   [8]  BeginFrame            0x42C070 (inherited)
//   [9]  UpdateSprites            0x42C050 (inherited)
//   [10] ProcessFrame           0x451180 (OVERRIDE - ProcessTargets)
//   [11] RenderBackground           0x450DB0 (OVERRIDE - Draw)
//   [12] PostRender           0x450F70 (OVERRIDE - UpdateMeter)
//   [13] HandleAction       0x4511C0 (OVERRIDE)
//   [14] StopAndCleanup     0x42BF20 (inherited)
//   [15] SetupViewport      0x42C8A0 (inherited)
//   [16] RenderState        0x42C920 (inherited)
//   [17] UpdateAndCheck     0x40BC90 (inherited)
class EngineB : public SC_CombatBase {
public:
  // EngineB-specific fields (SC_CombatBase ends at 0xF0)
  int* m_targetConfig;            // 0xF0 - allocated config object [0]=base, [1]=points
  int m_prevHitCount;             // 0xF4 - tracks score manager hit count
  int field_0xF8;                 // 0xF8
  int m_prevMissCount;            // 0xFC - tracks score manager miss count
  SoundList* m_localSoundList;    // 0x100 - local SoundList for combat sounds
  Sample* m_missSound;            // 0x104 - "cb_rats\\snd3012"
  int field_0x108;                // 0x108
  int field_0x10C;                // 0x10C
  Sample* field_0x110;            // 0x110 - "cb_rats\\snd5010"
  Sample* m_completionSound;      // 0x114 - "cb_rats\\snd5009"
  Sample* m_ambientSound;         // 0x118 - "cb_rats\\snd5001"
  int field_0x11C;                // 0x11C
  int field_0x120;                // 0x120
  Sample* m_hitSound1;            // 0x124 - "cb_rats\\snd5004" (hit reaction, rand%3)
  Sample* m_hitSound2;            // 0x128 - "cb_rats\\snd5005" (hit reaction, rand%3)
  Sample* m_hitSound3;            // 0x12C - "cb_rats\\snd5006" (hit reaction, rand%3)
  Sample* m_tauntSound1;          // 0x130 - "cb_rats\\snd5007" (idle taunt, rand%2)
  Sample* m_tauntSound2;          // 0x134 - "cb_rats\\snd5008" (idle taunt, rand%2)
  Sample* m_milestoneSound;       // 0x138 - "cb_rats\\snd5002" (progress milestone)
  Animation* m_meterAnimation;    // 0x13C - "combats\\nmeter.smk"
  int m_meterBuffer;              // 0x140 - VBuffer* for meter blitting
  GlyphRect m_meterEmptyRect;     // 0x144-0x153 - source rect for empty meter
  GlyphRect m_meterFullRect;      // 0x154-0x163 - source rect for full meter
  IntPair m_progress;             // 0x164-0x16B - {current, max}
  MeterPos m_meterPosition;       // 0x16C-0x173 - {x, y} destination
  int m_weaponParser;             // 0x174 - weapon/parser pointer

  EngineB();
  virtual ~EngineB();

  // Virtual method overrides
  virtual int LBLParse(char* line);       // [0] 0x451690
  virtual void OnProcessEnd();            // [2] 0x451230
  virtual void ProcessFrame();               // [10] 0x451180 - ProcessTargets
  virtual void RenderBackground();               // [11] 0x450DB0 - Draw
  virtual int PostRender();                // [12] 0x450F70 - UpdateMeter
  virtual int HandleAction(int* param);  // [13] 0x4511C0
};

#endif // ENGINEB_H
