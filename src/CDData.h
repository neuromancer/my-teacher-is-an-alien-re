#ifndef CDDATA_H
#define CDDATA_H

#include "Parser.h"
#include "FileSystem.h"

class CDData : public Parser {
public:
  char baseDir[0x80];          // offset 0x90 from base (after Parser)
  char cdFolder[0x40];         // offset 0x110
  char cdIdentifier[0x40];    // offset 0x150
  char cdPath[0x110];           // offset 0x190

  CDData(char *param_1, const char *param_2, const char *param_3);
  ~CDData();

  int LBLParse(char* line);             // 0x4332E0

  void Setup(char *param_1, const char *param_2, const char *param_3); // 0x432FD0
  int ResolvePath(char* param_1);
};

int __cdecl CopyFileContent(const char* src, const char* dest);
int __cdecl DeleteFileAndDir(char* path);
void __cdecl DeleteMatchingFiles(char* pattern, ...);
char* FormatAssetPath(char* format, ...);
void LogCacheEntries();
void LogCacheStats();
FILE* __cdecl OpenSaveFile(char* path, char* mode);

char* __cdecl ResolveAssetPath(char* name, ...);
const char* __cdecl CDData_ResolvePath(const char* format, ...);
int __cdecl GetFreeDiskSpaceMB(int drive);
void __cdecl InitMemoryCache(int param_1, int param_2, float param_3);
void __cdecl FileCacheCleanup();
void __cdecl FileCacheEntryCleanup(void*, int);
#endif
