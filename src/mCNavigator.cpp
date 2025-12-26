#include "mCNavigator.h"
#include "string.h"
#include <string.h>
#include "stdio.h"
#include "stdlib.h"
#include "Memory.h"

extern "C" {
#include "mss.h"
}

extern void ShowError(const char* message, ...);
extern void* NavNode_Constructor(void*);
extern void* MemoryPool_Allocate(void*, int, int);
extern void* ObjectPool_Allocate_2(int);

extern Parser* Parser_Parser(Parser* parser, Parser* dst, char* key_format);

extern int mCNavNode_Update(void*);
extern int mCNavNode_GetNextNode(void*);
extern int FindCharIndex(char*);
extern void CleanupObjectArray(void*, int);

extern Sprite* DAT_004360a0;

void* ObjectPool::Allocate_2()
{
    if (this->freeList == 0) {
        int* p = (int*)AllocateMemory(this->objectSize * 0x10 + 4);
        *p = (int)this->memoryBlock;
        this->memoryBlock = p;

        int i = this->objectSize;
        p = p + i * 4 - 3;
        for (i--; i >= 0; i--) {
            *p = (int)this->freeList;
            this->freeList = p;
            p = p - 4;
        }
    }

    int* p = (int*)this->freeList;
    this->freeList = (void*)*p;
    this->allocatedCount++;
    p[2] = 0;
    p[3] = 0;
    return p;
}

/* Function start: 0x413670 */
mCNavigator::mCNavigator()
{
    memset(&this->sprite, 0, 32);
    this->startingNode = 1;
    this->field_98 = 0;
}

/* Function start: 0x4136F0 */
mCNavigator::~mCNavigator()
{
    if (this->navNodePool) {
        if (this->navNodePool->memory && this->navNodePool->size) {
            unsigned int i = 0;
            do {
                for (NavNode* node = *(NavNode**)((int)this->navNodePool->memory + i * 4); node; node = node->next) {
                    CleanupObjectArray(node->field_C, 1);
                }
                i++;
            } while (i < this->navNodePool->size);
        }

        if (this->navNodePool->memory) {
            FreeMemory(this->navNodePool->memory);
            this->navNodePool->memory = 0;
        }

        NavNode* block = (NavNode*)this->navNodePool->memoryBlock;
        while (block) {
            NavNode* next = block->next;
            FreeMemory(block);
            block = next;
        }
        this->navNodePool->memoryBlock = 0;

        FreeMemory(this->navNodePool);
        this->navNodePool = 0;
    }

    if (this->sprite) {
        this->sprite->~Sprite();
        FreeMemory(this->sprite);
        this->sprite = 0;
    }
}

/* Function start: 0x413810 */
void mCNavigator::SetBearing(char* param_1)
{
    int iVar1 = FindCharIndex(param_1);
    this->field_98 = iVar1;
    if (5 < iVar1) {
        ShowError("mCNavigator::SetBearing() - Undefined direction '%s' %d", param_1, iVar1);
    }
}

/* Function start: 0x413840 */
void mCNavigator::SetCurrentNode()
{
    if (this->sprite) {
        // Sprite_Init(this->sprite);
    }

    if (this->navNodePool) {
        NavNode* node;
        for (node = *(NavNode**)((int)this->navNodePool->memory + ((this->startingNode >> 4) % this->navNodePool->size) * 4); node; node = node->next) {
            if (node->field_8 == this->startingNode) {
                break;
            }
        }

        if (node) {
            this->currentNode = node->field_C;
        }

        this->field_A0 = this->startingNode;
        if (this->currentNode) {
            DAT_004360a0 = this->sprite;
        }
    }
}

/* Function start: 0x413BC0 */
int mCNavigator::Update()
{
    if (this->currentNode == 0) {
        return 0;
    }

    int result = mCNavNode_Update(this->currentNode);
    if (result != 1) {
        if (result != 3 && result != 2) {
            return 0;
        }
        // *DAT_00435f28 = 2;
        return result;
    }

    int nextNodeId = mCNavNode_GetNextNode(this->currentNode);
    this->field_A0 = *(int*)((int)this->currentNode + 0xdc);

    int hash = (nextNodeId >> 4) / this->navNodePool->size;
    NavNode* node = *(NavNode**)((int)this->navNodePool->memory + hash * 4);
    while (node) {
        if (node->field_8 == nextNodeId) {
            break;
        }
        node = node->next;
    }

    if (node == 0) {
        return 2;
    }

    this->currentNode = node->field_C;
    return 0;
}

/* Function start: 0x413C80 */
void mCNavigator::Allocate()
{
    if (this->navNodePool == 0) {
        return;
    }

    this->navNodePool->Allocate_2();
}

/* Function start: 0x413CF0 */
void mCNavigator::MemoryPool_Allocate(unsigned int param_1, int param_2)
{
    if (this->navNodePool->memory != 0) {
        // FreeFromGlobalHeap(this->navNodePool->memory);
        this->navNodePool->memory = 0;
    }

    if (param_2 != 0) {
        this->navNodePool->memory = AllocateMemory(param_1 * 4);
        memset(this->navNodePool->memory, 0, param_1 * 4);
        this->navNodePool->size = param_1;
    }
    else {
        this->navNodePool->size = param_1;
    }
}


/* Function start: 0x4138c0 */
int mCNavigator::LBLParse(char* param_1)
{
    char local_b4[128];
    char local_34[32];
    void* local_14;
    int local_10;
    int local_8;

    local_b4[0] = '\0';
    local_34[0] = '\0';
    sscanf(param_1, "%s", local_34);
    if (_strcmpi(local_34, "BEARING") == 0) {
        sscanf(param_1, "%s %s", local_34, local_b4);
        SetBearing(local_b4);
        return 1;
    }
    else if (_strcmpi(local_34, "BASENODE") == 0) {
        if (this->navNodePool == 0) {
            this->navNodePool = (ObjectPool*)AllocateMemory(sizeof(ObjectPool));
            if (this->navNodePool) {
                this->navNodePool->memory = 0;
                this->navNodePool->size = 0x11;
                this->navNodePool->allocatedCount = 0;
                this->navNodePool->freeList = 0;
                this->navNodePool->memoryBlock = 0;
                this->navNodePool->objectSize = 10;
            }
        }

        Parser* parser = (Parser*)AllocateMemory(0x100);
        NavNode_Constructor(parser);
        Parser_Parser(parser, this, 0);

        if (*(int*)((int)parser + 0xdc) == 0) {
            ShowError("mCNavigator::LoadNodes() - Invalid Node Handle (%d)", 0);
        }

        int nodeId = *(int*)((int)parser + 0xdc);
        int hash = (nodeId >> 4) % this->navNodePool->size;

        NavNode* node = *(NavNode**)((int)this->navNodePool->memory + hash * 4);
        while (node) {
            if (node->field_8 == nodeId) {
                ShowError("mCNavigator::LoadNodes() - %s has a dublicate node handle (%d)", (char*)parser + 0xe0, nodeId);
            }
            node = node->next;
        }

        node = (NavNode*)this->navNodePool->Allocate_2();
        node->field_4 = hash;
        node->field_8 = nodeId;
        node->next = *(NavNode**)((int)this->navNodePool->memory + hash * 4);
        *(NavNode**)((int)this->navNodePool->memory + hash * 4) = node;
        node->field_C = parser;
    }
    else if (_strcmpi(local_34, "SPRITE") == 0) {
        this->sprite = (Sprite*)AllocateMemory(0xd8);
        if (this->sprite) {
            this->sprite->Sprite::Sprite((char*)0);
        }
        Parser_Parser((Parser*)this->sprite, (Parser*)this, (char*)0);
    }
    else if (_strcmpi(local_34, "STARTING_NODE") == 0) {
        sscanf(param_1, "%s %d", local_34, &this->startingNode);
    }
    else if (_strcmpi(local_34, "END") == 0) {
        return 1;
    }
    else {
        return Parser::LBLParse("mCNavigator");
    }

    return 0;
}
