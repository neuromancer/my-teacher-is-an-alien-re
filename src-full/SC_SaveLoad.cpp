#include "SC_SaveLoad.h"
#include "T_MenuHotspot.h"
#include "Sprite.h"
#include "Palette.h"
#include "Memory.h"
#include <string.h>
#include <new>

/* Function start: 0x421A50 */
T_MenuHotspot::T_MenuHotspot(char* name, int* rect)
    : rect_left(0), rect_top(0), rect_right(0), rect_bottom(0)
{
    memset(this, 0, 42 * 4);

    if (name != 0) {
        sprite = new Sprite(name);
        sprite->handle = 2;
        sprite->loc_x = rect[0];
        sprite->loc_y = rect[1];
        sprite->SetState2(2);
        sprite->flags |= 0x40;
        sprite->SetRange(0, 1, 1, 1);
        sprite->SetRange(1, 2, 2, 1);
    }

    rect_left = rect[0];
    rect_top = rect[1];
    rect_right = rect[2];
    rect_bottom = rect[3];
}

/* Function start: 0x421BC0 */
void T_MenuHotspot::Update()
{
    if (sprite == 0) {
        return;
    }

    extern void* DAT_0046aa08;
    int mouseX = 0;
    int* pMouse = *(int**)((char*)DAT_0046aa08 + 0x1A0);
    if (pMouse != 0) {
        mouseX = pMouse[1];
    }
    int mouseY;
    if (pMouse != 0) {
        mouseY = pMouse[0];
    } else {
        mouseY = 0;
    }

    if (rect_left <= mouseY && rect_right >= mouseY &&
        rect_top <= mouseX && rect_bottom >= mouseX) {
        sprite->ResetAnimation(1, 0);
    } else {
        sprite->ResetAnimation(0, 0);
    }

    sprite->Do(sprite->loc_x, sprite->loc_y, 1.0);
}

/* Function start: 0x421C40 */
SC_SaveLoad::SC_SaveLoad()
{
    memset(&timer, 0, 0x30);
    handlerId = 0x2F;
    timer.Reset();

    sprite = new Sprite("mainmenu\\save_yn.smk");
    sprite->handle = 0;

    palette = new Palette("mainmenu\\menu.col");

    int rect[4];
    rect[0] = 0x9B;
    rect[1] = 0xFB;
    rect[2] = 0xEA;
    rect[3] = 0x11A;
    btnYes = new T_MenuHotspot("mainmenu\\ansr_y.smk", rect);

    rect[0] = 0x11D;
    rect[1] = 0xFD;
    rect[2] = 0x16C;
    rect[3] = 0x11B;
    btnNo = new T_MenuHotspot("mainmenu\\ansr_n.smk", rect);

    rect[0] = 0x19C;
    rect[1] = 0xFC;
    rect[2] = 0x1EB;
    rect[3] = 0x11B;
    btnCancel = new T_MenuHotspot("mainmenu\\ansr_c.smk", rect);
}

/* Function start: 0x421F80 */
SC_SaveLoad::~SC_SaveLoad()
{
    if (sprite != 0) {
        delete sprite;
        sprite = 0;
    }
    if (btnYes != 0) {
        delete btnYes;
        btnYes = 0;
    }
    if (btnNo != 0) {
        delete btnNo;
        btnNo = 0;
    }
    if (btnCancel != 0) {
        delete btnCancel;
        btnCancel = 0;
    }
}
