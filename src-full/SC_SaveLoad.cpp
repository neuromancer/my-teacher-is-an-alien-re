#include "SC_SaveLoad.h"
#include "T_MenuHotspot.h"
#include "Sprite.h"
#include "Palette.h"
#include "Memory.h"
#include <string.h>
#include <new>

/* Function start: 0x420CE0 */
T_MenuHotspot::T_MenuHotspot(int param) : Parser()
{
    memset(&enabled, 0, 0x46 * 4);
    field_98 = param;
}

/* Function start: 0x420D90 */
T_MenuHotspot::~T_MenuHotspot()
{
    if (cursor != 0) {
        cursor->~Sprite();
        FreeMemory(cursor);
        cursor = 0;
    }
}

/* Function start: 0x421A50 */
T_MenuHotspot::T_MenuHotspot(char* name, int* rect)
    : bounds(0, 0, 0, 0)
{
    memset(&enabled, 0, 42 * 4);

    if (name != 0) {
        Sprite* spr = new Sprite(name);
        cursor = spr;
        spr->handle = 2;
        spr->loc_x = rect[0];
        spr->loc_y = rect[1];
        spr->SetState2(2);
        spr->flags |= 0x40;
        spr->SetRange(0, 1, 1, 1);
        spr->SetRange(1, 2, 2, 1);
    }

    bounds.left = rect[0];
    bounds.top = rect[1];
    bounds.right = rect[2];
    bounds.bottom = rect[3];
}

/* Function start: 0x421BC0 */
void T_MenuHotspot::Update()
{
    if (cursor == 0) {
        return;
    }

    class InputManager;
extern InputManager* DAT_0046aa08;
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

    if (bounds.left <= mouseY && bounds.right >= mouseY &&
        bounds.top <= mouseX && bounds.bottom >= mouseX) {
        cursor->ResetAnimation(1, 0);
    } else {
        cursor->ResetAnimation(0, 0);
    }

    cursor->Do(cursor->loc_x, cursor->loc_y, 1.0);
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

int T_MenuHotspot::LBLParse(char*) { return 0; }
