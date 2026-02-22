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
extern GameOutcome* g_GameOutcome_00435f28;

// NavNode constructor - initializes a 0x100 byte structure
void* NavNode_Constructor(void* mem) { return mem; }

// Direction characters for navigation (6 directions: N, E, S, W, U, D)
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
    int n;
    ObjectPool* pool;

    pool = navNodePool;
    if (pool == 0) {
        goto check_sprite;
    }

    if (pool->memory == 0) {
        goto free_memory;
    }

    i = 0;
    if (pool->size == 0) {
        goto free_memory;
    }

    do {
        NavNode* node = ((NavNode**)pool->memory)[i];
        while (node) {
            CleanupObjectArray(&node->field_C, 1);
            n = 0;
            while (n--)
                ;
            node = node->next;
        }
        i++;
    } while (i < pool->size);

free_memory:
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
	unsigned int h;
	NavNode* n;

	if (sprite) {
		sprite->Init();
	}

	ObjectPool* pool = navNodePool;
	if (pool == 0) {
		return;
	}

	int id = startingNode;
	h = ((unsigned int)id >> 4) % pool->size;

	if (pool->memory == 0) {
		goto not_found;
	}
	n = ((NavNode**)pool->memory)[h];
	goto loop_test;

loop_body:
	n = (NavNode*)n->next;
loop_test:
	if (n == 0) {
		goto not_found;
	}
	if (n->field_8 == id) {
		goto found;
	}
	goto loop_body;

not_found:
	n = 0;

found:
	if (n) {
		currentNode = n->field_C;
	}

	field_A0 = id;
	if (currentNode) {
		g_Sprite_004360a0 = sprite;
	}
}

/* Function start: 0x4138c0 */
int mCNavigator::LBLParse(char* param_1)
{
	char token[32];
	char value[128];
	mCNavNode* parser;
	unsigned int h;

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

		{
			unsigned int handle = parser->nodeHandle;
			ObjectPool* pool = navNodePool;
			h = (handle >> 4) % pool->size;

			NavNode* node = 0;
			if (pool->memory == 0) {
				goto lookup1_not_found;
			}
			node = ((NavNode**)pool->memory)[h];
			goto lookup1_test;

		lookup1_next:
			node = (NavNode*)node->next;
		lookup1_test:
			if (node == 0) {
				goto lookup1_not_found;
			}
			if (node->field_8 == handle) {
				goto lookup1_found;
			}
			goto lookup1_next;

		lookup1_not_found:
			node = 0;

		lookup1_found:
			if (node) {
				ShowError("mCNavigator::LoadNodes() - %s has a dublicate node handle (%d)", parser->nodeName, handle);
			}

			handle = parser->nodeHandle;
			pool = navNodePool;
			h = (handle >> 4) % pool->size;

			node = 0;
			if (pool->memory == 0) {
				goto lookup2_not_found;
			}
			node = ((NavNode**)pool->memory)[h];
			goto lookup2_test;

		lookup2_next:
			node = (NavNode*)node->next;
		lookup2_test:
			if (node == 0) {
				goto lookup2_not_found;
			}
			if (node->field_8 == handle) {
				goto lookup2_found;
			}
			goto lookup2_next;

		lookup2_not_found:
			node = 0;

		lookup2_found:
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
	}
	else if (_strcmpi(token, "SPRITE") == 0) {
		sprite = new Sprite(0);
		Parser::ProcessFile(sprite, this, 0);
	}
	else if (_strcmpi(token, "STARTING_NODE") == 0) {
		sscanf(param_1, "%s %d", token, &startingNode);
	}
	else if (_strcmpi(token, "END") == 0) {
		return 1;
	}
	else {
		Parser::LBLParse("mCNavigator");
	}

	return 0;
}

/* Function start: 0x413BC0 */
int mCNavigator::Update()
{
	unsigned int h;
	NavNode* node;

	if (currentNode == 0) {
		return 0;
	}

	int result = ((mCNavNode*)currentNode)->Update();
	if (result == 1) {
		int nextNodeId = ((mCNavNode*)currentNode)->GetNextNode();
		ObjectPool* pool = navNodePool;
		int handle = ((mCNavNode*)currentNode)->nodeHandle;
		field_A0 = handle;
		h = ((unsigned int)nextNodeId >> 4) % pool->size;

		if (pool->memory == 0) {
			goto not_found;
		}
		node = ((NavNode**)pool->memory)[h];
		goto loop_test;

	loop_body:
		node = (NavNode*)node->next;
	loop_test:
		if (node == 0) {
			goto not_found;
		}
		if (node->field_8 == nextNodeId) {
			goto found;
		}
		goto loop_body;

	not_found:
		node = 0;

	found:
		if (node == 0) {
			return 2;
		}

		currentNode = node->field_C;
		return 0;
	}

	if (result == 3 || result == 2) {
		g_GameOutcome_00435f28->outcome = 2;
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
