#include "GameState.h"
#include <stdio.h>
#include <string.h>

extern "C" {
    int _sscanf(const char* s, const char* format, ...);
    void ShowError(const char* message, ...);
    int AllocateMemory_Wrapper(int size);
    void FUN_004209c0(int);
    FILE* fsopen(const char* filename, const char* mode);
    size_t __fread_lk(void* ptr, size_t size, size_t count, FILE* stream);
    size_t __fwrite_lk(const void* ptr, size_t size, size_t count, FILE* stream);
    int _fclose(FILE* stream);
}

/* Function start: 0x420570 */
void GameState::Serialize(int param_1)
{
    unsigned int* puVar2 = (unsigned int*)AllocateMemory_Wrapper(0x110);
    char local_1c[28];

    strcpy(local_1c, "gamestat.sav");

    if (param_1 == 1) {
        FILE* _File = fsopen(local_1c, "wb");
        if (_File == NULL) {
            ShowError("GameState::Serialize unable to open %s ", local_1c);
        }
        puVar2[0] = this->field_0x90;
        puVar2[1] = 0x110;
        puVar2[2] = 1;
        puVar2[3] = this->field_0x90 * 4 + 0x110;
        __fwrite_lk(puVar2, 0x110, 1, _File);
        __fwrite_lk(this->field_0x88, 4, puVar2[0], _File);
        _fclose(_File);
    } else if (param_1 == 2) {
        FILE* _File = fsopen(local_1c, "rb");
        if (_File == NULL) {
            ShowError("GameState::Serialize unable to open %s ", local_1c);
        }
        __fread_lk(puVar2, 0x110, 1, _File);
        if (puVar2[0] != this->field_0x90 || puVar2[2] != 1) {
            ShowError("GameState::Serialize incompatible file");
        }
        __fread_lk(this->field_0x88, 0x110, this->field_0x90, _File);
        _fclose(_File);
    } else {
        ShowError("illegal in GameState::Serialize(%d)", param_1);
    }
}

/* Function start: 0x420780 */
int GameState::LBLParse(char* param_1)
{
    char local_40[32];
    char local_20[32];
    int local_44;

    _sscanf(param_1, "%s", local_40);

    if (strcmp(local_40, "MAXSTATES") == 0) {
        _sscanf(param_1, "%s %d %s", local_40, &local_44, local_20);
        this->SetMaxStates2(local_44);
    } else if (strcmp(local_40, "LABEL") == 0) {
        _sscanf(param_1, "%s %d %s", local_40, &local_44, local_20);
        int* mem = (int*)AllocateMemory_Wrapper(0x20);
        this->field_0x8c[local_44] = (int)mem;
        strcpy((char*)mem, local_20);
    } else if (strcmp(local_40, "END") == 0) {
        return 1;
    } else {
        Parser::LBLParse(param_1);
    }

    return 0;
}

/* Function start: 0x420940 */
int GameState::FindState(char* param_1)
{
    for (int i = 0; i < this->field_0x90; i++) {
        char* state = (char*)this->field_0x8c[i];
        if (state) {
            if (strstr(state, param_1)) {
                if (strlen(state) == strlen(param_1)) {
                    return i;
                }
            }
        }
    }
    return -1;
}

/* Function start: 0x420900 */
int GameState::GetState(int param_1)
{
    if ((param_1 > 0) && (this->field_0x90 <= param_1)) {
        ShowError("GameState Error #%d", 1);
    }
    return this->field_0x8c[param_1];
}

/* Function start: 0x4206e0 */
void GameState::SetMaxStates2(int param_1)
{
    if (this->field_0x88 != 0) {
        ShowError("GameState::SetMaxStates1");
    }
    if (this->field_0x8c != 0) {
        ShowError("GameState::SetMaxStates2");
    }
    this->field_0x90 = param_1;
    this->field_0x88 = (int*)AllocateMemory_Wrapper(param_1 * 4);
    FUN_004209c0((int)this);
    this->field_0x8c = (int*)AllocateMemory_Wrapper(this->field_0x90 * 4);
    int* arr = this->field_0x8c;
    for (int i = 0; i < this->field_0x90; i++) {
        arr[i] = 0;
    }
}
