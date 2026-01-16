#ifndef OPTIONMENU_H
#define OPTIONMENU_H

#include "Sprite.h"

struct SpriteNode;

class OptionMenu : Parser {
public:
    Sprite* options[3]; // 0x88, 0x8c, 0x90
    int field_94; // 0x94
    SpriteNode* spriteList; // 0x98
    int selected_option; // 0x9c

    OptionMenu();
    ~OptionMenu();
    int LBLParse(char* command);
    void UpdateSpriteStates(int sprite_count, int sprite_index);
    void Render(int characterIndex);
    void SetOptionState(int param_1, int param_2);
    void* GetOptionByIndex(int index);
    void SelectCharacter(int characterIndex);
    void Deactivate();
};

#endif // OPTIONMENU_H
