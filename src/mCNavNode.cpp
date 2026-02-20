#include "mCNavNode.h"
#include "Sprite.h"
#include "InputManager.h"
#include "globals.h"
#include "string.h"
#include "Animation.h"
#include "CombatSprite.h"
#include "Projectile.h"
#include "mCNavigator.h"
#include "Engine.h"
#include <stdlib.h>

// Globals
extern InputManager* g_InputManager_00436968;
extern Engine* g_CombatEngine_00435eb0;         // 0x00435eb0
extern int FindCharIndex(char* param_1);

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

/* Function start: 0x4130B0 */
void mCNavNode::SetNavLink(char* direction, int nodeId)
{
    int dirIndex = FindCharIndex(direction);
    if (dirIndex >= 6) {
        ShowError("mCNavNode::SetNavLink() - node %d : Undefined direction '%s' %d", mCNavNode::nodeHandle, direction, dirIndex);
    }
    mCNavNode::neighborNodes[dirIndex] = nodeId;
}

/* Function start: 0x4130F0 */
int mCNavNode::Activate()
{
    int fl;
    int randVal;
    unsigned int bucketIndex;
    HashEntry* entry;

    fl = mCNavNode::flags;
    if ((fl & 1) != 0) {
        if (g_Sprite_004360a0 != 0) {
            g_Sprite_004360a0->SetState2(mCNavNode::animationState);
        } else {
            mCNavNode::flags = fl & ~1;
        }
    }

    if ((mCNavNode::flags & 8) != 0) {
        randVal = rand();
        ObjectPool* pool = mCNavNode::randomPool;
        randVal = randVal % pool->allocatedCount;
        bucketIndex = ((unsigned int)randVal >> 4) % pool->size;
        void* mem = pool->memory;

        if (mem == 0) {
            goto no_entry;
        }
        entry = ((HashEntry**)mem)[bucketIndex];

loop_top:
        if (entry == 0) {
            goto no_entry;
        }
        if (entry->key == randVal) {
            goto found_entry;
        }
        entry = entry->next;
        goto loop_top;

no_entry:
        entry = 0;
found_entry:
        if (entry == 0) {
            ShowError("mCNavNode::Activate() - Invalid Sprite Id S %d of %s", bucketIndex, mCNavNode::nodeName);
        } else {
            g_SpriteList_00435f10->PlayById(entry->value);
        }
    }

    mCNavNode::counter = 0;
    mCNavNode::active = 1;
    g_CombatEngine_00435eb0->m_framesL = 1;

    return 0;
}

/* Function start: 0x4131D0 */
int mCNavNode::GetNextNode()
{
    int divisor;

    int* pDirectionIndex = &g_Navigator_00435f24->field_98;
    int currentNode = g_Navigator_00435f24->field_A0;
    int currentDir = *pDirectionIndex;
    InputState* pMouse = g_InputManager_00436968->pMouse;

    int direction = 0;
    if (pMouse != 0) {
        divisor = 0x6b;
        direction = pMouse->x / divisor;
    }

    if (direction == 1 && pMouse != 0 && pMouse->y >= 0x86) {
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
        Sprite* spr = g_Sprite_004360a0;
        int animResult = spr->Do(spr->loc_x, spr->loc_y, 1.0);
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

    if ((mCNavNode::flags & 0x10) != 0) {
        goto return_result;
    }
    return 0;

return_result:
    mCNavNode::active = 0;
    return mCNavNode::nextNodeId;
}
/* Function start: 0x412f40 */
void mCNavNode::AddSpriteList(char* name, int id)
{
    if (g_SpriteList_00435f10 == 0 || g_SpriteList_00435f10->FindSprite(id) == 0) {
        ShowError("mCNavNode::AddSpriteList() - Undefined sprite list %s %d", name, id);
    }

    if (randomPool == 0) {
        randomPool = new ObjectPool(0x11, 0xa);
    }

    ObjectPool* pool = randomPool;
    unsigned int count = pool->allocatedCount;
    unsigned int size = pool->size;
    unsigned int h = (count >> 4) % size;
    void* mem = pool->memory;
    HashEntry* entry;

    if (mem == 0) {
        goto no_entry;
    }
    entry = ((HashEntry**)mem)[h];

loop_top:
    if (entry == 0) {
        goto no_entry;
    }
    if (entry->key == count) {
        goto found_entry;
    }
    entry = entry->next;
    goto loop_top;

no_entry:
    entry = 0;
found_entry:
    if (entry == 0) {
        if (mem == 0) {
            pool->memory = new char[size * 4];
            memset(pool->memory, 0, size * 4);
            pool->size = size;
        }
        entry = (HashEntry*)pool->Allocate_2();
        entry->field_4 = h;
        entry->key = count;
        entry->next = ((HashEntry**)pool->memory)[h];
        ((HashEntry**)pool->memory)[h] = entry;
    }
    entry->value = id;
    flags |= 0x8;
}

/* Function start: 0x413380 */
int mCNavNode::LBLParse(char* line)
{
    char label[32];
    char value[128];
    int valInt;
    int spriteId;

    value[0] = 0;
    label[0] = 0;
    valInt = 0;
    spriteId = 0;

    sscanf(line, " %s ", label);

    if (_strcmpi(label, "EXIT_CODE") == 0) {
        sscanf(line, "%s %d", label, &nextNodeId);
        flags |= 0x10;
    }
    else if (_strcmpi(label, "IAM") == 0) {
        sscanf(line, "%s %d", label, &nodeHandle);
    }
    else if (_strcmpi(label, "LOOP") == 0) {
        sscanf(line, "%s %d", label, &counterLimit);
        flags |= 0x4;
    }
    else if (_strcmpi(label, "NAME") == 0) {
        if (sscanf(line, "%s %s", label, value) == 2) {
            if (strlen(value) < 32) {
                strcpy(nodeName, value);
            }
        }
    }
    else if (_strcmpi(label, "BG") == 0) {
        sscanf(line, "%s %d", label, &animationState);
        flags |= 0x1;
    }
    else if (_strcmpi(label, "NEXTNODE") == 0) {
        sscanf(line, "%s %s %d", label, value, &valInt);
        mCNavNode::SetNavLink(value, valInt);
    }
    else if (_strcmpi(label, "PLAY_ANIM") == 0) {
        sscanf(line, "%s %s", label, value);
        if (strlen(value) >= 32) {
            ShowError("mCNavNode::LBLParse() - anim name overflow '%s'", value);
        }
        strcpy(soundName, value);
        flags |= 0x2;
    }
    else if (_strcmpi(label, "SPRITE") == 0) {
        sscanf(line, "%s %s %d", label, value, &spriteId);
        mCNavNode::AddSpriteList(value, spriteId);
    }
    else if (_strcmpi(label, "END") == 0) {
        return 1;
    }
    else {
        Parser::LBLParse("mCNavNode");
    }

    return 0;
}
