#include "mCNavigator.h"
#include "globals.h"
#include "Parser.h"
#include "string.h"
#include <string.h>
#include "stdio.h"
#include "stdlib.h"
#include "Memory.h"

extern "C" {
#include "mss.h"
}

extern void ShowError(const char* message, ...);
extern void CleanupObjectArray(void*, int);

// mCNavNode method externs (thiscall)
extern int __fastcall FUN_00413280(void* node);   // mCNavNode::Update
extern int __fastcall FUN_004131d0(void* node);   // mCNavNode::GetNextNode

// Global for navigation state
extern int* DAT_00435f28;

// NavNode constructor - initializes a 0x100 byte structure
void* NavNode_Constructor(void* mem) { return mem; }

// Direction characters for navigation (6 directions: N, E, S, W, U, D or similar)
static char* g_DirectionChars = "NESWUD";

/* Function start: 0x412d60 */
int FindCharIndex(char* param_1)
{
    int index = 0;
    char* dirChars = g_DirectionChars;

    if (*dirChars != '\0') {
        do {
            if (*dirChars == *param_1) {
                return index;
            }
            dirChars++;
            index++;
        } while (*dirChars != '\0');
    }
    return index;
}

Sprite* g_Sprite_004360a0 = 0;



/* Function start: 0x413C80 */
void* ObjectPool::Allocate_2()
{
    if (freeList == 0) {
        int* p = (int*)AllocateMemory(objectSize * 0x10 + 4);
        *p = (int)memoryBlock;
        memoryBlock = p;

        int i = objectSize;
        int offset = i * 0x10;
        i--;
        p = (int*)((char*)p + offset - 0xc);
        if (i >= 0) {
            do {
                int prev = (int)freeList;
                i--;
                *p = prev;
                freeList = p;
                p = p - 4;
            } while (i >= 0);
        }
    }

    int* p = (int*)freeList;
    freeList = (void*)*p;
    allocatedCount++;
    int n = 0;
    p[2] = n;
    while (n--)
        ;
    n = 0;
    p[3] = n;
    while (n--)
        ;
    return p;
}

/* Function start: 0x413670 */
mCNavigator::mCNavigator()
{
    memset(&sprite, 0, 32);
    startingNode = 1;
    field_98 = 0;
}

extern void __fastcall FUN_0041ce30(void*);

/* Function start: 0x4136F0 */
mCNavigator::~mCNavigator()
{
    unsigned int i;
    void* block;
    void* next;

    if (navNodePool == 0) {
        goto check_sprite;
    }

    if (navNodePool->memory == 0) {
        goto free_memory;
    }

    if (navNodePool->size == 0) {
        goto free_memory;
    }

    i = 0;
    do {
        NavNode* node = *(NavNode**)((int)navNodePool->memory + i * 4);
        while (node) {
            CleanupObjectArray(node->field_C, 1);
            node = node->next;
        }
        i++;
    } while (i < navNodePool->size);

free_memory:
    FreeMemory(navNodePool->memory);
    navNodePool->memory = 0;
    navNodePool->allocatedCount = 0;
    navNodePool->freeList = 0;

    block = navNodePool->memoryBlock;
    while (block) {
        next = *(void**)block;
        FreeMemory(block);
        block = next;
    }
    navNodePool->memoryBlock = 0;

    FreeMemory(navNodePool);
    navNodePool = 0;

check_sprite:
    if (sprite) {
        FUN_0041ce30(sprite);
        FreeMemory(sprite);
        sprite = 0;
    }
}

/* Function start: 0x413810 */
void mCNavigator::SetBearing(char* param_1)
{
    int iVar1 = FindCharIndex(param_1);
    field_98 = iVar1;
    if (iVar1 >= 6) {
        ShowError("mCNavigator::SetBearing() - Undefined direction '%s' %d", param_1, iVar1);
    }
}

extern void __fastcall FUN_0041cf10(void*);

/* Function start: 0x413840 */
void mCNavigator::SetCurrentNode()
{
    unsigned int hash;

    if (sprite) {
        FUN_0041cf10(sprite);
    }

    if (navNodePool == 0) {
        return;
    }

    int nodeId = startingNode;
    hash = ((unsigned int)nodeId >> 4) % navNodePool->size;

    NavNode* node = 0;
    if (navNodePool->memory) {
        node = *(NavNode**)((int)navNodePool->memory + hash * 4);
    }

    while (node) {
        if (node->field_8 == nodeId) {
            break;
        }
        node = node->next;
    }

    if (node) {
        currentNode = node->field_C;
    }

    field_A0 = nodeId;
    if (currentNode) {
        g_Sprite_004360a0 = sprite;
    }
}

/* Function start: 0x413BC0 */
int mCNavigator::Update()
{
    unsigned int hash;

    if (currentNode == 0) {
        return 0;
    }

    int result = FUN_00413280(currentNode);
    if (result == 1) {
        int nextNodeId = FUN_004131d0(currentNode);
        field_A0 = *(int*)((int)currentNode + 0xdc);

        hash = ((unsigned int)nextNodeId >> 4) % navNodePool->size;

        NavNode* node = 0;
        if (navNodePool->memory) {
            node = *(NavNode**)((int)navNodePool->memory + hash * 4);
        }

        while (node) {
            if (node->field_8 == nextNodeId) {
                break;
            }
            node = node->next;
        }

        if (node == 0) {
            return 2;
        }

        currentNode = node->field_C;
        return 0;
    }

    if (result != 3) {
        if (result != 2) {
            return 0;
        }
    }
    *DAT_00435f28 = 2;
    return result;
}

/* Function start: 0x4138c0 */
int mCNavigator::LBLParse(char* param_1)
{
    char local_b4[128];
    char local_34[32];
    void* local_14;
    ObjectPool* pool;
    Parser* parser;
    Sprite* spr;

    local_b4[0] = '\0';
    local_34[0] = '\0';
    sscanf(param_1, " %s ", local_34);
    if (_strcmpi(local_34, "BEARING") == 0) {
        sscanf(param_1, "%s %s", local_34, local_b4);
        SetBearing(local_b4);
    }
    else if (_strcmpi(local_34, "BASENODE") == 0) {
        if (navNodePool == 0) {
            local_14 = AllocateMemory(sizeof(ObjectPool));
            pool = (ObjectPool*)local_14;
            if (pool) {
                pool->memory = 0;
                pool->size = 0x11;
                pool->allocatedCount = 0;
                pool->freeList = 0;
                pool->memoryBlock = 0;
                pool->objectSize = 10;
            }
            navNodePool = pool;
        }

        local_14 = AllocateMemory(0x100);
        parser = 0;
        if (local_14) {
            parser = (Parser*)NavNode_Constructor(local_14);
        }
        Parser::ProcessFile(parser, this, 0);

        if (*(unsigned int*)((int)parser + 0xdc) == 0) {
            ShowError("mCNavigator::LoadNodes() - Invalid Node Handle (%d)", 0);
        }

        unsigned int nodeId = *(unsigned int*)((int)parser + 0xdc);
        unsigned int hash = (nodeId >> 4) % navNodePool->size;

        NavNode* node = 0;
        if (navNodePool->memory) {
            node = *(NavNode**)((int)navNodePool->memory + hash * 4);
            while (node) {
                if (node->field_8 == nodeId) {
                    break;
                }
                node = node->next;
            }
        }

        if (node) {
            ShowError("mCNavigator::LoadNodes() - %s has a dublicate node handle (%d)", (char*)parser + 0xe0, nodeId);
        }

        nodeId = *(unsigned int*)((int)parser + 0xdc);
        hash = (nodeId >> 4) % navNodePool->size;
        node = 0;
        if (navNodePool->memory) {
            node = *(NavNode**)((int)navNodePool->memory + hash * 4);
            while (node) {
                if (node->field_8 == nodeId) {
                    break;
                }
                node = node->next;
            }
        }

        if (node == 0) {
            if (navNodePool->memory == 0) {
                navNodePool->MemoryPool_Allocate(navNodePool->size, 1);
            }
            node = (NavNode*)navNodePool->Allocate_2();
            node->field_4 = hash;
            node->field_8 = nodeId;
            node->next = *(NavNode**)((int)navNodePool->memory + hash * 4);
            *(NavNode**)((int)navNodePool->memory + hash * 4) = node;
        }
        node->field_C = parser;
    }
    else if (_strcmpi(local_34, "SPRITE") == 0) {
        local_14 = AllocateMemory(0xd8);
        spr = 0;
        if (local_14) {
            spr = (Sprite*)local_14;
            spr->Sprite::Sprite((char*)0);
        }
        sprite = spr;
        Parser::ProcessFile((Parser*)spr, (Parser*)this, (char*)0);
    }
    else if (_strcmpi(local_34, "STARTING_NODE") == 0) {
        sscanf(param_1, "%s %d", local_34, &startingNode);
    }
    else if (_strcmpi(local_34, "END") == 0) {
        return 1;
    }
    else {
        return Parser::LBLParse("mCNavigator");
    }

    return 0;
}
