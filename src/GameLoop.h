#ifndef GAMELOOP_H
#define GAMELOOP_H

// GameLoop class
// Size: 0x1C (28 bytes)
// Used locally in RunGame for main game loop
class GameLoop {
public:
    GameLoop* Init();    // 0x417200
    void Run();          // 0x417690
    void ResetLoop();    // 0x417420
    void ProcessInput(); // 0x4177B0
    void Cleanup();      // 0x417320
    void DrawFrame();    // 0x417ED0
    
    int field_0x00;
    int field_0x04;
    int field_0x08;
    void* timer1;        // 0x0c - Timer pointer
    void* timer2;        // 0x10 - Timer pointer  
    void* eventList;     // 0x14 - List pointer
    int field_0x18;
};

#endif // GAMELOOP_H
