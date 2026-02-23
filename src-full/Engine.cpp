#include "Engine.h"
#include "DrawEntry.h"
#include "Animation.h"
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
    VBuffer* videoBuffer;
    void* childObject;

    videoBuffer = DrawEntry::m_videoBuffer;
    if (videoBuffer != 0) {
        videoBuffer->~VBuffer();
        FreeFromGlobalHeap(videoBuffer);
        DrawEntry::m_videoBuffer = 0;
    }

    childObject = DrawEntry::m_childObject;
    if (childObject != 0) {
        delete (Animation*)childObject;
        DrawEntry::m_childObject = 0;
    }
}

/* Function start: 0x4110D0 */ /* DEMO ONLY - no full game match */
Engine::Engine() {
  int* p;
  p = (int*)&m_combatBonus1;
  p[0] = 0;
  p[1] = 0;

  p = (int*)&field_0xbc;
  p[0] = 0;
  p[1] = 0;

  p = (int*)&m_combatBonus2;
  p[0] = 0;
  p[1] = 0;

  p = (int*)&field_0xcc;
  p[0] = 0;
  p[1] = 0;

  memset(&m_targetList, 0, 0x60);

  int result = GetCurrentTimestamp(0);
  SetTimeSeed(result);
  Engine::Initialize();
}

/* Function start: 0x411230 */ /* DEMO ONLY - no full game match */
void Engine::SetupViewport() {
  int height;
  int width;
  Animation* anim;

  g_EngineViewport_00435f08->SetDimensions(g_EngineInfoParser_00435f00->anchorRect.right, g_EngineInfoParser_00435f00->anchorRect.bottom);
  
  anim = ((mCNavigator*)g_Navigator_00435f24)->sprite->animation_data;
  height = 0;
  if (anim != 0) {
    height = anim->targetBuffer->height;
  }
  width = 0;
  if (anim != 0) {
    width = anim->targetBuffer->width;
  }
  
  g_EngineViewport_00435f08->SetDimensions2(width - g_EngineViewport_00435f08->dim.a, height - g_EngineViewport_00435f08->dim.b);
  g_EngineViewport_00435f08->SetCenter();
  g_EngineViewport_00435f08->SetAnchor(g_EngineInfoParser_00435f00->anchorRect.left, g_EngineInfoParser_00435f00->anchorRect.top);
  
  g_ScoreManager_00435f20->scoreInitial = 100;
  m_framesL = 1;
  m_framesA = 1;
  
  BlankScreen();
  
  g_EnginePalette_00435f18->SetPalette(g_EngineInfoParser_00435f00->paletteRect.left, (g_EngineInfoParser_00435f00->paletteRect.top - g_EngineInfoParser_00435f00->paletteRect.left) + 1);
  
  if (m_backgroundSample != 0) {
    m_backgroundSample->Play(100, 0);
  }
}

/* Function start: 0x42BF00 */
void Engine::StopAndCleanup() {
  g_Sound_0043696c->StopAllSamples();
  VirtCleanup();
}

/* Function start: 0x411340 */ /* DEMO ONLY - no full game match */
int Engine::UpdateAndCheck() {
  int result;

  result = CheckNavState();
  if (result != 0) {
    return 1;
  }

  UpdateCrosshair();

  result = ((mCNavigator*)g_Navigator_00435f24)->Update();
  if (result != 0) {
    return 1;
  }

  result = UpdateSpriteFrame();
  if (result != 0) {
    return 1;
  }

  ProcessTargets();

  Engine::m_framesA++;
  Engine::m_framesL++;

  return 0;
}

/* Function start: 0x411550 */ /* DEMO ONLY - no full game match */
void Engine::Initialize() {
  Engine::m_weapon = new Weapon();
  Engine::m_soundList = new SoundList(0x32);
  Engine::m_engineInfoParser = new EngineInfoParser();
  Engine::m_navigator = new mCNavigator();
  Engine::m_timerManager = new Palette();
  Engine::m_combatSprite = new CombatSprite();
  Engine::m_targetList = new TargetList();
  Engine::m_cursorState = new CursorState();
  Engine::m_viewport = new Viewport();
  Engine::m_gameOutcome = new GameOutcome();

  CopyToGlobals();
}

/* Function start: 0x411D60 */ /* DEMO ONLY - no full game match */
void Engine::DisplayFrameRate() {
  char local_80[128];

  SetDrawColors(0xff, 0xff);
  SetDrawPosition(0, 199);
  sprintf(local_80, "frames: a=%d l=%d", m_framesA, m_framesL);
  if (g_TextManager_00436990 != 0) {
    g_TextManager_00436990->RenderText(local_80, -1);
  }
}

/* Function start: 0x411DE0 */ /* DEMO ONLY - no full game match */
int Engine::LBLParse(char* line) {
  char local_54[32];
  char local_34[32];
  RockThrower* weaponVar;
  Sprite* spriteVar;

  sscanf(line, "%s", local_34);

  if (strcmp(local_34, "[ENGINE_INFO]") == 0) {
    Parser::ProcessFile(g_EngineInfoParser_00435f00, this, 0);
  }
  else if (strcmp(local_34, "[TARGETS]") == 0) {
    Parser::ProcessFile(g_TargetList_00435f0c, this, 0);
  }
  else if (strcmp(local_34, "[SPRITELIST]") == 0) {
    Parser::ProcessFile(g_SpriteList_00435f10, this, 0);
  }
  else if (strcmp(local_34, "[NAVIGATION]") == 0) {
    Parser::ProcessFile(g_Navigator_00435f24, this, 0);
  }
  else if (strcmp(local_34, "WEAPON") == 0) {
    int iVar3 = sscanf(line, " %s %s ", local_34, local_54);
    if (iVar3 == 2) {
      if (Engine::m_weapon != 0) {
        delete Engine::m_weapon;
        Engine::m_weapon = 0;
      }
      if (strcmp(local_54, "ROCKTHROWER") == 0) {
        weaponVar = new RockThrower();
        Engine::m_weapon = weaponVar;
        g_Weapon_00435f14 = weaponVar;
      }
    }
  }
  else if (strcmp(local_34, "CONSOLE") == 0) {
    spriteVar = new Sprite(0);
    g_ConsoleSprite_00435f04 = spriteVar;
    Parser::ProcessFile(spriteVar, this, 0);
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
/* Function start: 0x411440 */ /* DEMO ONLY - no full game match */
int Engine::UpdateSpriteFrame() {
  if (g_SpriteList_00435f10 != 0) {
    g_SpriteList_00435f10->ProcessFrame(Engine::m_framesL);
  }
  return 0;
}

/* Function start: 0x411460 */ /* DEMO ONLY - no full game match */
void Engine::UpdateCrosshair() {
  InputState* mouse;

  mouse = g_InputManager_00436968->pMouse;
  if (mouse != 0) {
    g_Weapon_00435f14->m_crosshairX = mouse->x;
  } else {
    g_Weapon_00435f14->m_crosshairX = 0;
  }

  mouse = g_InputManager_00436968->pMouse;
  if (mouse != 0) {
    g_Weapon_00435f14->m_crosshairY = mouse->y;
  } else {
    g_Weapon_00435f14->m_crosshairY = 0;
  }

  mouse = g_InputManager_00436968->pMouse;
  if (mouse != 0) {
    if (mouse->x > 0xAA) {
      g_EngineViewport_00435f08->SetCenterX(g_EngineViewport_00435f08->scrollX + 4);
      return;
    }
  }
  if (mouse != 0 && mouse->x >= 0x96) {
    return;
  }
  g_EngineViewport_00435f08->SetCenterX(g_EngineViewport_00435f08->scrollX - 4);
}

/* Function start: 0x411DD0 */ /* DEMO ONLY - no full game match */
int Engine::CheckNavState() {
  return (unsigned int)g_GameOutcome_00435f28->outcome >= 1;
}
/* Function start: 0x4113A0 */ /* DEMO ONLY - no full game match */
void Engine::ProcessTargets() {
  Target* target;
  InputState* mouse;
  int buttonDown;

  target = g_TargetList_00435f0c->ProcessTargets();
  g_Weapon_00435f14->DrawCrosshairs();

  mouse = g_InputManager_00436968->pMouse;
  buttonDown = 0;
  if (mouse != 0) {
    buttonDown = mouse->buttons & 1;
  }
  if (buttonDown == 0 && (mouse->prevButtons & 1) != 0) {
    g_Weapon_00435f14->field_0xa0 = 1;
  } else {
    g_Weapon_00435f14->field_0xa0 = 0;
  }

  if (g_Weapon_00435f14->field_0xa0 != 0) {
    g_Weapon_00435f14->OnHit();
    if (target != 0) {
      target->UpdateProgress(1);
    }
  }

  Engine::Draw();
  Engine::UpdateMeter();
}
/* Function start: 0x411510 */ /* DEMO ONLY - no full game match */
void Engine::Draw() {
  Sprite* console;

  if (g_ConsoleSprite_00435f04 != 0) {
    console = g_ConsoleSprite_00435f04;
    console->Do(console->loc_x, console->loc_y, 1.0);
  }
}
void Engine::UpdateMeter() {}
/* Function start: 0x411CA0 */ /* DEMO ONLY - no full game match */
void Engine::CopyToGlobals() {
  g_ConsoleSprite_00435f04 = Engine::m_consoleSprite;
  g_Weapon_00435f14 = Engine::m_weapon;
  g_SoundList_00435f1c = Engine::m_soundList;
  g_EngineInfoParser_00435f00 = Engine::m_engineInfoParser;
  g_Navigator_00435f24 = Engine::m_navigator;
  g_EnginePalette_00435f18 = Engine::m_timerManager;
  g_SpriteList_00435f10 = Engine::m_combatSprite;
  g_TargetList_00435f0c = Engine::m_targetList;
  g_ScoreManager_00435f20 = Engine::m_cursorState;
  g_EngineViewport_00435f08 = Engine::m_viewport;
  g_GameOutcome_00435f28 = Engine::m_gameOutcome;
}

/* Function start: 0x411D20 */ /* DEMO ONLY - no full game match */
void Engine::ClearGlobals() {
  g_ConsoleSprite_00435f04 = 0;
  g_Weapon_00435f14 = 0;
  g_SoundList_00435f1c = 0;
  g_EngineInfoParser_00435f00 = 0;
  g_Navigator_00435f24 = 0;
  g_EnginePalette_00435f18 = 0;
  g_SpriteList_00435f10 = 0;
  g_TargetList_00435f0c = 0;
  g_ScoreManager_00435f20 = 0;
  g_EngineViewport_00435f08 = 0;
  g_GameOutcome_00435f28 = 0;
}
void Engine::PlayCompletionSound() {}
