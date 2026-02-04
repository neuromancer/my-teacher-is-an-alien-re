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
extern void CleanupObjectArray(void*, int);

// Global for game outcome state
extern GameOutcome* g_GameOutcome;

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

/* Function start: 0x413670 */
mCNavigator::mCNavigator()
{
    memset(&field_88, 0, 32);
    startingNode = 1;
    field_98 = 0;
}

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
        NavNode* node = ((NavNode**)navNodePool->memory)[i];
        while (node) {
            CleanupObjectArray(&node->field_C, 1);
            node = node->next;
        }
        i++;
    } while (i < navNodePool->size);

free_memory:
    delete navNodePool->memory;
    navNodePool->memory = 0;
    navNodePool->allocatedCount = 0;
    navNodePool->freeList = 0;

    block = navNodePool->memoryBlock;
    while (block) {
        next = *(void**)block;
        delete block;
        block = next;
    }
    navNodePool->memoryBlock = 0;

    delete navNodePool;
    navNodePool = 0;

check_sprite:
    if (sprite) {
        delete sprite;
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

/* Function start: 0x413840 */
void mCNavigator::OnProcessEnd()
{
	if (sprite) {
		sprite->Init();
	}

	ObjectPool* pool = navNodePool;
	if (pool) {
		int id = startingNode;
		unsigned int h = (unsigned int)id >> 4;
		h %= pool->size;

		NavNode* n = 0;
		if (pool->memory) {
			n = ((NavNode**)pool->memory)[h];
			while (n) {
				if (n->field_8 == id) {
					break;
				}
				n = n->next;
			}
		}

		if (n) {
			currentNode = n->field_C;
		}

		field_A0 = id;
		if (currentNode) {
			g_Sprite_004360a0 = sprite;
		}
	}
}

/* Function start: 0x4138c0 */
int mCNavigator::LBLParse(char* param_1)
{
	char token[32];
	char value[128];
	mCNavNode* parser;

	value[0] = '\0';
	token[0] = '\0';
	sscanf(param_1, " %s ", token);
	if (_strcmpi(token, "BEARING") == 0) {
		sscanf(param_1, "%s %s", token, value);
		SetBearing(value);
	}
	else if (_strcmpi(token, "BASENODE") == 0) {
		if (navNodePool == 0) {
			navNodePool = new ObjectPool(0x11, 0xa);
		}

		parser = new mCNavNode();
		Parser::ProcessFile(parser, this, 0);

		if (parser->nodeHandle == 0) {
			ShowError("mCNavigator::LoadNodes() - Invalid Node Handle (%d)", 0);
		}

		unsigned int handle = parser->nodeHandle;
		ObjectPool* pool = navNodePool;
		unsigned int h = (handle >> 4) % pool->size;

		NavNode* node = 0;
		if (pool->memory) {
			node = ((NavNode**)pool->memory)[h];
			while (node) {
				if (node->field_8 == handle) {
					break;
				}
				node = node->next;
			}
		}

		if (node) {
			ShowError("mCNavigator::LoadNodes() - %s has a dublicate node handle (%d)", parser->nodeName, handle);
		}

		handle = parser->nodeHandle;
		pool = navNodePool;
		h = (handle >> 4) % pool->size;
		
		node = 0;
		if (pool->memory) {
			node = ((NavNode**)pool->memory)[h];
			while (node) {
				if (node->field_8 == handle) {
					break;
				}
				node = node->next;
			}
		}

		if (node == 0) {
			if (pool->memory == 0) {
				pool->MemoryPool_Allocate(pool->size, 1);
			}
			node = (NavNode*)pool->Allocate_2();
			node->field_4 = h;
			node->field_8 = handle;
			node->next = ((NavNode**)pool->memory)[h];
			((NavNode**)pool->memory)[h] = node;
		}
		node->field_C = parser;
	}
	else if (_strcmpi(token, "SPRITE") == 0) {
		sprite = new Sprite(0);
		Parser::ProcessFile(sprite, this, (char*)0);
	}
	else if (_strcmpi(token, "STARTING_NODE") == 0) {
		sscanf(param_1, "%s %d", token, &startingNode);
	}
	else if (_strcmpi(token, "END") == 0) {
		return 1;
	}
	else {
		return Parser::LBLParse("mCNavigator");
	}

	return 0;
}

/* Function start: 0x413BC0 */
int mCNavigator::Update()
{
	if (currentNode == 0) {
		return 0;
	}

	int result = ((mCNavNode*)currentNode)->Update();
	if (result == 1) {
		int nextNodeId = ((mCNavNode*)currentNode)->GetNextNode();
		ObjectPool* pool = navNodePool;
		int handle = ((mCNavNode*)currentNode)->nodeHandle;
		unsigned int h = (unsigned int)nextNodeId >> 4;
		field_A0 = handle;
		h %= pool->size;

		NavNode* node = 0;
		if (pool->memory) {
			node = ((NavNode**)pool->memory)[h];
			while (node) {
				if (node->field_8 == nextNodeId) {
					break;
				}
				node = node->next;
			}
		}

		if (node == 0) {
			return 2;
		}

		currentNode = node->field_C;
		return 0;
	}

	if (result == 3 || result == 2) {
		g_GameOutcome->outcome = 2;
		return result;
	}

	return 0;
}

/* Function start: 0x413C80 */
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
