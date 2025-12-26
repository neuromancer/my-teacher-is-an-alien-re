#ifndef ENGINE_H
#define ENGINE_H

#include "Parser.h"

class Engine : public Parser {
public:
  // Parser fields 0x00-0x87
  // Engine fields start at 0x88
  char field_0x88[0x4c]; // 0x88 - 0xd3
  int field_0xd4;        // 0xd4
  int field_0xd8;        // 0xd8
  int field_0xdc;        // 0xdc
  char field_0xe0[0x8];  // 0xe0 - 0xe7

  Engine();

  void DisplayFrameRate();
};

#endif // ENGINE_H
