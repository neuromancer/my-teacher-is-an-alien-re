#ifndef OPTIONMENU_H
#define OPTIONMENU_H

#include "Sprite.h"

class OptionMenu {
public:
    void** vtable; // 0x0
    char pad[0x88];
    Sprite* options[6]; // 0x88
    int selected_option; // 0xa0

    void Init();
    ~OptionMenu();
    virtual int ParseCommand(char* command);
    void UpdateSpriteStates(int sprite_count, int sprite_index);
};

#endif // OPTIONMENU_H
