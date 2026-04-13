#include "mCNavNode.h"
#include "stubs.h"
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

// mCNavNode_TypeA stubs

/* Function start: 0x44AB50 */
int mCNavNode_TypeA::Activate() {
    TODO("mCNavNode_TypeA::Activate");
    return 0;
}

/* Function start: 0x44ABE0 */
int mCNavNode_TypeA::LBLParse(char* line) {
    TODO("mCNavNode_TypeA::LBLParse");
    return 0;
}

/* Function start: 0x44AC30 */
int mCNavNode_TypeA::virtual4() {
    TODO("mCNavNode_TypeA::virtual4");
    return 0;
}

// mCNavNode_TypeD stubs

/* Function start: 0x44A560 */
int mCNavNode_TypeD::Activate() {
    TODO("mCNavNode_TypeD::Activate");
    return 0;
}

/* Function start: 0x44A590 */
int mCNavNode_TypeD::LBLParse(char* line) {
    TODO("mCNavNode_TypeD::LBLParse");
    return 0;
}

/* Function start: 0x44A6C0 */
int mCNavNode_TypeD::virtual4() {
    TODO("mCNavNode_TypeD::virtual4");
    return 0;
}

// mCNavNode_TypeB stubs

/* Function start: 0x44A6D0 */
int mCNavNode_TypeB::Activate() {
    TODO("mCNavNode_TypeB::Activate");
    return 0;
}

/* Function start: 0x44A710 */
int mCNavNode_TypeB::LBLParse(char* line) {
    TODO("mCNavNode_TypeB::LBLParse");
    return 0;
}

/* Function start: 0x44A8F0 */
int mCNavNode_TypeB::virtual4() {
    TODO("mCNavNode_TypeB::virtual4");
    return 0;
}

// mCNavNode_TypeC stubs

/* Function start: 0x44AC40 */
int mCNavNode_TypeC::Activate() {
    TODO("mCNavNode_TypeC::Activate");
    return 0;
}

/* Function start: 0x44AC50 */
int mCNavNode_TypeC::virtual4() {
    TODO("mCNavNode_TypeC::virtual4");
    return 0;
}

// mCNavNode_TypeE stubs

/* Function start: 0x44AC60 */
int mCNavNode_TypeE::Activate() {
    TODO("mCNavNode_TypeE::Activate");
    return 0;
}

/* Function start: 0x44AC90 */
int mCNavNode_TypeE::LBLParse(char* line) {
    TODO("mCNavNode_TypeE::LBLParse");
    return 0;
}

/* Function start: 0x44AD80 */
int mCNavNode_TypeE::virtual4() {
    TODO("mCNavNode_TypeE::virtual4");
    return 0;
}
