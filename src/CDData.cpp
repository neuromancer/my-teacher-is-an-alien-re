
#include "CDData.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <direct.h>
#include "string.h"
#include "FileSystem.h"
#include "globals.h"

extern "C" {
int FileExists(const char *);
void ParsePath(const char *, char *, char *, char *, char *);
}

// External helper functions for CDData path resolution
extern "C" void __cdecl FUN_004195c0(char* param);      // FormatPath helper
extern "C" void __cdecl FUN_00419620(char* param);      // Path validation
extern "C" int __cdecl FUN_004195a0(const char* path);  // File check (returns 0 if exists)
extern "C" void __cdecl FUN_004261c0(char* param1, void* param2, char* outBuffer, void* param4, void* param5);
extern "C" int __cdecl FUN_004304a0(const char* path);  // Create directory
extern "C" void __cdecl FUN_00419660(const char* dest, const char* src);  // Path copy
extern "C" void __cdecl FUN_0042ad80(void);             // Some post-processing

/* Function start: 0x4195C0 */
extern "C" char* __cdecl CDData_FormatPath(char* param_1, ...)
{
  char local_104[260];
  va_list args;

  va_start(args, param_1);
  vsprintf(local_104, param_1, args);
  va_end(args);
  sprintf(g_CDData_0043697c->field_0xc0 + 5, "%s%s", g_CDData_0043697c->field_0xc0, local_104);
  return g_CDData_0043697c->field_0xc0 + 5;
}

/* Function start: 0x421E40 */
CDData::CDData(char *param_1, char *param_2) {
  int iVar1;
  unsigned int *puVar2;

  puVar2 = (unsigned int *)this;
  for (iVar1 = 0x79; iVar1 != 0; iVar1 = iVar1 - 1) {
    *puVar2 = 0;
    puVar2 = puVar2 + 1;
  }
  *(char *)puVar2 = 0;
  GetCurrentDir((char*)this, 0x80);
  if (param_1 != (char *)0x0) {
    strncpy(field_0x80, param_1, 0x40);
  }
  if (param_2 != (char *)0x0) {
    strncpy(field_0x1c5, param_2, 0x20);
  }
}

/* Function start: 0x421EA0 */
extern "C" void __fastcall CDData_ChangeToBaseDir(void *cdData) {
  chdir((char*)cdData);
}

/* Function start: 0x421EB0 */
int CDData::CheckFileOnDrive(int drive_letter) {
  char local_40[64];
  sprintf(local_40, "%c:\\%s\\%s", drive_letter + 0x40,
          field_0x80, field_0x1c5);
  return FileExists(local_40);
}

/* Function start: 0x421EF0 */
int CDData::ChangeDirectory(unsigned char *path) {
  if (path != 0 && *path != 0) {
    if (chdir((char *)path) != 0) {
      return 1;
    }
    ParsePath((char *)path, field_0xc0, 0, 0, 0);
  }
  return 0;
}

/* Function start: 0x421F40 */
int CDData::ChangeToDriveDirectory(int drive_letter) {
  char local_40[64];

  sprintf(local_40, "%c:\\%s\\%s", drive_letter + 0x40,
          field_0x80, field_0x1c5);
  int result = ChangeDirectory((unsigned char *)local_40);
  return result != 0;
}

/* Function start: 0x421F90 */
int CDData::ResolvePath(char* param_1) {
    char local_104[260];
    int len;
    char* ptr;

    FUN_004195c0(param_1);
    FUN_00419620(param_1);

    if (CDData::field_0xc0[0] == 0) {
        return 0;
    }

    // Check if file exists at path offset 0x145
    if (FUN_004195a0(CDData::field_0xc0 + 0x85) != 0) {
        return 0;
    }

    // Get path components
    FUN_004261c0(param_1, 0, local_104, 0, 0);

    // Remove trailing character if string is not empty
    if (local_104[0] != 0) {
        len = strlen(local_104);
        local_104[len - 1] = 0;
    }

    // Try to change directory
    if (chdir(local_104) == 0) {
        chdir("C:\\");  // DAT_004373cc is typically "C:"
    } else {
        FUN_004304a0(local_104);
    }

    // Copy resolved path
    FUN_00419660(CDData::field_0xc0 + 5, CDData::field_0xc0 + 0x85);
    FUN_0042ad80();

    return 1;
}
