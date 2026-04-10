#ifndef ENGINEA_H
#define ENGINEA_H

#include "Engine.h"

// EngineA inherits from Engine (vtable 0x431410, size 0xe8)
// Used for exploration/ductwork scenes (DUCTWORK.MIS)
// Same size as Engine - no additional fields
//
// Overrides from Engine:
//   [0] LBLParse: 0x00412ae0
//   [3] destructor: 0x00412a70
class EngineA : public Engine {
public:
  EngineA();
  virtual ~EngineA();

  // Virtual method overrides
  virtual int LBLParse(char* line);  // vtable[0] 0x412ae0
};

#endif // ENGINEA_H
