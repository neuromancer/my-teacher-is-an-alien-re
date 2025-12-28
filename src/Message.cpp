#include "Message.h"
#include <string.h>
#include "Memory.h"

extern int* DAT_00436988;

extern "C" {
    void FUN_0041a3b9();
}

/* Function start: 0x41A150 */
void SC_Message_Send(int param_1, int param_2, int param_3, int param_4,
    int param_5, int param_6, int param_7, int param_8,
    int param_9, int param_10)
{
    char local_dc[192];
    int* local_1c;
    int* local_18;
    int* local_14;
    Message* local_10;
    Message* pvVar1;
    int* piVar7;
    int* piVar6_2;
    int* puVar2;
    int iVar5;
    int* pEntry;
    int* piVar6;
    int aVar;
    int iVar;
    int tmp;
    char cl;

    if (DAT_00436988 != 0) {
        __try {
            pvVar1 = (Message*)local_dc;
            pvVar1->Message::Message(param_1, param_2, param_3, param_4, param_5, param_6, param_7, param_8, param_9, param_10);

            local_18 = DAT_00436988;
            local_14 = (int*)DAT_00436988[1];
            local_1c = DAT_00436988 + 1;
            piVar7 = DAT_00436988 + 3;

            if (*piVar7 == 0) {
                piVar6_2 = DAT_00436988 + 5;
                puVar2 = (int*)AllocateMemory(DAT_00436988[5] * 200 + 4);
                *puVar2 = local_18[4];
                local_18[4] = (int)puVar2;
                iVar5 = *piVar6_2;
                pEntry = puVar2 + iVar5 * 0x32 + -0x31;
                iVar5--;
                for (; -1 < iVar5; iVar5--) {
                    *pEntry = *piVar7;
                    *piVar7 = (int)pEntry;
                    pEntry = pEntry + -0x32;
                }
            }
            piVar6 = (int*)*piVar7;
            *piVar7 = *piVar6;
            piVar6[1] = (int)local_14;
            *piVar6 = 0;
            local_18[2] = local_18[2] + 1;

            local_10 = (Message*)(piVar6 + 2);
            memset(local_10, 0, 0x30 * sizeof(int));

            iVar5 = 0;
            do {
                if (local_10 != 0) {
                    local_10->Message::Message(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                }
                local_10 = (Message*)((int*)local_10 + 0x30);
                tmp = iVar5;
                iVar5--;
            } while (tmp != 0);

            piVar6[4] = pvVar1->field_0x8;
            piVar6[5] = pvVar1->field_0xc;

            aVar = 0;
            do {
                cl = pvVar1->data1[aVar];
                aVar++;
                ((char*)piVar6)[aVar + 0x17] = cl;
            } while (aVar < 0x20);

            piVar6[0xe] = pvVar1->field_0x30;
            iVar = pvVar1->field_0x3c;
            piVar6[0x10] = pvVar1->field_0x38;
            piVar6[0x11] = iVar;

            aVar = 0;
            do {
                cl = pvVar1->data2[aVar];
                aVar++;
                ((char*)piVar6)[aVar + 0x47] = cl;
            } while (aVar < 0x40);

            piVar6[0x22] = pvVar1->field_0x80;
            piVar6[0x24] = pvVar1->field_0x88;
            piVar6[0x25] = pvVar1->field_0x8c;
            piVar6[0x26] = pvVar1->field_0x90;
            piVar6[0x27] = pvVar1->field_0x94;
            piVar6[0x28] = pvVar1->field_0x98;
            piVar6[0x29] = pvVar1->field_0x9c;
            piVar6[0x2a] = pvVar1->field_0xa0;
            piVar6[0x2b] = pvVar1->field_0xa4;
            piVar6[0x2c] = pvVar1->field_0xa8;
            piVar6[0x2d] = pvVar1->field_0xac;
            piVar6[0x2e] = pvVar1->field_0xb0;
            piVar6[0x2f] = pvVar1->field_0xb4;
            piVar6[0x30] = pvVar1->field_0xb8;
            piVar6[0x31] = pvVar1->field_0xbc;

            if (*(int**)local_1c == 0) {
                *local_18 = (int)piVar6;
            }
            else {
                **(int**)local_1c = (int)piVar6;
            }
            *local_1c = (int)piVar6;
            FUN_0041a3b9();
        } __except(1) {
        }
    }
}
