#include "Projectile.h"
#include "globals.h"
#include "EngineSubsystems.h"
#include "InputManager.h"
#include "HashTable.h"
#include "VBuffer.h"
#include <string.h>

extern "C" int __cdecl SetFillColor(unsigned char param_1);
extern "C" int __cdecl SetDrawPosition(int param_1, int param_2);
extern "C" int __cdecl DrawCircle(int param_1);
extern "C" int __cdecl DrawLine(int param_1, int param_2);

// NOTE: 0x434660 is EngineInfoParser::EngineInfoParser (COMDAT),
// not Projectile. See EngineInfoParser.cpp for the correct implementation.

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

    frameCount = Projectile::ranges[Projectile::handle].dim.y
               - Projectile::ranges[Projectile::handle].dim.x + 1;

    Projectile::startPos.x = 0xa0;
    Projectile::startPos.y = 0xb4;

    if (g_InputManager_0046aa08->pMouse != 0) {
        mouseY = g_InputManager_0046aa08->pMouse->y;
    } else {
        mouseY = 0;
    }

    if (g_InputManager_0046aa08->pMouse != 0) {
        mouseX = g_InputManager_0046aa08->pMouse->x;
    } else {
        mouseX = 0;
    }

    Projectile::currentPos.x = mouseX;
    Projectile::currentPos.y = mouseY;

    height = 0;
    if (Projectile::animation_data != 0) {
        height = Projectile::animation_data->targetBuffer->height;
    }

    width = 0;
    if (Projectile::animation_data != 0) {
        width = Projectile::animation_data->targetBuffer->width;
    }

    Projectile::halfDim.x = width / 2;
    Projectile::halfDim.y = height / 2;
    *(float*)&Projectile::velocity.x = (float)(mouseX - 0xa0) / (float)frameCount;
    *(float*)&Projectile::velocity.y = (float)(mouseY - 0xb4) / (float)frameCount;
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
        Projectile::nextPos.x = Projectile::currentPos.x;
        Projectile::nextPos.y = Projectile::currentPos.y;
    } else {
        if (Projectile::animation_data == 0) {
            frameNum = 1;
        } else {
            frameNum = *(int*)(*(int*)((int)Projectile::animation_data + 0xc) + 0x374) + 1;
        }

        Projectile::nextPos.x = Projectile::startPos.x + (int)(*(float*)&Projectile::velocity.x * (float)frameNum);
        Projectile::nextPos.y = Projectile::startPos.y + (int)(*(float*)&Projectile::velocity.y * (float)frameNum);

        {
            int* pObj = (int*)Projectile::owner;
            int* vtbl = (int*)*pObj;
            if (((int (__fastcall*)(int*, int, Projectile*))vtbl[6])(pObj, 0, this) != 0) {
                Projectile::currentPos.x = Projectile::nextPos.x;
                Projectile::currentPos.y = Projectile::nextPos.y;
                Projectile::ResetAnimation(1, 0);
            }
        }
    }

    if (Projectile::Do(
            Projectile::nextPos.x - Projectile::halfDim.x,
            Projectile::nextPos.y - Projectile::halfDim.y,
            1.0)) {
        if (isExploding) {
            Projectile::active = 0;
            return;
        }
        g_ProjectileHits_0047325c++;
        Projectile::ResetAnimation(1, 0);
    }
}

/* Function start: 0x427150 */
Projectile::Projectile(int ownerParam) : Sprite(0)
{
    memset(&owner, 0, 0xC * 4);
    Projectile::owner = ownerParam;
}

