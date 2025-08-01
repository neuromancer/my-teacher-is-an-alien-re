#include "VBuffer.h"

extern void ShowError(const char*);
extern "C" void FUN_0041aa30(VBuffer*);
extern "C" void SetGraphicsMode(int);
extern "C" void ClearScreen();


void VBuffer::Init(unsigned int param_1, unsigned int param_2)
{
    // This is a guess based on the name and parameters.
    // The actual implementation is unknown.
    this->y_offset = param_1;
    this->field_0x1c = param_2;
}

/* Function start: 0x41a9f0 */
void VBuffer::VirtualBufferCreateAndClean(int width, int height)
{
    FUN_0041aa30(this);
    this->Init(width, height);
}

/* Function start: 0x41abc0 */
void VBuffer::ClearScreen(int color)
{
    VBuffer_SetCurrentVideoMode(this, this->field_0x1c);
    SetGraphicsMode(color);
    ::ClearScreen();
    VBuffer_InvalidateVideoMode(this);
}

/* Function start: 0x41ad40 */
void VBuffer::TPaste(void)
{
    ShowError("VBuffer::TPaste - Not implemented");
}

extern "C" {
    void FUN_0041b310(int, int, int, int, int, int, VBuffer*, VBuffer*, char, char);
    void FUN_0041ae0c(void);
}

/* Function start: 0x41ad50 */
void VBuffer::BlitTransparent(int param_1, int param_2, int param_3, int param_4, int param_5, int param_6, char param_7, char param_8)
{
    VBuffer local_40;
    local_40.Init(0,0);
    // This is a hack to match the stack layout.
    // The compiler will optimize this away.
    int local_1c;
    int local_18;

    try
    {
        local_40.VirtualBufferCreateAndClean((param_2 - param_1) + 1, (param_4 - param_3) + 1);
        local_40.ClearScreen(0);
        FUN_0041b310(param_1, param_2, param_3, param_4, param_5, param_6, this, &local_40, param_7, param_8);
        local_40.TPaste();
    }
    catch(...)
    {
        FUN_0041ae0c();
        throw;
    }
    FUN_0041ae0c();
}
