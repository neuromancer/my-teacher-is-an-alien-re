#include "mCNavNode.h"
#include "NavSubNode.h"
#include "stubs.h"
#include "SC_CombatBase.h"
#include "Sprite.h"
#include "InputManager.h"
#include "MouseControl.h"
#include "globals.h"
#include "string.h"
#include "Animation.h"
#include "mCNavigator.h"
#include "GameState.h"
#include "SpriteAction.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

extern char GetDirectionChar(int dir);
extern int FindCharIndex(char ch);
extern char* MakeAnimName(int index);
extern "C" char* FormatAssetPath(char* format, ...);
extern "C" int FileExists(const char* path);
extern void EnqueueSpriteAction(void* action);
extern void ParseSpriteAction(void* action, void* parent);
extern void ReportUnknownLabel(const char* name);

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
/* Function start: 0x44AF40 */
int mCNavNode::LBLParse(char* param_1)
{
    char token1[32];
    char token2[64];
    char token3[64];

    token1[0] = 0;
    sscanf(param_1, " %s %s %s ", token1, token2, token3);

    if (stricmp(token1, "FOR_BEARING") == 0) {
        int dir = FindCharIndex(token2[0]);
        NavSubNode* node = 0;

        if (stricmp(token3, "DO_NOTHING") == 0) {
            node = new NavSubNode();
        }
        else if (stricmp(token3, "DO_EXIT") == 0) {
            node = new mCNavNode_TypeC(0);
        }
        else if (stricmp(token3, "DO_ANIM") == 0) {
            node = new mCNavNode_TypeA(0);
        }
        else if (stricmp(token3, "DO_POSTMESSAGE") == 0) {
            node = new mCNavNode_TypeD(0);
        }
        else if (stricmp(token3, "DO_CHECKMESSAGE") == 0) {
            node = new mCNavNode_TypeB(0);
        }
        else if (stricmp(token3, "DO_LOGICFNC") == 0) {
            node = new mCNavNode_TypeLogic(0);
        }
        else if (stricmp(token3, "DO_BG") == 0) {
            node = new BG_SubNode();
        }
        else if (stricmp(token3, "DO_ONDIR") == 0) {
            node = new OnDir_SubNode();
        }
        else if (stricmp(token3, "DO_MOUSE") == 0) {
            node = new mCNavNode_TypeE(0);
        }
        else {
            ReportUnknownLabel("NavNode");
        }

        neighborNodes[dir] = node;
        if (node != 0) {
            node->parentNode = (int)this;
            node->field_94 = dir;
            char* pos = strstr(param_1, token3);
            if (pos != 0) {
                node->LBLParse(pos + strlen(token3));
            }
        }
    }
    else if (stricmp(token1, "END") == 0) {
        return 1;
    }
    else {
        ReportUnknownLabel("NavNode");
    }

    return 0;
}

// mCNavNode_TypeA stubs

/* Function start: 0x44AB50 */
int mCNavNode_TypeA::Activate() {
    char* name = MakeAnimName(animHandle);
    char* path = FormatAssetPath(name);
    if (FileExists(path)) {
        Animation anim;
        anim.Play(path, 0);
    }
    return 1;
}

/* Function start: 0x44ABE0 */
int mCNavNode_TypeA::LBLParse(char* line) {
    char str[32];
    int parsed = sscanf(line, " %d THEN_GOTO N%d BEARING %s ",
                        &animHandle, &nodeHandle, str);
    if (parsed >= 3) {
        bearing = FindCharIndex(str[0]);
    }
    return 0;
}

/* Function start: 0x44AC30 */
int mCNavNode_TypeA::virtual4() {
    return NavSubNode::virtual4();
}

// mCNavNode_TypeD stubs

/* Function start: 0x44A560 */
int mCNavNode_TypeD::Activate() {
    int* act = action;
    int result = 1;
    if (act[4] == 4) {
        result = 2;
    }
    EnqueueSpriteAction(act);
    return result;
}

/* Function start: 0x44A590 */
int mCNavNode_TypeD::LBLParse(char* line) {
    char token[32];
    token[0] = 0;
    sscanf(line, " %s ", token);
    if (stricmp(token, "PARSE:") == 0) {
        Parser::ProcessFile(this, (Parser*)parentNode, 0);
        return 1;
    }
    if (stricmp(token, "MESSAGE") == 0) {
        SpriteAction* sa = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        action = (int*)sa;
        ParseSpriteAction(sa, (void*)parentNode);
        return 0;
    }
    if (stricmp(token, "END") == 0) {
        return 1;
    }
    ReportUnknownLabel("PostMessage_SubNode");
    return 0;
}

/* Function start: 0x44A6C0 */
int mCNavNode_TypeD::virtual4() {
    return NavSubNode::virtual4();
}

// mCNavNode_TypeLogic (DO_LOGICFNC)

/* Function start: 0x44A900 */
mCNavNode_TypeLogic::~mCNavNode_TypeLogic()
{
    if (action != 0) {
        delete action;
        action = 0;
    }
}

/* Function start: 0x44A9A0 */
int mCNavNode_TypeLogic::Activate()
{
    int handled = g_CombatEngine_0046ae78->HandleAction((int*)action);
    return handled == 0 ? 1 : 2;
}

/* Function start: 0x44A9C0 */
int mCNavNode_TypeLogic::LBLParse(char* line)
{
    char token[32];
    char bearingText[32];

    token[0] = 0;
    sscanf(line, " %s ", token);

    if (stricmp(token, "PARSE:") == 0) {
        Parser::ProcessFile(this, (Parser*)parentNode, 0);
        return 1;
    }
    if (stricmp(token, "MESSAGE") == 0) {
        action = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        ParseSpriteAction(action, (void*)parentNode);
        return 0;
    }
    if (stricmp(token, "THEN_GOTO") == 0) {
        if (sscanf(line, " THEN_GOTO N%d BEARING %s ", &nodeHandle, bearingText) > 1) {
            bearing = FindCharIndex(bearingText[0]);
        }
        return 0;
    }
    if (stricmp(token, "END") == 0) {
        return 1;
    }
    ReportUnknownLabel("LogicFnc_SubNode");
    return 0;
}

/* Function start: 0x44AB40 */
int mCNavNode_TypeLogic::virtual4()
{
    return NavSubNode::virtual4();
}

// mCNavNode_TypeB stubs

/* Function start: 0x44A6D0 */
int mCNavNode_TypeB::Activate() {
    int idx = g_GameState_0046aa30->CheckCondition(conditionPtr);
    nodeHandle = destNode[idx];
    bearing = destBearing[idx];
    return 1;
}

/* Function start: 0x44A710 */
int mCNavNode_TypeB::LBLParse(char* line) {
    char token[32];
    char str1[32];
    char str2[32];
    int result_int;
    int idx = 0;

    token[0] = 0;
    sscanf(line, " %s ", token);

    if (stricmp(token, "PARSE:") == 0) {
        Parser::ProcessFile(this, (Parser*)parentNode, 0);
        return 1;
    }
    if (stricmp(token, "MESSAGE") == 0) {
        SpriteAction* sa = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        conditionPtr = (int*)sa;
        ParseSpriteAction(sa, (void*)parentNode);
        return 0;
    }
    if (stricmp(token, "ON") == 0) {
        int parsed = sscanf(line, " ON %s THEN_GOTO N%d BEARING %s ",
                            str1, &result_int, str2);
        if (parsed < 3) {
            ReportUnknownLabel("CheckMessage_SubNode");
        }
        if (stricmp(str1, "FALSE") == 0) {
            idx = 0;
        } else if (stricmp(str1, "TRUE") == 0) {
            idx = 1;
        } else {
            ReportUnknownLabel("CheckMessage_SubNode");
        }
        destNode[idx] = result_int;
        destBearing[idx] = FindCharIndex(str2[0]);
        return 0;
    }
    if (stricmp(token, "END") == 0) {
        return 1;
    }
    ReportUnknownLabel("CheckMessage_SubNode");
    return 0;
}

/* Function start: 0x44A8F0 */
int mCNavNode_TypeB::virtual4() {
    return NavSubNode::virtual4();
}

// mCNavNode_TypeC stubs

/* Function start: 0x44AC40 */
int mCNavNode_TypeC::Activate() {
    return 3;
}

/* Function start: 0x44AC50 */
int mCNavNode_TypeC::virtual4() {
    return NavSubNode::virtual4();
}

// mCNavNode_TypeE stubs

/* Function start: 0x44AC60 */
int mCNavNode_TypeE::Activate() {
    int state = stateValue;
    Sprite* spr = g_Mouse_0046aa18->m_sprite;
    if (spr != 0) {
        spr->ResetAnimation(state, 0);
    }
    return 1;
}

/* Function start: 0x44AC90 */
int mCNavNode_TypeE::LBLParse(char* line) {
    char str1[32];
    char str2[32];
    int parsed = sscanf(line, " %s THEN_GOTO N%d BEARING %s ",
                        str1, &nodeHandle, str2);
    if (parsed >= 3) {
        bearing = FindCharIndex(str2[0]);
    }
    if (strcmp(str1, "NO_MOUSE") == 0) {
        stateValue = -1;
        return 0;
    }
    if (strcmp(str1, "AUTO_MOUSE") == 0) {
        stateValue = 0;
        return 0;
    }
    stateValue = g_GameState_0046aa30->FindStateByName(str1);
    return 0;
}

/* Function start: 0x44AD80 */
int mCNavNode_TypeE::virtual4() {
    return NavSubNode::virtual4();
}
