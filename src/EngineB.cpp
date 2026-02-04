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
#include "RockThrower.h"
#include "GameOutcome.h"
#include "CursorState.h"
#include "string.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


// Forward declaration for atexit handler
static void AtExitCleanup_0043d140();

// Static TimeOut object and initialization guard
static TimeOut g_TimeOut_0043d140;
static unsigned char g_TimeOutInitFlag_0043d14c = 0;

/* Function start: 0x412110 */
EngineB::EngineB() {
  // Clearing explicitly to match the redundant writes in the original disassembly.
  // The small members have destructors, causing SEH state updates during construction.
  m_progress.start = 0;
  m_progress.end = 0;
  m_meterPosition.x = 0;
  m_meterPosition.y = 0;
  memset(&m_localSoundList, 0, 0x80);
}

/* Function start: 0x412210 */
EngineB::~EngineB() {
  if (m_meterAnimation != 0) {
    delete m_meterAnimation;
    m_meterAnimation = 0;
  }

  if (m_localSoundList != 0) {
    delete m_localSoundList;
    m_localSoundList = 0;
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
  if (g_ConsoleSprite == 0) {
    return;
  }

  if (EngineB::m_prevHitCount != g_ScoreManager->hitCount) {
    // Target was hit
    EngineB::m_prevHitCount = g_ScoreManager->hitCount;
    g_ConsoleSprite->SetState2(8);
    EngineB::m_progress.start += EngineB::m_targetConfig[1];

    int hitIdx = rand() % 3;
    Sample* hitSample = (&m_hitSound1)[hitIdx];
    EngineB::m_localSoundList->StopAll();

    if (hitSample != 0) {
      hitSample->Play(0x64, 1);
    }
  } else if (EngineB::m_prevMissCount != g_ScoreManager->missCount) {
    // Idle sound change
    EngineB::m_prevMissCount = g_ScoreManager->missCount;
    g_ConsoleSprite->SetState2(9);

    Sample* idleSample = (&m_tauntSound1)[rand() % 2];
    if (idleSample != 0) {
      idleSample->Play(0x64, 1);
    }
  }

  // Weapon hit effect
  int weaponHit = ((RockThrower*)EngineB::m_weaponParser)->field_0xb0;
  if (weaponHit != 0) {
    EngineB::m_progress.start += weaponHit;

    if (EngineB::m_missSound != 0) {
      EngineB::m_missSound->Play(0x64, 1);
    }

    if (EngineB::m_progress.start == 0x13 || EngineB::m_progress.start == 0x25) {
      if (EngineB::m_milestoneSound != 0) {
        EngineB::m_milestoneSound->Play(0x64, 1);
      }
    }
  }

  // Aim cursor based on mouse position
  if (g_Weapon->field_0xa0 != 0) {
    InputState* pMouse = g_InputManager_00436968->pMouse;
    int mouseX;
    if (pMouse == 0) {
      mouseX = 0;
    } else {
      mouseX = pMouse->x;
    }
    g_ConsoleSprite->SetState2(mouseX / 0x6a + 5);
  }

  // Animate sprite
  Sprite* consoleSprite = g_ConsoleSprite;
  if (consoleSprite->Do(consoleSprite->loc_x, consoleSprite->loc_y, 1.0) != 0) {
    InputState* pMouse = g_InputManager_00436968->pMouse;
    int mouseX;
    if (pMouse == 0) {
      mouseX = 0;
    } else {
      mouseX = pMouse->x;
    }
    g_ConsoleSprite->SetState2((mouseX + ((mouseX >> 31) & 0x3f)) >> 6);
  }
}

/* Function start: 0x412490 */
void EngineB::UpdateMeter() {
  int progressCurrent;
  int progressMax;
  int barPos;

  if (EngineB::m_meterBuffer == 0) {
    return;
  }

  progressMax = EngineB::m_progress.end;
  if (progressMax == 0 || EngineB::m_progress.start < progressMax) {
    // Calculate progress bar position
    progressCurrent = EngineB::m_progress.start;
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
      EngineB::m_meterFullRect.top,
      EngineB::m_meterFullRect.bottom,
      EngineB::m_meterPosition.x,
      EngineB::m_meterPosition.y,
      (VBuffer*)EngineB::m_meterBuffer);

    // Draw second part of progress bar
    g_WorkBuffer_00436974->CallBlitter2(
      barPos,
      EngineB::m_meterFullRect.right,
      EngineB::m_meterEmptyRect.top,
      EngineB::m_meterEmptyRect.bottom,
      EngineB::m_meterPosition.x + barPos,
      EngineB::m_meterPosition.y,
      (VBuffer*)EngineB::m_meterBuffer);
  } else {
    // Progress complete - use TimeOut
    if ((g_TimeOutInitFlag_0043d14c & 1) == 0) {
      g_TimeOutInitFlag_0043d14c |= 1;
      atexit(AtExitCleanup_0043d140);
    }

    // Draw full progress bar
    g_WorkBuffer_00436974->CallBlitter2(
      0,
      EngineB::m_meterFullRect.right,
      EngineB::m_meterFullRect.top,
      EngineB::m_meterFullRect.bottom,
      EngineB::m_meterPosition.x,
      EngineB::m_meterPosition.y,
      (VBuffer*)EngineB::m_meterBuffer);

    // Check if TimeOut is active
    if (g_TimeOut_0043d140.m_isActive != 1) {
      g_TimeOut_0043d140.Start(0x5dc);  // 1500ms timeout
      return;
    }

    if (g_TimeOut_0043d140.IsTimeOut() != 0) {
      g_GameOutcome->outcome = 1;
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
  g_TargetList->ProcessTargets();
  ((RockThrower*)EngineB::m_weaponParser)->UpdateProjectiles();
  EngineB::Draw();
  EngineB::UpdateMeter();
}

/* Function start: 0x412640 */
void EngineB::PlayCompletionSound() {
  if (EngineB::m_completionSound == 0) {
    return;
  }

  if (EngineB::m_localSoundList != 0) {
    EngineB::m_localSoundList->StopAll();
  }

  if (EngineB::m_backgroundSample != 0) {
    EngineB::m_backgroundSample->Fade(0x14, 0x2ee);
  }

  EngineB::m_completionSound->Play(0x64, 1);
  EngineB::m_completionSound->Stop();
}

/* Function start: 0x412690 */
void EngineB::OnProcessEnd() {
  int i;
  int numTargets;

  if (g_InputManager_00436968 != 0) {
    g_InputManager_00436968->PollEvents(1);
  }

  if (g_ConsoleSprite != 0) {
    InputState* pMouse = g_InputManager_00436968->pMouse;
    int timeVal;
    if (pMouse == 0) {
      timeVal = 0;
    } else {
      timeVal = pMouse->x;
    }
    g_ConsoleSprite->SetState2((timeVal + ((timeVal >> 31) & 0x3f)) >> 6);
  }

  // Allocate and initialize m_targetConfig (8 byte object)
  int* pObj164 = (int*)new char[8];
  pObj164[0] = 1;
  pObj164[1] = 3;
  EngineB::m_targetConfig = pObj164;
  EngineB::m_weaponParser = g_Weapon;

  // Update field_0x108 on all targets
  TargetList* targetList = g_TargetList;
  numTargets = targetList->count;
  if (numTargets > 0) {
    for (i = 0; i < numTargets; i++) {
      Target* pTarget = targetList->targets[i];
      pTarget->progressRange.end = *EngineB::m_targetConfig;
    }
  }

  // Create SoundList at m_localSoundList
  EngineB::m_localSoundList = new SoundList(10);

  // Create Animation at m_meterAnimation
  EngineB::m_meterAnimation = new Animation("rat1\\nmeter.smk");

  // DoFrame on animation
  if (EngineB::m_meterAnimation != 0) {
    EngineB::m_meterAnimation->DoFrame();
  }

  // Initialize progress meter fields from animation
  Animation* anim = EngineB::m_meterAnimation;
  EngineB::m_meterBuffer = (int)anim->targetBuffer;

  // Initialize meter configuration
  EngineB::m_meterEmptyRect.left = 0;
  EngineB::m_meterEmptyRect.top = 0;
  EngineB::m_meterEmptyRect.right = 0xff;
  EngineB::m_meterEmptyRect.bottom = 0xf;
  EngineB::m_meterFullRect.left = 0;
  EngineB::m_meterFullRect.top = 0x12;
  EngineB::m_meterFullRect.right = 0xff;
  EngineB::m_meterFullRect.bottom = 0x21;
  EngineB::m_progress.start = 0;
  EngineB::m_progress.end = 0x36;
  EngineB::m_meterPosition.x = 0x20;
  EngineB::m_meterPosition.y = 0x14;

  // Register sounds if g_SoundList_00435f1c is available
  if (g_SoundList_00435f1c != 0) {
    EngineB::m_missSound = (Sample*)g_SoundList_00435f1c->Register("audio\\slingmis.wav");
    EngineB::m_completionSound = (Sample*)g_SoundList_00435f1c->Register("audio\\ldu013_1.wav");
  }

  // Register sounds if m_localSoundList is available
  SoundList* sList = EngineB::m_localSoundList;
  if (sList != 0) {
    char* filename = FormatStringVA("audio\\ldu010_1.wav");
    EngineB::m_tauntSound1 = (Sample*)sList->Register(filename);
    EngineB::m_tauntSound2 = (Sample*)sList->Register("audio\\ldu011_1.wav");
    EngineB::m_hitSound1 = (Sample*)sList->Register("audio\\ldu008_1.wav");
    EngineB::m_hitSound2 = (Sample*)sList->Register("audio\\ldu009_1.wav");
    EngineB::m_hitSound3 = (Sample*)sList->Register("audio\\ldu007_2.wav");
    EngineB::m_milestoneSound = (Sample*)sList->Register("audio\\ldu006_1.wav");
    Sample* sample104 = (Sample*)sList->Register("audio\\ldu005_1.wav");
    EngineB::m_ambientSound = sample104;
    if (sample104 != 0) {
      sample104->Play(0x64, 1);
    }
  }
}
