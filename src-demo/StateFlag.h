#ifndef STATEFLAG_H
#define STATEFLAG_H

#include <windows.h>

extern "C" {
    extern BYTE DAT_00437620[];
    extern USHORT DAT_004374b2;

    int __cdecl SetStateFlag(BYTE param_1, int param_2);
}

#endif // STATEFLAG_H