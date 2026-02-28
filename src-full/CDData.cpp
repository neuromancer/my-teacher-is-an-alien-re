
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
char* __cdecl CDData_SetResolvedPath(const char* path); // 0x419620
int __cdecl FileExists(const char* path);           // 0x4195A0
int __cdecl CopyFileContent(const char* src, const char* dest); // 0x419660
int __cdecl FUN_00430310(const char* path, int param_2); // 0x430310
}
void ShowError(const char* format, ...);

/* Function start: 0x4195C0 */ /* DEMO ONLY - no full game match */
extern "C" char* __cdecl CDData_FormatPath(char* param_1, ...)
{
    char local_104[260];
    char* args = (char*)(&param_1 + 1);
    
    vsprintf(local_104, param_1, args);
    sprintf(g_CDData_0043697c->cdIdentifier + 5, "%s%s", g_CDData_0043697c->cdIdentifier, local_104);
    return g_CDData_0043697c->cdIdentifier + 5;
}

/* Function start: 0x419620 */ /* DEMO ONLY - no full game match */
extern "C" char* __cdecl CDData_SetResolvedPath(const char* path) {
    return strcpy(g_CDData_0043697c->field_190 + 0x45, path);
}

/* Function start: 0x432EC0 */
CDData::CDData(char *param_1, const char *param_2, const char *param_3) {
  memset(&baseDir, 0, 0x210);
  GetCurrentDir(baseDir, 0x80);
  Setup(param_1, param_2, param_3);
}

/* Function start: 0x432FD0 */
void CDData::Setup(char *param_1, const char *param_2, const char *param_3) {
  char local_80[128];
  int iVar4;

  strcpy(cdFolder, param_1);
  strcpy(cdIdentifier, param_2);

  if (FileExists("Develop.___")) {
    g_DevelopFlag_00472de4 = 1;
  }

  if (cdFolder[0] != '\0') {
    sprintf(local_80, "%s\\%s", cdFolder, param_2);
    if (FileExists(local_80) == 0) {
      ShowError("%s\nInvalid cmdLine-'%s'\nCan't find file '%s'",
                param_3, cdFolder, local_80);
    }
    sprintf(local_80, "%s\\CDDATA\\DATA", cdFolder);
    chdir(local_80);
    ParsePath(local_80, field_190, 0, 0, 0);
    sprintf(local_80, "%s\\DATA", baseDir);
    if (chdir(local_80) == 0) {
      return;
    }
    param_3 = "Game Not Properly Installed-Missing local DATA dir.";
  } else {
    iVar4 = 3;
    do {
      sprintf(local_80, "%c:\\%s", iVar4 + 0x40, cdIdentifier);
      if (FileExists(local_80) != 0) {
        ParsePath(local_80, field_190, 0, 0, 0);
        if (field_190[0] == baseDir[0]) {
          ShowError("Please run Setup.exe");
        }
        sprintf(local_80, "%s\\CDDATA\\DATA", field_190);
        if (chdir(local_80) != 0) {
          ShowError("%s\nInvalid CD in Drive-'%s'\nCan't find subdir '%s'",
                    param_3, field_190, local_80);
        }
        sprintf(local_80, "%s\\DATA", baseDir);
        if (chdir(local_80) != 0) {
          ShowError("Game Not Properly Installed-Missing local DATA dir.");
        }
        break;
      }
      iVar4++;
    } while (iVar4 < 0x1a);
    if (iVar4 < 0x19) {
      return;
    }
  }
  ShowError(param_3);
}

/* Function start: 0x421E40 */ /* DEMO ONLY - no full game match */
CDData::CDData(char *param_1, char *param_2) {
  memset(&baseDir, 0, 0x1e5);
  GetCurrentDir(baseDir, 0x80);
  if (param_1 != 0) {
    strncpy(cdFolder, param_1, 0x40);
  }
  if (param_2 != 0) {
    strncpy(field_190 + 0x35, param_2, 0x20);
  }
}

/* Function start: 0x421EA0 */ /* DEMO ONLY - no full game match */
extern "C" void __fastcall CDData_ChangeToBaseDir(void *cdData) {
  chdir(((CDData*)cdData)->baseDir);
}

/* Function start: 0x421EB0 */ /* DEMO ONLY - no full game match */
int CDData::CheckFileOnDrive(int drive_letter) {
  char local_40[64];
  sprintf(local_40, "%c:\\%s\\%s", drive_letter + 0x40,
          cdFolder, field_190 + 0x35);
  return FileExists(local_40);
}

/* Function start: 0x421EF0 */ /* DEMO ONLY - no full game match */
int CDData::ChangeDirectory(unsigned char *path) {
  if (path != 0 && *path != 0) {
    if (chdir((char *)path) != 0) {
      return 1;
    }
    ParsePath((char *)path, cdIdentifier, 0, 0, 0);
  }
  return 0;
}

/* Function start: 0x421F40 */ /* DEMO ONLY - no full game match */
int CDData::ChangeToDriveDirectory(int drive_letter) {
  char local_40[64];

  sprintf(local_40, "%c:\\%s\\%s", drive_letter + 0x40,
          cdFolder, field_190 + 0x35);
  int result = ChangeDirectory((unsigned char *)local_40);
  return result != 0;
}

/* Function start: 0x430310 */ /* DEMO ONLY - no full game match */
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

/* Function start: 0x426220 */ /* ~96% match */
extern "C" int __cdecl CopyFileContent(const char* src, const char* dest) {
    int hSrc;
    int hDest;
    unsigned int totalLen;
    unsigned int totalRead;
    unsigned int chunk;
    char* buf;
    unsigned int sz;

    totalRead = 0;
    hSrc = open(src, 0x8000, totalRead);
    if (hSrc < 0) {
        ShowError("Error Reading CD ROM, searching for %s", src);
    }

    hDest = open(dest, 0x8301, totalRead);
    if (hDest < 0) {
        ShowError("Error writing temporary file. Please check disk space.%d", hDest);
    }

    totalLen = filelength(hSrc);
    if (totalLen >= 0x2000) {
        chunk = 0x2000;
    } else {
        chunk = totalLen;
    }

    buf = g_Buffer_00436964;
    if (totalLen != 0) {
        do {
            sz = totalLen - totalRead;
            if (sz >= chunk) {
                sz = chunk;
            }

            if ((unsigned int)read(hSrc, buf, sz) != sz) {
                ShowError("Error Reading CD");
            }
            if ((unsigned int)write(hDest, buf, sz) != sz) {
                ShowError("Error writing temporary file. Please check disk space.");
            }
            totalRead += sz;
        } while (totalLen > totalRead);
    }

    _flushall();
    close(hDest);
    close(hSrc);
    return 0;
}

/* Function start: 0x421F90 */ /* DEMO ONLY - no full game match */
int CDData::ResolvePath(char* param_1) {
    char local_104[260];
    int len;

    CDData_FormatPath(param_1);
    CDData_SetResolvedPath(param_1);

    if (cdIdentifier[0] == 0) {
        return 0;
    }

    if (FileExists(field_190 + 0x45) != 0) {
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

    CopyFileContent(cdIdentifier + 5, field_190 + 0x45);
    _flushall();

    return 1;
}
