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

/* Function start: 0x4161B0 */
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
    Projectile::field_0x104 = 0;
    memset(&startX, 0, 12 * 4);
}

/* Function start: 0x4162C0 */
void Projectile::Launch() {
    int frameCount;
    int mouseY;
    int mouseX;
    int height;
    int width;
    InputState* pMouse;

    Projectile::active = 1;
    Projectile::SetState2(0);

    frameCount = Projectile::ranges[Projectile::current_state].end
               - Projectile::ranges[Projectile::current_state].start + 1;

    Projectile::startX = 0xa0;
    Projectile::startY = 0xb4;

    pMouse = g_InputManager_00436968->pMouse;

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

/* Function start: 0x4163E0 */
void Projectile::Update() {
    int isExploding;
    int frameNum;

    if (Projectile::active == 0) {
        return;
    }

    isExploding = (Projectile::current_state == 1);

    if (isExploding) {
        Projectile::nextX = Projectile::currentX;
        Projectile::nextY = Projectile::currentY;
    } else {
        if (Projectile::animation_data != 0) {
            frameNum = Projectile::animation_data->smk->FrameNum + 1;
        } else {
            frameNum = 1;
        }

        Projectile::nextX = Projectile::startX + (int)(Projectile::velocityX * (float)frameNum);
        Projectile::nextY = Projectile::startY + (int)(Projectile::velocityY * (float)frameNum);

        if (Projectile::CheckCollision()) {
            Projectile::currentX = Projectile::nextX;
            Projectile::currentY = Projectile::nextY;
            Projectile::SetState2(1);
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
        Projectile::SetState2(1);
    }
}

/* Function start: 0x416500 */
int Projectile::CheckCollision() {
    HashTable* hashTable;
    HashNode* current;
    HashNode* next;
    unsigned int bucketIdx;
    Target* target;
    int* bucket;

    hashTable = g_TargetList_00435f0c->hashTable;
    if (hashTable == 0) {
        return 0;
    }

    current = (HashNode*)(((unsigned int)hashTable->count < 1u) - 1);

    do {
        if (current == 0) {
            return 0;
        }

        if (current == (HashNode*)-1) {
            bucketIdx = 0;
            if ((unsigned int)hashTable->numBuckets != 0) {
                bucket = hashTable->buckets;
                do {
                    current = (HashNode*)*bucket;
                    if (current != 0) break;
                    bucket++;
                    bucketIdx++;
                } while (bucketIdx < (unsigned int)hashTable->numBuckets);
            }
        }

        next = current->next;
        if (next == 0) {
            bucketIdx = current->bucketIndex + 1;
            if (bucketIdx < (unsigned int)hashTable->numBuckets) {
                bucket = (int*)(bucketIdx * 4 + (int)hashTable->buckets);
                do {
                    next = (HashNode*)*bucket;
                    if (next != 0) break;
                    bucket++;
                    bucketIdx++;
                } while (bucketIdx < (unsigned int)hashTable->numBuckets);
            }
        }

        target = (Target*)current->reserved;
        current = next;

        if (target != 0) {
            if (target->CheckTimeInRangeParam((int*)((char*)this + 0xe8))) {
                target->UpdateProgress(1);
                return 1;
            }
        }
    } while (1);
}
