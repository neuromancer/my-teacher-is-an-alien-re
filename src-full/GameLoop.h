#ifndef GAMELOOP_H
#define GAMELOOP_H

class SC_Message;
class Timer;
class Handler;
#include "LinkedList.h"
#include "Parser.h"

// GameLoop class - full game version
// In the full game, GameLoop inherits from Parser (unlike the demo)
// and is parsed with "mis\\start.mis" via ParseFile
class GameLoop : public Parser {
public:
    GameLoop();
    ~GameLoop();
    void Run();
    void ResetLoop();
    void ProcessInput();
    void Cleanup();
    void DrawFrame();
    int UpdateGame();
    void CleanupLoop();
    void ProcessMessage(SC_Message* msg);
    void HandleSystemMessage(SC_Message* msg);
    int ProcessControlMessage(SC_Message* msg);
    int AddHandler(void* handler);
    int FindHandlerInEventList(int command);
    Handler* GetOrCreateHandler(int command);
    int RemoveHandler(int command);

    int field_0x00;          // after Parser fields
    int field_0x04;
    int field_0x08;
    Timer* timer1;
    Timer* timer2;
    EventList* eventList;
    Handler* currentHandler;
};

#endif // GAMELOOP_H
