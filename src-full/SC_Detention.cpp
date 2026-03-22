#include "SC_Detention.h"
#include <string.h>
#include <stdio.h>
#include "GameState.h"
#include "GameEngine.h"
#include "SpriteAction.h"
#include "SC_Question.h"
#include "globals.h"

extern "C" void ShowError(const char* format, ...);
extern "C" void SendGameMessage(int, int, int, int, int, int, int, int, int, int);

extern "C" extern GameState* g_GameState_0046aa30;
extern "C" extern int g_GameEngine_0046a6ec;
extern char* g_Buffer_0046aa00;
extern int DAT_00468764;
extern SpriteAction DAT_00472d58;


/* Function start: 0x4098C0 */
SC_Detention::SC_Detention() {
    GameState* gs;
    int idx;

    memset(&field_A8, 0, 0x140);
    handlerId = 0x2C;

    gs = g_GameState_0046aa30;
    idx = gs->FindState("PERIOD");
    if (idx < 0 || idx >= gs->maxStates - 1) {
        ShowError("Invalid gamestate %d", idx);
    }

    sprintf(g_Buffer_0046aa00, "ACTIONSPERIOD%2.2d", gs->stateValues[idx]);

    gs = g_GameState_0046aa30;
    idx = gs->FindState(g_Buffer_0046aa00);
    if (idx < 0 || idx >= gs->maxStates - 1) {
        ShowError("Invalid gamestate %d", idx);
    }

    field_148 = gs->stateValues[idx];

    ParseFile(this, "mis\\detention.mis", "[INITIALIZATION]");

    field_154 = g_GameState_0046aa30->FindState("NUM_ACTIONS");
    field_158 = g_GameState_0046aa30->FindState("IN_PRACTICEROOM");
    field_15C = g_GameState_0046aa30->FindState("COMBAT_POD_AVAILABLE");
    field_14C = g_GameState_0046aa30->FindState("PLAYED_ANNOUNCEMENT");
}

/* Function start: 0x409AD0 */
int SC_Detention::AddMessage(SC_Message* msg) {
    GameState* gs;
    int idx;

    CopyCommandData(msg);

    switch (((int*)msg)[1]) {
    case 0:
        InitDetention();
        break;
    case 1:
        if (field_160 == 0) {
            gs = g_GameState_0046aa30;
            idx = gs->FindState("WENT_TO_CLASS");
            if (idx < 0 || idx >= gs->maxStates - 1) {
                ShowError("Invalid gamestate %d", idx);
            }
            if (gs->stateValues[idx] == 0) {
                SetupDetentionState();
                ProcessDetention();
                break;
            }
        }
        GoToClass();
        break;
    case 2:
        DrawDetention();
        break;
    case 4:
        HandleCombat();
        break;
    case 5:
        HandlePractice();
        break;
    default:
        ShowError("SC_Detention::Init invalid SCmsg->to = %d", ((int*)msg)[1]);
        break;
    }

    gs = g_GameState_0046aa30;
    idx = gs->FindState("PERIOD");
    if (idx < 0 || idx >= gs->maxStates - 1) {
        ShowError("Invalid gamestate %d", idx);
    }

    sprintf(g_Buffer_0046aa00, "ACTIONSPERIOD%2.2d", gs->stateValues[idx]);

    gs = g_GameState_0046aa30;
    idx = gs->FindState(g_Buffer_0046aa00);
    if (idx < 0 || idx >= gs->maxStates - 1) {
        ShowError("Invalid gamestate %d", idx);
    }

    field_148 = gs->stateValues[idx];

    return 0;
}

/* Function start: 0x409F70 */
int SC_Detention::Exit(SC_Message* msg) {
    GameState* gs;
    int idx;
    int* msgData;

    msgData = (int*)msg;
    if (handlerId != msgData[0]) {
        return 0;
    }

    SetupDetentionState();

    switch (msgData[4]) {
    case 0:
        return 1;
    case 5:
        DrawDetention();
        return 1;
    case 0x3B:
        if (field_160 == 0) {
            gs = g_GameState_0046aa30;
            idx = gs->FindState("WENT_TO_CLASS");
            if (idx < 0 || idx >= gs->maxStates - 1) {
                ShowError("Invalid gamestate %d", idx);
            }
            if (gs->stateValues[idx] == 0) {
                SetupDetentionState();
                ProcessDetention();
                return 1;
            }
        }
        GoToClass();
        return 1;
    case 0x3C:
        HandleInput3C();
        return 1;
    case 0x3D:
        ProcessDetention();
        return 1;
    case 0x3E:
        InitDetention();
        return 1;
    case 0x3F:
        HandleInput3F();
        return 1;
    case 0x40:
        HandleInput40();
        return 1;
    case 0x41:
        HandleCombat();
        return 1;
    case 0x42:
        HandlePractice();
        return 1;
    default:
        return 0;
    }
}

/* Function start: 0x409A80 */
SC_Detention::~SC_Detention() {
}

/* Function start: 0x40A700 */
void SC_Detention::GoToClass() {
    GameState* gs;
    int idx;

    SetupDetentionState();
    ResetAnimations();
    if (field_150 == 0) {
        gs = g_GameState_0046aa30;
        idx = gs->FindState("PERIOD");
        if (idx < 0 || idx >= gs->maxStates - 1) {
            ShowError("Invalid gamestate %d", idx);
        }
        SendGameMessage(3, field_F0[gs->stateValues[idx]], 0x25, 1, 4, 0, 0, 0, 0, 0);
    }
}

/* Function start: 0x40A780 */
void SC_Detention::HandleInput3C() {
    GameState* gs;
    int idx;

    SetupDetentionState();
    gs = g_GameState_0046aa30;
    idx = gs->FindState(field_1A4);
    if (idx < 0 || idx >= gs->maxStates - 1) {
        ShowError("Invalid gamestate %d", idx);
    }
    gs->stateValues[idx]++;
    field_160 = 1;
    gs = g_GameState_0046aa30;
    idx = gs->FindState(field_1A4);
    if (idx < 0 || idx >= gs->maxStates - 1) {
        ShowError("Invalid gamestate %d", idx);
    }
    if (gs->stateValues[idx] % 3 == 0) {
        ProcessDetention();
    }
}

/* Function start: 0x40A820 */
void SC_Detention::ProcessDetention() {
    GameState* gs;
    int idx;

    SetupDetentionState();
    gs = g_GameState_0046aa30;
    idx = gs->FindState(field_1A4);
    if (idx < 0 || idx >= gs->maxStates - 1) {
        ShowError("Invalid gamestate %d", idx);
    }
    gs->stateValues[idx] += 3;
    field_160 = 1;
    gs = g_GameState_0046aa30;
    idx = gs->FindState(field_1A4);
    if (idx < 0 || idx >= gs->maxStates - 1) {
        ShowError("Invalid gamestate %d", idx);
    }
    if (gs->stateValues[idx] % 9 == 0) {
        InitDetention();
        return;
    }
    GoToClass();
}

/* Function start: 0x40A8D0 */
void SC_Detention::HandleInput3F() {
    GameState* gs;
    int idx;

    SetupDetentionState();
    gs = g_GameState_0046aa30;
    idx = gs->FindState(field_184);
    if (idx < 0 || idx >= gs->maxStates - 1) {
        ShowError("Invalid gamestate %d", idx);
    }
    gs->stateValues[idx]++;
    gs = g_GameState_0046aa30;
    idx = gs->FindState(field_184);
    if (idx < 0 || idx >= gs->maxStates - 1) {
        ShowError("Invalid gamestate %d", idx);
    }
    if (gs->stateValues[idx] >= 3) {
        gs = g_GameState_0046aa30;
        idx = gs->FindState(field_164);
        if (idx < 0 || idx >= gs->maxStates - 1) {
            ShowError("Invalid gamestate %d", idx);
        }
        gs->stateValues[idx] = 3;
        ProcessPeriodAction(0x1273);
    }
}

/* Function start: 0x40A9B0 */
void SC_Detention::InitDetention() {
    GameState* gs;
    int idx;

    SetupDetentionState();
    field_160 = 1;
    gs = g_GameState_0046aa30;
    idx = gs->FindState(field_164);
    if (idx < 0 || idx >= gs->maxStates - 1) {
        ShowError("Invalid gamestate %d", idx);
    }
    gs->stateValues[idx]++;
    gs = g_GameState_0046aa30;
    idx = gs->FindState(field_164);
    if (idx < 0 || idx >= gs->maxStates - 1) {
        ShowError("Invalid gamestate %d", idx);
    }
    if (gs->stateValues[idx] >= 3) {
        HandleInput40();
        return;
    }
    gs = g_GameState_0046aa30;
    idx = gs->FindState(field_164);
    if (idx < 0 || idx >= gs->maxStates - 1) {
        ShowError("Invalid gamestate %d", idx);
    }
    if (gs->stateValues[idx] == 1) {
        ProcessPeriodAction(0x1270);
        return;
    }
    gs = g_GameState_0046aa30;
    idx = gs->FindState(field_164);
    if (idx < 0 || idx >= gs->maxStates - 1) {
        ShowError("Invalid gamestate %d", idx);
    }
    if (gs->stateValues[idx] == 2) {
        ProcessPeriodAction(0x1271);
        return;
    }
    gs = g_GameState_0046aa30;
    idx = gs->FindState(field_164);
    if (idx < 0 || idx >= gs->maxStates - 1) {
        ShowError("Invalid gamestate %d", idx);
    }
    ShowError("SC_Detention::Error %s %d", field_164, gs->stateValues[idx]);
}

/* Function start: 0x40AB30 */
void SC_Detention::HandleInput40() {
    GameState* gs;
    int idx;

    SetupDetentionState();
    gs = g_GameState_0046aa30;
    idx = gs->FindState(field_164);
    if (idx < 0 || idx >= gs->maxStates - 1) {
        ShowError("Invalid gamestate %d", idx);
    }
    gs->stateValues[idx] = 3;
    ProcessPeriodAction(0x1272);
}

/* Function start: 0x40A130 */
void SC_Detention::ResetAnimations() {
    GameState* gs;
    int idx;
    char sectionBuf[64];

    field_160 = 0;
    DAT_00468764 = 0;

    {
        SpriteAction action(1, 0x20, 0, 0, 0x18, 0, 0, 0, 0, 0);
        ((GameEngine*)g_GameEngine_0046a6ec)->EnqueueAction(&action);
    }

    ParseFile(this, "mis\\detention.mis", "[MOVE_TO_NEXT_PERIOD]");

    gs = g_GameState_0046aa30;
    idx = gs->FindState("PERIOD");
    ((GameState*)gs)->ValidateIndex(idx);
    int periodVal = gs->stateValues[idx];

    sprintf(sectionBuf, "[PERIOD_%d]", periodVal);
    ParseFile(this, "mis\\detention.mis", sectionBuf);

    switch (periodVal) {
    case 5:
        gs = g_GameState_0046aa30;
        idx = gs->FindState("P_AWARE_MIKE");
        ((GameState*)gs)->ValidateIndex(idx);
        if (gs->stateValues[idx] != 0) break;

        gs = g_GameState_0046aa30;
        idx = gs->FindState("P_AWARE_LINSEY");
        ((GameState*)gs)->ValidateIndex(idx);
        if (gs->stateValues[idx] != 0) break;

        gs = g_GameState_0046aa30;
        idx = gs->FindState("S_AWARE_MIKE");
        ((GameState*)gs)->ValidateIndex(idx);
        if (gs->stateValues[idx] != 0) break;

        gs = g_GameState_0046aa30;
        idx = gs->FindState("S_AWARE_LINSEY");
        ((GameState*)gs)->ValidateIndex(idx);
        if (gs->stateValues[idx] != 0) break;

        gs = g_GameState_0046aa30;
        idx = gs->FindState("D_AWARE_MIKE");
        ((GameState*)gs)->ValidateIndex(idx);
        if (gs->stateValues[idx] != 0) break;

        gs = g_GameState_0046aa30;
        idx = gs->FindState("D_AWARE_LINSEY");
        ((GameState*)gs)->ValidateIndex(idx);
        if (gs->stateValues[idx] != 0) break;

        SC_Detention::SendPeriodAction(0x21B6);
        break;

    case 0xD:
        gs = g_GameState_0046aa30;
        idx = gs->FindState("AWARE_TEACHER");
        ((GameState*)gs)->ValidateIndex(idx);
        if (gs->stateValues[idx] == 0) {
            SC_Detention::SendPeriodAction(0x21AC);
        }
        break;

    case 0x11:
        gs = g_GameState_0046aa30;
        idx = gs->FindState("LINSEY_ALIEN");
        ((GameState*)gs)->ValidateIndex(idx);
        if (gs->stateValues[idx] == 0) break;

        gs = g_GameState_0046aa30;
        idx = gs->FindState("MIKE_ALIEN");
        ((GameState*)gs)->ValidateIndex(idx);
        if (gs->stateValues[idx] == 0) break;

        gs = g_GameState_0046aa30;
        idx = gs->FindState("WENDY_ALIEN");
        ((GameState*)gs)->ValidateIndex(idx);
        if (gs->stateValues[idx] == 0) break;

        gs = g_GameState_0046aa30;
        idx = gs->FindState("JACK_ALIEN");
        ((GameState*)gs)->ValidateIndex(idx);
        if (gs->stateValues[idx] == 0) break;

        gs = g_GameState_0046aa30;
        idx = gs->FindState("STACY_ALIEN");
        ((GameState*)gs)->ValidateIndex(idx);
        if (gs->stateValues[idx] == 0) break;

        gs = g_GameState_0046aa30;
        idx = gs->FindState("STACY_ALIEN");
        ((GameState*)gs)->ValidateIndex(idx);
        if (gs->stateValues[idx] == 0) break;

        SC_Detention::SendPeriodAction(0x11F8);
        break;

    case 0x15:
        ShowError("end of Game");
        break;
    }

    gs = g_GameState_0046aa30;
    idx = gs->FindState("PERIOD");
    ((GameState*)gs)->ValidateIndex(idx);
    sprintf(g_Buffer_0046aa00, "ACTIONSPERIOD%2.2d", gs->stateValues[idx]);

    gs = g_GameState_0046aa30;
    idx = gs->FindState(g_Buffer_0046aa00);
    if (idx < 0 || idx >= gs->maxStates - 1) {
        ShowError("Invalid gamestate %d", idx);
    }
    field_148 = gs->stateValues[idx];

    gs = g_GameState_0046aa30;
    idx = gs->FindState("P_IN_DETENTION");
    if (idx < 0 || idx >= gs->maxStates - 1) {
        ShowError("Invalid gamestate %d", idx);
    }
    if (gs->stateValues[idx] == 0) return;

    gs = g_GameState_0046aa30;
    idx = gs->FindState("S_IN_DETENTION");
    if (idx < 0 || idx >= gs->maxStates - 1) {
        ShowError("Invalid gamestate %d", idx);
    }
    if (gs->stateValues[idx] == 0) return;

    gs = g_GameState_0046aa30;
    idx = gs->FindState("D_IN_DETENTION");
    if (idx < 0 || idx >= gs->maxStates - 1) {
        ShowError("Invalid gamestate %d", idx);
    }
    if (gs->stateValues[idx] == 0) return;

    SC_Detention::SendPeriodAction(0x1202);
}

/* Function start: 0x40A620 */
void SC_Detention::SendPeriodAction(int param) {
    GameState* gs;
    int idx;

    field_150 = 1;
    SendGameMessage(3, param, 0x2D, 1, 4, 0, 0, 0, 0, 0);

    {
        SpriteAction action(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        DAT_00472d58.CopyFrom(&action);
    }

    gs = g_GameState_0046aa30;
    idx = gs->FindState("MUST_SAVEGAME");
    if (idx < 0 || idx >= gs->maxStates - 1) {
        ShowError("Invalid gamestate %d", idx);
    }
    gs->stateValues[idx] = 0;
}

/* Function start: 0x40AD80 */
void SC_Detention::ProcessPeriodAction(int param) {
    GameState* gs;
    int idx;
    int periodVal;
    SpriteAction* action;

    field_160 = 1;
    ResetAnimations();

    gs = g_GameState_0046aa30;
    idx = gs->FindState("PERIOD");
    if (idx < 0 || gs->maxStates - 1 < idx) {
        ShowError("Invalid gamestate %d", idx);
    }
    periodVal = gs->stateValues[idx];

    if (field_150 == 0) {
        action = new SpriteAction(3, field_F0[periodVal], 0x25, 1, 4, 0, 0, 0, 0, 0);

        SendGameMessage(3, param, handlerId, moduleParam, 4, 0, 0, (int)action, 0, 0);
    }

    gs = g_GameState_0046aa30;
    idx = gs->FindState(field_1C4);
    if (idx < 0 || gs->maxStates - 1 < idx) {
        ShowError("Invalid gamestate %d", idx);
    }
    gs->stateValues[idx] = 1;

    gs = g_GameState_0046aa30;
    idx = gs->FindState("P_IN_DETENTION");
    if (idx < 0 || gs->maxStates - 1 < idx) {
        ShowError("Invalid gamestate %d", idx);
    }
    if (gs->stateValues[idx] == 0) {
        idx = g_PeriodStateIdx_0046cb90;
        gs = g_GameState_0046aa30;
        if (idx < 0 || gs->maxStates - 1 < idx) {
            ShowError("Invalid gamestate %d", idx);
        }
        gs->stateValues[idx] = 0;
    } else {
        idx = g_GameState_0046aa30->FindState("S_IN_DETENTION");
        if (idx < 0 || gs->maxStates - 1 < idx) {
            ShowError("Invalid gamestate %d", idx);
        }
        if (gs->stateValues[idx] == 0) {
            idx = g_PeriodStateIdx_0046cb90;
            gs = g_GameState_0046aa30;
            if (idx < 0 || gs->maxStates - 1 < idx) {
                ShowError("Invalid gamestate %d", idx);
            }
            gs->stateValues[idx] = 1;
        } else {
            idx = g_GameState_0046aa30->FindState("D_IN_DETENTION");
            if (idx < 0 || gs->maxStates - 1 < idx) {
                ShowError("Invalid gamestate %d", idx);
            }
            if (gs->stateValues[idx] == 0) {
                idx = g_PeriodStateIdx_0046cb90;
                gs = g_GameState_0046aa30;
                if (idx < 0 || gs->maxStates - 1 < idx) {
                    ShowError("Invalid gamestate %d", idx);
                }
                gs->stateValues[idx] = 2;
            }
        }
    }

    SetupDetentionState();
}

/* Function start: 0x40AD50 */
void SC_Detention::DrawDetention() {
    SendGameMessage(1, 0, handlerId, moduleParam, 5, 0, 0, 0, 0, 0);
}

/* Function start: 0x40B030 */
void SC_Detention::SetupDetentionState() {
    GameState* gs;
    int idx;
    int periodChar;

    idx = g_PeriodStateIdx_0046cb90;
    gs = g_GameState_0046aa30;
    if (idx < 0 || idx >= gs->maxStates - 1) {
        ShowError("Invalid gamestate %d", idx);
    }
    periodChar = (signed char)g_PeriodCharTable_0046cb94[gs->stateValues[idx]];
    sprintf(field_164, "%c_DETENTION_SUSPENSION", periodChar);
    sprintf(field_184, "%c_TATTLES", periodChar);
    sprintf(field_1A4, "%c_LATE_CUT", periodChar);
    sprintf(field_1C4, "%c_IN_DETENTION", periodChar);
}

/* Function start: 0x40B25E */
int SC_Detention::LBLParse(char* line) { // prologue at 0x40B240
    char token[32];

    sscanf(line, "%s", token);

    if (strcmp(token, "POSTMESSAGE") == 0) {
        SpriteAction action(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        ParseSpriteAction(&action, this);
        EnqueueSpriteAction((void*)&action);
    } else if (strcmp(token, "ENDPERIODANNOUNCE") == 0) {
        int index, value;
        sscanf(line, "%s %d %d", token, &index, &value);
        field_A8[index + 20] = value;
    } else if (strcmp(token, "ANNOUNCE") == 0) {
        int index, value;
        sscanf(line, "%s %d %d", token, &index, &value);
        field_A8[index] = value;
    } else if (strcmp(token, "END") == 0) {
        return 1;
    } else {
        Parser::LBLParse("SCI_SearchScreen");
    }

    return 0;
}

/* Function start: 0x40AB90 */
void SC_Detention::HandleCombat() {
    SpriteAction temp(0x2D, 1, 0x2D, 1, 4, 0, 0, 0, 0, 0);
    DAT_00472d58.CopyFrom(&temp);

    SendGameMessage(3, 0xC7, 0x2D, 1, 4, 0, 0, 0, 0, 0);

    int idx = g_GameState_0046aa30->FindState("MUST_SAVEGAME");
    if (idx < 0 || g_GameState_0046aa30->maxStates - 1 < idx) {
        ShowError("Invalid gamestate %d", idx);
    }
    g_GameState_0046aa30->stateValues[idx] = 0;
}
/* Function start: 0x40AC70 */
void SC_Detention::HandlePractice() {
    GameState* gs;
    int idx;

    SpriteAction temp(0x2D, 1, 0x2D, 1, 4, 0, 0, 0, 0, 0);
    DAT_00472d58.CopyFrom(&temp);

    SendGameMessage(0x2D, 1, 0x2D, 1, 4, 0, 0, 0, 0, 0);

    gs = g_GameState_0046aa30;
    idx = gs->FindState("MUST_SAVEGAME");
    if (idx < 0 || gs->maxStates - 1 < idx) {
        ShowError("Invalid gamestate %d", idx);
    }
    gs->stateValues[idx] = 0;
}

/* Function start: 0x40B0D0 */
void SC_Detention::Serialize(void* param) {
    int strLen;
    FILE* fp;
    int id;

    strLen = strlen("DETENTION_INFO") + 1;
    fp = *(FILE**)((char*)param + 0x44);

    if (*(int*)param != 0) {
        /* SAVE */
        id = handlerId;
        fwrite(&id, 4, 1, fp);
        fwrite("DETENTION_INFO", strLen, 1, fp);
        fwrite(&field_148, 1, 4, fp);
        fwrite(&field_14C, 1, 4, fp);
        fwrite(&field_150, 1, 4, fp);
        fwrite(&field_160, 1, 4, fp);
    } else {
        /* LOAD */
        extern char* g_Buffer_0046aa00;
        *g_Buffer_0046aa00 = 0;
        fread(g_Buffer_0046aa00, strLen, 1, fp);

        if (strcmp(g_Buffer_0046aa00, "DETENTION_INFO") != 0) {
            ShowError("SCI_SearchScreen::Serialize() - Error Loading (Wrong ID '%s')", g_Buffer_0046aa00);
        }
        fread(&field_148, 1, 4, fp);
        fread(&field_14C, 1, 4, fp);
        fread(&field_150, 1, 4, fp);
        fread(&field_160, 1, 4, fp);
    }
}

extern int DAT_00468a18;
extern int WaitForInput();
extern int DAT_0046ae78;

/* Function start: 0x40B9E0 */
void UpdateCombatEngine()
{
    if (DAT_00468a18 != 0) {
        int key = WaitForInput();
        DAT_00468a18 = (key != 0x54) ? 1 : 0;
    }
    int* engine = (int*)DAT_0046ae78;
    int* vtbl = (int*)*engine;
    ((void (__fastcall *)(int*, int))vtbl[14])(engine, 0);
}
