#ifndef GAMELOOP_H
#define GAMELOOP_H

#include "Parser.h"

class SpriteAction;
class Handler;

// GameLoop class - full game version
// Extends Parser, constructed on the stack in RunGame (0x4236F0).
// Constructor is inlined: Parser::Parser() + vtable set to 0x461458.
// Parses "mis\\start.mis" via ParseFile.
// Size: 0x90 (same as Parser, no extra fields)
class GameLoop : public Parser {
public:
    GameLoop() {}
    ~GameLoop();                        // 0x424050
    int LBLParse(char* param_1);        // 0x4240A0
};

// Free function: handler factory used by GameEngine
Handler* CreateHandler(int command);    // 0x424240

#endif // GAMELOOP_H
