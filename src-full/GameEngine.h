#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include "Timer.h"
#include "LinkedList.h"
#include "TimedEvent.h"

class Handler;
class SC_Message;
class SpriteAction;

// GameEngine - Main game loop controller
// Size: 0x28 (40 bytes = 10 dwords)
// Global: [0x0046a6ec] in full game binary
// Constructor: 0x430A00, Destructor: 0x430B90
//
// Drives the main game loop: processes timed events, dispatches messages
// to handlers, renders via g_ZBufferManager_0046aa24 (ZBufferManager), frame timing.
// This is NOT a ZBufferManager — it's a separate 0x28-byte class.
class GameEngine {
public:
    GameEngine();                   // 0x430A00
    ~GameEngine();                  // 0x430B90

    void Init();                    // 0x430C90
    void CleanupLoop();             // 0x430CC0
    void RunGameLoop();             // 0x430CD0
    void ProcessInput();            // 0x430E10
    int ProcessEvents();            // 0x430FA0
    void ProcessMessage(SC_Message* msg);          // 0x431030
    int ProcessControlMessage(SC_Message* msg);    // 0x431160
    void UpdateHandlers();          // 0x431210
    void HandleSystemMessage(SC_Message* msg);     // 0x431290
    int AddHandler(Handler* handler);              // 0x431560
    int RemoveHandler(int command);                // 0x4317C0
    Handler* GetOrCreateHandler(int command);      // 0x431880
    Handler* FindHandlerInList(int command);       // 0x4318B0
    void EnqueueAction(SpriteAction* action);      // 0x431A90

    int m_exitFlag;                 // 0x00 - non-zero to exit game loop
    int field_0x04;                 // 0x04
    Timer* m_timer1;                // 0x08 - frame timing timer
    Timer* m_timer2;                // 0x0C - secondary timer
    LinkedList* m_handlerList;      // 0x10 - list of active handlers (0x10 bytes)
    TimedEventPool* m_eventPool;    // 0x14 - event pool (0x18 bytes, pool_size=10)
    void* m_activeHandler;          // 0x18 - active handler (vtable dispatch target)
    unsigned int m_frameTime;       // 0x1C - frame time in ms (default 0x54 = 84ms)
    unsigned int m_frameTimeCopy;   // 0x20 - frame time copy
    void* m_smackHandle;            // 0x24 - Smacker handle for SmackWait
};

#endif // GAMEENGINE_H
