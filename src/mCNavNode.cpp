#include "mCNavNode.h"
#include "Sprite.h"
#include "InputManager.h"
#include "globals.h"
#include "string.h"
#include "Animation.h"
#include "CombatSprite.h"
#include <stdlib.h>

// Globals
extern Parser* DAT_00435f24;           // mCNavigator instance
extern InputManager* g_InputManager_00436968;
extern Sprite* g_Sprite_004360a0;
extern Parser* DAT_00435f10;           // SPRITELIST parser (actually CombatSprite*)
extern Engine* g_CombatEngine;         // 0x00435eb0

// Hash table entry structure for random pool
struct HashEntry {
    HashEntry* next;    // 0x00
    int field_4;        // 0x04
    int key;            // 0x08 - hash key
    int value;          // 0x0C - sound/sprite ID
};

/* Function start: 0x4130F0 */
int mCNavNode::Activate()
{
    int fl;
    int randVal;
    int bucketIndex;
    HashEntry* entry;

    // Check animation flag (bit 0)
    fl = mCNavNode::flags;
    if ((fl & 1) != 0) {
        if (g_Sprite_004360a0 != 0) {
            g_Sprite_004360a0->SetState2(mCNavNode::animationState);
        } else {
            mCNavNode::flags = fl & ~1;
        }
    }

    // Check random sound flag (bit 3)
    if ((mCNavNode::flags & 8) != 0) {
        randVal = rand();
        ObjectPool* pool = mCNavNode::randomPool;
        randVal = randVal % pool->allocatedCount;
        bucketIndex = (((unsigned int)randVal) >> 4) % pool->size;

        entry = 0;
        if (pool->memory != 0) {
            entry = ((HashEntry**)pool->memory)[bucketIndex];
            while (entry != 0) {
                if (entry->key == randVal) {
                    break;
                }
                entry = entry->next;
            }
        }

        if (entry == 0) {
            ShowError("mCNavNode::Activate() - Invalid Sprite Id S %d of %s", bucketIndex, mCNavNode::nodeName);
        } else {
            ((CombatSprite*)DAT_00435f10)->PlayById(entry->value);
        }
    }

    mCNavNode::counter = 0;
    mCNavNode::active = 1;
    *(int*)((char*)g_CombatEngine + 0xdc) = 1;

    return 0;
}

/* Function start: 0x4131D0 */
int mCNavNode::GetNextNode()
{
    int divisor;

    int* pDirectionIndex = (int*)((char*)DAT_00435f24 + 0x98);
    int currentNode = *(int*)((char*)DAT_00435f24 + 0xa0);
    int currentDir = *pDirectionIndex;
    InputState* pMouse = g_InputManager_00436968->pMouse;

    int direction = 0;
    if (pMouse != 0) {
        divisor = 0x6b;
        direction = pMouse->x / divisor;
    }

    if (direction == 1 && pMouse != 0 && pMouse->y > 0x85) {
        direction = 3;
    }

    // Calculate new direction: (currentDir + 3 + direction) % 4
    // The assembly does: ((val ^ sign) - sign) & 3) ^ sign) - sign
    // which is a signed modulo 4 operation
    unsigned int val = currentDir + 3 + direction;
    int sign = (int)val >> 31;
    direction = ((((val ^ sign) - sign) & 3) ^ sign) - sign;
    *pDirectionIndex = direction;

    int result = mCNavNode::neighborNodes[direction];
    if (result == 0) {
        int* pNeighbors = &mCNavNode::neighborNodes[0];
        *pDirectionIndex = 0;
        for (;;) {
            result = *pNeighbors;
            if (*pDirectionIndex == 6) {
                result = 0;
                break;
            }
            if (result != 0 && result != currentNode) {
                break;
            }
            pNeighbors++;
            *pDirectionIndex = *pDirectionIndex + 1;
        }
    }

    return result;
}

/* Function start: 0x413280 */
int mCNavNode::Update()
{
    if (mCNavNode::active == 0) {
        mCNavNode::Activate();
    }

    if ((mCNavNode::flags & 1) != 0) {
        int animResult = g_Sprite_004360a0->Do(
                                       g_Sprite_004360a0->loc_x,
                                       g_Sprite_004360a0->loc_y,
                                       1.0);
        if (animResult != 0) {
            if ((mCNavNode::flags & 4) != 0) {
                mCNavNode::counter = mCNavNode::counter + 1;
                if (mCNavNode::counterLimit > mCNavNode::counter) {
                    goto check_sound;
                }
            }
            goto return_result;
        }
    }

check_sound:
    if ((mCNavNode::flags & 2) != 0) {
        Animation anim;
        anim.Play((char*)&mCNavNode::soundName[0], 0x20);
        goto return_result;
    }

    if ((mCNavNode::flags & 0x10) == 0) {
        return 0;
    }

return_result:
    mCNavNode::active = 0;
    return mCNavNode::nextNodeId;
}
