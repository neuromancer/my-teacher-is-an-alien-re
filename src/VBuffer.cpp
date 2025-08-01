#include "VBuffer.h"

extern "C" {
    void ShowError(const char*, ...);
    void FUN_0041aa30(VBuffer*);
    void SetGraphicsMode(int);
    void ClearScreen();
    unsigned int CreateTable(unsigned int, unsigned int);
    void FUN_00422e8f();
    int FUN_00422e71(unsigned int);
    void FUN_0041a9d0(unsigned int);
    unsigned int GetCurrentVideoMode();
    void InvalidateVideoMode();
    void FUN_0041b310(int, int, int, int, int, int, VBuffer*, VBuffer*, char, char);
    void FUN_0041ae0c(void);
    void FUN_004231ce(int, int, int, int, int, int, unsigned int, unsigned int);
    void FUN_004233e8(int, int, int, int, int, int, unsigned int, unsigned int);
    int FUN_0041b590(int*, int*, int*, int*);
    void FUN_0041af9f(void);
    void FUN_0041afb1(void);
    void FUN_00401680(void);
    int FUN_004230d9(int);
    void FUN_00423296(int, int, int, int, int, int);
    void FUN_0042333a(int, int, int, int, int, int, int, int);
}

/* Function start: 0x41aaa0 */
VBuffer::VBuffer(unsigned int param_1, unsigned int param_2)
{
    if ((param_1 == 0) || (param_2 == 0)) {
        ShowError("Error! Invalid buffer size specified =>%lu", param_2 * param_1);
    }
    this->width = param_1;
    this->height = param_2;
    this->field_0x28 = 0;
    this->field_0x2c = (void*)(param_1 - 1);
    this->field_0x20 = 0;
    this->field_0x24 = param_2 - 1;
    if (this->data != 0) {
        ShowError("Error! Virtual buffer already allocated");
    }

    unsigned int uVar2 = (int)param_1 >> 0x1f;
    if (((param_1 ^ uVar2) - uVar2 & 3 ^ uVar2) != uVar2) {
        do {
            param_1 = param_1 + 1;
            uVar2 = (int)param_1 >> 0x1f;
        } while (((param_1 ^ uVar2) - uVar2 & 3 ^ uVar2) != uVar2);
    }
    uVar2 = (int)param_2 >> 0x1f;
    if (((param_2 ^ uVar2) - uVar2 & 3 ^ uVar2) != uVar2) {
        do {
            param_2 = param_2 + 1;
            uVar2 = (int)param_2 >> 0x1f;
        } while (((param_2 ^ uVar2) - uVar2 & 3 ^ uVar2) != uVar2);
    }
    uVar2 = CreateTable(param_1, param_2);
    this->field_0x1c = uVar2;
    if (uVar2 == 0xffffffff) {
        ShowError("VBuffer::Init - Unable To create vb. Table Full");
    }
    if (uVar2 == 0xfffffffe) {
        ShowError("VBuffer::Init - Unable To create vb. No memory");
    }
    this->SetCurrentVideoMode(uVar2);
    FUN_00422e8f();
    this->InvalidateVideoMode();
    this->data = (void*)FUN_00422e71(this->field_0x1c);
    FUN_0041a9d0(this->field_0x1c);
}

/* Function start: 0x41aa10 */
VBuffer::~VBuffer()
{
    Release();
    Free();
}


/* Function start: 0x41ac10 */
void VBuffer::Release()
{
    unsigned int uVar1 = GetCurrentVideoMode();
    if (uVar1 == this->field_0x1c) {
        this->InvalidateVideoMode();
    }
}

extern "C" {
    void FUN_00422e1a(int);
    void FUN_0041a9e0(int);
}

/* Function start: 0x41aa60 */
void VBuffer::Free()
{
    if (this->data != 0) {
        FUN_00422e1a(this->field_0x1c);
        FUN_0041a9e0(this->field_0x1c);
        this->field_0x1c = 0xffffffff;
        this->data = 0;
    }
}

/* Function start: 0x41a9f0 */
void VBuffer::VirtualBufferCreateAndClean(int width, int height)
{
    InitFields(); //FUN_0041aa30(this);
    VBuffer(width, height);
}

/* Function start: 0x41abc0 */
void VBuffer::ClearScreen(int color)
{
    this->SetCurrentVideoMode(this->field_0x1c);
    SetGraphicsMode(color);
    ::ClearScreen();
    this->InvalidateVideoMode();
}

extern "C" void FUN_0041a9a0(void);

/* Function start: 0x41aa30 */
void VBuffer::InitFields()
{
    for (int i = 0; i < 0xc; i++) {
        ((int*)this)[i] = 0;
    }
    this->field_0x1c = 0xffffffff;
    *(int*)this = 0xffffffff;
    FUN_0041a9a0();
}

/* Function start: 0x41ad40 */
void VBuffer::TPaste(void)
{
    ShowError("VBuffer::TPaste - Not implemented");
}


/* Function start: 0x41ad50 */
void VBuffer::BlitTransparent(int param_1, int param_2, int param_3, int param_4, int param_5, int param_6, char param_7, char param_8)
{
    VBuffer local_40(0,0);

    local_40.VirtualBufferCreateAndClean((param_2 - param_1) + 1, (param_4 - param_3) + 1);
    local_40.ClearScreen(0);
    FUN_0041b310(param_1, param_2, param_3, param_4, param_5, param_6, this, &local_40, param_7, param_8);
    local_40.TPaste();

    try {
        FUN_0041ae0c();
    } catch (...) {

    }
}


/* Function start: 0x41ae20 */
void VBuffer::CallBlitter(int param_1, int param_2, int param_3, int param_4, int param_5, int param_6, int param_7)
{
    FUN_004231ce(param_1, param_2, param_3, param_4, param_5, param_6, *(unsigned int*)(param_7 + 0x1c), this->field_0x1c);
}


/* Function start: 0x41ae60 */
void VBuffer::CallBlitter2(int param_1, int param_2, int param_3, int param_4, int param_5, int param_6, int param_7)
{
    FUN_004233e8(param_1, param_2, param_3, param_4, param_5, param_6, *(unsigned int*)(param_7 + 0x1c), this->field_0x1c);
}

/* Function start: 0x41aea0 */
void VBuffer::CallBlitter3(int param_1, int param_2, int param_3, int param_4, int param_5, int param_6, int param_7, char param_8, char param_9)
{
    FUN_0041b310(param_1, param_2, param_3, param_4, param_5, param_6, (VBuffer*)param_7, this, param_8, param_9);
}


/* Function start: 0x41aee0 */
void VBuffer::ClipAndBlit(int param_1, int param_2, int param_3, int param_4, int param_5, int param_6, int param_7)
{
    int local_30 = (int)this->field_0x28;
    int local_28 = (int)this->field_0x2c;
    int local_2c = 0; // this->field_0x20
    int local_24 = 0; // this->field_0x24
    int local_20 = param_1;
    int local_1c = param_3;
    int local_18 = param_2;
    int local_14 = param_4;

    __try
    {
        if (FUN_0041b590(&local_30, &local_20, &param_5, &param_6))
        {
            this->CallBlitter(local_20, local_18, local_1c, local_14, param_5, param_6, param_7);
        }
    }
    __finally
    {
        FUN_0041af9f();
        FUN_0041afb1();
    }
}


/* Function start: 0x41abf0 */
int VBuffer::SetVideoMode()
{
    int iVar1 = FUN_004230d9(this->field_0x1c);
    this->field_0x20 = 0xffffffff;
    this->field_0x24 = 1;
    return iVar1;
}

/* Function start: 0x41ac30 */
void* VBuffer::GetData()
{
    return this->data;
}

/* Function start: 0x41ac50 */
int VBuffer::SetCurrentVideoMode(int param_1)
{
    this->field_0x24 = this->field_0x24 + 1;
    unsigned int uVar1 = GetCurrentVideoMode();
    if (param_1 != uVar1) {
        this->field_0x20 = uVar1;
        uVar1 = FUN_004230d9(param_1);
    }
    return uVar1;
}

/* Function start: 0x41ac40 */
void EmptyFunction()
{
}


/* Function start: 0x41acb0 */
void VBuffer::CallBlitter4(int param_1, int param_2, int param_3, int param_4, int param_5, int param_6)
{
    this->SetCurrentVideoMode(this->field_0x1c);
    FUN_00423296(param_1, param_2, param_3, param_4, param_5, param_6);
    this->InvalidateVideoMode();
}


/* Function start: 0x41acf0 */
void VBuffer::CallBlitter5(int param_1, int param_2, int param_3, int param_4, int param_5, int param_6, int param_7, int param_8)
{
    this->SetCurrentVideoMode(this->field_0x1c);
    FUN_0042333a(param_1, param_2, param_3, param_4, param_5, param_6, param_7, param_8);
    this->InvalidateVideoMode();
}

/* Function start: 0x41ac80 */
void VBuffer::InvalidateVideoMode()
{
    if ((this->field_0x24 != 0) && (this->field_0x24 = this->field_0x24 - 1, this->field_0x24 == 0)) {
        ::InvalidateVideoMode();
        if (this->field_0x20 != 0xffffffff) {
            FUN_004230d9(this->field_0x20);
            this->field_0x20 = 0xffffffff;
        }
    }
}
