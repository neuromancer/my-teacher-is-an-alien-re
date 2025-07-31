#include "AILData.h"
#include <stdio.h>

extern "C" {
    void FUN_0041e670(int);
    void _AIL_mem_free_lock_4(void*);
    void* _AIL_mem_alloc_lock_4(unsigned int);
    FILE* OpenFileAndFindKey(char*, char*, const char*, unsigned int*);
    size_t __fread_lk(void*, size_t, size_t, FILE*);
    int _fclose(FILE*);
}

extern int* DAT_0043696c;

/* Function start: 0x41E460 */
AILData::AILData()
{
    this->data = 0;
    this->size = 0;
}

/* Function start: 0x41E470 */
AILData::~AILData()
{
    if (this->data != 0) {
        FUN_0041e670((int)this);
        _AIL_mem_free_lock_4(this->data);
        this->data = 0;
    }
}

/* Function start: 0x41E490 */
int AILData::Load(char* filename)
{
    if (*(int*)((char*)DAT_0043696c + 0x38) == 0) {
        return 1;
    }
    if (this->data != 0) {
        this->~AILData();
    }
    this->size = 0;
    FILE* _File = OpenFileAndFindKey((char*)0x0, filename, "rb", (unsigned int*)&this->size);
    if ((_File != 0) && (this->size != 0)) {
        this->data = (char*)_AIL_mem_alloc_lock_4(this->size);
        if (this->data == 0) {
            return 1;
        }
        __fread_lk(this->data, this->size, 1, _File);
        _fclose(_File);
        return 0;
    }
    return 1;
}
