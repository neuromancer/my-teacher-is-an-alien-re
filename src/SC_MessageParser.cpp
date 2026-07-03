#include "SC_Question.h"
#include "GameState.h"
#include "SpriteAction.h"
#include <string.h>
#include <stdio.h>

#include "globals.h"
#define g_GameState_0046aa30 (g_GameState_0046aa30)

/* Function start: 0x444E60 */
int SC_MessageParser::LBLParse(char* param_1)
{
    extern GameState* g_StringState_0046aa38;
    char local_30[32];
    char local_54[32];
    char local_74[32];
    int idx;

    local_54[0] = '\0';
    local_74[0] = '\0';
    local_30[0] = '\0';

    sscanf(param_1, " %s ", local_30);

    if (strcmp(local_30, "ADDRESS") == 0) {
        sscanf(param_1, "%s %s %s", local_30, local_54, local_74);
        idx = g_StringTable_0046aa34->FindState(local_54);
        ((SpriteAction*)targetAddress)->addressType = idx;
        if (((SpriteAction*)targetAddress)->addressType < 0) {
            ShowError("SC_MessageParser::ADDRESS illegal index %s %s", local_54, param_1);
        }
        if (((SpriteAction*)targetAddress)->addressType == 2) {
            if (g_GameState_0046aa30 != 0) {
                idx = g_GameState_0046aa30->FindState(local_74);
                if (idx < 0 || g_GameState_0046aa30->maxStates - 1 < idx) {
                    ShowError("Invalid gamestate %d", idx);
                }
                ((SpriteAction*)targetAddress)->addressValue = idx;
            }
        } else {
            sscanf(local_74, "%lu", &((SpriteAction*)targetAddress)->addressValue);
        }
    } else if (strcmp(local_30, "FROM") == 0) {
        sscanf(param_1, "%s %s %s", local_30, local_54, local_74);
        idx = g_StringTable_0046aa34->FindState(local_54);
        ((SpriteAction*)targetAddress)->fromType = idx;
        if (((SpriteAction*)targetAddress)->fromType < 0) {
            ShowError("FROM illegal index %s %s", local_54, param_1);
        }
        if (((SpriteAction*)targetAddress)->fromType == 2) {
            if (g_GameState_0046aa30 != 0) {
                idx = g_GameState_0046aa30->FindState(local_74);
                if (idx < 0 || g_GameState_0046aa30->maxStates - 1 < idx) {
                    ShowError("Invalid gamestate %d", idx);
                }
                ((SpriteAction*)targetAddress)->fromValue = idx;
            }
        } else {
            sscanf(local_74, "%lu", &((SpriteAction*)targetAddress)->fromValue);
        }
    } else if (strcmp(local_30, "INSTRUCTION") == 0) {
        sscanf(param_1, "%s %s", local_30, local_54);
        idx = ((GameState*)g_StringState_0046aa38)->FindState(local_54);
        ((SpriteAction*)targetAddress)->instruction = idx;
        if (((SpriteAction*)targetAddress)->instruction < 0) {
            ShowError("INSTRUCTION illegal index %d %s", idx, param_1);
        }
    } else if (strcmp(local_30, "MESSAGE") == 0) {
        if ((int)((SpriteAction*)targetAddress)->childAction != 0) {
            ShowError("double reserve in Message %s", param_1);
        }
        SpriteAction* child = new SpriteAction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        ((SpriteAction*)targetAddress)->childAction = child;
        ParseSpriteAction(child, this);
    } else if (strcmp(local_30, "MOUSE") == 0) {
        sscanf(param_1, "%s %d %d", local_30, &((SpriteAction*)targetAddress)->mousePos.x, &((SpriteAction*)targetAddress)->mousePos.y);
    } else if (strcmp(local_30, "BUTTON1") == 0) {
        sscanf(param_1, "%s %d", local_30, &((SpriteAction*)targetAddress)->button1);
    } else if (strcmp(local_30, "BUTTON2") == 0) {
        sscanf(param_1, "%s %d", local_30, &((SpriteAction*)targetAddress)->button2);
    } else if (strcmp(local_30, "LASTKEY") == 0) {
        sscanf(param_1, "%s %d", local_30, &((SpriteAction*)targetAddress)->lastKey);
    } else if (strcmp(local_30, "TIME") == 0) {
        sscanf(param_1, "%s %lu", local_30, &((SpriteAction*)targetAddress)->time);
    } else if (strcmp(local_30, "EXTRA1") == 0) {
        sscanf(param_1, "%s %lu", local_30, &((SpriteAction*)targetAddress)->extra1);
    } else if (strcmp(local_30, "EXTRA2") == 0) {
        sscanf(param_1, "%s %lu", local_30, &((SpriteAction*)targetAddress)->extra2);
    } else if (strcmp(local_30, "END") == 0) {
        return 1;
    } else {
        ReportUnknownLabel("SC_Message");
    }

    return 0;
}

void SC_MessageParser::Dump(int) {}

/* Function start: 0x445450 */
void ParseSpriteAction(void* param_1, void* param_2)
{
    SC_MessageParser msg((int)param_1);
    Parser::ProcessFile(&msg, (Parser*)param_2, 0);
}
