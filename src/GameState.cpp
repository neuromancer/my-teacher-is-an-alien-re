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


GameState::GameState()
{
    stateValues = 0;
    stateLabels = 0;
    maxStates = 0;
    reserved = 0;
}

/* Function start: 0x420480 */
GameState::~GameState()
{
    if (stateValues != 0) {
        FreeMemory(stateValues);
        stateValues = 0;
    }

    for (int i = 0; i < maxStates; i++) {
        if (stateLabels[i] != 0) {
            FreeMemory((void*)stateLabels[i]);
            stateLabels[i] = 0;
        }
    }

    if (stateLabels != 0) {
        FreeMemory(stateLabels);
        stateLabels = 0;
    }
}

/* Function start: 0x420570 */
void GameState::Serialize(int mode)
{
    unsigned int* header = (unsigned int*)AllocateMemory(0x110);
    char filename[28];
    FILE* file;

    strcpy(filename, "gamestat.sav");

    if (mode == 1) {
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
    } else {
        if (mode != 2) {
            ShowError("illegal in GameState::Serialize(%d)", mode);
        }
        file = fsopen(filename, "rb");
        if (file == NULL) {
            ShowError("GameState::Serialize unable to open %s ", filename);
        }
        fread(header, 0x110, 1, file);
        if (header[0] != maxStates || header[2] != 1) {
            ShowError("GameState::Serialize incompatible file");
        }
        fread(stateValues, 0x110, maxStates, file);
    }
    fclose(file);
}

/* Function start: 0x420780 */
int GameState::LBLParse(char* line)
{
    int index;
    char keyword[32];
    char labelName[32];

    labelName[0] = '\0';
    keyword[0] = '\0';
    sscanf(line, " %s ", keyword);

    if (strcmp(keyword, "MAXSTATES") == 0) {
        sscanf(line, "%s %d %s", keyword, &index, labelName);
        SetMaxStates(index);
    } else if (strcmp(keyword, "LABEL") == 0) {
        sscanf(line, "%s %d %s", keyword, &index, labelName);
        stateLabels[index] = (char*)AllocateMemory(0x20);
        strcpy(stateLabels[index], labelName);
    } else if (strcmp(keyword, "END") == 0) {
        return 1;
    } else {
        Parser::LBLParse("GameState");
    }

    return 0;
}

/* Function start: 0x420940 */
int GameState::FindState(char* stateName)
{
    for (int i = 0; i < maxStates; i++) {
        char* label = stateLabels[i];
        if (label) {
            if (strstr(label, stateName)) {
                if (strlen(label) == strlen(stateName)) {
                    return i;
                }
            }
        }
    }
    return -1;
}

/* Function start: 0x420900 */
char *GameState::GetState(int stateIndex)
{
    if ((stateIndex > 0) && (maxStates <= stateIndex)) {
        ShowError("GameState Error  #%d", 1);
    }
    return stateLabels[stateIndex];
}

/* Function start: 0x4209C0 */
void GameState::ClearStates()
{
    int* ptr = stateValues;
    for (unsigned int i = maxStates; i != 0; i--) {
        *ptr = 0;
        ptr++;
    }
}

/* Function start: 0x4206e0 */
void GameState::SetMaxStates(int count)
{
    if (stateValues != 0) {
        ShowError("GameState::SetMaxStates1");
    }
    if (stateLabels != 0) {
        ShowError("GameState::SetMaxStates2");
    }
    maxStates = count;
    stateValues = (int*)AllocateMemory(count * 4);
    ClearStates();
    stateLabels = (char**)AllocateMemory(maxStates * 4);
    char** arr = stateLabels;
    for (int i = 0; i < maxStates; i++) {
        arr[i] = 0;
    }
}

