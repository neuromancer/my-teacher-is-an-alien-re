#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <windows.h>
#include <mmsystem.h>

struct InputState {
    int x;          // 0x0
    int y;          // 0x4
    int buttons;    // 0x8
    int prevButtons;// 0xC
    int ext1;       // 0x10
    int ext2;       // 0x14
};

class InputManager {
public:
    InputManager* Init(int param_1);
    int PollEvents(int); // Was CheckInput / FUN_00421d10
    
// private:
    // Helper functions
    int PollMouse(InputState* state); // FUN_00421b20
    int PollJoystick(InputState* state); // FUN_00421c30

    union {
        JOYCAPSA joyCaps;
        char joyCapsBuffer[0x198];
    };

    InputState* pJoystick;      // 0x198
    InputState* pMouseLocal;    // 0x19C
    InputState* pMouse;         // 0x1A0
    
    int field_1a4;
    int field_1a8;
    int field_1ac;
    int field_1b0;
};

#endif // INPUTMANAGER_H
