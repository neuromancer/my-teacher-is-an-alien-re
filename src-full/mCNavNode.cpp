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

extern "C" char GetDirectionChar(int dir);

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
    int dir = g_Navigator_0046ae70->bearing;
    mCNavNode* neighbor = (mCNavNode*)neighborNodes[dir];
    if (neighbor == 0) {
        ShowError("Need Node Info for %s bearing %c", nodeName, (int)(signed char)GetDirectionChar(dir));
    }
    neighbor->virtual7();
}

/* Function start: 0x44B830 */
int mCNavNode::CallActivate()
{
    int dir = g_Navigator_0046ae70->bearing;
    mCNavNode* neighbor = (mCNavNode*)neighborNodes[dir];
    if (neighbor == 0) {
        ShowError("Need Node Info for %s bearing %c", nodeName, (int)(signed char)GetDirectionChar(dir));
    }
    return neighbor->Activate();
}

/* Function start: 0x44B880 */
int mCNavNode::CallGetNextNode()
{
    int dir = g_Navigator_0046ae70->bearing;
    mCNavNode* neighbor = (mCNavNode*)neighborNodes[dir];
    if (neighbor == 0) {
        ShowError("Need Node Info for %s bearing %c", nodeName, (int)(signed char)GetDirectionChar(dir));
    }
    return neighbor->GetNextNode();
}

// Stub virtual methods
void mCNavNode::virtual3() {}
int mCNavNode::virtual4() { return 0; }
int mCNavNode::Activate() { return 0; }
int mCNavNode::GetNextNode() { return 0; }
void mCNavNode::virtual7() {}
int mCNavNode::LBLParse(char*) { return 0; }
