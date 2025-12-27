#include "GameState.h"
#include "string.h"
#include "Memory.h"
#include <stdio.h>
#include <string.h>

/* Function start: 0x420480 */
GameState::~GameState()
{
    //this->vtable = (void**)0x431268;

    if (this->stateValues != 0) {
        FreeMemory(this->stateValues);
        this->stateValues = 0;
    }

    for (int i = 0; i < this->maxStates; i++) {
        if (this->stateLabels[i] != 0) {
            FreeMemory((void*)this->stateLabels[i]);
            this->stateLabels[i] = 0;
        }
    }

    if (this->stateLabels != 0) {
        FreeMemory(this->stateLabels);
        this->stateLabels = 0;
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
        header[0] = this->maxStates;
        header[1] = 0x110;
        header[2] = 1;
        header[3] = this->maxStates * 4 + 0x110;
        fwrite(header, 0x110, 1, file);
        fwrite(this->stateValues, 4, header[0], file);
    } else {
        if (mode != 2) {
            ShowError("illegal in GameState::Serialize(%d)", mode);
        }
        file = fsopen(filename, "rb");
        if (file == NULL) {
            ShowError("GameState::Serialize unable to open %s ", filename);
        }
        fread(header, 0x110, 1, file);
        if (header[0] != this->maxStates || header[2] != 1) {
            ShowError("GameState::Serialize incompatible file");
        }
        fread(this->stateValues, 0x110, this->maxStates, file);
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
    sscanf(line, "%s", keyword);

    if (strcmp(keyword, "MAXSTATES") == 0) {
        sscanf(line, "%s %d %s", keyword, &index, labelName);
        this->SetMaxStates(index);
    } else if (strcmp(keyword, "LABEL") == 0) {
        sscanf(line, "%s %d %s", keyword, &index, labelName);
        this->stateLabels[index] = (char*)AllocateMemory(0x20);
        strcpy(this->stateLabels[index], labelName);
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
    for (int i = 0; i < this->maxStates; i++) {
        char* label = this->stateLabels[i];
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
int GameState::GetState(int stateIndex)
{
    if ((stateIndex > 0) && (this->maxStates <= stateIndex)) {
        ShowError("GameState Error #%d", 1);
    }
    return (int)this->stateLabels[stateIndex];
}

/* Function start: 0x4209C0 */
void GameState::ClearStates()
{
    int* ptr = this->stateValues;
    for (unsigned int i = this->maxStates; i != 0; i--) {
        *ptr = 0;
        ptr++;
    }
}

/* Function start: 0x4206e0 */
void GameState::SetMaxStates(int count)
{
    if (this->stateValues != 0) {
        ShowError("GameState::SetMaxStates1");
    }
    if (this->stateLabels != 0) {
        ShowError("GameState::SetMaxStates2");
    }
    this->maxStates = count;
    this->stateValues = (int*)AllocateMemory(count * 4);
    this->ClearStates();
    this->stateLabels = (char**)AllocateMemory(this->maxStates * 4);
    char** arr = this->stateLabels;
    for (int i = 0; i < this->maxStates; i++) {
        arr[i] = 0;
    }
}

