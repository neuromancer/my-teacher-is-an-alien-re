#include "SC_SaveLoad.h"
#include "T_MenuHotspot.h"
#include "Sprite.h"
#include "Palette.h"
#include "Memory.h"
#include <string.h>
#include <new>

/* Function start: 0x421C40 */
SC_SaveLoad::SC_SaveLoad()
{
    int rect[4];

    memset(&timer, 0, 0xC * 4);
    handlerId = 0x2F;
    timer.Reset();

    sprite = new Sprite("mainmenu\\save_yn.smk");
    sprite->priority = 0;

    palette = new Palette("mainmenu\\menu.col");

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
    {
        Sprite* spr = sprite;
        if (spr != 0) {
            spr->~Sprite();
            FreeMemory(spr);
            sprite = 0;
        }
    }
    if (btnYes != 0) {
        Sprite* spr = btnYes->sprite;
        if (spr != 0) {
            spr->~Sprite();
            FreeMemory(spr);
            btnYes->sprite = 0;
        }
        FreeMemory(btnYes);
        btnYes = 0;
    }
    if (btnNo != 0) {
        Sprite* spr = btnNo->sprite;
        if (spr != 0) {
            spr->~Sprite();
            FreeMemory(spr);
            btnNo->sprite = 0;
        }
        FreeMemory(btnNo);
        btnNo = 0;
    }
    if (btnCancel != 0) {
        Sprite* spr = btnCancel->sprite;
        if (spr != 0) {
            spr->~Sprite();
            FreeMemory(spr);
            btnCancel->sprite = 0;
        }
        FreeMemory(btnCancel);
        btnCancel = 0;
    }
}
