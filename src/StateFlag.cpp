#include "StateFlag.h"

#include <windows.h>

BYTE DAT_00437620[256];
USHORT DAT_004374b2;

/* Function start: 0x422DC1 */
int __cdecl SetStateFlag(BYTE param_1, int param_2)
{
  BYTE bVar1;

  bVar1 = (BYTE)param_2 | (BYTE)((UINT)param_2 >> 8);
  DAT_00437620[param_1] = bVar1;
  DAT_004374b2 = DAT_004374b2 & ~(USHORT)(1 << (param_1 & 0x1f));
  DAT_004374b2 = DAT_004374b2 | (USHORT)(bVar1 != 0) << (param_1 & 0x1f);
  return 0;
}