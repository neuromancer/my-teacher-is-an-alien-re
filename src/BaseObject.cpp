/*
Function: BaseObject_Create
Address: 0x417450

MOV EAX,FS:[0x0]
PUSH EBP
MOV EBP,ESP
PUSH -0x1
PUSH 0x417648
PUSH EAX
MOV dword ptr FS:[0x0],ESP
SUB ESP,0x10
MOV dword ptr [EBP + -0x14],ECX
PUSH EBX
PUSH ESI
PUSH EDI
MOV ESI,dword ptr [0x0043698c]
MOV EDI,dword ptr [ESI + 0xa0]
MOV EAX,dword ptr [EDI]
TEST EAX,EAX
JZ 0x5A
MOV dword ptr [EDI + 0x8],EAX
CMP dword ptr [EDI],0x0
JZ 0x5A
MOV ECX,EDI
CALL 0x00401710
TEST EAX,EAX
JZ 0x55
PUSH EAX
MOV dword ptr [EAX],0x431050
CALL 0x00424940
ADD ESP,0x4
CMP dword ptr [EDI],0x0
JNZ 0x3B
MOV EDI,dword ptr [ESI + 0xa4]
MOV EAX,dword ptr [EDI]
TEST EAX,EAX
JZ 0xB2
MOV dword ptr [EDI + 0x8],EAX
CMP dword ptr [EDI],0x0
JZ 0xB2
MOV ECX,EDI
CALL 0x00401790
MOV EBX,EAX
TEST EBX,EBX
JZ 0xAD
MOV ECX,dword ptr [EBX + 0x4]
TEST ECX,ECX
JZ 0x90
PUSH 0x1
CALL 0x00417660
MOV dword ptr [EBX + 0x4],0x0
MOV ECX,dword ptr [EBX + 0x8]
TEST ECX,ECX
JZ 0xA4
PUSH 0x1
MOV EAX,dword ptr [ECX]
CALL dword ptr [EAX]
MOV dword ptr [EBX + 0x8],0x0
PUSH EBX
CALL 0x00424940
ADD ESP,0x4
CMP dword ptr [EDI],0x0
JNZ 0x6E
MOV ECX,dword ptr [ESI + 0x9c]
MOV dword ptr [EBP + -0x10],ECX
MOV EAX,dword ptr [ECX]
TEST EAX,EAX
JZ 0x189
MOV dword ptr [ECX + 0x8],EAX
CMP dword ptr [ECX],0x0
JZ 0x189
MOV ECX,dword ptr [EBP + -0x10]
MOV EAX,dword ptr [ECX + 0x8]
TEST EAX,EAX
JNZ 0xE4
MOV dword ptr [EBP + -0x18],0x0
JMP 0x153
MOV ECX,dword ptr [EBP + -0x10]
CMP dword ptr [ECX],EAX
JNZ 0xF0
MOV EAX,dword ptr [EAX + 0x4]
MOV dword ptr [ECX],EAX
MOV ECX,dword ptr [EBP + -0x10]
MOV EAX,dword ptr [ECX + 0x8]
CMP dword ptr [ECX + 0x4],EAX
JNZ 0x100
MOV EAX,dword ptr [EAX]
MOV dword ptr [ECX + 0x4],EAX
MOV ECX,dword ptr [EBP + -0x10]
MOV EAX,dword ptr [ECX + 0x8]
MOV ECX,dword ptr [EAX]
TEST ECX,ECX
JZ 0x112
MOV EAX,dword ptr [EAX + 0x4]
MOV dword ptr [ECX + 0x4],EAX
MOV ECX,dword ptr [EBP + -0x10]
MOV EAX,dword ptr [ECX + 0x8]
MOV ECX,dword ptr [EAX + 0x4]
TEST ECX,ECX
JZ 0x123
MOV EAX,dword ptr [EAX]
MOV dword ptr [ECX],EAX
MOV ECX,dword ptr [EBP + -0x10]
CALL 0x00417680
MOV ESI,EAX
MOV ECX,dword ptr [EBP + -0x10]
MOV ECX,dword ptr [ECX + 0x8]
TEST ECX,ECX
JZ 0x148
PUSH 0x1
CALL 0x004189a0
MOV ECX,dword ptr [EBP + -0x10]
MOV dword ptr [ECX + 0x8],0x0
MOV ECX,dword ptr [EBP + -0x10]
MOV dword ptr [EBP + -0x18],ESI
MOV EAX,dword ptr [ECX]
MOV dword ptr [ECX + 0x8],EAX
CMP dword ptr [EBP + -0x18],0x0
JZ 0x17D
MOV dword ptr [EBP + -0x4],0xffffffff
MOV EAX,dword ptr [EBP + -0x18]
MOV dword ptr [EBP + -0x1c],EAX
MOV dword ptr [EAX],0x431058
CALL 0x00417652
MOV EAX,dword ptr [EBP + -0x18]
PUSH EAX
CALL 0x00424940
ADD ESP,0x4
MOV ECX,dword ptr [EBP + -0x10]
CMP dword ptr [ECX],0x0
JNZ 0xD1
MOV EAX,dword ptr [EBP + -0x14]
MOV EDX,dword ptr [EBP + -0x14]
MOV EAX,dword ptr [EAX + 0x14]
MOV ECX,dword ptr [EAX]
MOV dword ptr [EAX + 0x8],ECX
MOV EAX,dword ptr [EDX + 0x14]
CMP dword ptr [EAX + 0x8],0x0
JZ 0x1E8
XOR ESI,ESI
MOV EAX,dword ptr [EBP + -0x14]
MOV ECX,dword ptr [EAX + 0x14]
MOV EAX,dword ptr [ECX + 0x8]
MOV EDX,dword ptr [EAX + 0x8]
TEST EDX,EDX
JZ 0x1E8
MOV ECX,0x0
PUSH ESI
CMP EAX,0x1
ADC ECX,-0x1
AND ECX,EDX
MOV EAX,dword ptr [ECX]
CALL dword ptr [EAX + 0x18]
MOV EAX,dword ptr [EBP + -0x14]
MOV ECX,dword ptr [EAX + 0x14]
MOV EAX,dword ptr [ECX + 0x8]
CMP dword ptr [ECX + 0x4],EAX
JZ 0x1E8
TEST EAX,EAX
JZ 0x1DD
MOV EAX,dword ptr [EAX + 0x4]
MOV dword ptr [ECX + 0x8],EAX
MOV EAX,dword ptr [EBP + -0x14]
MOV ECX,dword ptr [EAX + 0x14]
CMP dword ptr [ECX + 0x8],ESI
JNZ 0x1A2
MOV EAX,dword ptr [EBP + -0xc]
POP EDI
MOV FS:[0x0],EAX
POP ESI
POP EBX
MOV ESP,EBP
POP EBP
RET
*/

extern "C" void FreeFromGlobalHeap(void* ptr);
extern "C" void* SC__ZBuffer__PopNode(void* p);
extern "C" void* SC__ZBuffer__PopNode_2(void* p);
extern "C" void FUN_00417660(void* p, int v);
extern "C" void* FUN_00417680(void* p);
extern "C" void FUN_004189a0(void* p, int v);
extern "C" void FUN_00417652();

extern "C" void* g_SoundManager;

class BaseObject {
public:
    void Destroy();
    void Copy(BaseObject* source);
    void Copy_2(BaseObject* source);
private:
    char pad_8[8];
    int field_8;
    int field_c;
    char field_10[32];
    int field_30;
    char pad_38[4];
    int field_38;
    int field_3c;
    char field_40[64];
    int field_80;
    char pad_90[12];
    int field_90;
    int field_94;
};

void BaseObject::Destroy() {
    void* soundManager = g_SoundManager;
    void* node;
    void* val;
    void** vtable;

    void** list1 = (void**)*((int*)soundManager + 0xa0);
    if (*list1 != 0) {
        list1[2] = *list1;
        while (*list1 != 0) {
            node = SC__ZBuffer__PopNode(list1);
            *(void**)node = (void*)0x431050;
            FreeFromGlobalHeap(node);
        }
    }

    void** list2 = (void**)*((int*)soundManager + 0xa4);
    if (*list2 != 0) {
        list2[2] = *list2;
        while (*list2 != 0) {
            node = SC__ZBuffer__PopNode_2(list2);
            if (*((void**)node + 1) != 0) {
                FUN_00417660(*((void**)node + 1), 1);
                *((void**)node + 1) = 0;
            }
            if (*((void***)node + 2) != 0) {
                void* obj = *((void**)node + 2);
                vtable = *(void***)obj;
                ((void (*)(int))vtable[0])(1);
                *((void***)node + 2) = 0;
            }
            FreeFromGlobalHeap(node);
        }
    }

    void** list3 = (void**)*((int*)soundManager + 0x9c);
    if (*list3 != 0) {
        list3[2] = *list3;
        while (*list3 != 0) {
            node = FUN_00417680(list3);
            if (node != 0) {
                __try {
                    *(void**)node = (void*)0x431058;
                    FUN_00417652();
                    FreeFromGlobalHeap(node);
                } __except(1) {
                }
            }
        }
    }

    void** list4 = (void**)*((int*)this + 5);
    list4[2] = *list4;
    while(list4[2] != 0) {
        node = list4[2];
        val = *((void**)node + 2);
        if (val == 0) break;

        vtable = *(void***)val;
        ((void (*)(int))(vtable[6]))(0);

        if (list4[1] == list4[2]) break;
        if (list4[2] != 0) {
            list4[2] = *((void**)list4[2] + 1);
        }
    }
}

/*
Function: Copy
Address: 0x417180

MOV EAX,dword ptr [ESP + 0x4]
TEST EAX,EAX
JZ 0x20
MOV EDX,dword ptr [EAX + 0x90]
MOV dword ptr [ECX + 0x90],EDX
MOV EAX,dword ptr [EAX + 0x94]
MOV dword ptr [ECX + 0x94],EAX
RET 0x4
*/
void BaseObject::Copy(BaseObject* source) {
    if (source != 0) {
        this->field_90 = source->field_90;
        this->field_94 = source->field_94;
    }
}

/*
Function: Copy
Address: 0x406610

MOV EDX,dword ptr [ESP + 0x4]
PUSH EBX
PUSH ESI
PUSH EDI
MOV EAX,dword ptr [EDX + 0x8]
MOV dword ptr [ECX + 0x8],EAX
XOR EAX,EAX
MOV EBX,dword ptr [EDX + 0xc]
MOV dword ptr [ECX + 0xc],EBX
MOV BL,byte ptr [EDX + EAX*0x1 + 0x10]
INC EAX
CMP EAX,0x20
MOV byte ptr [ECX + EAX*0x1 + 0xf],BL
JC 0x00406625
MOV EAX,dword ptr [EDX + 0x30]
LEA ESI,[ECX + 0x38]
MOV dword ptr [ECX + 0x30],EAX
LEA EAX,[EDX + 0x38]
MOV EDI,dword ptr [EAX]
MOV EBX,dword ptr [EAX + 0x4]
XOR EAX,EAX
MOV dword ptr [ESI],EDI
MOV dword ptr [ESI + 0x4],EBX
MOV BL,byte ptr [EDX + EAX*0x1 + 0x40]
INC EAX
CMP EAX,0x40
MOV byte ptr [ECX + EAX*0x1 + 0x3f],BL
JC 0x0040664b
MOV EAX,dword ptr [EDX + 0x80]
POP EDI
POP ESI
MOV dword ptr [ECX + 0x80],EAX
MOV EAX,ECX
POP EBX
RET 0x4
*/
#include <string.h>

void BaseObject::Copy_2(BaseObject* source) {
    this->field_8 = source->field_8;
    this->field_c = source->field_c;
    memcpy(this->field_10, source->field_10, 32);
    this->field_30 = source->field_30;
    this->field_38 = source->field_38;
    this->field_3c = source->field_3c;
    memcpy(this->field_40, source->field_40, 64);
    this->field_80 = source->field_80;
}
