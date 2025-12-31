#include "FlagArray.h"
#include "globals.h"
#include "string.h"
#include <string.h>
#include <stdio.h>

extern "C" {
    int FUN_00421d10(void*);
    int _SmackWait_4(int);
    void FUN_0041ac50(void*, int);
    void FUN_0041fc20(void*, int, int);
    void FUN_0041fca0(int);
    void FUN_0041acf0(void*, int, int, int, int, int, int, int, int);
    void FUN_0041fcb0(int);
    void FUN_0041ac80(void*);
    void* FUN_004224f0();
    void* FUN_004224e0();
    int FUN_00421af0();
    void FUN_00425f30(FILE* fp, long offset, int origin);
    
    // Using standard library wrapper names to match link expectations or implementations
    FILE* fsopen(const char* filename, const char* mode);
    // Stub for fopen wrapper
    FILE* FUN_00425e50(const char* filename, const char* mode) {
        return fopen(filename, mode);
    }
    // Stub for fwrite wrapper
    void FUN_004269e0(const void* ptr, size_t size, size_t n, FILE* stream) {
        fwrite(ptr, size, n, stream);
    }
    // Stub for fread wrapper
    void __fread_lk(void* ptr, size_t size, size_t n, FILE* stream) {
        fread(ptr, size, n, stream);
    }
}

// These strings need to be defined somewhere. Defining them here or referencing globals.
// "rb+"
char DAT_004371ac[] = "rb+"; 
// "wb+" (overrides globals.cpp if duplicate, but better check globals first)
// extern char DAT_004371a8[]; // in globals.cpp it is "rb", needs fixing



/* Function start: 0x420140 */
FlagArray::FlagArray(char* filename, int max_states) {
    FILE* fp_temp;
    char buffer[4] = {0, 0, 0, 0};
    int i;
    
    // Clear the object memory (0x33 dwords = 204 bytes)
    memset(this, 0, 204);
    
    // Copy filename (max 50 chars)
    if (strlen(filename) > 0x32) { // 50
         ShowError("Error in flagaray.cpp (SetFilename, filename too long)");
    }
    strcpy(this->filename, filename);
    
    // Try open rb+
    int result = (int)FUN_00425e50(this->filename, DAT_004371ac);
    this->fp = (FILE*)result;
    
    if (result == 0) {
        // Failed, create new wb+
        // Need to reference DAT_004371a8 (wb+) properly.
        // Assuming "wb+"
        fp_temp = FUN_00425e50(this->filename, "wb+");
        this->fp = fp_temp;
        
        if (fp_temp == NULL) {
             ShowError("Error in flagaray.cpp (Create, Could not create file)", filename);
        } else {
             this->max_states = max_states;
             this->field_0x38 = max_states * 4 + 0x94;
             this->field_0x3c = 0x94;
             this->field_0x44 = 4;
             
             // Write header (from field_0x38 onwards, size 0x94)
             FUN_004269e0(&this->field_0x38, 0x94, 1, this->fp);
             
             // Write flags (0)
             for (i = 0; i < this->max_states; i++) {
                 FUN_004269e0(buffer, 4, 1, this->fp);
             }
        }
    }
    
    if (this->fp != NULL) {
        fclose(this->fp);
    }
    this->fp = NULL;
}

/* Function start: 0x420250 */
void FlagArray::SafeClose() {
    if (this->fp != NULL) {
        fclose(this->fp);
    }
    this->fp = 0;
}

/* Function start: 0x420270 */
void FlagArray::Open() {
    if (this->fp != 0) {
        ShowError("double FlagArray::Open()");
    }
    
    FILE* fp_temp = FUN_00425e50(this->filename, DAT_004371ac);
    this->fp = fp_temp;
    
    if (fp_temp == NULL) {
        ShowError("Error opening file in flagarray.cpp");
    }
    
    // Read header back
    __fread_lk(&this->field_0x38, 0x94, 1, this->fp);
}

/* Function start: 0x4202D0 */
void FlagArray::Close() {
    if (this->fp == 0) {
        ShowError("FlagArray::Close() - attempt to close NULL fp");
    }
    fclose(this->fp);
    this->fp = 0;
}

/* Function start: 0x420300 */
void FlagArray::Seek(int index) {
    if (this->fp == 0) {
        ShowError("FlagArray::Seek");
    }
    
    int offset = this->field_0x44 * index + this->field_0x3c;
    
    if (index < 0 || index >= this->max_states) {
        ShowError("Error in flagaray.cpp (Seek, Index out of Range)", index);
    }
    
    // Check if offset is within file bounds logic
    // Decompiled: if ((*(int *)((int)this + 0x38) - *(int *)((int)this + 0x44)) + 1 < iVar1) 
    // offset > field_0x38 - field_0x44 + 1 ??
    // iVar1 is offset.
    if ((this->field_0x38 - this->field_0x44) + 1 < offset) {
        ShowError("Error in flagaray.cpp (Seek, Attempt to seek past end of file)");
    }
    
    FUN_00425f30(this->fp, offset, 0); // SEEK_SET
}

/* Function start: 0x420370 */
unsigned int FlagArray::GetFlag(int index, unsigned int mask) {
    unsigned int val = 0;
    Open();
    Seek(index);
    __fread_lk(&val, this->field_0x44, 1, this->fp);
    Close();
    return val & mask;
}

/* Function start: 0x4203C0 */
void FlagArray::SetFlag(int index, unsigned int mask) {
    unsigned int val = 0;
    Open();
    Seek(index);
    __fread_lk(&val, this->field_0x44, 1, this->fp);
    val |= mask;
    Seek(index); // Go back
    FUN_004269e0(&val, this->field_0x44, 1, this->fp);
    Close();
}

/* Function start: 0x420430 */
void FlagArray::ClearAllFlags() {
    int i = 0;
    char buffer[4] = {0, 0, 0, 0};
    
    Open();
    Seek(0);
    if (this->max_states > 0) {
        do {
            FUN_004269e0(buffer, 4, 1, this->fp);
            i++;
        } while (i < this->max_states);
    }
    Close();
}
