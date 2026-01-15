#ifndef ENGINE_SUBSYSTEMS_H
#define ENGINE_SUBSYSTEMS_H

#include "Parser.h"

// Stub classes for Engine subsystems (sizes match original)

class EngineInfoParser : public Parser {
public:
  char padding[0xa8 - sizeof(Parser)];
  EngineInfoParser() {}
};

class SceneManager {
public:
  char data[0x1c8];
  SceneManager() {}
};

class SpriteManager {
public:
  char data[0xb8];
  SpriteManager() {}
};

class DialogManager {
public:
  char data[0x2c];
  DialogManager() {}
};

class InputHandler {
public:
  char data[0x98];
  InputHandler() {}
};

class CursorManager {
public:
  char data[0x24];
  CursorManager() {}
};

class StateManager {
public:
  char data[0x4];
  StateManager() {}
};

#endif // ENGINE_SUBSYSTEMS_H
