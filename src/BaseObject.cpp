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
extern "C" void __amsg_exit(int);
extern "C" void FUN_004011ef();
extern "C" int FUN_0041ebb0(void *p);
extern "C" void FUN_00419390();
extern "C" void FUN_0041eb70(void *p, int v, int size);
extern "C" void ShowError(const char *msg, int val);
extern "C" void FUN_0041c72c();
extern "C" void VBuffer_VBuffer_Owner___VBuffer_Owner(void *p);
#include "BaseObject.h"
#include "Timer.h"

extern "C" void FUN_0041c94c();


extern "C" void* g_SoundManager;
extern "C" void* g_WorkBuffer;

void BaseObject::Destroy() {
    SoundManager* soundManager = (SoundManager*)g_SoundManager;
    ListNode* node;

    if (soundManager->list1 != 0) {
        while (soundManager->list1 != 0) {
            node = (ListNode*)SC__ZBuffer__PopNode(soundManager->list1);
            FreeFromGlobalHeap(node);
        }
    }

    if (soundManager->list2 != 0) {
        while (soundManager->list2 != 0) {
            node = (ListNode*)SC__ZBuffer__PopNode_2(soundManager->list2);
            if (node->next != 0) {
                FUN_00417660(node->next, 1);
                node->next = 0;
            }
            if (node->data != 0) {
                void(**vtable)(int) = *(void(***)(int))node->data;
                vtable[0](1);
                node->data = 0;
            }
            FreeFromGlobalHeap(node);
        }
    }

    if (soundManager->list3 != 0) {
        while (soundManager->list3 != 0) {
            node = (ListNode*)FUN_00417680(soundManager->list3);
            if (node != 0) {
                __try {
                    FreeFromGlobalHeap(node);
                } __except(1) {
                }
            }
        }
    }

    ListNode* list4 = (ListNode*)this->bounding_box_x1;
    if (list4 != 0) {
        for (ListNode* it = list4; it != 0; it = it->next) {
            if (it->data == 0) break;
            void(**vtable)(int) = *(void(***)(int))it->data;
            vtable[6](0);
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

/*
Function: BaseObject_Init
Address: 0x4189F0

MOV dword ptr [ECX],0x4314f8
PUSH EDI
LEA EDI,[ECX + 0x4]
MOV EDX,ECX
XOR EAX,EAX
MOV ECX,0x21
STOSD.REP ES:EDI
MOV EAX,EDX
POP EDI
RET
*/
BaseObject* BaseObject::BaseObject_Init() {
    *(void**)this = (void*)0x4314f8;
    int* p = (int*)&this->pad_4;
    for (int i = 0; i < 0x21; i++) {
        p[i] = 0;
    }
    return this;
}

/*
Function: BaseObject::PureVirtualCall
Address: 0x424540

PUSH 0x19
CALL 0x00424f90
ADD ESP,0x4
RET
*/
void BaseObject::PureVirtualCall() {
    __amsg_exit(0x19);
}

/*
Function: BaseObject::~BaseObject
Address: 0x401190

MOV EAX,FS:[0x0]
PUSH EBP
MOV EBP,ESP
PUSH -0x1
PUSH 0x4011e5
MOV dword ptr [ECX],0x431000
PUSH EAX
MOV dword ptr FS:[0x0],ESP
SUB ESP,0x4
MOV dword ptr [EBP + -0x4],0xffffffff
MOV dword ptr [EBP + -0x10],ECX
CALL 0x004011ef
TEST byte ptr [EBP + 0x8],0x1
JZ 0x42
MOV EAX,dword ptr [EBP + -0x10]
PUSH EAX
CALL 0x00424940
ADD ESP,0x4
MOV EAX,dword ptr [EBP + -0x10]
MOV ECX,dword ptr [EBP + -0xc]
MOV dword ptr FS:[0x0],ECX
MOV ESP,EBP
POP EBP
RET 0x4
*/
BaseObject::~BaseObject() {
    *(void**)this = (void*)0x431000;
    FUN_004011ef();
    // This is not correct, but it might generate closer assembly
    if ((int)this & 1) {
        FreeFromGlobalHeap(this);
    }
}

/*
Function: VFunc5
Address: 0x4171B0

MOV EDX,dword ptr [ESP + 0x4]
TEST EDX,EDX
JNZ 0x10
MOV EAX,0xffffffff
RET 0x4
MOV EAX,dword ptr [ECX + 0x88]
MOV dword ptr [EDX + 0x88],EAX
MOV EAX,dword ptr [ECX + 0x8c]
MOV dword ptr [EDX + 0x8c],EAX
MOV EAX,dword ptr [ECX + 0x88]
MOV dword ptr [EDX + 0x90],EAX
XOR EAX,EAX
MOV ECX,dword ptr [ECX + 0x8c]
MOV dword ptr [EDX + 0x98],0x0
MOV dword ptr [EDX + 0x94],ECX
RET 0x4
*/
int BaseObject::VFunc5(BaseObject* other) {
    if (other == 0) {
        return -1;
    }
    other->field_88 = this->field_88;
    other->field_8c = this->field_8c;
    other->field_90 = this->field_88;
    other->field_94 = this->field_8c;
    other->field_98 = 0;
    return 0;
}

/*
Function: BaseObject_CreateFromQueue
Address: 0x41C5A0

MOV EAX,FS:[0x0]
PUSH EBP
MOV EBP,ESP
PUSH -0x1
PUSH 0x41c942
PUSH EAX
MOV dword ptr [ECX + 0x4],0x0
MOV dword ptr FS:[0x0],ESP
MOV EAX,dword ptr [ECX + 0x98]
SUB ESP,0x18
MOV dword ptr [EBP + -0x10],ECX
CMP EAX,0x2
PUSH EBX
PUSH ESI
PUSH EDI
JZ 0x41
CMP EAX,0x3
JZ 0x194
JMP 0x392
MOV EAX,dword ptr [EBP + -0x10]
MOV ECX,dword ptr [EAX + 0xa8]
TEST ECX,ECX
JZ 0x7E
CALL 0x0041ebb0
TEST EAX,EAX
JNZ 0x71
CALL 0x00419390
PUSH 0x100
MOV EAX,dword ptr [EBP + -0x10]
PUSH 0x0
MOV ECX,dword ptr [EAX + 0xa8]
CALL 0x0041eb70
MOV EAX,dword ptr [EBP + -0x10]
MOV dword ptr [EAX + 0xa8],0x0
MOV EAX,[0x00436974]
XOR ESI,ESI
MOV ECX,dword ptr [EAX + 0x18]
MOV EAX,dword ptr [EAX + 0x14]
DEC ECX
DEC EAX
MOV dword ptr [EBP + -0x20],ESI
MOV dword ptr [EBP + -0x4],ESI
MOV dword ptr [EBP + -0x1c],ESI
MOV dword ptr [EBP + -0x18],EAX
MOV EAX,dword ptr [EBP + -0x10]
MOV dword ptr [EBP + -0x14],ECX
MOV ECX,dword ptr [EAX + 0xa0]
CMP dword ptr [ECX],ESI
JZ 0x120
MOV EAX,dword ptr [EBP + -0x10]
XOR EDI,EDI
MOV EBX,dword ptr [EAX + 0xa0]
MOV EAX,dword ptr [EBX + 0xc]
CMP EAX,0x1
JZ 0xDF
CMP EAX,0x4
JZ 0xDF
CMP EAX,0x2
JZ 0xDA
TEST EAX,EAX
JZ 0xDA
PUSH EAX
PUSH 0x4355f8
CALL 0x00419110
MOV EAX,dword ptr [EBX + 0x4]
JMP 0xE1
MOV EAX,dword ptr [EBX]
MOV dword ptr [EBX + 0x8],EAX
CMP dword ptr [EBX + 0x8],ESI
JZ 0xF2
MOV ECX,EBX
CALL 0x00401710
MOV EDI,EAX
TEST EDI,EDI
JZ 0x113
LEA EAX,[EBP + -0x20]
MOV EBX,dword ptr [EDI]
PUSH EAX
MOV ECX,EDI
CALL dword ptr [EBX]
TEST EDI,EDI
JZ 0x113
PUSH EDI
MOV dword ptr [EDI],0x431050
CALL 0x00424940
ADD ESP,0x4
MOV EAX,dword ptr [EBP + -0x10]
MOV ECX,dword ptr [EAX + 0xa0]
CMP dword ptr [ECX],ESI
JNZ 0xA9
MOV dword ptr [EBP + -0x4],0xffffffff
CALL 0x18C
MOV EAX,dword ptr [EBP + -0x10]
MOV EDI,dword ptr [EAX + 0xa4]
MOV EAX,dword ptr [EDI]
TEST EAX,EAX
JZ 0x165
MOV dword ptr [EDI + 0x8],EAX
CMP dword ptr [EDI],0x0
JZ 0x165
MOV ECX,EDI
CALL 0x00401790
MOV ESI,EAX
TEST ESI,ESI
JZ 0x160
MOV ECX,ESI
CALL 0x00401500
PUSH ESI
CALL 0x00424940
ADD ESP,0x4
CMP dword ptr [EDI],0x0
JNZ 0x143
MOV EAX,dword ptr [EBP + -0x10]
TEST byte ptr [EAX],0x2
JZ 0x392
LEA ECX,[EAX + 0x84]
CALL 0x00418f10
CMP EAX,0x7d0
JBE 0x392
JMP 0x38C
*/
void BaseObject::BaseObject_CreateFromQueue() {
    this->field_4 = 0;

    if (this->object_type == 2) {
        if (this->bounding_box_y1 != 0) {
            if (FUN_0041ebb0((void*)this->bounding_box_y1) == 0) {
                FUN_00419390();
                FUN_0041eb70((void*)this->bounding_box_y1, 0, 0x100);
            }
            this->bounding_box_y1 = 0;
        }

        int local_24 = 0;

        QueueNode* queue = (QueueNode*)this->bounding_box_x1;
        while (queue != 0) {
            ListNode* node = 0;
            if (queue->type == 1 || queue->type == 4) {
                //...
            } else if (queue->type == 2 || queue->type == 0) {
                //...
            } else {
                ShowError("bad queue type %lu", queue->type);
            }
            if (queue != 0) {
                node = (ListNode*)SC__ZBuffer__PopNode(queue);
            }
            if (node != 0) {
                void(**vtable)(int*) = *(void(***)(int*))node;
                (*vtable)(&local_24);
                if (node != 0) {
                    FreeFromGlobalHeap(node);
                }
            }
            queue = (QueueNode*)queue->field_0;
        }
        FUN_0041c72c();
        ListNode* queue2 = (ListNode*)this->bounding_box_x2;
        if (queue2 != 0) {
            while (queue2 != 0) {
                ListNode* node = (ListNode*)SC__ZBuffer__PopNode_2(queue2);
                if (node != 0) {
                    VBuffer_VBuffer_Owner___VBuffer_Owner(node);
                    FreeFromGlobalHeap(node);
                }
                queue2 = queue2->next;
            }
        }
    } else if (this->object_type == 3) {
        if (this->bounding_box_y1 != 0) {
            if (FUN_0041ebb0((void*)this->bounding_box_y1) == 0) {
                FUN_00419390();
                FUN_0041eb70((void*)this->bounding_box_y1, 0, 0x100);
            }
            this->bounding_box_y1 = 0;
        }
        QueueNode* queue = (QueueNode*)this->object_type;
        while (queue != 0) {
            ListNode* node = 0;
            if (queue->type == 1 || queue->type == 4) {
                //...
            } else if (queue->type == 2 || queue->type == 0) {
                //...
            } else {
                ShowError("bad queue type %lu", queue->type);
            }
            if (queue != 0) {
                // This is a manual implementation of PopNode
                // ...
            if (node != 0) {
                FUN_004189a0(node, 1);
            }
        }
        ListNode* queue2 = (ListNode*)this->bounding_box_x1;
        if (queue2 != 0) {
            for (ListNode* it = queue2; it != 0; it = it->next) {
                void(**vtable)(void*) = *(void(***)(void*))it->data;
                    (*vtable)((char*)node + 4);
                }
            }
            if (node != 0) {
                FUN_0041c94c();
                FreeFromGlobalHeap(node);
            }
            queue = (QueueNode*)queue->field_0;
        }
        ListNode* queue3 = (ListNode*)this->bounding_box_x2;
        if (queue3 != 0) {
            while (queue3 != 0) {
                ListNode* node = (ListNode*)SC__ZBuffer__PopNode_2(queue3);
                if (node != 0) {
                    VBuffer_VBuffer_Owner___VBuffer_Owner(node);
                    FreeFromGlobalHeap(node);
                }
                queue3 = queue3->next;
            }
        }
    }

    if ((this->field_4 & 2) != 0) {
        if (this->timer->Update() > 2000) {
            this->field_4 &= ~2;
        }
    }
}
