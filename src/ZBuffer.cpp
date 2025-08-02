#include "ZBuffer.h"
#include "Message.h"
#include "string.h"
#include <stdlib.h>

/* Function start: 0x4012D0 */
void ZBuffer::Update(int param_1, int param_2)
{
    if (timer.Update() > 10000) {
        SC_Message_Send(3, field_0x88, field_0x88, field_0x8c, 0x14, 0, 0, 0, 0, 0);
    }
    if (field_0x88 == param_2) {
        ShowError("SC_ZBuffer::Update");
    }
}

/* Function start: 0x401330 */
void ZBuffer::AddMessage(int param_1)
{
    //VFunc5(param_1);
    ShowError("SC_ZBuffer::AddMessage");
}
