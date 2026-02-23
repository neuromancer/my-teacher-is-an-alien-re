#include "EngineA.h"
#include <stdio.h>
#include <string.h>

/* Function start: 0x43A4C0 */
EngineA::EngineA() {
  // Engine::Engine() is called automatically via inheritance
  // EngineA has no additional fields to initialize
}

/* Function start: 0x409A80 */
EngineA::~EngineA() {
  // Destructor body - calls Engine destructor automatically
}

/* Function start: 0x451690 */
int EngineA::LBLParse(char* line) {
  char token[32];
  sscanf(line, "%s", token);
  if (strcmp(token, "END_ENGINEB_INFO") == 0) {
    return 1;
  }
  Engine::LBLParse(line);
  return 0;
}
