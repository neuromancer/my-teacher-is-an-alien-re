#ifndef JOYSTICKMANAGER_H
#define JOYSTICKMANAGER_H

class JoystickManager {
public:
    JoystickManager* Init(int param_1);
    int CheckInput(int);  // FUN_00421d10

private:
    char padding[0x1a4];  // Padding to offset 0x1a4
    int field_1a4;
    int field_1a8;
    int field_1ac;
    int field_1b0;
};

#endif // JOYSTICKMANAGER_H
