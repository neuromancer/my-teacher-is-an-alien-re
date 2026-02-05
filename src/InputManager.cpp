#include <windows.h>
#include "InputManager.h"
#include "Timer.h"
#include "Memory.h"
#include "globals.h" // For globals like DAT_004373bc

extern "C" int* GetWindowWidth();  // FUN_004224e0
extern "C" int* GetWindowHeight(); // FUN_004224f0
// Forward declarations for functions implemented below
void InitClickTimers();      // 0x421AC0
int GetJoystickCount();      // 0x421AE0
int MapJoystickValue(int value, int min, int max, int range); // 0x421CE0
extern "C" int GetMousePosition(int*, int*); // 0x4239E4 - in Graphics.cpp

extern "C" int ProcessMessages(); // 0x4192F0 - Message pump loop

Timer g_leftClickTimer;  // 0x43de40
Timer g_rightClickTimer; // 0x43de58

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

    g_DoubleClickTime_004373b8 = GetDoubleClickTime();

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

    if (GetJoystickCount() >= 1 && joyGetPos(0, &joyInfo) == 0) {
        pState = new InputState();
        pJoystick = pState;
        if (param_1 == 1) {
            pMouse = pState;
        }
        joyGetDevCapsA(0, &joyCaps, 0x194);
        joyCaps.wXmax = joyInfo.wXpos * 2;
        joyCaps.wYmax = joyInfo.wYpos * 2;
    }

    InitClickTimers();
}

/* Function start: 0x421AC0 */
void InitClickTimers()
{
    g_leftClickTimer.Reset();
    g_rightClickTimer.Reset();
}

/* Function start: 0x421AE0 */
int GetJoystickCount()
{
    return joyGetNumDevs();
}

/* Function start: 0x421b20 */
int InputManager::PollMouse(InputState* state) {
    if (state == 0) {
        return 0;
    }

    state->prevButtons = state->buttons;
    
    MousePoint localPos;
    localPos.x = 0;
    localPos.y = 0;
    
    GetMousePosition(&localPos.x, &localPos.y);

    if (localPos.x == -1 || localPos.y == -1) {
        mouseValid = 0;
    } else {
        if (bounds.left > localPos.x) {
            localPos.x = bounds.left;
        } else if (bounds.right < localPos.x) {
            localPos.x = bounds.right;
        }
        state->x = localPos.x;

        if (bounds.top > localPos.y) {
            localPos.y = bounds.top;
        } else if (bounds.bottom < localPos.y) {
            localPos.y = bounds.bottom;
        }
        state->y = localPos.y;

        mouseValid = 1;
    }

    SHORT (WINAPI *pGetAsyncKeyState)(int) = GetAsyncKeyState;
    state->buttons = 0;
    if (pGetAsyncKeyState(1)) {
        state->buttons |= 1;
    }
    if (pGetAsyncKeyState(2)) {
        state->buttons |= 2;
    }

    return 1;
}

/* Function start: 0x421c24 */
MousePoint::~MousePoint() {
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

    iVar1 = MapJoystickValue(joyInfo.wXpos, joyCaps.wXmin, joyCaps.wXmax, bounds.right);
    state->x = iVar1;

    iVar1 = MapJoystickValue(joyInfo.wYpos, joyCaps.wYmin, joyCaps.wYmax, bounds.bottom);
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

/* Function start: 0x421CE0 */
int MapJoystickValue(int value, int min, int max, int range)
{
    int result = (range * (value - min)) / (max - min);
    if (result < 0) {
        result = 0;
    }
    else if (range < result) {
        return range;
    }
    return result;
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
            if (uVar2 < (unsigned int)g_DoubleClickTime_004373b8) {
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
        if (uVar2 < (unsigned int)g_DoubleClickTime_004373b8) {
            pMouse->ext2 = 3;
        }
        else {
            pMouse->ext2 = 2;
        }
        g_rightClickTimer.Reset();
    }
    return 0;
}
