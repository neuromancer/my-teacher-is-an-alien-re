#include "SC_Question.h"
#include <string.h>
#include "Memory.h"

inline void* operator new(unsigned int, void* p) { return p; }

extern int* DAT_00436988;

extern "C" {
    void FUN_0041a3b9();
}

/* Function start: 0x41A150 */
void SC_Message_Send(int param_1, int param_2, int param_3, int param_4,
    int param_5, int param_6, int param_7, int param_8,
    int param_9, int param_10)
{
    char local_dc[0xC0];
    int* local_1c;
    int* local_18;
    int* local_14;
    SC_Message* local_10;
    int* piVar7;
    int* piVar6_2;
    int* puVar2;
    int iVar5;
    int* pEntry;
    int* piVar6;
    unsigned int aVar;
    int iVar;
    int tmp;
    SC_Message* pvVar1;

    if (DAT_00436988 != 0) {
        __try {
            pvVar1 = new (local_dc) SC_Message(param_1, param_2, param_3, param_4, param_5, param_6, param_7, param_8, param_9, param_10);

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

            local_10 = (SC_Message*)(piVar6 + 2);
            memset(local_10, 0, 0x30 * sizeof(int));

            iVar5 = 0;
            do {
                if (local_10 != 0) {
                    local_10->SC_Message::SC_Message(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                }
                local_10 = (SC_Message*)((int*)local_10 + 0x30);
                tmp = iVar5;
                iVar5--;
            } while (tmp != 0);

            piVar6[4] = ((int*)pvVar1)[2];
            piVar6[5] = ((int*)pvVar1)[3];

            aVar = 0;
            do {
                ((char*)piVar6)[aVar + 0x18] = ((char*)pvVar1)[aVar + 0x10];
                aVar++;
            } while (aVar < 0x20);

            piVar6[0xe] = *(int*)((char*)pvVar1 + 0x30);
            iVar = *(int*)((char*)pvVar1 + 0x3c);
            piVar6[0x10] = *(int*)((char*)pvVar1 + 0x38);
            piVar6[0x11] = iVar;

            aVar = 0;
            do {
                ((char*)piVar6)[aVar + 0x48] = ((char*)pvVar1)[aVar + 0x40];
                aVar++;
            } while (aVar < 0x40);

            piVar6[0x22] = *(int*)((char*)pvVar1 + 0x80);
            piVar6[0x24] = *(int*)((char*)pvVar1 + 0x88);
            piVar6[0x25] = *(int*)((char*)pvVar1 + 0x8c);
            piVar6[0x26] = *(int*)((char*)pvVar1 + 0x90);
            piVar6[0x27] = *(int*)((char*)pvVar1 + 0x94);
            piVar6[0x28] = *(int*)((char*)pvVar1 + 0x98);
            piVar6[0x29] = *(int*)((char*)pvVar1 + 0x9c);
            piVar6[0x2a] = *(int*)((char*)pvVar1 + 0xa0);
            piVar6[0x2b] = *(int*)((char*)pvVar1 + 0xa4);
            piVar6[0x2c] = *(int*)((char*)pvVar1 + 0xa8);
            piVar6[0x2d] = *(int*)((char*)pvVar1 + 0xac);
            piVar6[0x2e] = *(int*)((char*)pvVar1 + 0xb0);
            piVar6[0x2f] = *(int*)((char*)pvVar1 + 0xb4);
            piVar6[0x30] = *(int*)((char*)pvVar1 + 0xb8);
            piVar6[0x31] = *(int*)((char*)pvVar1 + 0xbc);

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
