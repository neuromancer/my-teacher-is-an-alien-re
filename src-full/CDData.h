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
  ~CDData();

  void Setup(char *param_1, const char *param_2, const char *param_3); // 0x432FD0
  int ResolvePath(char* param_1);
};

#endif
