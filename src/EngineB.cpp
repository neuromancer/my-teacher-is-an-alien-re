#include "EngineB.h"
#include "SoundList.h"
#include "Memory.h"
#include <string.h>
#include <stdio.h>

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

// Stub implementations for virtual method overrides (to be implemented later)
void EngineB::OnProcessEnd() {}
void EngineB::VirtProcess() {}
void EngineB::VirtDraw() {}
void EngineB::Virt13() {}
void EngineB::Virt17() {}
