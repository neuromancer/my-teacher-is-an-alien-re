
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

extern void* DAT_0046aa1c; // CDData* for path resolution
extern int __cdecl GetFileSize(char*);
extern "C" int FileExists(const char*);
extern "C" void WriteToLog(const char*, ...);

// FileCache globals
#include "MemoryCache.h"
extern MemoryCache* DAT_0046b78c;  // cache pointer
extern int DAT_0046b784;   // cache hit counter
extern int DAT_0046b788;   // cache miss counter
extern int DAT_0046b790;   // cache eviction threshold
extern int DAT_00473440;   // total cached size
extern int DAT_00473444;   // cache size limit

extern "C" int __cdecl FUN_004260a0(char*);  // delete file + rmdir parent
extern "C" void FUN_00434520();              // cache error handler 1
extern "C" void FUN_004345b0();              // cache error handler 2
extern int DAT_004719c0;

/* Function start: 0x434030 */
void __cdecl FileCacheEntryCleanup(void* entries, int count) {
    int* ptr;
    char* name;

    if (count == 0) return;
    ptr = (int*)entries;
    do {
        count--;
        name = (char*)*ptr;
        if (name != 0) {
            if (FUN_004260a0(name) == -1) {
                FUN_00434520();
                FUN_004345b0();
                WriteToLog("HDCache::Unable to delete '%s' (%d)", name, DAT_004719c0);
            }
            FreeMemory(name);
            *ptr = 0;
        }
        ptr++;
    } while (count != 0);
}

/* Function start: 0x4344B0 */
void __cdecl FileCacheCleanup() {
    int* cache = (int*)DAT_0046b78c;
    if (cache == 0) return;

    int* node = (int*)cache[0];
    while (node != 0) {
        FileCacheEntryCleanup((void*)(node + 2), 1);
        node = (int*)node[0];
    }
    cache[2] = 0;
    cache[3] = 0;
    cache[1] = 0;
    cache[0] = 0;
    int* block = (int*)cache[4];
    while (block != 0) {
        int* next = (int*)block[0];
        FreeMemory(block);
        block = next;
    }
    cache[4] = 0;
    DAT_0046b788 = 0;
    DAT_0046b784 = 0;
}

/* Function start: 0x4341F0 */
int __cdecl FileCacheLookup(char* name) {
    int* node;

    if (DAT_0046b78c == 0) {
        return 0;
    }
    node = (int*)((int*)DAT_0046b78c)[0];
    while (node != 0) {
        int* next = (int*)node[0];
        char* entryName = (char*)node[2];
        if (strcmp(entryName, name) == 0) {
            DAT_0046b784++;
            *(int*)((char*)entryName + 0x24) += 1;
            *(int*)((char*)entryName + 0x28) = GetTickCount();
            // Move to front of LRU
            if (((int*)DAT_0046b78c)[0] != (int)node) {
                int* prev = (int*)node[1];
                int* nxt = (int*)node[0];
                prev[0] = (int)nxt;
                if (nxt != 0) {
                    nxt[1] = (int)prev;
                } else {
                    ((int*)DAT_0046b78c)[1] = (int)prev;
                }
                node[0] = ((int*)DAT_0046b78c)[0];
                *(int*)(((int*)DAT_0046b78c)[0] + 4) = (int)node;
                ((int*)DAT_0046b78c)[0] = (int)node;
                node[1] = 0;
            }
            return 1;
        }
        node = next;
    }
    DAT_0046b788++;
    return 0;
}

/* Function start: 0x4342D0 */
void __cdecl FileCacheRegister(char* name, int size) {
    int* cache;
    int* node;
    char* entry;

    if (DAT_0046b78c == 0) return;

    DAT_00473440 += size;

    // Evict if over limit
    if (DAT_00473444 != 0 && DAT_00473444 <= DAT_00473440) {
        do {
            cache = (int*)DAT_0046b78c;
            node = (int*)cache[1]; // tail
            entry = (char*)node[2];
            int entrySize = *(int*)(entry + 0x20);
            DAT_00473440 -= entrySize;
            if (DAT_00473440 <= 0) DAT_00473440 = 0;

            // Remove from tail
            int* prev = (int*)node[1];
            cache[1] = (int)prev;
            if (prev != 0) {
                prev[0] = 0;
            } else {
                cache[0] = 0;
            }

            FileCacheEntryCleanup(entry, 1);

            // Return node to free list
            node[0] = cache[3];
            cache[3] = (int)node;
            cache[2]--;
        } while (cache[2] > 0 && DAT_0046b790 < DAT_00473440);
    }

    // Allocate new entry (0x2C bytes)
    entry = (char*)operator new(0x2C);
    if (entry != 0) {
        strcpy(entry, name);
        *(int*)(entry + 0x20) = size;
        *(int*)(entry + 0x24) = 0;
        *(int*)(entry + 0x28) = GetTickCount();
    }

    // Get a free node from pool
    cache = (int*)DAT_0046b78c;
    if (cache[3] == 0) {
        // Allocate new block of nodes
        int blockSize = cache[5];
        int* block = (int*)operator new(blockSize * 12 + 4);
        block[0] = cache[4];
        cache[4] = (int)block;
        int cnt = blockSize - 1;
        int* p = block + blockSize * 3 - 2;
        while (cnt >= 0) {
            p[0] = cache[3];
            cache[3] = (int)p;
            p -= 3;
            cnt--;
        }
    }
    node = (int*)cache[3];
    cache[3] = node[0];
    node[1] = 0;
    node[0] = (int)(int*)cache[0]; // was head
    cache[2]++;
    node[2] = 0;
    {
        int dummy = 0;
        do { int tmp = dummy; dummy--; if (tmp == 0) break; } while (1);
    }
    node[2] = (int)entry;

    if (cache[0] != 0) {
        *(int*)(cache[0] + 4) = (int)node;
    } else {
        cache[1] = (int)node;
    }
    cache[0] = (int)node;
}

extern "C" char* FormatAssetPath(char* format, ...);
extern "C" int __cdecl CopyFileContent(const char*, const char*);

/* Function start: 0x433230 */
int CDData::ResolvePath(char* name) {
    char drive[260];
    char pathBuf[260];

    FormatAssetPath(name);
    if (field_190[0] == 0) {
        return 0;
    }
    if (FileExists(name) != 0) {
        return 0;
    }

    _splitpath(name, 0, pathBuf, 0, drive);
    if (_chdir(pathBuf) != 0) {
        _mkdir(drive);
    } else {
        _chdir("C:\\");
    }
    CopyFileContent(name, (char*)((int)this + 0x195));
    return 1;
}

/* Function start: 0x4260F0 */
extern "C" char* FormatAssetPath(char* format, ...)
{
    char localPath[260];

    vsprintf(localPath, format, (char*)&format + 4);

    if (FileExists(localPath) != 0) {
        strcpy((char*)DAT_0046aa1c + 0x195, localPath);
    } else {
        sprintf((char*)DAT_0046aa1c + 0x195, "%s%s", (char*)DAT_0046aa1c + 0x190, localPath);
    }
    return (char*)DAT_0046aa1c + 0x195;
}

/* Function start: 0x426190 */
char* ResolveAssetPath(char* name) {
    char* basePath = (char*)((int)DAT_0046aa1c + 0x21a);
    sprintf(basePath, "%s", name);

    if (FileCacheLookup(basePath) != 0) {
        return basePath;
    }
    if (FileExists(basePath) != 0) {
        return basePath;
    }

    char* resolved = (char*)FormatAssetPath(basePath);
    int size = GetFileSize(resolved);
    if (size == -1) {
        resolved = (char*)FormatAssetPath(basePath);
        WriteToLog("missing file %s", resolved);
        return basePath;
    }
    FileCacheRegister(basePath, size);
    ((CDData*)DAT_0046aa1c)->ResolvePath(basePath);
    return basePath;
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

// FUN_00421F90 = CDData::ResolvePath (DEMO ONLY) — replaced by 0x433230 full game version above

CDData::~CDData() {}
