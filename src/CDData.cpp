
#include "CDData.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include "string.h"
#include "FileSystem.h"

extern "C" {
int FileExists(const char *);
void ParsePath(const char *, char *, char *, char *, char *);
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
void __fastcall CDData_ChangeToBaseDir(char *cdData) {
  chdir(cdData);
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
