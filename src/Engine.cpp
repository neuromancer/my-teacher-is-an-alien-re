#include "Engine.h"
#include "DrawEntry.h"
#include "AnimatedAsset.h"
#include "globals.h"
#include "SoundCommand.h"
#include "VBuffer.h"
#include "Memory.h"
#include "Sprite.h"
#include <stdio.h>
#include <string.h>

#include "VideoTable.h"

extern "C" {
int GetCurrentTimestamp(int*);  // 0x425000 - returns timestamp, optionally stores in *param
void SetTimeSeed(int);          // 0x424FC0 - sets global time seed (DAT_0043bc88)
void FUN_00411550(Engine*);     // Engine::Initialize implementation

// Parser subsystem function - processes a sub-parser with the engine context
int __cdecl FUN_00418dc0(Parser* subParser, int engineThis, char* param);

// Constructor for RockThrower weapon (size 0xb8)
Parser* __fastcall FUN_004165d0(void* mem);

// Constructor for Console (size 0xd8) - use Sprite::Sprite(char*)

// Reports unknown label during parsing - __thiscall
void __fastcall FUN_00418b30(void* parser, int dummy, char* className);

// Globals for sub-parsers
extern Parser* DAT_00435f00;  // ENGINE_INFO parser
extern Parser* DAT_00435f04;  // Console parser
extern Parser* DAT_00435f0c;  // TARGETS parser
extern Parser* DAT_00435f10;  // SPRITELIST parser
extern Parser* DAT_00435f14;  // Weapon parser
extern Parser* DAT_00435f24;  // NAVIGATION parser
}

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

  memset(&m_sceneManager, 0, 0x60);

  int result = GetCurrentTimestamp(0);
  SetTimeSeed(result);
  Engine::Initialize();
}

/* Function start: 0x411550 */
void Engine::Initialize() {
  FUN_00411550(this);
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
  void* local_14;
  Parser* piVar4;

  sscanf(line, "%s", local_34);

  if (strcmp(local_34, "[ENGINE_INFO]") == 0) {
    FUN_00418dc0(DAT_00435f00, (int)this, (char*)0);
  }
  else if (strcmp(local_34, "[TARGETS]") == 0) {
    FUN_00418dc0(DAT_00435f0c, (int)this, (char*)0);
  }
  else if (strcmp(local_34, "[SPRITELIST]") == 0) {
    FUN_00418dc0(DAT_00435f10, (int)this, (char*)0);
  }
  else if (strcmp(local_34, "[NAVIGATION]") == 0) {
    FUN_00418dc0(DAT_00435f24, (int)this, (char*)0);
  }
  else if (strcmp(local_34, "WEAPON") == 0) {
    int iVar3 = sscanf(line, " %s %s ", local_34, local_54);
    if (iVar3 == 2) {
      if (Engine::m_subParser != (Parser*)0) {
        (*(void (**)(int))(*((int*)Engine::m_subParser) + 0xc))(1);
        Engine::m_subParser = (Parser*)0;
      }
      if (strcmp(local_54, "ROCKTHROWER") == 0) {
        local_14 = AllocateMemory(0xb8);
        piVar4 = (Parser*)0;
        if (local_14 != (void*)0) {
          piVar4 = FUN_004165d0(local_14);
        }
        Engine::m_subParser = piVar4;
        DAT_00435f14 = piVar4;
      }
    }
  }
  else if (strcmp(local_34, "CONSOLE") == 0) {
    piVar4 = new Sprite((char*)0);
    DAT_00435f04 = piVar4;
    FUN_00418dc0(piVar4, (int)this, (char*)0);
  }
  else if (strcmp(local_34, "END") == 0) {
    return 1;
  }
  else {
    FUN_00418b30(this, 0, "Engine");
  }
  return 0;
}
