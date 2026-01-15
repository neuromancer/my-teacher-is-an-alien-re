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
extern void* MemoryPool_Allocate(void*, int, int);
extern void* ObjectPool_Allocate_2(int);
extern void CleanupObjectArray(void*, int);

// mCNavNode functions - stub implementations
int mCNavNode_Update(void* node) { return 0; }
int mCNavNode_GetNextNode(void* node) { return 0; }

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



void* ObjectPool::Allocate_2()
{
    if (freeList == 0) {
        int* p = (int*)AllocateMemory(objectSize * 0x10 + 4);
        *p = (int)memoryBlock;
        memoryBlock = p;

        int i = objectSize;
        p = p + i * 4 - 3;
        for (i--; i >= 0; i--) {
            *p = (int)freeList;
            freeList = p;
            p = p - 4;
        }
    }

    int* p = (int*)freeList;
    freeList = (void*)*p;
    allocatedCount++;
    p[2] = 0;
    p[3] = 0;
    return p;
}

/* Function start: 0x413670 */
mCNavigator::mCNavigator()
{
    memset(&sprite, 0, 32);
    startingNode = 1;
    field_98 = 0;
}

/* Function start: 0x4136F0 */
mCNavigator::~mCNavigator()
{
    if (navNodePool) {
        if (navNodePool->memory && navNodePool->size) {
            unsigned int i = 0;
            do {
                for (NavNode* node = *(NavNode**)((int)navNodePool->memory + i * 4); node; node = node->next) {
                    CleanupObjectArray(node->field_C, 1);
                }
                i++;
            } while (i < navNodePool->size);
        }

        if (navNodePool->memory) {
            FreeMemory(navNodePool->memory);
            navNodePool->memory = 0;
        }

        NavNode* block = (NavNode*)navNodePool->memoryBlock;
        while (block) {
            NavNode* next = block->next;
            FreeMemory(block);
            block = next;
        }
        navNodePool->memoryBlock = 0;

        FreeMemory(navNodePool);
        navNodePool = 0;
    }

    if (sprite) {
        sprite->~Sprite();
        FreeMemory(sprite);
        sprite = 0;
    }
}

/* Function start: 0x413810 */
void mCNavigator::SetBearing(char* param_1)
{
    int iVar1 = FindCharIndex(param_1);
    field_98 = iVar1;
    if (5 < iVar1) {
        ShowError("mCNavigator::SetBearing() - Undefined direction '%s' %d", param_1, iVar1);
    }
}

/* Function start: 0x413840 */
void mCNavigator::SetCurrentNode()
{
    if (sprite) {
        // Sprite_Init(sprite);
    }

    if (navNodePool) {
        NavNode* node;
        for (node = *(NavNode**)((int)navNodePool->memory + ((startingNode >> 4) % navNodePool->size) * 4); node; node = node->next) {
            if (node->field_8 == startingNode) {
                break;
            }
        }

        if (node) {
            currentNode = node->field_C;
        }

        field_A0 = startingNode;
        if (currentNode) {
            g_Sprite_004360a0 = sprite;
        }
    }
}

/* Function start: 0x413BC0 */
int mCNavigator::Update()
{
    if (currentNode == 0) {
        return 0;
    }

    int result = mCNavNode_Update(currentNode);
    if (result != 1) {
        if (result != 3 && result != 2) {
            return 0;
        }
        // *DAT_00435f28 = 2;
        return result;
    }

    int nextNodeId = mCNavNode_GetNextNode(currentNode);
    field_A0 = *(int*)((int)currentNode + 0xdc);

    int hash = (nextNodeId >> 4) / navNodePool->size;
    NavNode* node = *(NavNode**)((int)navNodePool->memory + hash * 4);
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

/* Function start: 0x413C80 */
void mCNavigator::Allocate()
{
    if (navNodePool == 0) {
        return;
    }

    navNodePool->Allocate_2();
}

/* Function start: 0x413CF0 */
void mCNavigator::MemoryPool_Allocate(unsigned int param_1, int param_2)
{
    if (navNodePool->memory != 0) {
        // FreeFromGlobalHeap(navNodePool->memory);
        navNodePool->memory = 0;
    }

    if (param_2 != 0) {
        navNodePool->memory = AllocateMemory(param_1 * 4);
        memset(navNodePool->memory, 0, param_1 * 4);
        navNodePool->size = param_1;
    }
    else {
        navNodePool->size = param_1;
    }
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
                MemoryPool_Allocate(navNodePool->size, 1);
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
