#include "EngineSubsystems.h"
#include <stdio.h>
#include <string.h>

extern "C" int _stricmp(const char*, const char*);

/* Function start: 0x414D80 */
TargetList::TargetList() : Parser() {
  memset(&count, 0, 80 * sizeof(void*));
}

/* Function start: 0x414DF0 */
TargetList::~TargetList() {
  // vtable is set to 0x431498 by the compiler in the destructor
  for (int i = 0; i < count; i++) {
    if (targets[i]) {
      delete targets[i];
    }
  }
}

/* Function start: 0x414FD0 */
int TargetList::LBLParse(char* line) {
  int id = 0;
  char type[32];

  type[0] = 0;
  sscanf(line, "%s", type);

  if (type[0] == 'F') {
    sscanf(&type[1], "%d", &id);

    Target* pTarget = new Target();

    int currentCount = count;
    targets[currentCount] = pTarget;
    targets[currentCount]->id = currentCount;
    count++;

    targets[currentCount]->Init(line + 2);
    ProcessFile(targets[currentCount], this, NULL);
    return 0;
  } else {
    if (_stricmp(type, "END") == 0) {
      return 1;
    }
    Parser::LBLParse((char*)"TargetList");
  }

  return 0;
}
