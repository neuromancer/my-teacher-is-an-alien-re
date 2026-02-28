#ifndef CDDATA_H
#define CDDATA_H

#include "Parser.h"
#include "FileSystem.h"

class CDData : public Parser {
public:
  char baseDir[0x80];          // offset 0x90 from base (after Parser)
  char cdFolder[0x40];         // offset 0x110
  char cdIdentifier[0x40];    // offset 0x150
  char field_190[0x110];       // offset 0x190

  CDData(char *param_1, const char *param_2, const char *param_3);
  CDData(char *param_1, char *param_2);
  ~CDData();

  void Setup(char *param_1, const char *param_2, const char *param_3); // 0x432FD0
  int CheckFileOnDrive(int drive_letter);
  int ChangeToDriveDirectory(int drive_letter);
  int ChangeDirectory(unsigned char *path);
  int ResolvePath(char* param_1);
};

#endif
