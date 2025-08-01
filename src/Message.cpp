#include "Message.h"
#include <string.h>

// TODO: Define g_GameStruct2
extern int* g_GameStruct2;

extern "C" {
    void* Message_Constructor(void* mem, int param_1, int param_2, int param_3, int param_4, int param_5, int param_6, int param_7, int param_8, int param_9, int param_10);
    int* AllocateMemory_Wrapper(int size);
    void FUN_0041a3b9();
}

/* Function start: 0x41A150 */
void SC_Message_Send(int param_1, int param_2, int param_3, int param_4,
    int param_5, int param_6, int param_7, int param_8,
    int param_9, int param_10)
{
    char local_dc[192];
    void* pvVar1;
    int* piVar6;
    int* piVar7;

    if (g_GameStruct2 != 0) {
        pvVar1 = Message_Constructor(local_dc, param_1, param_2, param_3, param_4, param_5, param_6, param_7,
            param_8, param_9, param_10);

        int* local_18 = g_GameStruct2;
        int* local_14 = (int*)g_GameStruct2[1];
        int** local_1c = (int**)&g_GameStruct2[1];
        piVar7 = &g_GameStruct2[3];

        if (*piVar7 == 0) {
            int* piVar6_2 = &g_GameStruct2[5];
            int* puVar2 = AllocateMemory_Wrapper(g_GameStruct2[5] * 200 + 4);
            *puVar2 = local_18[4];
            local_18[4] = (int)puVar2;
            int iVar5 = *piVar6_2;
            piVar6_2 = puVar2 + iVar5 * 0x32 + -0x31;
            for (; -1 < iVar5; iVar5--) {
                *piVar6_2 = *piVar7;
                *piVar7 = (int)piVar6_2;
                piVar6_2 = piVar6_2 + -0x32;
            }
        }
        piVar6 = (int*)*piVar7;
        *piVar7 = *piVar6;
        piVar6[1] = (int)local_14;
        *piVar6 = 0;
        local_18[2] = local_18[2] + 1;

        Message* msg = (Message*)(piVar6 + 2);
        Message_Constructor(msg, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

        Message* src_msg = (Message*)pvVar1;
        msg->field_0x8 = src_msg->field_0x8;
        msg->field_0xc = src_msg->field_0xc;
        int i;
        for (i = 0; i < 0x20; i++) {
            msg->data1[i] = src_msg->data1[i];
        }
        msg->field_0x30 = src_msg->field_0x30;
        msg->field_0x38 = src_msg->field_0x38;
        msg->field_0x3c = src_msg->field_0x3c;
        for (i = 0; i < 0x40; i++) {
            msg->data2[i] = src_msg->data2[i];
        }
        msg->field_0x80 = src_msg->field_0x80;
        msg->field_0x88 = src_msg->field_0x88;
        msg->field_0x8c = src_msg->field_0x8c;
        msg->field_0x90 = src_msg->field_0x90;
        msg->field_0x94 = src_msg->field_0x94;
        msg->field_0x98 = src_msg->field_0x98;
        msg->field_0x9c = src_msg->field_0x9c;
        msg->field_0xa0 = src_msg->field_0xa0;
        msg->field_0xa4 = src_msg->field_0xa4;
        msg->field_0xa8 = src_msg->field_0xa8;
        msg->field_0xac = src_msg->field_0xac;
        msg->field_0xb0 = src_msg->field_0xb0;
        msg->field_0xb4 = src_msg->field_0xb4;
        msg->field_0xb8 = src_msg->field_0xb8;
        msg->field_0xbc = src_msg->field_0xbc;

        if (*local_1c == 0) {
            *local_18 = (int)piVar6;
        }
        else {
            **(int***)local_1c = piVar6;
        }
        *local_1c = piVar6;
        FUN_0041a3b9();
    }
}

