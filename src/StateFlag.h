#ifndef STATEFLAG_H
#define STATEFLAG_H

#include <windows.h>

// g_PaletteData_00437620, g_StateFlags_004374b2 — declared in globals.h

extern "C" {
    int __cdecl SetStateFlag(BYTE param_1, int param_2);
}

#endif // STATEFLAG_H