#include "mCNavigator.h"
#include "mCNavNode.h"
#include "globals.h"
#include "Parser.h"
#include "Hotspot.h"
#include "string.h"
#include <string.h>
#include "stdio.h"
#include "stdlib.h"
#include "Memory.h"
#include "GameOutcome.h"

extern "C" {
#include "mss.h"
}

extern void ShowError(const char* message, ...);

// Global for game outcome state
extern GameOutcome* g_GameOutcome_00435f28;

// Direction characters for navigation (6 directions: N, E, S, W, U, D)
char* g_DirectionChars = "NESWUD";

Sprite* g_Sprite_004360a0 = 0;

#include "mCNavNode.h"

/* Function start: 0x449AE0 */
void __cdecl CleanupNavNodes(void** array, int count)
{
    void* node;
    int i = count;

    while (i-- != 0) {
        node = *array;
        if (node != 0) {
            ((mCNavNode*)node)->~mCNavNode();
            FreeMemory(node);
            *array = 0;
        }
        array++;
    }
}

/* Function start: 0x449B20 */
int FindCharIndex(char ch)
{
    int i = 0;
    while (i < 6) {
        if (g_DirectionChars[i] == ch) {
            break;
        }
        i++;
    }
    if (i == 6) {
        ShowError("Bearing - Invalid bearing '%c'", (int)ch);
    }
    return i;
}

/* Function start: 0x44B8D0 */
mCNavigator::mCNavigator()
{
    memset(&bearing, 0, 24);
    startingNode = 1;
    bearing = 0;
}

/* Function start: 0x44B950 */ /* ~90% match */
mCNavigator::~mCNavigator()
{
    unsigned int i;
    void* block;
    void* next;
    int n;
    ObjectPool* pool;

    pool = navNodePool;
    if (pool != 0) {
        if (pool->memory != 0) {
            i = 0;
            if (pool->size != 0) {
                do {
                    NavNode* node = ((NavNode**)pool->memory)[i];
                    while (node) {
                        n = 0;
                        while (n--)
                            ;
                        node = node->next;
                    }
                    i++;
                } while (i < pool->size);
            }
        }

        delete pool->memory;
        pool->memory = 0;
        pool->allocatedCount = 0;
        pool->freeList = 0;

        block = pool->memoryBlock;
        while (block) {
            next = *(void**)block;
            delete block;
            block = next;
        }
        pool->memoryBlock = 0;

        delete pool;
        navNodePool = 0;
    }

    if (sprite) {
        delete sprite;
        sprite = 0;
    }
}

// _strcmpi is in the CRT (string.h)
#include <string.h>

/* Function start: 0x44BA70 */
void mCNavigator::SetMode(int param)
{
    bearing = param;
}

/* Function start: 0x44BA80 */
void mCNavigator::SetField(unsigned int nodeId)
{
    startingNode = nodeId;
    currentNode = FindNodeInPool(nodeId);
    if (currentNode == 0) {
        ShowError("Navigator::SetNode() - invalid node N%d", nodeId);
    }
}

/* Function start: 0x44BAC0 */
void mCNavigator::SetNavParams(unsigned int param1, int param2)
{
    SetField(param1);
    SetMode(param2);
    ((mCNavNode*)currentNode)->CallActivateNeighbor();
}

/* Function start: 0x44BAF0 */
void* mCNavigator::FindNodeInPool(unsigned int nodeId)
{
    ObjectPool* pool;
    NavNode* node;
    unsigned int h;
    void* result;

    pool = navNodePool;
    node = 0;
    result = 0;
    if (pool == 0) {
        goto done;
    }

    h = (nodeId >> 4) % pool->size;
    if (pool->memory == 0) {
        goto search_done;
    }
    node = ((NavNode**)pool->memory)[h];

search_loop:
    if (node == 0) {
        goto search_done;
    }
    if (node->key == nodeId) {
        goto search_done;
    }
    node = node->next;
    goto search_loop;

search_done:
    if (node == 0) {
        goto done;
    }
    result = node->value;

done:
    return result;
}

/* Function start: 0x44BB50 */
void mCNavigator::OnProcessEnd()
{
    SetField(startingNode);
}

/* Function start: 0x44BB60 */
int mCNavigator::LBLParse(char* param_1)
{
    char value[128];
    char token[32];
    mCNavNode* parser;
    unsigned int h;

    value[0] = '\0';
    token[0] = '\0';
    sscanf(param_1, " %s ", token);

    if (_strcmpi(token, "BEARING") == 0) {
        sscanf(param_1, "%s %s", token, value);
        int idx = FindCharIndex(value[0]);
        SetMode(idx);
    }
    else if (_strcmpi(token, "BASENODE") == 0) {
        if (navNodePool == 0) {
            navNodePool = new ObjectPool(0x11, 0xa);
        }

        parser = new mCNavNode(param_1);

        Parser::ProcessFile(parser, this, (char*)0);

        if (FindNodeInPool(parser->nodeHandle) != 0) {
            ShowError("Navigator::LoadNodes() - %s has a dublicate node handle (%d)",
                      &parser->nodeName, parser->nodeHandle);
        }

        {
            unsigned int handle = parser->nodeHandle;
            ObjectPool* pool = navNodePool;
            unsigned int poolSize = pool->size;
            h = (handle >> 4) % poolSize;

            NavNode* node = 0;
            int* mem = (int*)pool->memory;
            if (mem != 0) {
                node = (NavNode*)mem[h];
                while (node != 0) {
                    if (node->key == handle) {
                        break;
                    }
                    node = node->next;
                }
            }

            if (node == 0) {
                if (mem == 0) {
                    pool->MemoryPool_Allocate(poolSize, 1);
                }
                node = (NavNode*)pool->Allocate_2();
                node->bucketIndex = h;
                node->key = handle;
                node->next = ((NavNode**)pool->memory)[h];
                ((NavNode**)pool->memory)[h] = node;
            }
            node->value = parser;
        }
    }
    else if (_strcmpi(token, "SPRITE") == 0) {
        sprite = new Sprite((char*)0);
        Parser::ProcessFile(sprite, this, (char*)0);
    }
    else if (_strcmpi(token, "STARTING_NODE") == 0) {
        sscanf(param_1, "%s N%d", token, &startingNode);
    }
    else if (_strcmpi(token, "END") == 0) {
        return 1;
    }
    else {
        Parser::LBLParse("Navigator");
    }

    return 0;
}

/* Function start: 0x44BE70 */
int mCNavigator::Update()
{
    int result;

    if (currentNode == 0) {
        return 0;
    }

    do {
        result = ((mCNavNode*)currentNode)->CallActivate();
        if (result != 1) {
            break;
        }
        int nextId = ((mCNavNode*)currentNode)->CallGetNextNode();
        SetField(nextId);
        ((mCNavNode*)currentNode)->CallActivateNeighbor();
    } while (result == 1);

    return result;
}

/* Function start: 0x44BEC0 */ /* ~89% match */
void* ObjectPool::Allocate_2()
{
    if (freeList == 0) {
        int* p = (int*)new char[objectSize * 0x10 + 4];
        *p = (int)memoryBlock;
        memoryBlock = p;

        int i = objectSize;
        int offset = i * 0x10;
        i--;
        p = (int*)((char*)p + offset -  0xc);
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
