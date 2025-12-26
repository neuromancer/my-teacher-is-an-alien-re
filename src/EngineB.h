#ifndef ENGINEB_H
#define ENGINEB_H

#include "Parser.h"

struct SubObj16 {
  int field_0;
  int field_4;
  int field_8;
  int field_c;
};

struct SubObj8 {
  int field_0;
  int field_4;
};

// Note: EngineB inherits from Parser only to match vtable structure,
// but manually calls Engine constructor (FUN_004110d0)
class EngineB : public Parser {
public:
  // Parser fields 0x00-0x87
  // Engine fields 0x88-0xe7 (inherited by composition, not inheritance)
  char field_0x88[0x60]; // Engine's fields
  // EngineB-specific fields
  int field_0xe8[0x10]; // 0xe8-0x127 (first part of memset area)
  void *field_0x128;    // 0x128 - pointer to object with vtable
  int field_0x12c;      // 0x12c - rest of memset area
  SubObj16 field_0x130; // 0x130-0x13f
  SubObj16 field_0x140; // 0x140-0x14f
  SubObj8 field_0x150;  // 0x150-0x157
  SubObj8 field_0x158;  // 0x158-0x15f
  int field_0x160[2];   // 0x160-0x167 (end of memset area)

  EngineB();
  ~EngineB();

  void DestructorHelper();
};

#endif // ENGINEB_H
