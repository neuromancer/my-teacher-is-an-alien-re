
#include "CDData.h"
#include "stubs.h"
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
unsigned long __stdcall timeGetTime(void);
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
#include "Memory.h"

// Forward declarations (defined below)
extern "C" int __cdecl DeleteFileAndDir(char*);
extern "C" void LogCacheStats();
extern "C" void LogCacheEntries();

/* Function start: 0x434030 */
void __cdecl FileCacheEntryCleanup(void* entries, int count) {
    int saved = count;
    count--;
    if (saved == 0) return;
    FileCacheEntry** slot = (FileCacheEntry**)entries;
    do {
        FileCacheEntry* entry = *slot;
        if (entry != 0) {
            if (DeleteFileAndDir(entry->name) == -1) {
                LogCacheStats();
                LogCacheEntries();
                WriteToLog("HDCache::Unable to delete '%s' (errno=%d)", entry->name, g_FileDeleteError_004719c0);
            }
            FreeMemory(entry);
            *slot = 0;
        }
        slot++;
        saved = count;
        count--;
    } while (saved != 0);
}

/* Function start: 0x4344B0 */
void __cdecl FileCacheCleanup() {
    if (g_FileCache_0046b78c == 0) return;
    MemoryCache* cache = g_FileCache_0046b78c;

    CacheNode* node = cache->head;
    while (node != 0) {
        FileCacheEntryCleanup(&node->entry, 1);
        node = node->next;
    }
    cache->count = 0;
    cache->freeList = 0;
    cache->tail = 0;
    cache->head = 0;
    int* block = cache->blockList;
    while (block != 0) {
        int* next = (int*)block[0];
        FreeMemory(block);
        block = next;
    }
    cache->blockList = 0;
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
        g_FileCache_0046b78c->count, g_CacheEntryCount_0046b780, g_CacheTotalSize_00473440, g_CacheSizeLimit_00473444);
    WriteToMessageLog("Hits=%d Misses = %d", g_PathCacheHits_0046b784, g_PathCacheMisses_0046b788);
}

/* Function start: 0x4345B0 */
extern "C" void LogCacheEntries() {
    if (g_FileCache_0046b78c == 0) {
        WriteToMessageLog("HDCache::LogCache() - HDCache not initialized");
        return;
    }
    int idx = 1;
    CacheNode* node = g_FileCache_0046b78c->head;
    if (node != 0) {
        WriteToMessageLog("     %-20.20s %6.6s %10.10s %4.4s",
            "FILE", "FSIZE", "BSIZE", "HITS");
    } else {
        WriteToMessageLog("HDCache::LogCache() - No items in Cache");
    }
    if (node != 0) {
        do {
            FileCacheEntry* entry = node->entry;
            node = node->next;
            WriteToMessageLog("%4d %-20.20s %6.6lu %10lu %4.4d",
                idx, entry->name,
                (unsigned long)entry->size,
                (unsigned long)entry->tickTime,
                entry->hitCount);
            idx++;
        } while (node != 0);
    }
}

/* Function start: 0x4341F0 */
int __cdecl FileCacheLookup(char* name) {
    if (g_FileCache_0046b78c == 0) {
        return 0;
    }
    CacheNode* volatile current = g_FileCache_0046b78c->head;

    while (current != 0) {
        CacheNode* node = current;
        FileCacheEntry* entry = current->entry;
        current = current->next;
        if (strcmp(entry->name, name) == 0) {
            g_PathCacheHits_0046b784++;
            entry->hitCount++;
            entry->tickTime = timeGetTime();
            // Move to front of LRU
            MemoryCache* cache = g_FileCache_0046b78c;
            if (cache->head != node) {
                node->prev->next = node->next;
                if (node->next != 0) {
                    node->next->prev = node->prev;
                } else {
                    cache->tail = node->prev;
                }
                node->next = cache->head;
                cache->head->prev = node;
                cache->head = node;
                node->prev = 0;
            }
            return 1;
        }
    }
    g_PathCacheMisses_0046b788++;
    return 0;
}

/* Function start: 0x4342D0 */
void __cdecl FileCacheRegister(char* name, int size) {
    if (g_FileCache_0046b78c == 0) return;

    g_CacheTotalSize_00473440 += size;

    // Evict LRU entries while over the size limit
    MemoryCache* cache;
    if (g_CacheSizeLimit_00473444 != 0 && g_CacheSizeLimit_00473444 <= g_CacheTotalSize_00473440) {
        do {
            cache = g_FileCache_0046b78c;
            CacheNode* evictNode = cache->tail;
            FileCacheEntry* evictEntry = evictNode->entry;
            g_CacheTotalSize_00473440 -= evictEntry->size;
            if (g_CacheTotalSize_00473440 <= 0) g_CacheTotalSize_00473440 = 0;

            cache = g_FileCache_0046b78c;
            CacheNode* prev = evictNode->prev;
            cache->tail = prev;
            if (prev != 0) {
                prev->next = 0;
            } else {
                cache->head = 0;
            }

            FileCacheEntryCleanup(&evictNode->entry, 1);

            // Return node to free list
            evictNode->next = cache->freeList;
            cache->freeList = evictNode;
            cache->count--;
        } while (cache->count > 0 && g_CacheEvictThreshold_0046b790 < g_CacheTotalSize_00473440);
    }

    // Allocate new entry
    FileCacheEntry* entry = (FileCacheEntry*)operator new(sizeof(FileCacheEntry));
    if (entry != 0) {
        strcpy(entry->name, name);
        entry->size = size;
        entry->hitCount = 0;
        entry->tickTime = timeGetTime();
    }

    // Get a free node from the pool
    cache = g_FileCache_0046b78c;
    if (cache->freeList == 0) {
        // Allocate new block of nodes (first dword links to next block)
        int blockSize = cache->poolCapacity;
        int* block = (int*)operator new(blockSize * sizeof(CacheNode) + sizeof(int));
        block[0] = (int)cache->blockList;
        cache->blockList = block;
        // Chain nodes onto the free list, last-first
        int cnt = blockSize - 1;
        CacheNode* p = (CacheNode*)(block + blockSize * 3 - 2);
        while (cnt >= 0) {
            p->next = cache->freeList;
            cache->freeList = p;
            p = (CacheNode*)((char*)p - sizeof(CacheNode));
            cnt--;
        }
    }
    CacheNode* node = cache->freeList;
    cache->freeList = node->next;
    node->prev = 0;
    node->next = cache->head;
    cache->count++;
    node->entry = 0;
    {
        int dummy = 0;
        do { int tmp = dummy; dummy--; if (tmp == 0) break; } while (1);
    }
    node->entry = entry;

    if (cache->head != 0) {
        cache->head->prev = node;
    } else {
        cache->tail = node;
    }
    cache->head = node;
}

/* Function start: 0x4332E0 */
int CDData::LBLParse(char* line) {
    char token[24];
    char arg1[16];
    char path[128];

    token[0] = 0;
    path[0] = 0;

    sscanf(line, " %s ", token);
    if (strcmp(token, "COPY_TO_HD") == 0) {
        sscanf(line, "%s %s", arg1, path);
        ResolvePath(path);
        return 0;
    }
    if (strcmp(token, "END") == 0) {
        return 1;
    }
    ReportUnknownLabel("GameDirectory");
    return 0;
}

extern "C" char* FormatAssetPath(char* format, ...);
extern "C" int __cdecl CopyFileContent(const char*, const char*);

/* Function start: 0x433230 */
int CDData::ResolvePath(char* name) {
    char drive[256];
    char pathBuf[260];

    FormatAssetPath(name);
    if (cdPath[0] == 0) {
        return 0;
    }
    if (FileExists(name) != 0) {
        return 0;
    }

    _splitpath(name, 0, pathBuf, drive, 0);
    if (_chdir(pathBuf) != 0) {
        _mkdir(drive);
    } else {
        _chdir("..");
    }
    CopyFileContent((char*)((int)this + 0x195), name);
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
    return fsopen(path, mode);
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
char* __cdecl ResolveAssetPath(char* name, ...) {
    char* basePath = g_PathResolver_0046aa1c->cdPath + 0x8a;
    vsprintf(basePath, name, (char*)&name + 4);

    if (FileCacheLookup(basePath) == 0 && FileExists(basePath) == 0) {
        char* resolved = (char*)FormatAssetPath(basePath);
        int size = GetFileSize(resolved);
        if (size == -1) {
            resolved = (char*)FormatAssetPath(basePath);
            WriteToLog("HDFile:: - Unable to find file '%s'", resolved);
            return basePath;
        }
        FileCacheRegister(basePath, size);
        g_PathResolver_0046aa1c->ResolvePath(basePath);
    }
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
        ShowError("CopyFile::Error Reading CD ROM, searching for %s", src);
    }

    hDest = open(dest, 0x8301, 0x180);
    if (hDest < 0) {
        ShowError("CopyFile::Error writing temporary file. Please check disk space.%d", hDest);
    }

    totalLen = filelength(hSrc);
    if (totalLen >= 0x2000) {
        chunk = 0x2000;
    } else {
        chunk = totalLen;
    }

    buf = g_Buffer_0046aa04;
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
