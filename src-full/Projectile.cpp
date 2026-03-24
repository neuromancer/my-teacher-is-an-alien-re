#include "Projectile.h"
#include "globals.h"
#include "EngineSubsystems.h"
#include "InputManager.h"
#include "HashTable.h"
#include "VBuffer.h"
#include <string.h>

extern int g_ProjectileHits_0043d150;

extern "C" int __cdecl SetFillColor(unsigned char param_1);
extern "C" int __cdecl SetDrawPosition(int param_1, int param_2);
extern "C" int __cdecl DrawCircle(int param_1);
extern "C" int __cdecl DrawLine(int param_1, int param_2);

/* Function start: 0x434660 */
Projectile::Projectile() : Sprite(0) {
    int* pCurrent = &currentX;
    int* pNext = &nextX;

    Projectile::startX = 0;
    Projectile::startY = 0;
    pCurrent[0] = 0;
    pCurrent[1] = 0;
    pNext[0] = 0;

    int* pVelocity = (int*)&velocityX;
    int* pHalf = &halfWidth;

    pNext[1] = 0;
    pHalf[0] = 0;
    pHalf[1] = 0;
    pVelocity[0] = 0;
    pVelocity[1] = 0;
    Projectile::active = 0;
    Projectile::currentX = 0;
    memset(&startX, 0, 12 * 4);
}

/* Function start: 0x427270 */
void Projectile::Launch() {
    int frameCount;
    int mouseY;
    int mouseX;
    int height;
    int width;
    InputState* pMouse;

    Projectile::active = 1;
    Projectile::ResetAnimation(0, 0);

    frameCount = Projectile::ranges[Projectile::handle].end
               - Projectile::ranges[Projectile::handle].start + 1;

    Projectile::startX = 0xa0;
    Projectile::startY = 0xb4;

    pMouse = g_InputManager_0046aa08->pMouse;

    mouseY = 0;
    if (pMouse != 0) {
        mouseY = pMouse->y;
    }

    mouseX = 0;
    if (pMouse != 0) {
        mouseX = pMouse->x;
    }

    Projectile::currentX = mouseX;
    Projectile::currentY = mouseY;

    height = 0;
    if (Projectile::animation_data != 0) {
        height = Projectile::animation_data->targetBuffer->height;
    }

    width = 0;
    if (Projectile::animation_data != 0) {
        width = Projectile::animation_data->targetBuffer->width;
    }

    Projectile::halfWidth = width / 2;
    Projectile::halfHeight = height / 2;
    Projectile::velocityX = (float)(mouseX - 0xa0) / (float)frameCount;
    Projectile::velocityY = (float)(mouseY - 0xb4) / (float)frameCount;
}

/* Function start: 0x427390 */
void Projectile::Update() {
    int isExploding;
    int frameNum;

    if (Projectile::active == 0) {
        return;
    }

    isExploding = (Projectile::handle == 1);

    if (isExploding) {
        Projectile::nextX = Projectile::currentX;
        Projectile::nextY = Projectile::currentY;
    } else {
        if (Projectile::animation_data != 0) {
            frameNum = *(int*)(*(int*)((int)Projectile::animation_data + 0xc) + 0x374) + 1;
        } else {
            frameNum = 1;
        }

        Projectile::nextX = Projectile::startX + (int)(Projectile::velocityX * (float)frameNum);
        Projectile::nextY = Projectile::startY + (int)(Projectile::velocityY * (float)frameNum);

        {
            int* pObj = (int*)Projectile::field_0xF8;
            int* vtbl = (int*)*pObj;
            if (((int (__fastcall*)(int*, int, Projectile*))vtbl[6])(pObj, 0, this) != 0) {
                Projectile::currentX = Projectile::nextX;
                Projectile::currentY = Projectile::nextY;
                Projectile::ResetAnimation(1, 0);
            }
        }
    }

    if (Projectile::Do(
            Projectile::nextX - Projectile::halfWidth,
            Projectile::nextY - Projectile::halfHeight,
            1.0)) {
        if (isExploding) {
            Projectile::active = 0;
            return;
        }
        g_ProjectileHits_0043d150++;
        Projectile::ResetAnimation(1, 0);
    }
}

/* Function start: 0x427150 */
Projectile::Projectile(int owner) : Sprite(0)
{
    int* p = (int*)&field_0xF8;
    int i;
    for (i = 0xC; i != 0; i--) {
        *p = 0;
        p++;
    }
    field_0xF8 = owner;
}
