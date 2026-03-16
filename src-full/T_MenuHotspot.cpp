#include "T_MenuHotspot.h"
#include "Memory.h"
#include "InputManager.h"
#include <string.h>
#include <new>

extern InputManager* DAT_0046aa08;

/* Function start: 0x420CE0 */
T_MenuHotspot::T_MenuHotspot(int param) : Parser()
{
    memset(&sprite, 0, 0x46 * 4);
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
    memset(&sprite, 0, 42 * 4);

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

/* Function start: 0x421080 */ /* stub — full implementation is large */
int T_MenuHotspot::LBLParse(char*) { return 0; }

void T_MenuHotspot::Cleanup() {}

