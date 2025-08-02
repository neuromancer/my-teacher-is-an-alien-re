#include "FlagArray.h"
#include <string.h>

extern "C" {
    void __cdecl ShowError(const char*, ...);
    void __cdecl WriteToMessageLog(const char*, ...);
    FILE* __cdecl fsopen(const char*, const char*);
    int __cdecl fclose(FILE*);
    size_t __cdecl fread(void*, size_t, size_t, FILE*);
    size_t __cdecl fwrite(const void*, size_t, size_t, FILE*);
    int __cdecl fseek(FILE*, long, int);
    void FUN_004269e0(void*, int, int, FILE*);
}

const char* s_Error_in_flagaray_cpp_Create_Cannot_create = "Error in flagaray.cpp - Create:  Cannot create %s";
const char* s_Error_in_flagaray_cpp_SetFileName_File_name_too_long = "Error in flagaray.cpp - SetFileName: File name too long";
const char* s_rb = "rb";
const char* s_wb = "wb";
const char* s_double_FlagArray__Open = "double FlagArray::Open()";
const char* s_Error_opening_file_in_flagarray_Open = "Error opening file in flagarray::Open";
const char* s_FlagArray_Close_attempt_to_close_a_file_that_is_not_open = "FlagArray::Close() - attempt to close a file that is not open";
const char* s_FlagArray__Seek = "FlagArray::Seek";
const char* s_Error_in_flagaray_cpp_Seek_Attempt_to_seek_past_end_of_file = "Error in flagaray.cpp - Seek: Attempt to seek past end of file";
const char* s_Error_in_flagaray_cpp_Seek_Index_out_of_range = "Error in flagaray.cpp - Seek: Index out of range %d";

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

/* Function start: 0x420250 */
void FlagArray::SafeClose()
{
    if (this->fp != NULL) {
        fclose(this->fp);
    }
    this->fp = 0;
}

/* Function start: 0x420270 */
void FlagArray::Open()
{
    if (this->fp != 0) {
        ShowError(s_double_FlagArray__Open);
    }
    this->fp = fsopen(this->filename, s_rb);
    if (this->fp == 0) {
        ShowError(s_Error_opening_file_in_flagarray_Open);
    }
    fread(&this->field_0x38, 0x94, 1, this->fp);
}

/* Function start: 0x4202D0 */
void FlagArray::Close()
{
    if (this->fp == 0) {
        ShowError(s_FlagArray_Close_attempt_to_close_a_file_that_is_not_open);
    }
    fclose(this->fp);
    this->fp = 0;
}

/* Function start: 0x420300 */
void FlagArray::Seek(int index)
{
    if (this->fp == 0) {
        ShowError(s_FlagArray__Seek);
    }

    if (index < 0 || index >= this->max_states) {
        ShowError(s_Error_in_flagaray_cpp_Seek_Index_out_of_range, index);
    }

    int offset = this->field_0x44 * index + this->field_0x3c;
    if (offset > (this->field_0x38 - this->field_0x44 + 1)) {
        WriteToMessageLog(s_Error_in_flagaray_cpp_Seek_Attempt_to_seek_past_end_of_file);
    }

    fseek(this->fp, offset, SEEK_SET);
}

/* Function start: 0x420370 */
unsigned int FlagArray::GetFlag(int index, unsigned int mask)
{
    unsigned int flag;
    Open();
    Seek(index);
    fread(&flag, this->field_0x44, 1, this->fp);
    Close();
    return flag & mask;
}

/* Function start: 0x4203C0 */
void FlagArray::SetFlag(int index, unsigned int mask)
{
    unsigned int flag;
    Open();
    Seek(index);
    fread(&flag, this->field_0x44, 1, this->fp);
    flag |= mask;
    Seek(index);
    fwrite(&flag, this->field_0x44, 1, this->fp);
    Close();
}

/* Function start: 0x420430 */
void FlagArray::ClearAllFlags()
{
    int zero = 0;
    Open();
    Seek(0);
    for (int i = 0; i < this->max_states; i++) {
        fwrite(&zero, 4, 1, this->fp);
    }
    Close();
}
