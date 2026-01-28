#include "Engine.h"
#include "DrawEntry.h"
#include "AnimatedAsset.h"
#include "globals.h"
#include "SoundCommand.h"
#include "VBuffer.h"
#include "Memory.h"
#include "Sprite.h"
#include "SoundList.h"
#include "mCNavigator.h"
#include "Palette.h"
#include "EngineSubsystems.h"
#include "CombatSprite.h"
#include "Viewport.h"
#include "CursorState.h"
#include "GameOutcome.h"
#include "Sound.h"
#include "RockThrower.h"
#include "Sample.h"
#include "InputManager.h"
#include "Target.h"
#include <stdio.h>
#include <string.h>

#include "VideoTable.h"

extern "C" {
int GetCurrentTimestamp(int*);  // 0x425000 - returns timestamp, optionally stores in *param
void SetTimeSeed(int);          // 0x424FC0 - sets global time seed (DAT_0043bc88)
}
void BlankScreen();             // 0x419390

// Globals for sub-parsers
// (Now declared in globals.h)

// Empty virtual function overrides (0x4010E0, 0x4010F0)
void Engine::OnProcessStart() {}
void Engine::OnProcessEnd() {}

// DrawEntry implementation (originally in same source file as Engine)
DrawEntry::~DrawEntry() {
    // Destructor called via vtable[0]
}

/* Function start: 0x411080 */
void* DrawEntry::Cleanup(int freeFlag) {
    VBuffer* videoBuffer;
    SoundCommand* childObject;

    // Release video buffer at offset 0x04
    videoBuffer = DrawEntry::m_videoBuffer;
    if (videoBuffer != 0) {
        delete videoBuffer;
        DrawEntry::m_videoBuffer = 0;
    }

    // Destroy child object at offset 0x08 via delete (calls virtual destructor)
    childObject = DrawEntry::m_childObject;
    if (childObject != 0) {
        delete childObject;
        DrawEntry::m_childObject = 0;
    }

    // Optionally free self
    if ((freeFlag & 1) != 0) {
        FreeMemory(this);
    }

    return this;
}

/* Function start: 0x4110D0 */
Engine::Engine() {
  int* pb4 = &field_0xb4;
  pb4[0] = 0;
  pb4[1] = 0;

  int* pbc = &field_0xbc;
  pbc[0] = 0;
  pbc[1] = 0;

  int* pc4 = &field_0xc4;
  pc4[0] = 0;
  pc4[1] = 0;

  int* pcc = &field_0xcc;
  pcc[0] = 0;
  pcc[1] = 0;

  memset(&m_targetList, 0, 0x60);

  int result = GetCurrentTimestamp(0);
  SetTimeSeed(result);
  Engine::Initialize();
}

/* Function start: 0x411230 */
void Engine::SetupViewport() {
  int height;
  int width;
  Animation* anim;

  g_EngineViewport->SetDimensions(DAT_00435f00->anchorRect.right, DAT_00435f00->anchorRect.bottom);
  
  anim = ((mCNavigator*)DAT_00435f24)->sprite->animation_data;
  height = 0;
  if (anim != 0) {
    height = anim->targetBuffer->height;
  }
  width = 0;
  if (anim != 0) {
    width = anim->targetBuffer->width;
  }
  
  g_EngineViewport->SetDimensions2(width - g_EngineViewport->x1, height - g_EngineViewport->y1);
  g_EngineViewport->SetCenter();
  g_EngineViewport->SetAnchor(DAT_00435f00->anchorRect.left, DAT_00435f00->anchorRect.top);
  
  g_ScoreManager[1] = 100;
  m_framesL = 1;
  m_framesA = 1;
  
  BlankScreen();
  
  g_EnginePalette->SetPalette(DAT_00435f00->paletteRect.left, (DAT_00435f00->paletteRect.top - DAT_00435f00->paletteRect.left) + 1);
  
  if (field_0xe0 != 0) {
    ((Sample*)field_0xe0)->Play(100, 0);
  }
}

/* Function start: 0x411320 */
void Engine::StopAndCleanup() {
  g_Sound_0043696c->StopAllSamples();
  VirtCleanup();
}

/* Function start: 0x411340 */
int Engine::UpdateAndCheck() {
  int result;

  result = VirtCheck1();
  if (result != 0) {
    return 1;
  }

  VirtUpdate();

  result = ((mCNavigator*)DAT_00435f24)->Update();
  if (result != 0) {
    return 1;
  }

  result = VirtCheck2();
  if (result != 0) {
    return 1;
  }

  ProcessTargets();

  Engine::m_framesA++;
  Engine::m_framesL++;

  return 0;
}

/* Function start: 0x411550 */
void Engine::Initialize() {
  Engine::m_subParser = new EngineInfoParser();
  Engine::m_soundList = new SoundList(0x32);
  Engine::m_engineInfoParser = new EngineInfoParser();
  Engine::m_navigator = new mCNavigator();
  Engine::m_timerManager = new Palette();
  Engine::m_combatSprite = new CombatSprite();
  Engine::m_targetList = new TargetList();
  Engine::m_cursorState = new CursorState();
  Engine::m_viewport = new Viewport();
  Engine::m_stateManager = new GameOutcome();

  Engine::OnProcessStart();
}

/* Function start: 0x411D60 */
void Engine::DisplayFrameRate() {
  char local_80[128];

  SetDrawColors(0xff, 0xff);
  SetDrawPosition(0, 199);
  sprintf(local_80, "frames: a=%d l=%d", m_framesA, m_framesL);
  if (g_TextManager_00436990 != 0) {
    g_TextManager_00436990->RenderText(local_80, -1);
  }
}

/* Function start: 0x411DE0 */
int Engine::LBLParse(char* line) {
  char local_54[32];
  char local_34[32];
  Parser* piVar4;

  sscanf(line, "%s", local_34);

  if (strcmp(local_34, "[ENGINE_INFO]") == 0) {
    Parser::ProcessFile(DAT_00435f00, this, (char*)0);
  }
  else if (strcmp(local_34, "[TARGETS]") == 0) {
    Parser::ProcessFile(DAT_00435f0c, this, (char*)0);
  }
  else if (strcmp(local_34, "[SPRITELIST]") == 0) {
    Parser::ProcessFile(DAT_00435f10, this, (char*)0);
  }
  else if (strcmp(local_34, "[NAVIGATION]") == 0) {
    Parser::ProcessFile(DAT_00435f24, this, (char*)0);
  }
  else if (strcmp(local_34, "WEAPON") == 0) {
    int iVar3 = sscanf(line, " %s %s ", local_34, local_54);
    if (iVar3 == 2) {
      if (Engine::m_subParser != (Parser*)0) {
        delete Engine::m_subParser;
        Engine::m_subParser = (Parser*)0;
      }
      if (strcmp(local_54, "ROCKTHROWER") == 0) {
        piVar4 = new RockThrower();
        Engine::m_subParser = piVar4;
        DAT_00435f14 = piVar4;
      }
    }
  }
  else if (strcmp(local_34, "CONSOLE") == 0) {
    piVar4 = new Sprite((char*)0);
    DAT_00435f04 = piVar4;
    Parser::ProcessFile(piVar4, this, (char*)0);
  }
  else if (strcmp(local_34, "END") == 0) {
    return 1;
  }
  else {
    Parser::LBLParse("Engine");
  }
  return 0;
}

// Stub implementations for virtual methods (to be implemented later)
Engine::~Engine() {}
void Engine::CleanupSubsystems() {}
void Engine::VirtCleanup() {}
void Engine::VirtUpdate() {}
int Engine::VirtCheck1() { return 0; }
int Engine::VirtCheck2() { return 0; }
/* Function start: 0x4113A0 */
void Engine::ProcessTargets() {
  Target* target;
  InputState* mouse;
  int buttonDown;

  target = ((TargetList*)DAT_00435f0c)->ProcessTargets();
  ((Weapon*)DAT_00435f14)->DrawCrosshairs();

  mouse = g_InputManager_00436968->pMouse;
  buttonDown = 0;
  if (mouse != 0) {
    buttonDown = mouse->buttons & 1;
  }
  if (buttonDown == 0 && (mouse->prevButtons & 1) != 0) {
    ((Weapon*)DAT_00435f14)->field_0xa0 = 1;
  } else {
    ((Weapon*)DAT_00435f14)->field_0xa0 = 0;
  }

  if (((Weapon*)DAT_00435f14)->field_0xa0 != 0) {
    ((Weapon*)DAT_00435f14)->OnHit();
    if (target != 0) {
      target->UpdateProgress(1);
    }
  }

  Engine::Draw();
  Engine::UpdateMeter();
}
/* Function start: 0x411510 */
void Engine::Draw() {
  Sprite* console;

  if (DAT_00435f04 != 0) {
    console = (Sprite*)DAT_00435f04;
    console->Do(console->loc_x, console->loc_y, 1.0);
  }
}
void Engine::UpdateMeter() {}
void Engine::Virt15() {}
void Engine::Virt16() {}
void Engine::PlayCompletionSound() {}
