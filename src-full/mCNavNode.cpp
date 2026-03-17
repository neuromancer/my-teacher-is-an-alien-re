#include "mCNavNode.h"
#include "SC_CombatBase.h"
#include "Sprite.h"
#include "InputManager.h"
#include "globals.h"
#include "string.h"
#include "Animation.h"
#include "mCNavigator.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Globals

// Direction characters (shared with mCNavigator)
extern char* g_DirectionChars;

// Demo version of FindCharIndex (takes char*) — used by demo SetNavLink
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

// =========================================================================
// Full game methods
// =========================================================================

/* Function start: 0x44AE10 */
mCNavNode::mCNavNode(char* param_1) : Parser()
{
    char local_30[32];
    memset(&neighborNodes, 0, 0x10 * 4);
    sscanf(param_1, " %s %s ", local_30, nodeName);
    sscanf(nodeName, "N%d", &nodeHandle);
}

/* Function start: 0x44AEC0 */
mCNavNode::~mCNavNode()
{
    int i;
    for (i = 0; i < 6; i++) {
        if (neighborNodes[i] != 0) {
            int* obj = (int*)neighborNodes[i];
            int* vtbl = (int*)*obj;
            ((void (__fastcall *)(int*, int, int))vtbl[3])(obj, 0, 1);
            neighborNodes[i] = 0;
        }
    }
}

/* Function start: 0x44B7E0 */
void mCNavNode::CallActivateNeighbor()
{
    int* gs = (int*)DAT_0046ae70;
    int dir = gs[0x90 / 4];
    mCNavNode* neighbor = (mCNavNode*)neighborNodes[dir];
    if (neighbor == 0) {
        ShowError("Need Node Info for %s bearing %c", nodeName, g_DirectionChars[dir]);
    }
    neighbor->virtual7();
}

/* Function start: 0x44B830 */
int mCNavNode::CallActivate()
{
    int* gs = (int*)DAT_0046ae70;
    int dir = gs[0x90 / 4];
    mCNavNode* neighbor = (mCNavNode*)neighborNodes[dir];
    if (neighbor == 0) {
        ShowError("Need Node Info for %s bearing %c", nodeName, g_DirectionChars[dir]);
    }
    return neighbor->Activate();
}

/* Function start: 0x44B880 */
int mCNavNode::CallGetNextNode()
{
    int* gs = (int*)DAT_0046ae70;
    int dir = gs[0x90 / 4];
    mCNavNode* neighbor = (mCNavNode*)neighborNodes[dir];
    if (neighbor == 0) {
        ShowError("Need Node Info for %s bearing %c", nodeName, g_DirectionChars[dir]);
    }
    return neighbor->GetNextNode();
}

// Stub virtual methods — full game implementations at different addresses
void mCNavNode::virtual3() {}
int mCNavNode::virtual4() { return 0; }
int mCNavNode::Activate() { return 0; }    // full game: 0x44A9A0
int mCNavNode::GetNextNode() { return 0; } // full game: 0x44ADA0
void mCNavNode::virtual7() {}

// =========================================================================
// Demo constructor (needed for demo code compatibility)
// =========================================================================

mCNavNode::mCNavNode() : Parser()
{
    memset(&neighborNodes, 0, 0x10 * 4);
    strcpy(nodeName, "NONAME");
}

// =========================================================================
// Demo-only methods (kept for linking demo code)
// =========================================================================

void mCNavNode::SetNavLink(char* direction, int nodeId)
{
    int dirIndex = FindCharIndex(direction);
    if (dirIndex >= 6) {
        ShowError("mCNavNode::SetNavLink() - node %d : Undefined direction '%s' %d", nodeHandle, direction, dirIndex);
    }
    neighborNodes[dirIndex] = (void*)nodeId;
}

void mCNavNode::AddSpriteList(char* name, int id) {}

int mCNavNode::LBLParse(char*) { return 0; }
