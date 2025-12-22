#ifndef OPTIONMENU_H
#define OPTIONMENU_H

#include "Sprite.h"

struct SpriteNode;

class OptionMenu : Parser {
public:
    Sprite* options[4]; // 0x88, 0x8c, 0x90, 0x94
    SpriteNode* spriteList; // 0x98
    int selected_option; // 0x9c

    void Init();
    ~OptionMenu();
    int LBLParse(char* command);
    void UpdateSpriteStates(int sprite_count, int sprite_index);
};

#endif // OPTIONMENU_H
