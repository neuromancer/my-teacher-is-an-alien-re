#include "SCI_SearchScreen.h"
#include "stubs.h"
#include "SpriteAction.h"
#include "Parser.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

extern void ParseSpriteAction(void*, void*);
extern void EnqueueSpriteAction(void*);

/* Function start: 0x40B420 */
SCI_SearchScreen::SCI_SearchScreen()
{
    handlerId = 0x48;
}

/* Function start: 0x40B4B0 */
SCI_SearchScreen::~SCI_SearchScreen()
{
    ShutDown(0);
}

#include "globals.h"
#include "SC_CombatBase.h"
extern "C" void SendGameMessage(int, int, int, int, int, int, int, int, int, int);

/* Function start: 0x40B510 */
void SCI_SearchScreen::Init(SC_MessageParser* msg) {
    TODO("SCI_SearchScreen::Init");
}

/* Function start: 0x40B780 */
int SCI_SearchScreen::ShutDown(SC_MessageParser* msg) {
    if (g_CombatEngine_0046ae78 != 0) {
        delete g_CombatEngine_0046ae78;
        g_CombatEngine_0046ae78 = 0;
    }
    return SC_Combat::ShutDown(msg);
}

/* Function start: 0x40B7C0 */
void SCI_SearchScreen::Update(int p1, int p2) {
    if (handlerId == p2) {
        SC_Combat::Update(p1, p2);
    }
}

/* Function start: 0x40B7E0 */
int SCI_SearchScreen::AddMessage(SC_MessageParser* msg) {
    int ret = SC_Combat::AddMessage(msg);
    if (ret != 0) {
        return 1;
    }
    SpriteAction* action = (SpriteAction*)msg;
    if (action->lastKey == 0x1B && savedCommand == 0x2B) {
        statusPtr[2] = 1;
    }
    return 1;
}

/* Function start: 0x40B830 */
int SCI_SearchScreen::Exit(SC_MessageParser* msg) {
    SpriteAction* action = (SpriteAction*)msg;
    if (action->addressType != handlerId) {
        return 0;
    }
    SC_Combat::Exit(msg);
    return 1;
}

/* Function start: 0x40B860 */
void SCI_SearchScreen::ProcessLose() {
    TODO("SCI_SearchScreen::ProcessLose");
}

/* Function start: 0x40BA10 */
void SCI_SearchScreen::ProcessAction(int action, int* data) {
    TODO("SCI_SearchScreen::ProcessAction");
}

/* Function start: 0x40BAC0 */
void SCI_SearchScreen::OnProcessEnd() {
    TODO("SCI_SearchScreen::OnProcessEnd");
}

/* Function start: 0x40BAD0 */
/* Function start: 0x40B25E */
int SCI_SearchScreen::LBLParse(char* line) {
    char token[32];

    sscanf(line, "%s", token);

    if (strcmp(token, "POSTMESSAGE") == 0) {
        SpriteAction action(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        ParseSpriteAction(&action, this);
        EnqueueSpriteAction((void*)&action);
    } else if (strcmp(token, "ENDPERIODANNOUNCE") == 0) {
        int index, value;
        sscanf(line, "%s %d %d", token, &index, &value);
        ((int*)&statusPtr)[index + 20] = value;
    } else if (strcmp(token, "ANNOUNCE") == 0) {
        int index, value;
        sscanf(line, "%s %d %d", token, &index, &value);
        ((int*)&statusPtr)[index] = value;
    } else if (strcmp(token, "END") == 0) {
        return 1;
    } else {
        Parser::LBLParse("SCI_SearchScreen");
    }

    return 0;
}

/* Function start: 0x40EFB0 */
extern "C" void ClearGameStateSave() {
    memset(&g_SaveState0_0043d130, 0, 16);
}

static int g_SearchScreenRects[36];
static int g_DefaultSearchCoords[12];

/* Function start: 0x44E960 */
void InitSearchScreenRects()
{
    g_SearchScreenRects[1] = 0x147;
    g_SearchScreenRects[2] = 0x205;
    g_SearchScreenRects[3] = 0x16D;
    g_SearchScreenRects[4] = 0x209;
    g_SearchScreenRects[5] = 0x147;
    g_SearchScreenRects[7] = 0x16D;
    g_SearchScreenRects[9] = 0x147;
    g_SearchScreenRects[0] = 0x1DE;
    g_SearchScreenRects[6] = 0x230;
    g_SearchScreenRects[11] = 0x16D;
    g_SearchScreenRects[13] = 0x11C;
    g_SearchScreenRects[14] = 0x205;
    g_SearchScreenRects[16] = 0x209;
    g_SearchScreenRects[15] = 0x143;
    g_SearchScreenRects[17] = 0x11C;
    g_SearchScreenRects[8] = 0x234;
    g_SearchScreenRects[10] = 0x25B;
    g_SearchScreenRects[19] = 0x143;
    g_SearchScreenRects[21] = 0x11C;
    g_SearchScreenRects[12] = 0x1DE;
    g_SearchScreenRects[18] = 0x230;
    g_SearchScreenRects[23] = 0x143;
    g_SearchScreenRects[25] = 0xF1;
    g_SearchScreenRects[26] = 0x205;
    g_SearchScreenRects[28] = 0x209;
    g_SearchScreenRects[27] = 0x118;
    g_SearchScreenRects[29] = 0xF1;
    g_SearchScreenRects[20] = 0x234;
    g_SearchScreenRects[22] = 0x25B;
    g_SearchScreenRects[31] = 0x118;
    g_SearchScreenRects[33] = 0xF1;
    g_SearchScreenRects[24] = 0x1DE;
    g_SearchScreenRects[35] = 0x118;
    g_SearchScreenRects[30] = 0x230;
    g_SearchScreenRects[32] = 0x234;
    g_SearchScreenRects[34] = 0x25B;
}

/* Function start: 0x44EAE0 */
void InitDefaultSearchCoords()
{
    g_DefaultSearchCoords[1] = 0xA1;
    g_DefaultSearchCoords[3] = 0x137;
    g_DefaultSearchCoords[0] = 0x14;
    g_DefaultSearchCoords[2] = 0x46;
    g_DefaultSearchCoords[4] = 0xB2;
    g_DefaultSearchCoords[5] = 0xAC;
    g_DefaultSearchCoords[6] = 0xEF;
    g_DefaultSearchCoords[7] = 0xA1;
    g_DefaultSearchCoords[9] = 0x137;
    g_DefaultSearchCoords[8] = 0x118;
    g_DefaultSearchCoords[10] = 0x15D;
    g_DefaultSearchCoords[11] = 0x18F;
}

static void AtExitHandler() {}

/* Function start: 0x44EB60 */
extern "C" void RegisterAtExitHandler() {
    atexit(AtExitHandler);
}
