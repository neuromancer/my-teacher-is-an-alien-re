#include "FlagArray.h"
#include "globals.h"
#include "string.h"
#include <string.h>
#include <stdio.h>

extern "C" char DAT_004371a8[];
char DAT_004371ac[] = "rb+";

// 0x4269E0 = fwrite, 0x425E70 = fclose (CRT functions)

/* Function start: 0x420140 */ /* DEMO ONLY - no full game match */
FlagArray::FlagArray(char* f, int numStates) {
    int buffer;
    int i;
    FILE* fp_temp;
    char* fname;

    memset(this, 0, 0x33 * 4);
    buffer = 0;

    if (strlen(f) > 0x32) {
         ShowError("Error in flagaray.cpp - SetFileName: File name too long");
    }
    fname = filename;
    strcpy(fname, f);

    fp = fsopen(fname, DAT_004371ac);

    if (fp == 0) {
        fp_temp = fsopen(fname, DAT_004371a8);
        fp = fp_temp;

        if (fp_temp != 0) {
            max_states = numStates;
            field_0x38 = numStates * 4 + 0x94;
            field_0x3c = 0x94;
            field_0x44 = 4;
            fwrite(&field_0x38, 0x94, 1, fp_temp);
            for (i = 0; i < max_states; i++) {
                fwrite(&buffer, 4, 1, fp);
            }
        } else {
            ShowError("Error in flagaray.cpp - Create:  Cannot create %s", f);
        }
    }

    fclose(fp);
    fp = 0;
}

FlagArray::~FlagArray()
{
    SafeClose();
}

/* Function start: 0x44C0E0 */
void FlagArray::SafeClose() {
    if (fp != NULL) {
        fclose(fp);
    }
    fp = 0;
}

/* Function start: 0x420900 */
void FlagArray::Open() {
    if (fp != 0) {
        ShowError("double FlagArray::Open()");
    }
    
    FILE* fp_temp = fopen(filename, DAT_004371ac);
    fp = fp_temp;
    
    if (fp_temp == NULL) {
        ShowError("Error opening file in flagarray.cpp");
    }
    
    // Read header back
    fread(&field_0x38, 0x94, 1, fp);
}

/* Function start: 0x420960 */ /* ~87% match */
void FlagArray::Close() {
    if (fp == 0) {
        ShowError("FlagArray::Close() - attempt to close NULL fp");
    }
    fclose(fp);
    fp = 0;
}

/* Function start: 0x420990 */
void FlagArray::Seek(int index) {
    if (fp == 0) {
        WriteToMessageLog("FlagArray::Seek");
    }
    
    int offset = field_0x44 * index + field_0x3c;
    
    if (index < 0 || index >= max_states) {
        ShowError("Error in flagaray.cpp - Seek: Invalid index number! %d", index);
    }
    
    // Check if offset is within file bounds logic
    // Decompiled: if ((*(int *)((int)this + 0x38) - *(int *)((int)this + 0x44)) + 1 < iVar1) 
    // offset > field_0x38 - field_0x44 + 1 ??
    // iVar1 is offset.
    if ((field_0x38 - field_0x44) + 1 < offset) {
        ShowError("Error in flagaray.cpp - Seek: Attempt to read past end of file");
    }
    
    fseek(fp, offset, 0); // SEEK_SET
}

/* Function start: 0x420A00 */
unsigned int FlagArray::GetFlag(int index, unsigned int mask) {
    unsigned int val;
    Open();
    Seek(index);
    fread(&val, field_0x44, 1, fp);
    mask = val & mask;
    Close();
    return mask;
}

/* Function start: 0x420A50 */
void FlagArray::SetFlag(int index, unsigned int mask) {
    unsigned int val;
    Open();
    Seek(index);
    fread(&val, field_0x44, 1, fp);
    val |= mask;
    Seek(index); // Go back
    fwrite(&val, field_0x44, 1, fp);
    Close();
}

/* Function start: 0x420B30 */
void FlagArray::ClearAllFlags() {
    int i;
    int buffer = 0;

    Open();
    Seek(0);
    for (i = 0; i < max_states; i++) {
        fwrite(&buffer, 4, 1, fp);
    }
    Close();
}
