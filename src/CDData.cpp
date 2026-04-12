
#include "CDData.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <direct.h>
#include <io.h>
#include <share.h>
#include "string.h"
#include "FileSystem.h"
#include "globals.h"

extern "C" {
void ParsePath(const char *, char *, char *, char *, char *);
int __cdecl FileExists(const char* path);           // 0x4195A0
int __cdecl CopyFileContent(const char* src, const char* dest); // 0x419660
}
void ShowError(const char* format, ...);

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
    ParsePath(local_80, cdPath, 0, 0, 0);
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
        ParsePath(local_80, cdPath, 0, 0, 0);
        if (cdPath[0] == baseDir[0]) {
          ShowError("Please run Setup.exe");
        }
        sprintf(local_80, "%s\\CDDATA\\DATA", cdPath);
        if (chdir(local_80) != 0) {
          ShowError("%s\nInvalid CD in Drive-'%s'\nCan't find subdir '%s'",
                    param_3, cdPath, local_80);
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

extern int __cdecl GetFileSize(char*);
extern "C" int FileExists(const char*);
extern "C" void WriteToLog(const char*, ...);

// FileCache globals
#include "MemoryCache.h"

// Forward declarations (defined below)
extern "C" int __cdecl DeleteFileAndDir(char*);
extern "C" void LogCacheStats();
extern "C" void LogCacheEntries();

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
            if (DeleteFileAndDir(name) == -1) {
                LogCacheStats();
                LogCacheEntries();
                WriteToLog("HDCache::Unable to delete '%s' (%d)", name, g_FileDeleteError_004719c0);
            }
            FreeMemory(name);
            *ptr = 0;
        }
        ptr++;
    } while (count != 0);
}

/* Function start: 0x4344B0 */
void __cdecl FileCacheCleanup() {
    int* cache = (int*)g_FileCache_0046b78c;
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
    g_PathCacheMisses_0046b788 = 0;
    g_PathCacheHits_0046b784 = 0;
}

/* Function start: 0x434520 */
extern "C" void LogCacheStats() {
    if (g_FileCache_0046b78c == 0) {
        WriteToMessageLog("HDCache::LogStats() - HDCache not initialized");
        return;
    }
    WriteToMessageLog("***********************");
    WriteToMessageLog("HD File Cache");
    WriteToMessageLog("PreLoading Cache  = %d ms", g_CachePreloadTime_0046b794);
    WriteToMessageLog("Cache Entries  = %d of %d (%d of %d Bytes)",
        g_FileCache_0046b78c->field_8, g_CacheEntryCount_0046b780, g_CacheTotalSize_00473440, g_CacheSizeLimit_00473444);
    WriteToMessageLog("Hits=%d Misses = %d", g_PathCacheHits_0046b784, g_PathCacheMisses_0046b788);
}

/* Function start: 0x4345B0 */
extern "C" void LogCacheEntries() {
    int* node;
    int idx;

    if (g_FileCache_0046b78c == 0) {
        WriteToMessageLog("HDCache::LogCache() - HDCache not initialized");
        return;
    }
    idx = 1;
    node = (int*)((int*)g_FileCache_0046b78c)[0];
    if (node != 0) {
        WriteToMessageLog("     %-20.20s %6.6s %10.10s %4.4s",
            "FILE", "FSIZE", "BSIZE", "HITS");
    } else {
        WriteToMessageLog("HDCache::LogCache() - No items in Cache");
    }
    if (node != 0) {
        do {
            int* entry = (int*)node[2];
            node = (int*)node[0];
            WriteToMessageLog("%4d %-20.20s %6.6lu %10lu %4.4d",
                idx, entry, *(unsigned long*)((char*)entry + 0x20),
                *(unsigned long*)((char*)entry + 0x28),
                *(int*)((char*)entry + 0x24));
            idx++;
        } while (node != 0);
    }
}

/* Function start: 0x4341F0 */
int __cdecl FileCacheLookup(char* name) {
    int* current;
    int* node;
    char* entryName;

    if (g_FileCache_0046b78c == 0) {
        return 0;
    }
    current = (int*)((int*)g_FileCache_0046b78c)[0];

    while (current != 0) {
        node = current;
        entryName = (char*)current[2];
        current = (int*)current[0];
        if (strcmp(entryName, name) == 0) {
            g_PathCacheHits_0046b784++;
            *(int*)(entryName + 0x24) += 1;
            *(int*)(entryName + 0x28) = GetTickCount();
            // Move to front of LRU
            int* cache = (int*)g_FileCache_0046b78c;
            if (cache[0] != (int)node) {
                *(int*)node[1] = node[0];
                if (node[0] != 0) {
                    *(int*)(node[0] + 4) = node[1];
                } else {
                    cache[1] = node[1];
                }
                node[0] = cache[0];
                *(int*)(cache[0] + 4) = (int)node;
                cache[0] = (int)node;
                node[1] = 0;
            }
            return 1;
        }
    }
    g_PathCacheMisses_0046b788++;
    return 0;
}

/* Function start: 0x4342D0 */
void __cdecl FileCacheRegister(char* name, int size) {
    int* cache;
    int* node;
    char* entry;

    if (g_FileCache_0046b78c == 0) return;

    g_CacheTotalSize_00473440 += size;

    // Evict if over limit
    if (g_CacheSizeLimit_00473444 != 0 && g_CacheSizeLimit_00473444 <= g_CacheTotalSize_00473440) {
        do {
            cache = (int*)g_FileCache_0046b78c;
            node = (int*)cache[1]; // tail
            entry = (char*)node[2];
            int entrySize = *(int*)(entry + 0x20);
            g_CacheTotalSize_00473440 -= entrySize;
            if (g_CacheTotalSize_00473440 <= 0) g_CacheTotalSize_00473440 = 0;

            // Remove tail node
            cache = (int*)g_FileCache_0046b78c;
            int* prev = (int*)node[1];
            cache[1] = (int)prev;
            if (prev != 0) {
                prev[0] = 0;
            } else {
                cache[0] = 0;
            }

            FileCacheEntryCleanup((void*)(node + 2), 1);

            // Return node to free list
            node[0] = cache[3];
            cache[3] = (int)node;
            cache[2]--;
        } while (cache[2] > 0 && g_CacheEvictThreshold_0046b790 < g_CacheTotalSize_00473440);
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
    cache = (int*)g_FileCache_0046b78c;
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
    if (cdPath[0] == 0) {
        return 0;
    }
    if (FileExists(name) != 0) {
        return 0;
    }

    _splitpath(name, 0, pathBuf, 0, drive);
    if (_chdir(pathBuf) != 0) {
        _mkdir(drive);
    } else {
        _chdir("..");
    }
    CopyFileContent(name, (char*)((int)this + 0x195));
    return 1;
}

/* Function start: 0x426050 */
extern "C" FILE* __cdecl OpenSaveFile(char* path, char* mode) {
    char dir[64];
    dir[0] = 0;
    _splitpath(path, 0, dir, 0, 0);
    if (dir[0] != 0) {
        _mkdir(dir);
    }
    return _fsopen(path, mode, _SH_DENYNO);
}

/* Function start: 0x4260A0 */
extern "C" int __cdecl DeleteFileAndDir(char* path) {
    char dir[64];
    int result;

    result = remove(path);
    if (result == 0) {
        dir[0] = 0;
        _splitpath(path, 0, dir, 0, 0);
        if (dir[0] != 0) {
            _rmdir(dir);
        }
    }
    return result;
}

/* Function start: 0x4260F0 */
extern "C" char* FormatAssetPath(char* format, ...)
{
    char localPath[260];

    vsprintf(localPath, format, (char*)&format + 4);

    if (FileExists(localPath) != 0) {
        strcpy(g_PathResolver_0046aa1c->cdPath + 5, localPath);
    } else {
        sprintf(g_PathResolver_0046aa1c->cdPath + 5, "%s%s", g_PathResolver_0046aa1c->cdPath, localPath);
    }
    return g_PathResolver_0046aa1c->cdPath + 5;
}

/* Function start: 0x426190 */
char* ResolveAssetPath(char* name) {
    char* basePath = g_PathResolver_0046aa1c->cdPath + 0x8a;
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
    g_PathResolver_0046aa1c->ResolvePath(basePath);
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

#include "FileSystem.h"

/* Function start: 0x426330 */
extern "C" void __cdecl DeleteMatchingFiles(char* pattern, ...) {
    char resolved[260];
    char dir[64];
    char fullpath[64];
    struct _finddata_t findData;

    vsprintf(resolved, pattern, (char*)(&pattern + 1));
    char* formatted = FormatAssetPath(resolved);
    int handle = _findfirst(formatted, &findData);
    if (handle != -1) {
        _splitpath(resolved, 0, dir, 0, 0);
        do {
            sprintf(fullpath, "%s%s", dir, findData.name);
            ResolveAssetPath(fullpath);
        } while (_findnext(handle, &findData) == 0);
    }
}

CDData::~CDData() {}
