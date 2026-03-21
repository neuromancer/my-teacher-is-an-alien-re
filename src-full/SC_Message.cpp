#include "SC_Question.h"
#include "GameState.h"
#include <string.h>
#include <stdio.h>

extern GameState* g_StringTable_0046aa34;
extern "C" extern GameState* g_GameState_0046aa30;
#define g_GameState_0046aa30 (g_GameState_0046aa30)

SC_Message::~SC_Message() {}

/* Function start: 0x444E60 */
int SC_Message::LBLParse(char* param_1)
{
    extern GameState* DAT_0046aa38;
    char local_30[32];
    char local_54[32];
    char local_74[32];
    int idx;

    local_54[0] = '\0';
    local_74[0] = '\0';
    local_30[0] = '\0';
    sscanf(param_1, "%s", local_30);

    if (strcmp(local_30, "ADDRESS") == 0) {
        sscanf(param_1, "%s %s %s", local_30, local_54, local_74);
        if (g_StringTable_0046aa34 != 0) {
            idx = g_StringTable_0046aa34->FindState(local_54);
        } else {
            sscanf(local_54, "%d", &idx);
        }
        targetAddress = idx;
        if (targetAddress < 0) {
            ShowError("SC_MessageParser::ADDRESS illegal index %s %s", local_54, param_1);
        }
        if (targetAddress == 2) {
            if (g_GameState_0046aa30 != 0) {
                idx = g_GameState_0046aa30->FindState(local_74);
                if (idx < 0 || g_GameState_0046aa30->maxStates - 1 < idx) {
                    ShowError("Invalid gamestate %d", idx);
                }
                sourceAddress = idx;
            }
        } else {
            sscanf(local_74, "%d", &sourceAddress);
        }
    } else if (strcmp(local_30, "FROM") == 0) {
        sscanf(param_1, "%s %s %s", local_30, local_54, local_74);
        if (g_StringTable_0046aa34 != 0) {
            idx = g_StringTable_0046aa34->FindState(local_54);
        } else {
            sscanf(local_54, "%d", &idx);
        }
        command = idx;
        if (command < 0) {
            ShowError("FROM illegal index %s %s", local_54, param_1);
        }
        if (command == 2) {
            if (g_GameState_0046aa30 != 0) {
                idx = g_GameState_0046aa30->FindState(local_74);
                if (idx < 0 || g_GameState_0046aa30->maxStates - 1 < idx) {
                    ShowError("Invalid gamestate %d", idx);
                }
                data = idx;
            }
        } else {
            sscanf(local_74, "%d", &data);
        }
    } else if (strcmp(local_30, "INSTRUCTION") == 0) {
        sscanf(param_1, "%s %s", local_30, local_54);
        if (DAT_0046aa38 != 0) {
            idx = ((GameState*)DAT_0046aa38)->FindState(local_54);
        } else {
            sscanf(local_54, "%d", &idx);
        }
        priority = idx;
        if (priority < 0) {
            ShowError("INSTRUCTION illegal index %d %s", idx, param_1);
        }
    } else if (strcmp(local_30, "MESSAGE") == 0) {
        if (userPtr != 0) {
            ShowError("double reserve in Message %s", param_1);
        }
        SC_Message* msg = new SC_Message(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        userPtr = (int)msg;
        Parser::ProcessFile((Parser*)msg, (Parser*)this, 0);
    } else if (strcmp(local_30, "MOUSE") == 0) {
        sscanf(param_1, "%s %d %d", local_30, &clickX, &clickY);
    } else if (strcmp(local_30, "BUTTON1") == 0) {
        sscanf(param_1, "%s %d", local_30, &mouseX);
    } else if (strcmp(local_30, "BUTTON2") == 0) {
        sscanf(param_1, "%s %d", local_30, &mouseY);
    } else if (strcmp(local_30, "LASTKEY") == 0) {
        sscanf(param_1, "%s %d", local_30, &lastKey);
    } else if (strcmp(local_30, "TIME") == 0) {
        sscanf(param_1, "%s %lu", local_30, &time);
    } else if (strcmp(local_30, "EXTRA1") == 0) {
        sscanf(param_1, "%s %lu", local_30, &param1);
    } else if (strcmp(local_30, "EXTRA2") == 0) {
        sscanf(param_1, "%s %lu", local_30, &param2);
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
