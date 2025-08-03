#ifndef OPTIONMENU_H
#define OPTIONMENU_H

#include "Sprite.h"

class OptionMenu : Parser {
public:
    Sprite* options[6]; // 0x88
    int selected_option; // 0xa0

    void Init();
    ~OptionMenu();
    int LBLParse(char* command);
    void UpdateSpriteStates(int sprite_count, int sprite_index);
};

#endif // OPTIONMENU_H
