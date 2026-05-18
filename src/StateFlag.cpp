#include "StateFlag.h"

#include <windows.h>
#include "globals.h"

// g_PaletteData_0046d208 and g_StateFlags_0046d09a are declared in globals.h and defined in globals.cpp

/* Function start: 0x453BBE */
int __cdecl SetStateFlag(BYTE param_1, int param_2)
{
  BYTE bVar1;

  bVar1 = (BYTE)param_2 | (BYTE)((UINT)param_2 >> 8);
  g_PaletteData_0046d208[param_1] = bVar1;
  g_StateFlags_0046d09a = g_StateFlags_0046d09a & ~(USHORT)(1 << (param_1 & 0x1f));
  g_StateFlags_0046d09a = g_StateFlags_0046d09a | (USHORT)(bVar1 != 0) << (param_1 & 0x1f);
  return 0;
}
