
#include "CDData.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <direct.h>
#include <io.h>
#include "string.h"
#include "FileSystem.h"
#include "globals.h"

extern "C" {
void ParsePath(const char *, char *, char *, char *, char *);
char* __cdecl CDData_FormatPath(char* format, ...);  // 0x4195C0
void __cdecl CDData_SetResolvedPath(const char* path); // 0x419620
int __cdecl FileExists(const char* path);           // 0x4195A0
void __cdecl CopyFileContent(const char* src, const char* dest); // 0x419660
int __cdecl FUN_00430310(const char* path, int param_2); // 0x430310
}

/* Function start: 0x4195C0 */
extern "C" char* __cdecl CDData_FormatPath(char* param_1, ...)
{
    char local_104[260];
    char* args = (char*)(&param_1 + 1);
    
    vsprintf(local_104, param_1, args);
    sprintf(g_CDData_0043697c->field_0xc0 + 5, "%s%s", g_CDData_0043697c->field_0xc0, local_104);
    return g_CDData_0043697c->field_0xc0 + 5;
}

/* Function start: 0x419620 */
extern "C" void __cdecl CDData_SetResolvedPath(const char* path) {
    strcpy(g_CDData_0043697c->field_0xc0 + 0x85, path);
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

/* Function start: 0x430310 */
extern "C" int __cdecl FUN_00430310(const char* path, int param_2) {
    DWORD attr = GetFileAttributesA(path);
    if (attr == 0xFFFFFFFF) {
        return -1;
    }
    
    if ((attr & FILE_ATTRIBUTE_READONLY) && (param_2 & 2)) {
        return -1;
    }
    
    return 0;
}

/* Function start: 0x419660 */
extern "C" void __cdecl CopyFileContent(const char* src, const char* dest) {
    int hSrc, hDest;
    long totalLen;
    unsigned int totalRead;
    unsigned int chunk;

    hSrc = open(src, 0x8000 | 0); // O_BINARY | O_RDONLY
    if (hSrc < 0) {
        ShowError("Error Reading CD ROM, searching for %s", src);
    }

    hDest = open(dest, 0x8000 | 0x301, 0x0040); // O_BINARY | O_WRONLY | O_CREAT | O_TRUNC, S_IREAD
    if (hDest < 0) {
        ShowError("Error writing temporary file. Please check disk space.%d", hDest);
    }

    totalLen = filelength(hSrc);
    totalRead = 0;

    if (totalLen > 0) {
        void* buffer = g_Buffer_00436964;
        while (totalRead < (unsigned int)totalLen) {
            chunk = (totalLen - totalRead > 0x2000) ? 0x2000 : (totalLen - totalRead);
            
            if (read(hSrc, buffer, chunk) != (int)chunk) {
                ShowError("Error Reading CD");
            }
            if (write(hDest, buffer, chunk) != (int)chunk) {
                ShowError("Error writing temporary file. Please check disk space.");
            }
            totalRead += chunk;
        }
    }

    _flushall();
    close(hSrc);
    close(hDest);
}

/* Function start: 0x421F90 */
int CDData::ResolvePath(char* param_1) {
    char local_104[260];
    int len;

    CDData_FormatPath(param_1);
    CDData_SetResolvedPath(param_1);

    if (field_0xc0[0] == 0) {
        return 0;
    }

    if (FileExists(field_0xc0 + 0x85) != 0) {
        return 0;
    }

    ParsePath(param_1, 0, local_104, 0, 0);

    if (local_104[0] != 0) {
        len = strlen(local_104);
        local_104[len - 1] = 0;
    }

    if (chdir(local_104) == 0) {
        chdir("C:\\"); 
    } else {
        mkdir(local_104);
    }

    CopyFileContent(field_0xc0 + 5, field_0xc0 + 0x85);
    _flushall();

    return 1;
}
