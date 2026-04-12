#include "SC_Question.h"
#include "GameState.h"
#include "SpriteAction.h"
extern "C" void WriteToLog(const char* format, ...);
#include <string.h>
#include <stdio.h>

#include "globals.h"
#define g_GameState_0046aa30 (g_GameState_0046aa30)

SC_Message::~SC_Message() {}

/* Function start: 0x444E60 */
int SC_Message::LBLParse(char* param_1)
{
    extern GameState* g_StringState_0046aa38;
    char local_30[32];
    char local_54[32];
    char local_74[32];
    int idx;
    SpriteAction* target = (SpriteAction*)targetAddress;

    local_54[0] = '\0';
    local_74[0] = '\0';
    local_30[0] = '\0';
    sscanf(param_1, "%s", local_30);

    if (strcmp(local_30, "ADDRESS") == 0) {
        sscanf(param_1, "%s %s %s", local_30, local_54, local_74);
        idx = g_StringTable_0046aa34->FindState(local_54);
        target->addressType = idx;
        if (target->addressType < 0) {
            ShowError("SC_MessageParser::ADDRESS illegal index %s %s", local_54, param_1);
        }
        if (target->addressType == 2) {
            if (g_GameState_0046aa30 != 0) {
                idx = g_GameState_0046aa30->FindState(local_74);
                if (idx < 0 || g_GameState_0046aa30->maxStates - 1 < idx) {
                    ShowError("Invalid gamestate %d", idx);
                }
                target->addressValue = idx;
            }
        } else {
            sscanf(local_74, "%d", &target->addressValue);
        }
    } else if (strcmp(local_30, "FROM") == 0) {
        sscanf(param_1, "%s %s %s", local_30, local_54, local_74);
        idx = g_StringTable_0046aa34->FindState(local_54);
        target->fromType = idx;
        if (target->fromType < 0) {
            ShowError("FROM illegal index %s %s", local_54, param_1);
        }
        if (target->fromType == 2) {
            if (g_GameState_0046aa30 != 0) {
                idx = g_GameState_0046aa30->FindState(local_74);
                if (idx < 0 || g_GameState_0046aa30->maxStates - 1 < idx) {
                    ShowError("Invalid gamestate %d", idx);
                }
                target->fromValue = idx;
            }
        } else {
            sscanf(local_74, "%d", &target->fromValue);
        }
    } else if (strcmp(local_30, "INSTRUCTION") == 0) {
        sscanf(param_1, "%s %s", local_30, local_54);
        idx = ((GameState*)g_StringState_0046aa38)->FindState(local_54);
        target->instruction = idx;
        if (target->instruction < 0) {
            ShowError("INSTRUCTION illegal index %d %s", idx, param_1);
        }
    } else if (strcmp(local_30, "MESSAGE") == 0) {
        if ((int)target->childAction != 0) {
            ShowError("double reserve in Message %s", param_1);
        }
        SpriteAction* child = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        target->childAction = child;
        ParseSpriteAction(child, this);
    } else if (strcmp(local_30, "MOUSE") == 0) {
        sscanf(param_1, "%s %d %d", local_30, &target->mousePos.x, &target->mousePos.y);
    } else if (strcmp(local_30, "BUTTON1") == 0) {
        sscanf(param_1, "%s %d", local_30, &target->button1);
    } else if (strcmp(local_30, "BUTTON2") == 0) {
        sscanf(param_1, "%s %d", local_30, &target->button2);
    } else if (strcmp(local_30, "LASTKEY") == 0) {
        sscanf(param_1, "%s %d", local_30, &target->lastKey);
    } else if (strcmp(local_30, "TIME") == 0) {
        sscanf(param_1, "%s %lu", local_30, &target->time);
    } else if (strcmp(local_30, "EXTRA1") == 0) {
        sscanf(param_1, "%s %lu", local_30, &target->extra1);
    } else if (strcmp(local_30, "EXTRA2") == 0) {
        sscanf(param_1, "%s %lu", local_30, &target->extra2);
    } else if (strcmp(local_30, "END") == 0) {
        return 1;
    } else {
        Parser::LBLParse("SC_Message");
    }

    return 0;
}

void SC_Message::Dump(int) {}

/* Function start: 0x445450 */
void ParseSpriteAction(void* param_1, void* param_2)
{
    SC_Message msg(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    msg.targetAddress = (int)param_1;
    Parser::ProcessFile(&msg, (Parser*)param_2, 0);
}
