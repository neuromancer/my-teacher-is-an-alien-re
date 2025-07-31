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
