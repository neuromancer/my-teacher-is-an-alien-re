#include "FlagArray.h"
#include <string.h>

extern "C" {
    void __cdecl ShowError(const char*, ...);
    FILE* __cdecl fsopen(const char*, const char*);
    int __cdecl fclose(FILE*);
    void FUN_004269e0(void*, int, int, FILE*);
}

const char* s_Error_in_flagaray_cpp_Create_Cannot_create = "Error in flagaray.cpp - Create:  Cannot create %s";
const char* s_Error_in_flagaray_cpp_SetFileName_File_name_too_long = "Error in flagaray.cpp - SetFileName: File name too long";
const char* s_rb = "rb";
const char* s_wb = "wb";

/* Function start: 0x420140 */
void FlagArray::Create(char* filename, int max_states)
{
    memset(this, 0, sizeof(FlagArray));

    if (strlen(filename) > 50) {
        ShowError(s_Error_in_flagaray_cpp_SetFileName_File_name_too_long);
    }
    strcpy(this->filename, filename);

    this->fp = fsopen(this->filename, s_rb);
    if (this->fp == 0) {
        this->fp = fsopen(this->filename, s_wb);
        if (this->fp == 0) {
            ShowError(s_Error_in_flagaray_cpp_Create_Cannot_create, filename);
        }

        this->max_states = max_states;
        this->field_0x38 = max_states * 4 + 0x94;
        this->field_0x3c = 0x94;
        this->field_0x44 = 4;

        FUN_004269e0(&this->field_0x38, 0x94, 1, this->fp);
        for (int i = 0; i < this->max_states; i++) {
            int local_4 = 0;
            FUN_004269e0(&local_4, 4, 1, this->fp);
        }
    }

    fclose(this->fp);
    this->fp = 0;
}
