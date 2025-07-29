#include "SCTimer.h"
#include "string.h"
#include "Timer.h"

// Forward declarations for external functions and classes
extern "C" {
    void *AllocateMemory_Wrapper(unsigned int size);
    void Queue__Insert(void *queue, int event);
    void Queue__Push(void *queue, int event);
    void *Queue__Pop(void *queue);
    void Queue___Queue(void *queue, int);
    void FreeFromGlobalHeap(void *ptr);
    void FUN_00419fd0(void *param_1);
    void SC_Message_Send(int, int, int, int, int, int, int, int, int, int);
    void SC__Timer__Timer1_dtor();
    void SC__Timer__Timer2_dtor();
    void SC__Timer__Base_dtor();

    // These are the vtable pointers
    extern void *PTR_LBLParse_MustBeDefined_00431000;
    extern void *PTR_LBLParse_MustBeDefined_00431060;
}

/*
Function: Update
Address: 0x401E30

PUSH EBX
PUSH ESI
PUSH EDI
MOV ESI,ECX
PUSH EBP
LEA EDI,[ESI + 0xa0]
MOV ECX,EDI
CALL 0x00418f10
CMP EAX,0x2710
JBE 0x4A
MOV EAX,dword ptr [ESI + 0xc8]
CMP dword ptr [EAX],0x0
JNZ 0x4A
PUSH 0x0
MOV EAX,dword ptr [ESI + 0x8c]
PUSH 0x0
MOV ECX,dword ptr [ESI + 0x88]
PUSH 0x0
PUSH 0x0
PUSH 0x0
PUSH 0x14
PUSH EAX
PUSH ECX
PUSH ECX
PUSH 0x3
CALL 0x0041a150
ADD ESP,0x28
MOV ECX,EDI
CALL 0x00418ef0
MOV EAX,dword ptr [ESI + 0xc8]
MOV ECX,dword ptr [EAX]
MOV dword ptr [EAX + 0x8],ECX
MOV EAX,dword ptr [ESI + 0xc8]
CMP dword ptr [EAX],0x0
JZ 0x139
XOR EDI,EDI
MOV EAX,dword ptr [ESI + 0xc8]
MOV ECX,0x0
MOV EAX,dword ptr [EAX + 0x8]
TEST EAX,EAX
JZ 0x82
MOV ECX,dword ptr [EAX + 0x8]
CALL 0x004019a0
TEST EAX,EAX
JZ 0x113
MOV EBX,dword ptr [ESI + 0xc8]
MOV EAX,dword ptr [EBX + 0x8]
TEST EAX,EAX
JNZ 0xA0
XOR EBP,EBP
JMP 0xFF
CMP dword ptr [EBX],EAX
JNZ 0xA9
MOV EAX,dword ptr [EAX + 0x4]
MOV dword ptr [EBX],EAX
MOV EAX,dword ptr [EBX + 0x8]
CMP dword ptr [EBX + 0x4],EAX
JNZ 0xB6
MOV EAX,dword ptr [EAX]
MOV dword ptr [EBX + 0x4],EAX
MOV EAX,dword ptr [EBX + 0x8]
MOV ECX,dword ptr [EAX]
TEST ECX,ECX
JZ 0xC5
MOV EAX,dword ptr [EAX + 0x4]
MOV dword ptr [ECX + 0x4],EAX
MOV EAX,dword ptr [EBX + 0x8]
MOV ECX,dword ptr [EAX + 0x4]
TEST ECX,ECX
JZ 0xD3
MOV EAX,dword ptr [EAX]
MOV dword ptr [ECX],EAX
MOV EAX,dword ptr [EBX + 0x8]
MOV EBP,0x0
TEST EAX,EAX
JZ 0xE2
MOV EBP,dword ptr [EAX + 0x8]
TEST EAX,EAX
JZ 0xFA
PUSH EAX
MOV dword ptr [EAX + 0x8],EDI
MOV dword ptr [EAX],EDI
MOV dword ptr [EAX + 0x4],EDI
CALL 0x00424940
ADD ESP,0x4
MOV dword ptr [EBX + 0x8],EDI
MOV EAX,dword ptr [EBX]
MOV dword ptr [EBX + 0x8],EAX
TEST EBP,EBP
JZ 0x113
MOV ECX,EBP
CALL 0x00401910
PUSH EBP
CALL 0x00424940
ADD ESP,0x4
MOV ECX,dword ptr [ESI + 0xc8]
MOV EAX,dword ptr [ECX + 0x8]
CMP dword ptr [ECX + 0x4],EAX
JZ 0x139
TEST EAX,EAX
JZ 0x12B
MOV EAX,dword ptr [EAX + 0x4]
MOV dword ptr [ECX + 0x8],EAX
MOV EAX,dword ptr [ESI + 0xc8]
CMP dword ptr [EAX],EDI
JNZ 0x6D
MOV EAX,dword ptr [ESP + 0x18]
CMP dword ptr [ESI + 0x88],EAX
JNZ 0x152
PUSH 0x4350e0
CALL 0x00419110
POP EBP
POP EDI
POP ESI
POP EBX
RET 0x8
*/

void SCTimer::Update(int param_1, int param_2) {
    if (this->timer1.Update() > 10000 && *(int*)this->field_0xc8 == 0) {
        SC_Message_Send(3, this->field_0x88[0], this->field_0x88[0], this->field_0x88[1], 0x14, 0, 0, 0, 0, 0);
    }
    this->timer1.Reset();

    Queue* queue = (Queue*)this->field_0xc8;
    queue->current = queue->head;

    while(queue->head) {
        TimedEvent* event = 0;
        if (queue->current) {
            event = *(TimedEvent**)((char*)queue->current + 8);
        }

        if (event->Update()) {
            void* popped = Queue__Pop(queue);
            ((TimedEvent*)popped)->~TimedEvent();
            FreeFromGlobalHeap(popped);
        }

        if (queue->tail == queue->current) {
            break;
        }

        if (queue->current) {
            queue->current = *(void**)((char*)queue->current + 4);
        }
    }

    if (this->field_0x88[0] == param_2) {
        ShowError("SC_Timer::Update");
    }
}

/*
Function: SC::Timer::Init
Address: 0x401B60

MOV EAX,FS:[0x0]
PUSH EBP
MOV EBP,ESP
PUSH -0x1
PUSH 0x401c6c
PUSH EAX
MOV dword ptr FS:[0x0],ESP
SUB ESP,0x8
PUSH ESI
PUSH EDI
LEA EDI,[ECX + 0x88]
MOV dword ptr [EBP + -0x10],ECX
MOV ESI,ECX
CALL 0x004189f0
XOR EAX,EAX
MOV ECX,0x6
MOV dword ptr [EBP + -0x4],0x0
MOV dword ptr [ESI],0x431000
STOSD.REP ES:EDI
MOV dword ptr [EBP + -0x4],0x1
LEA EDI,[ESI + 0xa0]
MOV ECX,EDI
CALL 0x00418eb0
MOV byte ptr [EBP + -0x4],0x2
LEA ECX,[ESI + 0xb4]
CALL 0x00418eb0
MOV byte ptr [EBP + -0x4],0x3
MOV ECX,EDI
MOV dword ptr [ESI],0x431060
MOV dword ptr [ESI + 0x88],0xd
CALL 0x00418ef0
PUSH 0x10
CALL 0x004249c0
MOV dword ptr [EBP + -0x14],EAX
ADD ESP,0x4
MOV byte ptr [EBP + -0x4],0x4
MOV EDI,EAX
MOV EAX,0x0
TEST EDI,EDI
JZ 0xB8
MOV dword ptr [EDI + 0xc],0x0
MOV dword ptr [EDI],0x0
MOV dword ptr [EDI + 0x4],0x0
MOV EAX,dword ptr [EDI]
MOV dword ptr [EDI + 0x8],EAX
MOV EAX,EDI
MOV byte ptr [EBP + -0x4],0x3
POP EDI
MOV dword ptr [ESI + 0xc8],EAX
MOV dword ptr [EBP + -0x4],0xffffffff
MOV EAX,ESI
MOV ECX,dword ptr [EBP + -0xc]
MOV dword ptr FS:[0x0],ECX
POP ESI
MOV ESP,EBP
POP EBP
RET
*/

void SCTimer::Init() {
    try {
        BaseObject_Init();
        *(void**)this = &PTR_LBLParse_MustBeDefined_00431000;
        for (int i = 6; i > 0; i--) {
            field_0x88[i-1] = 0;
        }
        timer1.Init();
        timer2.Init();
        *(void**)this = &PTR_LBLParse_MustBeDefined_00431060;
        field_0x88[0] = 0xd;
        timer1.Reset();

        void** memory = (void**)AllocateMemory_Wrapper(0x10);
        if (memory) {
            memory[3] = 0;
            *memory = 0;
            memory[1] = 0;
            memory[2] = *memory;
        }
        field_0xc8 = memory;
    } catch(...) {
        // Matching SEH
    }
}

/*
Function: Input
Address: 0x401FB0

MOV EAX,FS:[0x0]
PUSH EBP
MOV EBP,ESP
PUSH -0x1
PUSH 0x40227e
PUSH EAX
MOV dword ptr FS:[0x0],ESP
SUB ESP,0x4
PUSH EBX
PUSH ESI
MOV EAX,dword ptr [ECX + 0x88]
PUSH EDI
MOV ESI,ECX
MOV EBX,dword ptr [EBP + 0x8]
CMP dword ptr [EBX + 0x88],EAX
JZ 0x38
XOR EAX,EAX
JMP 0x31A
LEA ECX,[ESI + 0xa0]
CALL 0x00418ef0
MOV EAX,dword ptr [EBX + 0x98]
SUB EAX,0xe
CMP EAX,0xd
JA 0x60
XOR ECX,ECX
MOV CL,byte ptr [EAX + 0x4022f8]
JMP dword ptr [ECX*0x4 + 0x4022e0]
XOR EAX,EAX
JMP 0x31A
MOV ESI,dword ptr [ESI + 0xc8]
MOV EAX,dword ptr [ESI]
TEST EAX,EAX
JZ 0x315
MOV dword ptr [ESI + 0x8],EAX
CMP dword ptr [ESI],0x0
JZ 0x315
MOV EAX,dword ptr [ESI + 0x8]
TEST EAX,EAX
JNZ 0x8E
XOR EDI,EDI
JMP 0xE7
CMP dword ptr [ESI],EAX
JNZ 0x97
MOV EAX,dword ptr [EAX + 0x4]
MOV dword ptr [ESI],EAX
MOV EAX,dword ptr [ESI + 0x8]
CMP dword ptr [ESI + 0x4],EAX
JNZ 0xA4
MOV EAX,dword ptr [EAX]
MOV dword ptr [ESI + 0x4],EAX
MOV EAX,dword ptr [ESI + 0x8]
MOV ECX,dword ptr [EAX]
TEST ECX,ECX
JZ 0xB3
MOV EAX,dword ptr [EAX + 0x4]
MOV dword ptr [ECX + 0x4],EAX
MOV EAX,dword ptr [ESI + 0x8]
MOV ECX,dword ptr [EAX + 0x4]
TEST ECX,ECX
JZ 0xC1
MOV EAX,dword ptr [EAX]
MOV dword ptr [ECX],EAX
MOV ECX,dword ptr [ESI + 0x8]
MOV EDI,0x0
TEST ECX,ECX
JZ 0xD0
MOV EDI,dword ptr [ECX + 0x8]
TEST ECX,ECX
JZ 0xE2
PUSH 0x1
CALL 0x00402700
MOV dword ptr [ESI + 0x8],0x0
MOV EAX,dword ptr [ESI]
MOV dword ptr [ESI + 0x8],EAX
TEST EDI,EDI
JZ 0xFB
MOV ECX,EDI
CALL 0x00401910
PUSH EDI
CALL 0x00424940
ADD ESP,0x4
CMP dword ptr [ESI],0x0
JNZ 0x83
JMP 0x315
CMP dword ptr [EBX + 0xbc],0x0
JZ 0x208
PUSH 0x28
MOV EDI,0x0
CALL 0x004249c0
ADD ESP,0x4
MOV dword ptr [EBP + -0x10],EAX
MOV dword ptr [EBP + -0x4],0x0
TEST EAX,EAX
JZ 0x138
MOV ECX,EAX
CALL 0x00401890
MOV EDI,EAX
MOV dword ptr [EBP + -0x4],0xffffffff
MOV EAX,dword ptr [EBX + 0xb8]
MOV dword ptr [EDI + 0xc],EAX
MOV ECX,dword ptr [EBX + 0x8c]
MOV dword ptr [EDI + 0x8],ECX
MOV ECX,EDI
MOV EDX,dword ptr [EBX + 0xbc]
MOV dword ptr [EDI + 0x10],EDX
MOV dword ptr [EBX + 0xbc],0x0
MOV EAX,dword ptr [EBX + 0x9c]
PUSH EAX
CALL 0x00401990
MOV ESI,dword ptr [ESI + 0xc8]
TEST EDI,EDI
JNZ 0x189
PUSH 0x435120
CALL 0x00419110
MOV ECX,dword ptr [ESI]
MOV dword ptr [ESI + 0x8],ECX
MOV EAX,dword ptr [ESI + 0xc]
CMP EAX,0x1
JZ 0x1A8
CMP EAX,0x2
JZ 0x1A8
PUSH EDI
MOV ECX,ESI
CALL 0x004024d0
JMP 0x315
TEST ECX,ECX
JNZ 0x1B9
PUSH EDI
MOV ECX,ESI
CALL 0x004024d0
JMP 0x315
MOV EAX,dword ptr [ESI + 0x8]
MOV EDX,dword ptr [EDI + 0xc]
MOV ECX,dword ptr [EAX + 0x8]
CMP dword ptr [ECX + 0xc],EDX
JC 0x00402191
CMP dword ptr [ESI + 0x4],EAX
JZ 0x1EE
TEST EAX,EAX
JZ 0x1D6
MOV EAX,dword ptr [EAX + 0x4]
MOV dword ptr [ESI + 0x8],EAX
CMP dword ptr [ESI + 0x8],0x0
JNZ 0x1B9
JMP 0x315
PUSH EDI
MOV ECX,ESI
CALL 0x004024d0
JMP 0x315
PUSH EDI
MOV ECX,ESI
CALL 0x004025a0
JMP 0x315
PUSH 0x0
MOV ECX,EBX
CALL 0x00419fd0
PUSH 0x435134
CALL 0x00419110
PUSH 0x28
MOV EDI,0x0
CALL 0x004249c0
ADD ESP,0x4
MOV dword ptr [EBP + -0x10],EAX
MOV dword ptr [EBP + -0x4],0x1
TEST EAX,EAX
JZ 0x249
MOV ECX,EAX
CALL 0x00401890
MOV EDI,EAX
MOV dword ptr [EBP + -0x4],0xffffffff
MOV EAX,dword ptr [EBX + 0x8c]
TEST EDI,EDI
MOV dword ptr [EDI + 0x8],EAX
MOV EBX,dword ptr [ESI + 0xc8]
JNZ 0x270
PUSH 0x43510c
CALL 0x00419110
MOV EAX,dword ptr [EBX]
MOV dword ptr [EBX + 0x8],EAX
TEST EAX,EAX
JZ 0x315
MOV ECX,dword ptr [EBX + 0x8]
MOV EAX,0x0
TEST ECX,ECX
JZ 0x28C
MOV EAX,dword ptr [ECX + 0x8]
MOV EDX,dword ptr [EDI + 0xc]
CMP dword ptr [EAX + 0xc],EDX
JZ 0x2AB
CMP dword ptr [EBX + 0x4],ECX
JZ 0x315
TEST ECX,ECX
JZ 0x2A3
MOV EAX,dword ptr [ECX + 0x4]
MOV dword ptr [EBX + 0x8],EAX
CMP dword ptr [EBX + 0x8],0x0
JNZ 0x27D
JMP 0x315
MOV ECX,dword ptr [ESI + 0xc8]
CALL 0x00402680
MOV ESI,EAX
TEST ESI,ESI
JZ 0x315
MOV ECX,ESI
CALL 0x00401910
PUSH ESI
CALL 0x00424940
ADD ESP,0x4
JMP 0x315
*/

int SCTimer::Input(void *message) {
    try {
        Message* msg = (Message*)message;
        if (field_0x88[0] != msg->field_0x88) {
            return 0;
        }

        timer1.Reset();

        switch (msg->field_0x98) {
            case 0xe:
                break;
            case 0xf: {
                Queue* queue = (Queue*)field_0xc8;
                if (queue->head) {
                    do {
                        void* event = Queue__Pop(queue);
                        if (event) {
                            ((TimedEvent*)event)->~TimedEvent();
                            FreeFromGlobalHeap(event);
                        }
                    } while (queue->head);
                }
                break;
            }
            case 0x13: {
                if (msg->field_0xbc == 0) {
                    FUN_00419fd0(message);
                    ShowError("SC_Timer::Input");
                }
                TimedEvent* event = (TimedEvent*)AllocateMemory_Wrapper(0x28);
                if (event) {
                    event->Init();
                    event->field_c = msg->field_0xb8;
                    event->field_8 = msg->field_0x8c;
                    event->field_10 = msg->field_0xbc;
                    msg->field_0xbc = 0;
                    event->SetData(msg->field_0x9c);
                    Queue* queue = (Queue*)field_0xc8;
                    if (!event) {
                        ShowError("queue fault 0101");
                    }

                    if (queue->mode == 1 || queue->mode == 2) {
                        if (queue->head == 0) {
                            Queue__Insert(queue, (int)event);
                        } else {
                            queue->current = queue->head;
                            do {
                                TimedEvent* current_event = (TimedEvent*)queue->current;
                                if (current_event->field_c < event->field_c) {
                                    Queue__Insert(queue, (int)event);
                                    break;
                                }
                                if (queue->tail == queue->current) {
                                    Queue__Push(queue, (int)event);
                                    break;
                                }
                                queue->current = *(void**)((char*)queue->current + 4);
                            } while (queue->current);
                        }
                    } else {
                        Queue__Insert(queue, (int)event);
                    }
                }
                break;
            }
            case 0x14: {
                TimedEvent* event = (TimedEvent*)AllocateMemory_Wrapper(0x28);
                if (event) {
                    event->Init();
                    event->field_8 = msg->field_0x8c;
                    Queue* queue = (Queue*)field_0xc8;
                    if (!event) {
                        ShowError("queue fault 0103");
                    }

                    queue->current = queue->head;
                    while(queue->current) {
                        TimedEvent* current_event = *(TimedEvent**)((char*)queue->current + 8);
                        if (current_event->field_c == event->field_c) {
                            void* popped = Queue__Pop(queue);
                            if (popped) {
                                ((TimedEvent*)popped)->~TimedEvent();
                                FreeFromGlobalHeap(popped);
                            }
                            break;
                        }
                        if (queue->tail == queue->current) {
                            break;
                        }
                        queue->current = *(void**)((char*)queue->current + 4);
                    }
                }
                break;
            }
            case 0x1b: {
                if (*(void**)field_0xc8 == 0) {
                    SC_Message_Send(3, field_0x88[0], field_0x88[0], field_0x88[1], 0x14, 0, 0, 0, 0, 0);
                }
                break;
            }
            default:
                return 0;
        }

        return 1;
    } catch (...) {
        return 0;
    }
}

/*
Function: ~Timer
Address: 0x401C80

PUSH ESI
MOV ESI,ECX
CALL 0x00401ca0
TEST byte ptr [ESP + 0x8],0x1
JZ 0x18
PUSH ESI
CALL 0x00424940
ADD ESP,0x4
MOV EAX,ESI
POP ESI
RET 0x4
*/

SCTimer::~SCTimer() {
    Timer_impl_dtor(this);
}

/*
Function: ~Timer_impl
Address: 0x401CA0

MOV EAX,FS:[0x0]
PUSH EBP
MOV EBP,ESP
PUSH -0x1
PUSH 0x401dd9
PUSH EAX
MOV dword ptr FS:[0x0],ESP
MOV dword ptr [ECX],0x431060
SUB ESP,0x4
MOV dword ptr [EBP + -0x10],ECX
PUSH EBX
PUSH ESI
PUSH EDI
XOR ESI,ESI
MOV dword ptr [EBP + -0x4],ESI
MOV byte ptr [EBP + -0x4],0x2
MOV EDI,dword ptr [ECX + 0xc8]
CMP EDI,ESI
JZ 0xEF
MOV EAX,dword ptr [EDI]
TEST EAX,EAX
JZ 0xD9
MOV dword ptr [EDI + 0x8],EAX
CMP dword ptr [EDI],ESI
JZ 0xD9
MOV EAX,dword ptr [EDI + 0x8]
TEST EAX,EAX
JNZ 0x5E
XOR EBX,EBX
JMP 0xBD
CMP dword ptr [EDI],EAX
JNZ 0x67
MOV EAX,dword ptr [EAX + 0x4]
MOV dword ptr [EDI],EAX
MOV EAX,dword ptr [EDI + 0x8]
CMP dword ptr [EDI + 0x4],EAX
JNZ 0x74
MOV EAX,dword ptr [EAX]
MOV dword ptr [EDI + 0x4],EAX
MOV EAX,dword ptr [EDI + 0x8]
MOV ECX,dword ptr [EAX]
TEST ECX,ECX
JZ 0x83
MOV EAX,dword ptr [EAX + 0x4]
MOV dword ptr [ECX + 0x4],EAX
MOV EAX,dword ptr [EDI + 0x8]
MOV ECX,dword ptr [EAX + 0x4]
TEST ECX,ECX
JZ 0x91
MOV EAX,dword ptr [EAX]
MOV dword ptr [ECX],EAX
MOV EAX,dword ptr [EDI + 0x8]
MOV EBX,0x0
TEST EAX,EAX
JZ 0xA0
MOV EBX,dword ptr [EAX + 0x8]
TEST EAX,EAX
JZ 0xB8
PUSH EAX
MOV dword ptr [EAX + 0x8],ESI
MOV dword ptr [EAX],ESI
MOV dword ptr [EAX + 0x4],ESI
CALL 0x00424940
ADD ESP,0x4
MOV dword ptr [EDI + 0x8],ESI
MOV EAX,dword ptr [EDI]
MOV dword ptr [EDI + 0x8],EAX
TEST EBX,EBX
JZ 0xD1
MOV ECX,EBX
CALL 0x00401910
PUSH EBX
CALL 0x00424940
ADD ESP,0x4
CMP dword ptr [EDI],ESI
JNZ 0x53
PUSH EDI
CALL 0x00424940
ADD ESP,0x4
MOV EAX,dword ptr [EBP + -0x10]
MOV dword ptr [EAX + 0xc8],0x0
MOV byte ptr [EBP + -0x4],0x1
CALL 0x00401dbd
MOV byte ptr [EBP + -0x4],0x0
CALL 0x00401dcb
MOV dword ptr [EBP + -0x4],0xffffffff
CALL 0x00401de3
MOV EAX,dword ptr [EBP + -0xc]
POP EDI
MOV FS:[0x0],EAX
POP ESI
POP EBX
MOV ESP,EBP
POP EBP
RET
*/

void Timer_impl_dtor(void* timer) {
    try {
        SCTimer* self = (SCTimer*)timer;
        *(void**)self = &PTR_LBLParse_MustBeDefined_00431060;
        Queue* queue = (Queue*)self->field_0xc8;
        if (queue) {
            while (queue->head) {
                void* event = Queue__Pop(queue);
                if (event) {
                    ((TimedEvent*)event)->~TimedEvent();
                    FreeFromGlobalHeap(event);
                }
            }
            FreeFromGlobalHeap(queue);
            self->field_0xc8 = 0;
        }
        SC__Timer__Timer1_dtor();
        SC__Timer__Timer2_dtor();
        SC__Timer__Base_dtor();
    } catch (...) {
    }
}
