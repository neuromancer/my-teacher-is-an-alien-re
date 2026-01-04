#include "Engine.h"
#include "AnimatedAsset.h"
#include "globals.h"
#include <stdio.h>
#include <string.h>

extern "C" {
void FUN_00422a2f(int, int);
void FUN_004229ea(int, int);
int FUN_00425000(int);
void FUN_00424fc0(int);
void FUN_00411550(void *);
}

/* Function start: 0x4110D0 */
Engine::Engine() {
  int *pb4 = (int *)((char *)this + 0xb4);
  pb4[0] = 0;
  pb4[1] = 0;

  int *pbc = (int *)((char *)this + 0xbc);
  pbc[0] = 0;
  pbc[1] = 0;

  int *pc4 = (int *)((char *)this + 0xc4);
  pc4[0] = 0;
  pc4[1] = 0;

  int *pcc = (int *)((char *)this + 0xcc);
  pcc[0] = 0;
  pcc[1] = 0;

  *(void **)this = (void *)0x431340;
  memset((char *)this + 0x88, 0, 0x60);

  int result = FUN_00425000(0);
  FUN_00424fc0(result);
  FUN_00411550(this);
}

/* Function start: 0x411D60 */
void Engine::DisplayFrameRate() {
  char local_80[128];

  FUN_00422a2f(0xff, 0xff);
  FUN_004229ea(0, 199);
  sprintf(local_80, "frames: a=%d l=%d", field_0xd4, field_0xdc);
  if (g_TextManager_00436990 != 0) {
    g_TextManager_00436990->RenderText(local_80, -1);
  }
}
