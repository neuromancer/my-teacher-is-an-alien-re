#include "globals.h"
#include "AILData.h"
#include "string.h"
#include <mss.h>
#include <stdio.h>

extern "C" {
void FUN_0041e670(int);
}


/* Function start: 0x41E460 */
AILData *AILData::ClearFields() {
  this->data = 0;
  this->size = 0;
  this->field_8 = 0;
  this->field_C = 0;
  return this;
}

/* Function start: 0x41E470 */
AILData::~AILData() {
  if (this->data != 0) {
    FUN_0041e670((int)this);
    AIL_mem_free_lock(this->data);
    this->data = 0;
  }
}

/* Function start: 0x41E490 */
int AILData::Load(char *filename) {
  if (*(int *)((char *)g_Sound_0043696c + 0x38) == 0) {
    return 1;
  }
  if (this->data != 0) {
    this->~AILData();
  }
  this->size = 0;
  FILE *_File = OpenFileAndFindKey((char *)0x0, filename, "rb",
                                   (unsigned int *)&this->size);
  if ((_File != 0) && (this->size != 0)) {
    this->data = (char *)AIL_mem_alloc_lock(this->size);
    if (this->data == 0) {
      return 1;
    }
    fread(this->data, this->size, 1, _File);
    fclose(_File);
    return 0;
  }
  return 1;
}
