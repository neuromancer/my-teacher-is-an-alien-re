#include "mCNavNode.h"
#include "Sprite.h"
#include "InputManager.h"
#include "globals.h"
#include "string.h"
#include "Animation.h"
#include "CombatSprite.h"
#include "Projectile.h"
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

/* Function start: 0x412d80 */
mCNavNode::mCNavNode() : Parser()
{
    memset(&active, 0, 120);
    nextNodeId = 1;
    strcpy(nodeName, "NONAME");
}

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
        int* pNeighbors = &neighborNodes[0];
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
        anim.Play((char*)&soundName[0], 0x20);
        goto return_result;
    }

    if ((mCNavNode::flags & 0x10) == 0) {
        return 0;
    }

return_result:
    mCNavNode::active = 0;
    return mCNavNode::nextNodeId;
}
/* Function start: 0x412f40 */
void mCNavNode::AddSpriteList(char* name, int id)
{
    if (DAT_00435f10 != 0) {
        void* nodeResult = ((CombatSprite*)DAT_00435f10)->FindSprite(id);
        if (nodeResult != 0) {
            if (randomPool == 0) {
                randomPool = new ObjectPool(0x11, 0xa);
            }
            
            ObjectPool* pool = randomPool;
            int count = pool->allocatedCount;
            int h = (count >> 4) % pool->size;
            
            HashEntry* entry = 0;
            if (pool->memory != 0) {
                entry = ((HashEntry**)pool->memory)[h];
                while (entry != 0) {
                    if (entry->key == count) break;
                    entry = entry->next;
                }
            }
            
            if (entry == 0) {
                if (pool->memory == 0) {
                    pool->MemoryPool_Allocate(pool->size, 1);
                }
                entry = (HashEntry*)pool->Allocate_2();
                entry->field_4 = h;
                entry->key = count;
                entry->next = ((HashEntry**)pool->memory)[h];
                ((HashEntry**)pool->memory)[h] = entry;
            }
            entry->value = id;
            flags |= 0x8;
            return;
        }
    }
    ShowError("mCNavNode::AddSpriteList() - Undefined sprite list %s %d", name, id);
}

/* Function start: 0x413380 */
int mCNavNode::LBLParse(char* line)
{
    char label[32];
    char value[128];
    int valInt;

    if (sscanf(line, " %s ", label) != 1) {
        return 0;
    }

    if (_strcmpi(label, "EXIT_CODE") == 0) {
        if (sscanf(line, "%s %d", label, &valInt) == 2) {
            nextNodeId = valInt;
            flags |= 0x10;
        }
    }
    else if (_strcmpi(label, "IAM") == 0) {
        if (sscanf(line, "%s %d", label, &valInt) == 2) {
            nodeHandle = valInt;
        }
    }
    else if (_strcmpi(label, "LOOP") == 0) {
        if (sscanf(line, "%s %d", label, &valInt) == 2) {
            counterLimit = valInt;
            flags |= 0x4;
        }
    }
    else if (_strcmpi(label, "NAME") == 0) {
        if (sscanf(line, "%s %s", label, value) == 2) {
            if (strlen(value) < 32) {
                strcpy(nodeName, value);
            }
        }
    }
    else if (_strcmpi(label, "BG") == 0) {
        if (sscanf(line, "%s %d", label, &valInt) == 2) {
            animationState = valInt;
            flags |= 0x1;
        }
    }
    else if (_strcmpi(label, "NEXTNODE") == 0) {
        char direction[32];
        if (sscanf(line, "%s %s %d", label, direction, &valInt) == 3) {
            extern int FindCharIndex(char*); // in mCNavigator.cpp
            int idx = FindCharIndex(direction);
            if (idx < 6) {
                neighborNodes[idx] = valInt;
            }
        }
    }
    else if (_strcmpi(label, "PLAY_ANIM") == 0) {
        if (sscanf(line, "%s %s", label, value) == 2) {
            if (strlen(value) >= 32) {
                ShowError("mCNavNode::LBLParse() - anim name overflow '%s'", value);
            }
            strcpy(soundName, value);
            flags |= 0x2;
        }
    }
    else if (_strcmpi(label, "SPRITE") == 0) {
        if (sscanf(line, "%s %s %d", label, value, &valInt) == 3) {
            AddSpriteList(value, valInt);
        }
    }
    else if (_strcmpi(label, "END") == 0) {
        return 1;
    }
    else {
        Parser::LBLParse("mCNavNode");
    }

    return 0;
}
