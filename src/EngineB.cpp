#include "EngineB.h"
#include "EngineSubsystems.h"
#include "SoundList.h"
#include "Memory.h"
#include "Sample.h"
#include "Animation.h"
#include "VBuffer.h"
#include "TimeOut.h"
#include "Sprite.h"
#include "globals.h"
#include "InputManager.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Extern function declarations
extern void FUN_00416880(int* param);
extern char* FUN_0040d200(char* filename);
extern int* DAT_00435f28;

// Forward declaration for atexit handler
static void AtExitCleanup_0043d140();

// Static TimeOut object and initialization guard
static TimeOut g_TimeOut_0043d140;
static unsigned char g_TimeOutInitFlag_0043d14c = 0;

/* Function start: 0x412110 */
EngineB::EngineB() {
  // Engine::Engine() is called automatically via inheritance

  // Initialize SubObj16 at 0x130
  EngineB::field_0x130.field_0 = 0;
  EngineB::field_0x130.field_4 = 0;
  EngineB::field_0x130.field_8 = 0;
  EngineB::field_0x130.field_c = 0;

  // Initialize SubObj16 at 0x140
  EngineB::field_0x140.field_0 = 0;
  EngineB::field_0x140.field_4 = 0;
  EngineB::field_0x140.field_8 = 0;
  EngineB::field_0x140.field_c = 0;

  // Initialize SubObj8 at 0x150
  EngineB::field_0x150.field_0 = 0;
  EngineB::field_0x150.field_4 = 0;

  // Initialize SubObj8 at 0x158
  EngineB::field_0x158.field_0 = 0;
  EngineB::field_0x158.field_4 = 0;

  memset(&field_0xe8, 0, 0x80);
}

/* Function start: 0x4121f0 */
EngineB::~EngineB() {
  EngineB::DestructorHelper();
}

/* Function start: 0x412210 */
void EngineB::DestructorHelper() {
  // Delete field_0x128 object via its virtual destructor
  void* obj128 = EngineB::field_0x128;
  if (obj128 != 0) {
    delete obj128;
    EngineB::field_0x128 = 0;
  }

  // Delete field_0xe8 SoundList
  SoundList* soundList = (SoundList*)EngineB::field_0xe8;
  if (soundList != 0) {
    delete soundList;
    EngineB::field_0xe8 = 0;
  }
}

/* Function start: 0x4129A0 */
int EngineB::LBLParse(char* line) {
  char token[32];
  sscanf(line, "%s", token);
  if (strcmp(token, "END_ENGINEA_INFO") == 0) {
    return 1;
  }
  Engine::LBLParse(line);
  return 0;
}

/* Function start: 0x412300 */
void EngineB::Draw() {
}

/* Function start: 0x412490 */
void EngineB::UpdateMeter() {
  int progressCurrent;
  int progressMax;
  int barPos;

  if (EngineB::field_0x12c == 0) {
    return;
  }

  progressMax = EngineB::field_0x150.field_4;
  if (progressMax == 0 || EngineB::field_0x150.field_0 < progressMax) {
    // Calculate progress bar position
    progressCurrent = EngineB::field_0x150.field_0;
    barPos = (progressCurrent * 0x36) / progressMax - rand() % 3 + 1;
    if (barPos < 0) {
      barPos = 0;
    } else if (barPos > 0x36) {
      barPos = 0x36;
    }
    barPos = barPos * 4 + 2;

    // Draw first part of progress bar
    g_WorkBuffer_00436974->CallBlitter2(
      0, barPos,
      EngineB::field_0x140.field_4,
      EngineB::field_0x140.field_c,
      EngineB::field_0x158.field_0,
      EngineB::field_0x158.field_4,
      (VBuffer*)EngineB::field_0x12c);

    // Draw second part of progress bar
    g_WorkBuffer_00436974->CallBlitter2(
      barPos,
      EngineB::field_0x140.field_8,
      EngineB::field_0x130.field_4,
      EngineB::field_0x130.field_c,
      EngineB::field_0x158.field_0 + barPos,
      EngineB::field_0x158.field_4,
      (VBuffer*)EngineB::field_0x12c);
  } else {
    // Progress complete - use TimeOut
    if ((g_TimeOutInitFlag_0043d14c & 1) == 0) {
      g_TimeOutInitFlag_0043d14c |= 1;
      atexit(AtExitCleanup_0043d140);
    }

    // Draw full progress bar
    g_WorkBuffer_00436974->CallBlitter2(
      0,
      EngineB::field_0x140.field_8,
      EngineB::field_0x140.field_4,
      EngineB::field_0x140.field_c,
      EngineB::field_0x158.field_0,
      EngineB::field_0x158.field_4,
      (VBuffer*)EngineB::field_0x12c);

    // Check if TimeOut is active by comparing first dword (m_isActive) to 1
    if (*(int*)&g_TimeOut_0043d140 != 1) {
      g_TimeOut_0043d140.Start(0x5dc);  // 1500ms timeout
      return;
    }

    if (g_TimeOut_0043d140.IsTimeOut() != 0) {
      *DAT_00435f28 = 1;
      return;
    }
  }
}

/* Function start: 0x412600 */
void AtExitCleanup_0043d140() {
  g_TimeOut_0043d140.Stop();
}

/* Function start: 0x412610 */
void EngineB::ProcessTargets() {
  ((TargetList*)DAT_00435f0c)->ProcessTargets();
  FUN_00416880((int*)EngineB::field_0x160);
  EngineB::Draw();
  EngineB::UpdateMeter();
}

/* Function start: 0x412640 */
void EngineB::PlayCompletionSound() {
  if (EngineB::field_0x100 == 0) {
    return;
  }

  if (EngineB::field_0xe8 != 0) {
    ((SoundList*)EngineB::field_0xe8)->StopAll();
  }

  Sample* sampleE0 = (Sample*)EngineB::field_0xe0;
  if (sampleE0 != 0) {
    sampleE0->Fade(0x14, 0x2ee);
  }

  Sample* sample100 = (Sample*)EngineB::field_0x100;
  sample100->Play(0x64, 1);
  sample100->Stop();
}

/* Function start: 0x412690 */
void EngineB::OnProcessEnd() {
  int i;
  int numTargets;

  if (g_InputManager_00436968 != 0) {
    g_InputManager_00436968->PollEvents(1);
  }

  if (DAT_00435f04 != 0) {
    int* pTime = (int*)((char*)g_InputManager_00436968 + 0x1a0);
    int timeVal;
    if (*pTime == 0) {
      timeVal = 0;
    } else {
      timeVal = **(int**)pTime;
    }
    ((Sprite*)DAT_00435f04)->SetState2((timeVal + ((timeVal >> 31) & 0x3f)) >> 6);
  }

  // Allocate and initialize field_0x164 (8 byte object)
  int* pObj164 = (int*)AllocateMemory(8);
  if (pObj164 != 0) {
    pObj164[0] = 1;
    pObj164[1] = 3;
  } else {
    pObj164 = 0;
  }
  EngineB::field_0x164 = (int)pObj164;
  EngineB::field_0x160 = (int)DAT_00435f14;

  // Update field_0x108 on all targets
  numTargets = *(int*)((char*)DAT_00435f0c + 0x88);
  if (numTargets > 0) {
    for (i = 0; i < numTargets; i++) {
      int* pTarget = *(int**)((char*)DAT_00435f0c + 0x8c + i * 4);
      *(int*)((char*)pTarget + 0x108) = *(int*)EngineB::field_0x164;
    }
  }

  // Create SoundList at field_0xe8
  SoundList* soundListResult = 0;
  void* soundListMem = AllocateMemory(0x10);
  if (soundListMem != 0) {
    soundListResult = new SoundList(10);
  }
  EngineB::field_0xe8 = soundListResult;

  // Create Animation at field_0x128
  Animation* animResult = 0;
  void* animMem = AllocateMemory(0x2c);
  if (animMem != 0) {
    animResult = new Animation("rat1\\nmeter.smk");
  }
  EngineB::field_0x128 = animResult;

  // DoFrame on animation
  if (animResult != 0) {
    ((Animation*)animResult)->DoFrame();
  }

  // Initialize progress meter fields from animation
  Animation* anim = (Animation*)EngineB::field_0x128;
  EngineB::field_0x12c = (int)anim->smk->Height;

  // Initialize meter configuration
  EngineB::field_0x130.field_0 = 0;
  EngineB::field_0x130.field_4 = 0;
  EngineB::field_0x130.field_8 = 0xff;
  EngineB::field_0x130.field_c = 0xf;
  EngineB::field_0x140.field_0 = 0;
  EngineB::field_0x140.field_4 = 0x12;
  EngineB::field_0x140.field_8 = 0xff;
  EngineB::field_0x140.field_c = 0x21;
  EngineB::field_0x150.field_0 = 0;
  EngineB::field_0x150.field_4 = 0x36;
  EngineB::field_0x158.field_0 = 0x20;
  EngineB::field_0x158.field_4 = 0x14;

  // Register sounds if g_SoundList_00435f1c is available
  if (g_SoundList_00435f1c != 0) {
    EngineB::field_0xec = (int)g_SoundList_00435f1c->Register("audio\\slingmis.wav");
    EngineB::field_0x100 = (int)g_SoundList_00435f1c->Register("audio\\ldu013_1.wav");
  }

  // Register sounds if field_0xe8 SoundList is available
  SoundList* sList = (SoundList*)EngineB::field_0xe8;
  if (sList != 0) {
    char* filename = FUN_0040d200("audio\\ldu010_1.wav");
    EngineB::field_0x11c = (int)sList->Register(filename);
    EngineB::field_0x120 = (int)((SoundList*)EngineB::field_0xe8)->Register("audio\\ldu011_1.wav");
    EngineB::field_0x108[2] = (int)((SoundList*)EngineB::field_0xe8)->Register("audio\\ldu008_1.wav");
    EngineB::field_0x108[3] = (int)((SoundList*)EngineB::field_0xe8)->Register("audio\\ldu009_1.wav");
    EngineB::field_0x118 = (int)((SoundList*)EngineB::field_0xe8)->Register("audio\\ldu007_2.wav");
    EngineB::field_0x124 = (int)((SoundList*)EngineB::field_0xe8)->Register("audio\\ldu006_1.wav");
    Sample* sample104 = (Sample*)((SoundList*)EngineB::field_0xe8)->Register("audio\\ldu005_1.wav");
    EngineB::field_0x104 = (int)sample104;
    if (sample104 != 0) {
      sample104->Play(0x64, 1);
    }
  }
}
