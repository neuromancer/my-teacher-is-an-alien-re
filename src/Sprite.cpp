#include "Sprite.h"

// TODO: Move to a proper header
void (*ShowError)(const char* message, ...) = (void (*)(const char*, ...))0x00419110;

const char* s_error_Sprite_CheckRanges0_00436c04 = "error Sprite::CheckRanges0";
const char* s_error_Sprite_CheckRanges1_00436be8 = "error Sprite::CheckRanges1";
const char* s_bad_range_d_start_d_in_s_00436bc8 = "bad range[%d].start = %d in %s";

struct AnimationData {
    char pad[0xc];
    void* p;
};

struct AnimationData2 {
    char pad[0xc];
    int value;
};

/* Function start: 0x41D500 */
void Sprite::CheckRanges1()
{
    if (this->animation_data == 0) {
        ShowError(s_error_Sprite_CheckRanges0_00436c04);
    }
    if (this->field136_0x88 == 0) {
        ShowError(s_error_Sprite_CheckRanges1_00436be8);
    }
    int iVar4 = 0;
    if (0 < this->field_0xac) {
        int iVar3 = 0;
        do {
            int* piVar2 = (int*)((char*)this->field136_0x88 + 4 + iVar3);
            int iVar1 = ((AnimationData2*)(((AnimationData*)this->animation_data)->p))->value;
            if (iVar1 < *piVar2) {
                *piVar2 = iVar1;
            }
            piVar2 = (int*)((char*)this->field136_0x88 + iVar3);
            iVar1 = *piVar2;
            if (piVar2[1] < iVar1) {
                ShowError(s_bad_range_d_start_d_in_s_00436bc8, iVar4, iVar1, (char*)&this->filename);
            }
            iVar3 += 8;
            iVar4++;
        } while (iVar4 < this->field_0xac);
    }
}

/* Function start: 0x41d590 */
// TODO: Move to a proper header
// NOTE: Once the GameState class is reversed, this should be an instance of it
extern void* g_GameState;

int Sprite::CheckConditions()
{
    int iVar1;
    int* piVar2;
    int iVar3;
    int local_4;

    iVar3 = 0;
    if ((this->field_0xa0 == 0) || (this->field_0x98 == 0)) {
        return 1;
    }
    local_4 = 0;
    if (0 < this->field_0xa0) {
        do {
            piVar2 = (int*)((char*)this->field_0x98 + iVar3);
            if (piVar2[1] == 1) {
                iVar1 = *piVar2;
                if ((0 < iVar1) && (*(int*)((char*)g_GameState + 0x90) <= iVar1)) {
                    ShowError("GameState Error  #%d", 1);
                }
                if (*(int*)(*(int*)((char*)g_GameState + 0x88) + iVar1 * 4) == 0) {
                    return 0;
                }
            }
            if (piVar2[1] == 2) {
                iVar1 = *piVar2;
                if ((0 < iVar1) && (*(int*)((char*)g_GameState + 0x90) <= iVar1)) {
                    ShowError("GameState Error  #%d", 1);
                }
                if (*(int*)(*(int*)((char*)g_GameState + 0x88) + iVar1 * 4) != 0) {
                    return 0;
                }
            }
            if (piVar2[1] == 3) {
                iVar1 = *piVar2;
                if ((0 < iVar1) && (*(int*)((char*)g_GameState + 0x90) <= iVar1)) {
                    ShowError("GameState Error  #%d", 1);
                }
                if (*(int*)(*(int*)((char*)g_GameState + 0x88) + iVar1 * 4) != this->field_0x9c) {
                    return 0;
                }
            }
            iVar3 = iVar3 + 8;
            local_4 = local_4 + 1;
        } while (local_4 < this->field_0xa0);
    }
    return 1;
}
