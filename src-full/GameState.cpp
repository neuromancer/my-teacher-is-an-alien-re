#include "GameState.h"
#include "string.h"
#include "Memory.h"
#include <stdio.h>
#include <string.h>

char s_COMBATS[] = "COMBATS";
char s_duncan[] = "duncan";
char s_DIALOG[] = "DIALOG";
char s_COMBAT[] = "COMBAT";
char s_susan[] = "susan";
char s_MEET_MY[] = "MEET MY";
char s_TESTPUZZLE[] = "TESTPUZZLE";


/* Function start: 0x409F20 */
void GameState::FUN_00409f20(int) {}

/* Function start: 0x432E20 */
int GameState::FUN_00432e20(char*) { return 0; }

/* Function start: 0x4333D0 */
GameState::~GameState()
{
    if (stateValues != 0) {
        delete stateValues;
        stateValues = 0;
    }

    for (int i = 0; i < maxStates; i++) {
        if (stateLabels[i] != 0) {
            delete (void*)stateLabels[i];
            stateLabels[i] = 0;
        }
    }

    if (stateLabels != 0) {
        delete stateLabels;
        stateLabels = 0;
    }
}

/* Function start: 0x433480 */
void GameState::Serialize(int mode)
{
    unsigned int* header = new unsigned int[0x44];
    char filename[28];
    FILE* file;

    strcpy(filename, "gamestat.sav");

    switch (mode) {
    case 1:
        file = fsopen(filename, "wb");
        if (file == NULL) {
            ShowError("GameState::Serialize unable to open %s ", filename);
        }
        header[0] = maxStates;
        header[1] = 0x110;
        header[2] = 1;
        header[3] = maxStates * 4 + 0x110;
        fwrite(header, 0x110, 1, file);
        fwrite(stateValues, 4, header[0], file);
        break;
    case 2:
        file = fsopen(filename, "rb");
        if (file == NULL) {
            ShowError("GameState::Serialize unable to open %s ", filename);
        }
        fread(header, 0x110, 1, file);
        if (header[0] != maxStates || header[2] != 1) {
            ShowError("GameState::Serialize incompatible file");
        }
        fread(stateValues, 0x110, maxStates, file);
        break;
    default:
        ShowError("illegal in GameState::Serialize(%d)", mode);
        break;
    }
    fclose(file);
}

/* Function start: 0x4335D0 */
void GameState::SetMaxStates(int count)
{
    if (stateValues != 0) {
        ShowError("GameState::SetMaxStates1");
    }
    if (stateLabels != 0) {
        ShowError("GameState::SetMaxStates2");
    }
    maxStates = count;
    stateValues = new int[count];
    ClearStates();
    stateLabels = new char*[maxStates];
    for (int i = 0; i < maxStates; i++) {
        stateLabels[i] = 0;
    }
}

/* Function start: 0x433710 */
int GameState::LBLParse(char* line)
{
    int index;
    int defaultValue;
    int numParsed;
    char keyword[32];
    char labelName[32];

    labelName[0] = '\0';
    keyword[0] = '\0';
    sscanf(line, " %s ", keyword);

    if (strcmp(keyword, "MAXSTATES") == 0) {
        sscanf(line, "%s %d %s", keyword, &index, labelName);
        SetMaxStates(index);
    } else if (strcmp(keyword, "LABEL") == 0) {
        numParsed = sscanf(line, "%s %d %s %d", keyword, &index, labelName, &defaultValue);
        stateLabels[index] = new char[0x20];
        strcpy(stateLabels[index], labelName);
        if (numParsed > 3) {
            if (index < 0 || maxStates - 1 < index) {
                ShowError("Invalid gamestate %d", index);
            }
            stateValues[index] = defaultValue;
        }
    } else if (strcmp(keyword, "END") == 0) {
        return 1;
    } else {
        Parser::LBLParse("GameState");
    }

    return 0;
}

/* Function start: 0x433A20 */
char *GameState::GetState(int stateIndex)
{
    if ((stateIndex > 0) && (maxStates <= stateIndex)) {
        ShowError("GameState Error  #%d", 1);
    }
    return stateLabels[stateIndex];
}

/* Helper used by FindLabel - demo equivalent was at 0x420940 */
int GameState::FindState(char* stateName)
{
    for (int i = 0; i < maxStates; i++) {
        if (stateLabels[i]) {
            if (strstr(stateLabels[i], stateName)) {
                if (strlen(stateLabels[i]) == strlen(stateName)) {
                    return i;
                }
            }
        }
    }
    return -1;
}

/* Function start: 0x433AE0 */
int GameState::FindLabel(char* name) {
    int idx = FindState(name);
    if (idx == -1) {
        ShowError("GameState::StateIndex()-Invalid gamestate = '%s'", name);
    }
    return idx;
}

/* Function start: 0x433B90 */
void GameState::ClearStates()
{
    memset(stateValues, 0, maxStates * 4);
}

/* Function start: 0x433BB0 */
int GameState::FUN_00433bb0(int* param_1) {
    int idx;
    unsigned int val;

    if (param_1 == 0) {
        ShowError("illegal message13");
    }
    if (param_1[0] != 2) {
        ShowError("illegal message12");
    }
    idx = param_1[1];
    if (idx < 0 || maxStates - 1 < idx) {
        ShowError("Invalid gamestate %d", idx);
    }
    val = (unsigned int)stateValues[idx];
    switch (param_1[4]) {
    case 8:
        return val != 0;
    case 9:
        return val == 0;
    case 10:
        return val < (unsigned int)param_1[5];
    case 11:
        return (unsigned int)param_1[5] < val;
    case 12:
        return (unsigned int)param_1[5] == val;
    case 13:
        return (unsigned int)param_1[5] != val;
    default:
        ShowError("illegal message12b");
    }
    return 0;
}
