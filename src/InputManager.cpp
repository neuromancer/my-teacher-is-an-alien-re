#include <windows.h>
#include "InputManager.h"
#include "Timer.h"
#include "globals.h" // For globals like DAT_004373bc

extern "C" void* FUN_004249c0(int size); // AllocateMemory
extern "C" void FUN_00424940(void* ptr); // FreeMemory
extern "C" int* GetWindowWidth();  // FUN_004224e0
extern "C" int* GetWindowHeight(); // FUN_004224f0
extern "C" void FUN_00421ac0();    // InitKeyboardState
extern "C" int FUN_00421ce0(int, int, int, int); // Helper for joystick
extern "C" void FUN_004239e4(int*, int*); // Helper for mouse pos

extern "C" int ProcessMessages(); // 0x4192F0 - Message pump loop

extern Timer g_leftClickTimer;  // 0x43de40
extern Timer g_rightClickTimer; // 0x43de58

/* Function start: 0x421c24 */
MousePoint::~MousePoint() {
}

InputState::InputState() {
    x = 0;
    y = 0;
    memset(this, 0, sizeof(InputState));
}

InputState::~InputState() {
}

/* Function start: 0x4217C0 */
InputManager::InputManager(int param_1) {
    InitDevices(param_1);
}

InputManager::~InputManager() {
}

extern "C" int FUN_00421ae0(); // joyGetNumDevs wrapper

/* Function start: 0x421890 */
void InputManager::InitDevices(int param_1) {
    int screenWidth;
    int screenHeight;
    InputState* pState;
    JOYINFO joyInfo;

    memset(this, 0, 0x6e * 4);

    screenHeight = *GetWindowHeight();
    screenWidth = *GetWindowWidth();
    screenHeight = screenHeight - 1;
    screenWidth = screenWidth - 1;

    bounds.left = 0;
    bounds.top = 0;
    bounds.right = screenWidth;
    bounds.bottom = screenHeight;

    DAT_004373b8 = GetDoubleClickTime();

    pState = pMouseLocal;
    if (pState != 0) {
        if (pState != 0) {
            delete pState;
        }
        pMouseLocal = 0;
    }

    pState = new InputState();
    pMouseLocal = pState;
    pMouse = pState;

    pState = pJoystick;
    if (pState != 0) {
        delete pState;
        pJoystick = 0;
    }

    if (FUN_00421ae0() >= 1 && joyGetPos(0, &joyInfo) == 0) {
        pState = new InputState();
        pJoystick = pState;
        if (param_1 == 1) {
            pMouse = pState;
        }
        joyGetDevCapsA(0, &joyCaps, 0x194);
        joyCaps.wXmax = joyInfo.wXpos * 2;
        joyCaps.wYmax = joyInfo.wYpos * 2;
    }

    FUN_00421ac0();
}

/* Function start: 0x421b20 */
int InputManager::PollMouse(InputState* state) {
    int iVar1;
    MousePoint localPos;

    if (state == 0) {
        return 0;
    }

    state->prevButtons = state->buttons;
    localPos.x = 0;
    localPos.y = 0;
    FUN_004239e4(&localPos.x, &localPos.y);

    if ((localPos.x == -1) || (localPos.y == -1)) {
        mouseValid = 0;
    }
    else {
        iVar1 = bounds.left;
        if ((bounds.left <= localPos.x) && (iVar1 = bounds.right, localPos.x <= bounds.right)) {
            iVar1 = localPos.x;
        }
        localPos.x = iVar1;
        state->x = localPos.x;

        iVar1 = bounds.top;
        if ((bounds.top <= localPos.y) && (iVar1 = bounds.bottom, localPos.y <= bounds.bottom)) {
            iVar1 = localPos.y;
        }
        localPos.y = iVar1;
        state->y = localPos.y;

        mouseValid = 1;
    }

    state->buttons = 0;
    if (GetAsyncKeyState(1) != 0) {
        state->buttons = state->buttons | 1;
    }
    if (GetAsyncKeyState(2) != 0) {
        state->buttons = state->buttons | 2;
    }

    return 1;
}

/* Function start: 0x421c30 */
int InputManager::PollJoystick(InputState* state) {
    int iVar1;
    JOYINFO joyInfo;

    if (state == 0) {
        return 0;
    }

    state->prevButtons = state->buttons;
    joyGetPos(0, &joyInfo);

    iVar1 = FUN_00421ce0(joyInfo.wXpos, joyCaps.wXmin, joyCaps.wXmax, bounds.right);
    state->x = iVar1;

    iVar1 = FUN_00421ce0(joyInfo.wYpos, joyCaps.wYmin, joyCaps.wYmax, bounds.bottom);
    state->y = iVar1;

    state->buttons = joyInfo.wButtons;

    iVar1 = bounds.left;
    if ((state->x < iVar1) || (iVar1 = bounds.right, iVar1 < state->x)) {
        state->x = iVar1;
    }

    iVar1 = bounds.top;
    if ((state->y < iVar1) || (iVar1 = bounds.bottom, iVar1 < state->y)) {
        state->y = iVar1;
    }

    return 1;
}

/* Function start: 0x421d10 */
int InputManager::PollEvents(int param) {
    int iVar1;
    unsigned int uVar2;
    InputState* pState;

    iVar1 = ProcessMessages();
    if (iVar1 != 0) {
        return 1;
    }

    PollMouse(pMouseLocal);
    PollJoystick(pJoystick);

    pMouse->ext2 = 0;
    pMouse->ext1 = pMouse->ext2;

    pState = pMouse;
    uVar2 = 0;
    if (pState != 0) {
        uVar2 = pState->buttons & 1;
    }
    if (uVar2 != 0) {
        pState->ext1 = 1;
    }
    else {
        uVar2 = 0;
        if (pState != 0) {
            uVar2 = pState->buttons & 1;
        }
        if ((uVar2 == 0) && ((pState->prevButtons & 1) != 0)) {
            uVar2 = g_leftClickTimer.Update();
            if (uVar2 < (unsigned int)DAT_004373b8) {
                pMouse->ext1 = 3;
            }
            else {
                pMouse->ext1 = 2;
            }
            g_leftClickTimer.Reset();
        }
    }

    pState = pMouse;
    uVar2 = 0;
    if (pState != 0) {
        uVar2 = pState->buttons & 2;
    }
    if (uVar2 != 0) {
        pState->ext2 = 1;
        return 0;
    }
    uVar2 = 0;
    if (pState != 0) {
        uVar2 = pState->buttons & 2;
    }
    if ((uVar2 == 0) && ((pState->prevButtons & 2) != 0)) {
        uVar2 = g_rightClickTimer.Update();
        if (uVar2 < (unsigned int)DAT_004373b8) {
            pMouse->ext2 = 3;
        }
        else {
            pMouse->ext2 = 2;
        }
        g_rightClickTimer.Reset();
    }
    return 0;
}
