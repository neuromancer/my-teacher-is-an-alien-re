#include "FlagArray.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

extern "C" {
    void __cdecl ShowError(const char*, ...);
    void __cdecl WriteToMessageLog(const char*, ...);
    void FUN_004269e0(void*, int, int, FILE*);
}

/* Function start: 0x425e50 */
FILE *fsopen(const char* filename, const char* mode) {
    return _fsopen(filename, mode, 0x40);
}

/* Function start: 0x420140 */
void FlagArray::Create(char* filename, int max_states)
{
    memset(this, 0, sizeof(FlagArray));

    if (strlen(filename) > 50) {
        ShowError("Error in flagaray.cpp - SetFileName: File name too long");
    }
    strcpy(this->filename, filename);

    this->fp = fsopen(this->filename, "rb");
    if (this->fp == 0) {
        this->fp = fsopen(this->filename, "wb");
        if (this->fp == 0) {
            ShowError("Error in flagaray.cpp - Create:  Cannot create %s", filename);
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
        ShowError("double FlagArray::Open()");
    }
    this->fp = fsopen(this->filename, "rb");
    if (this->fp == 0) {
        ShowError("Error opening file in flagarray::Open");
    }
    fread(&this->field_0x38, 0x94, 1, this->fp);
}

/* Function start: 0x4202D0 */
void FlagArray::Close()
{
    if (this->fp == 0) {
        ShowError("FlagArray::Close() - attempt to close a file that is not open");
    }
    fclose(this->fp);
    this->fp = 0;
}

/* Function start: 0x420300 */
void FlagArray::Seek(int index)
{
    if (this->fp == 0) {
        ShowError("FlagArray::Seek");
    }

    int offset = this->field_0x44 * index + this->field_0x3c;

    if (index < 0 || index >= this->max_states) {
        ShowError("Error in flagaray.cpp - Seek: Index out of range %d", index);
    }

    if (offset > (this->field_0x38 - this->field_0x44 + 1)) {
        WriteToMessageLog("Error in flagaray.cpp - Seek: Attempt to seek past end of file");
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
