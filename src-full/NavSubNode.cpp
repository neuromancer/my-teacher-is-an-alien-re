#include "NavSubNode.h"
#include "mCNavigator.h"
#include "mCNavNode.h"
#include "CombatSprite.h"
#include "Sprite.h"
#include "globals.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern void ShowError(const char* message, ...);
extern char* g_DirectionChars;
extern "C" int g_CombatEngine_0046ae78;
extern int FindCharIndex(char ch);

// Global: stick direction characters (5 directions)
char* g_StickDirChars = "FBLRU";

// Global: cached navigation sprite
Sprite* g_NavSprite = 0;

// =========================================================================
// Free functions
// =========================================================================

/* Function start: 0x449B90 */
int FindStickDirIndex(char ch)
{
    int i = 0;
    do {
        if (g_StickDirChars[i] == ch) {
            break;
        }
        i++;
    } while (i < 5);
    if (i == 5) {
        ShowError("StickDir - Invalid stick dir '%c'", (int)(signed char)ch);
    }
    return i;
}

// =========================================================================
// NavSubNode (vtable 0x461AC8)
// =========================================================================

/* Function start: 0x449C90 */
NavSubNode::~NavSubNode()
{
}

// =========================================================================
// OnDir_SubNode (vtable 0x461AE8)
// =========================================================================

/* Function start: 0x449BD0 */
OnDir_SubNode::OnDir_SubNode() : NavSubNode()
{
    int* p;
    memset(&field_A0, 0, 12 * 4);
    p = &destNode[0];
    p[0] = -1;
    p[1] = -1;
    p[2] = -1;
    p[3] = -1;
    p[4] = -1;
    p = &bearingForDir[0];
    p[0] = -1;
    p[1] = -1;
    p[2] = -1;
    p[3] = -1;
    p[4] = -1;
}

/* Function start: 0x449D80 */
OnDir_SubNode::~OnDir_SubNode()
{
}

/* Function start: 0x449E70 */
int OnDir_SubNode::LBLParse(char* param_1)
{
    int destNodeId;
    char stickStr[32];
    char bearingStr[32];

    int result = sscanf(param_1, " %s THEN_GOTO N%d BEARING %s ", stickStr, &destNodeId, bearingStr);
    if (result < 3) {
        return 1;
    }
    int stickDir = FindStickDirIndex(stickStr[0]);
    destNode[stickDir] = destNodeId;
    bearingForDir[stickDir] = FindCharIndex(bearingStr[0]);
    return 0;
}

// =========================================================================
// BG_SubNode (vtable 0x461B08)
// =========================================================================

/* Function start: 0x44A010 */
BG_SubNode::BG_SubNode() : NavSubNode()
{
    frameCounter = 0;
    maxFrames = 0;
    memset(&state, 0, 6 * 4);
    BG_SubNode::virtual7();
}

/* Function start: 0x44A0E0 */
BG_SubNode::~BG_SubNode()
{
    ObjectPool* pool = spritePool;
    if (pool != 0) {
        if (pool->memory != 0 && pool->size != 0) {
            int* buckets = (int*)pool->memory;
            int count = pool->size;
            do {
                int* node = (int*)*buckets;
                while (node != 0) {
                    int n = 0;
                    while (n-- != 0)
                        ;
                    n = 0;
                    while (n-- != 0)
                        ;
                    node = (int*)*node;
                }
                buckets++;
                count--;
            } while (count != 0);
        }

        operator delete(pool->memory);
        pool->memory = 0;
        pool->allocatedCount = 0;
        pool->freeList = 0;

        void* block = pool->memoryBlock;
        while (block != 0) {
            void* next = *(void**)block;
            operator delete(block);
            block = next;
        }
        pool->memoryBlock = 0;

        operator delete(pool);
        spritePool = 0;
    }
}

/* Function start: 0x44A1D0 */
void BG_SubNode::virtual7()
{
    state = 0;
    g_NavSprite = g_Navigator_0046ae70->sprite;
}

/* Function start: 0x44A1F0 */
int BG_SubNode::Activate()
{
    volatile int local;

    g_NavSprite = g_Navigator_0046ae70->sprite;

    if (state == 2) {
        state = 0;
        return 1;
    }

    if (state != 0) {
        goto do_sprite;
    }

    state = 1;
    g_NavSprite->ResetAnimation(spriteHandle, 0);
    *(int*)(g_CombatEngine_0046ae78 + 0xe8) = 1;
    frameCounter = 0;

    if (spritePool != 0) {
        unsigned int spriteId;
        NavNode* node;
        spriteId = rand() % spritePool->allocatedCount;
        {
            unsigned int h;
            h = (spriteId >> 4) % spritePool->size;
            node = 0;
            local = h;
            if (spritePool->memory != 0) {
                node = ((NavNode**)spritePool->memory)[local];
loop:
                if (node == 0) goto not_found;
                if ((unsigned int)node->key == spriteId) goto found;
                node = node->next;
                goto loop;
            }
not_found:
            node = 0;
found:
            if (node == 0) {
                ShowError("BG_SubNode::DoAction() - Invalid Sprite Id S%d of %s", (int)local, ((mCNavNode*)parentNode)->nodeName);
            } else {
                local = (int)node->value;
                DAT_0046ae5c->PlayById((int)local);
            }
        }
    }

do_sprite:
    ;
    int done = g_NavSprite->Do(g_NavSprite->loc_x, g_NavSprite->loc_y, 1.0);
    if (done != 0) {
        int count = frameCounter + 1;
        int isDone;
        frameCounter = count;
        if (maxFrames == 0) {
            isDone = 0;
        } else if (maxFrames <= count) {
            isDone = 1;
        } else {
            isDone = 0;
        }
        if (isDone) {
            state = 2;
        }
    }

    if (g_NavSprite->animation_data == 0) {
        *(int*)(g_CombatEngine_0046ae78 + 0xe4) = 0;
    } else {
        int* smk = (int*)g_NavSprite->animation_data->smk;
        *(int*)(g_CombatEngine_0046ae78 + 0xe4) = *(int*)((char*)smk + 0x374);
    }
    return 0;
}

/* Function start: 0x44A380 */
void BG_SubNode::AddSpriteList(unsigned int param_1)
{
    volatile int local_10;
    NavNode* node;

    if (DAT_0046ae5c == 0 || DAT_0046ae5c->FindSprite(param_1) == 0) {
        ShowError("BG_SubNode::AddSpriteList() - Undefined sprite list S%d", param_1);
    }

    if (spritePool == 0) {
        spritePool = new ObjectPool(0x11, 0xa);
    }

    ObjectPool* pool;
    unsigned int key;
    unsigned int poolSize;
    int* mem;

    pool = spritePool;
    key = pool->allocatedCount;
    poolSize = pool->size;
    {
        unsigned int h = (key >> 4) % poolSize;
        local_10 = h;
    }
    mem = (int*)pool->memory;

    if (mem != 0) {
        node = (NavNode*)mem[local_10];
loop2:
        if (node == 0) goto not_found2;
        if ((unsigned int)node->key == key) goto found2;
        node = node->next;
        goto loop2;
    }
not_found2:
    node = 0;
found2:
    if (node == 0) {
        if (mem == 0) {
            int* newMem = (int*)operator new(poolSize * 4);
            pool->memory = newMem;
            memset(newMem, 0, poolSize * 4);
            pool->size = poolSize;
        }
        node = (NavNode*)pool->Allocate_2();
        {
            int h = (int)local_10;
            int offset = h * 4;
            node->bucketIndex = h;
            node->key = key;
            int* buckets = (int*)pool->memory;
            node->next = (NavNode*)*(int*)((char*)buckets + offset);
            buckets = (int*)pool->memory;
            *(int*)((char*)buckets + offset) = (int)node;
        }
    }
    node->value = (void*)param_1;
}

// =========================================================================
// NavSubNode virtual method stubs
// =========================================================================

int NavSubNode::virtual4() { return 0; }
int NavSubNode::Activate() { return 0; }

/* Function start: 0x44ADA0 */
int NavSubNode::GetNextNode()
{
    if (bearing != -1) {
        g_Navigator_0046ae70->SetMode(bearing);
    }
    return nodeHandle;
}

void NavSubNode::virtual7() {}

// BG_SubNode::LBLParse stub (actual implementation at 0x44A4D0, not yet reimplemented)
int BG_SubNode::LBLParse(char* param_1) { return 0; }
