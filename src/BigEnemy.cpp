#include "BigEnemy.h"
#include <string.h>

extern "C" {
    void FUN_004110d0(void*);
    void FUN_0041e7d0(void*);
    void FUN_00424940(void*);
}

/* Function start: 0x412110 */
BigEnemy::BigEnemy()
{
    FUN_004110d0(this);
    *(void**)this = (void*)0x43140c;
    memset(this->field_0x4, 0, 0xe4);
    this->field_0xe8 = 0;
    memset(this->field_0xec, 0, 0x98);
    this->field_0x184 = 0;
    memset(this->field_0x188, 0, 4);
}

/* Function start: 0x4121f0 */
BigEnemy::~BigEnemy()
{
    DestructorHelper();
    if (*(int*)((char*)this + 0x184) == 1) {
        FUN_00424940(this);
    }
}

/* Function start: 0x412210 */
void BigEnemy::DestructorHelper()
{
    *(void**)this = (void*)0x43140c;
    if (this->field_0xe8) {
        FUN_0041e7d0(this->field_0xe8);
        FUN_00424940(this->field_0xe8);
        this->field_0xe8 = 0;
    }
}
