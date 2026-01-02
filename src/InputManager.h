#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <windows.h>
#include <mmsystem.h>
#include "GlyphRect.h"

struct InputState {
    int x;          // 0x0
    int y;          // 0x4
    int buttons;    // 0x8
    int prevButtons;// 0xC
    int ext1;       // 0x10
    int ext2;       // 0x14
    InputState();
    ~InputState();
};

class InputManager {
public:
    InputManager(int param_1);
    ~InputManager();
    void InitDevices(int param_1); // FUN_00421890
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
    
    GlyphRect bounds;           // 0x1A4 - mouse bounds (left, top, right, bottom)
    int mouseValid;             // 0x1B4 - flag indicating if mouse position is valid
};

#endif // INPUTMANAGER_H
