#include "EngineB.h"
#include <string.h>

extern "C" {
void FUN_004110d0(void *);
void FUN_0041e7d0(void *);
void FUN_00424940(void *);
}

/* Function start: 0x412110 */
EngineB::EngineB() {
  FUN_004110d0(this);

  int *p130 = (int *)((char *)this + 0x130);
  p130[0] = 0;
  p130[1] = 0;
  p130[2] = 0;
  p130[3] = 0;

  int *p140 = (int *)((char *)this + 0x140);
  p140[0] = 0;
  p140[1] = 0;
  p140[2] = 0;
  p140[3] = 0;

  int *p150 = (int *)((char *)this + 0x150);
  p150[0] = 0;
  p150[1] = 0;

  int *p158 = (int *)((char *)this + 0x158);
  p158[0] = 0;
  p158[1] = 0;

  *(void **)this = (void *)0x4313c0;
  memset((char *)this + 0xe8, 0, 0x80);
}

/* Function start: 0x4121f0 */
EngineB::~EngineB() { DestructorHelper(); }

/* Function start: 0x412210 */
void EngineB::DestructorHelper() {
  *(void **)this = (void *)0x4313c0;
  void *obj128 = *(void **)((char *)this + 0x128);
  if (obj128) {
    (*(void (**)(void *, int)) * (void **)obj128)(obj128, 1);
    *(void **)((char *)this + 0x128) = 0;
  }
  void *obj0xe8 = *(void **)((char *)this + 0xe8);
  if (obj0xe8) {
    FUN_0041e7d0(obj0xe8);
    FUN_00424940(obj0xe8);
    *(void **)((char *)this + 0xe8) = 0;
  }
}
