#include "Timer.h"
#include "string.h"

void Timer::Reset() {
    this->field_8 = timeGetTime();
    this->field_0 = 0;
}

// Forward declarations for external functions and classes
extern "C" {
    void *AllocateMemory_Wrapper(unsigned int size);
    int _sprintf(char *buffer, const char *format, ...);
    void FUN_0041c000(void *soundManager, char *str, int, int, int);
    void FreeFromGlobalHeap(void *ptr);
    void Message_Constructor(void* this_ptr, int, int, int, int, int, int, int, int, int, int);
}

TimedEvent::~TimedEvent() {
    // Dummy implementation
}

/*
Function: TimedEvent::CopyConstructor
Address: 0x402310

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
JC 0x00402325
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
JC 0x0040234b
MOV EAX,dword ptr [EDX + 0x80]
MOV dword ptr [ECX + 0x80],EAX
MOV EBX,dword ptr [EDX + 0x88]
MOV dword ptr [ECX + 0x88],EBX
MOV ESI,dword ptr [EDX + 0x8c]
MOV dword ptr [ECX + 0x8c],ESI
MOV EAX,dword ptr [EDX + 0x90]
MOV dword ptr [ECX + 0x90],EAX
MOV EBX,dword ptr [EDX + 0x94]
MOV dword ptr [ECX + 0x94],EBX
MOV ESI,dword ptr [EDX + 0x98]
MOV dword ptr [ECX + 0x98],ESI
MOV EAX,dword ptr [EDX + 0x9c]
MOV dword ptr [ECX + 0x9c],EAX
MOV EBX,dword ptr [EDX + 0xa0]
MOV dword ptr [ECX + 0xa0],EBX
MOV ESI,dword ptr [EDX + 0xa4]
MOV dword ptr [ECX + 0xa4],ESI
MOV EAX,dword ptr [EDX + 0xa8]
MOV dword ptr [ECX + 0xa8],EAX
MOV EBX,dword ptr [EDX + 0xac]
MOV dword ptr [ECX + 0xac],EBX
MOV ESI,dword ptr [EDX + 0xb0]
MOV dword ptr [ECX + 0xb0],ESI
MOV EAX,dword ptr [EDX + 0xb4]
POP EDI
POP ESI
MOV dword ptr [ECX + 0xb4],EAX
MOV EBX,dword ptr [EDX + 0xb8]
MOV EAX,ECX
MOV dword ptr [ECX + 0xb8],EBX
POP EBX
MOV EDX,dword ptr [EDX + 0xbc]
MOV dword ptr [ECX + 0xbc],EDX
RET 0x4
*/
TimedEvent::TimedEvent(const TimedEvent& other) {
    int i;
    this->field_8 = other.field_8;
    this->field_c = other.field_c;
    for (i = 0; i < sizeof(this->data_10); ++i) {
        this->data_10[i] = other.data_10[i];
    }
    this->field_30 = other.field_30;
    this->field_38 = other.field_38;
    this->field_3c = other.field_3c;
    for (i = 0; i < sizeof(this->data_40); ++i) {
        this->data_40[i] = other.data_40[i];
    }
    this->field_80 = other.field_80;
    this->field_88 = other.field_88;
    this->field_8c = other.field_8c;
    this->field_90 = other.field_90;
    this->field_94 = other.field_94;
    this->field_98 = other.field_98;
    this->field_9c = other.field_9c;
    this->field_a0 = other.field_a0;
    this->field_a4 = other.field_a4;
    this->field_a8 = other.field_a8;
    this->field_ac = other.field_ac;
    this->field_b0 = other.field_b0;
    this->field_b4 = other.field_b4;
    this->field_b8 = other.field_b8;
    this->field_bc = other.field_bc;
}

int DAT_00436684 = 0;

/*
Function: Timer::Init
Address: 0x418EB0

PUSH ESI
XOR EAX,EAX
MOV dword ptr [ECX],EAX
MOV ESI,ECX
MOV dword ptr [ECX + 0x4],EAX
MOV dword ptr [ESI + 0x8],EAX
MOV dword ptr [ESI + 0xc],EAX
MOV dword ptr [ESI + 0x10],EAX
INC dword ptr [0x00436684]
CALL 0x00418ef0
MOV EAX,ESI
POP ESI
RET
*/
Timer* Timer::Init() {
    this->field_0 = 0;
    this->field_4 = 0;
    this->field_8 = 0;
    this->field_c = 0;
    this->field_10 = 0;
    DAT_00436684++;
    this->Reset();
    return this;
}

/*
Function: Timer::Update
Address: 0x418F10

PUSH ESI
MOV ESI,ECX
CALL dword ptr [0x004404ac]
MOV dword ptr [ESI + 0xc],EAX
SUB EAX,dword ptr [ESI + 0x8]
SUB EAX,dword ptr [ESI]
MOV dword ptr [ESI + 0x10],EAX
POP ESI
RET
*/
int Timer::Update()
{
  this->field_c = timeGetTime();
  this->field_10 = (this->field_c - this->field_8) - this->field_0;
  return this->field_10;
}

/*
Function: TimedEvent::Init
Address: 0x401890

MOV EAX,FS:[0x0]
PUSH EBP
MOV EBP,ESP
PUSH -0x1
PUSH 0x4018f0
PUSH EAX
MOV dword ptr FS:[0x0],ESP
SUB ESP,0x4
PUSH EBX
PUSH ESI
LEA EBX,[ECX + 0x14]
MOV dword ptr [EBP + -0x10],ECX
PUSH EDI
MOV ESI,ECX
MOV ECX,EBX
MOV EDI,ESI
CALL 0x00418eb0
XOR EAX,EAX
MOV ECX,0xa
MOV dword ptr [EBP + -0x4],0x0
STOSD.REP ES:EDI
MOV ECX,EBX
CALL 0x00418ef0
MOV EAX,ESI
POP EDI
MOV dword ptr [EBP + -0x4],0xffffffff
MOV ECX,dword ptr [EBP + -0xc]
POP ESI
MOV dword ptr FS:[0x0],ECX
POP EBX
MOV ESP,EBP
POP EBP
RET
*/

void TimedEvent::Init() {
    try {
        this->timer.Init();
        this->field_0 = 0;
        this->field_4 = 0;
        this->field_8 = 0;
        this->field_c = 0;
        this->field_10 = 0;
        this->timer.Reset();
    } catch(...) {
        // Matching SEH
    }
}

/*
Function: TimedEvent::SetData
Address: 0x401990

MOV EAX,dword ptr [ESP + 0x4]
MOV dword ptr [ECX + 0x4],EAX
RET 0x4
*/
void TimedEvent::SetData(void* data) {
    this->field_4 = (int)data;
}

/*
Function: TimedEvent::Update
Address: 0x4019A0

PUSH EBX
PUSH ESI
LEA EBX,[ECX + 0x14]
PUSH EDI
MOV EDI,dword ptr [ECX + 0xc]
PUSH EBP
MOV ESI,ECX
MOV ECX,EBX
CALL 0x00418f10
SUB EDI,EAX
MOV EAX,dword ptr [ESI + 0x4]
TEST EAX,EAX
JZ 0x46
CMP EAX,0x1
JZ 0xAE
CMP EAX,0x2
JZ 0xFC
PUSH EAX
PUSH 0x4350ac
CALL 0x00419110
TEST EDI,EDI
JLE 0x51
XOR EAX,EAX
POP EBP
POP EDI
POP ESI
POP EBX
RET
MOV EBP,dword ptr [ESI + 0x10]
TEST EBP,EBP
JZ 0x86
MOV EBX,dword ptr [0x00436988]
PUSH 0x0
MOV ECX,EBX
MOV EAX,dword ptr [EBX + 0x4]
PUSH EAX
CALL 0x00402420
LEA ECX,[EAX + 0x8]
MOV EDI,EAX
PUSH EBP
CALL 0x00402310
MOV EAX,dword ptr [EBX + 0x4]
TEST EAX,EAX
JZ 0x81
MOV dword ptr [EAX],EDI
JMP 0x83
MOV dword ptr [EBX],EDI
MOV dword ptr [EBX + 0x4],EDI
MOV EDI,dword ptr [ESI + 0x10]
TEST EDI,EDI
JZ 0xA4
MOV ECX,EDI
CALL 0x004199a0
PUSH EDI
CALL 0x00424940
MOV dword ptr [ESI + 0x10],0x0
ADD ESP,0x4
MOV EAX,0x1
POP EBP
POP EDI
POP ESI
POP EBX
RET
TEST EDI,EDI
JLE 0xB9
XOR EAX,EAX
POP EBP
POP EDI
POP ESI
POP EBX
RET
MOV EBP,dword ptr [ESI + 0x10]
TEST EBP,EBP
JZ 0xEE
MOV EDI,dword ptr [0x00436988]
PUSH 0x0
MOV ECX,EDI
MOV EAX,dword ptr [EDI + 0x4]
PUSH EAX
CALL 0x00402420
LEA ECX,[EAX + 0x8]
MOV ESI,EAX
PUSH EBP
CALL 0x00402310
MOV EAX,dword ptr [EDI + 0x4]
TEST EAX,EAX
JZ 0xE9
MOV dword ptr [EAX],ESI
JMP 0xEB
MOV dword ptr [EDI],ESI
MOV dword ptr [EDI + 0x4],ESI
MOV ECX,EBX
CALL 0x00418ef0
XOR EAX,EAX
POP EBP
POP EDI
POP ESI
POP EBX
RET
MOV ECX,0x3e8
MOV EAX,EDI
MOV EBX,0x3c
MOV EBP,0xea60
CDQ
IDIV ECX
CDQ
IDIV EBX
PUSH EDX
MOV EAX,EDI
CDQ
IDIV EBP
PUSH EAX
PUSH 0x4350d0
MOV EAX,[0x00436960]
PUSH EAX
CALL 0x00424950
ADD ESP,0x10
MOV EAX,[0x00436960]
MOV ECX,dword ptr [0x0043698c]
PUSH 0x8
PUSH 0x2710
PUSH 0x1c2
PUSH 0x208
PUSH EAX
CALL 0x0041c000
TEST EDI,EDI
JLE 0x15A
XOR EAX,EAX
POP EBP
POP EDI
POP ESI
POP EBX
RET
MOV EBP,dword ptr [ESI + 0x10]
TEST EBP,EBP
JZ 0x18F
MOV EBX,dword ptr [0x00436988]
PUSH 0x0
MOV ECX,EBX
MOV EAX,dword ptr [EBX + 0x4]
PUSH EAX
CALL 0x00402420
LEA ECX,[EAX + 0x8]
MOV EDI,EAX
PUSH EBP
CALL 0x00402310
MOV EAX,dword ptr [EBX + 0x4]
TEST EAX,EAX
JZ 0x18A
MOV dword ptr [EAX],EDI
JMP 0x18C
MOV dword ptr [EBX],EDI
MOV dword ptr [EBX + 0x4],EDI
MOV EDI,dword ptr [ESI + 0x10]
TEST EDI,EDI
JZ 0x1AD
MOV ECX,EDI
CALL 0x004199a0
PUSH EDI
CALL 0x00424940
MOV dword ptr [ESI + 0x10],0x0
ADD ESP,0x4
MOV EAX,0x1
POP EBP
POP EDI
POP ESI
POP EBX
RET
*/

extern void *g_GameStruct2[];
extern char DAT_00436960[];
extern void *g_SoundManager;

/*
Function: TimedEvent::Create
Address: 0x402420

PUSH EBX
PUSH ESI
PUSH EDI
MOV EDI,ECX
CMP dword ptr [ECX + 0xc],0x0
JNZ 0x50
MOV EAX,dword ptr [EDI + 0x14]
LEA ECX,[EAX + EAX*0x4]
LEA EAX,[ECX + ECX*0x4]
SHL EAX,0x3
ADD EAX,0x4
PUSH EAX
CALL 0x004249c0
ADD ESP,0x4
MOV ECX,dword ptr [EDI + 0x10]
MOV dword ptr [EAX],ECX
MOV EDX,dword ptr [EDI + 0x14]
MOV dword ptr [EDI + 0x10],EAX
LEA EBX,[EDX + EDX*0x4]
DEC EDX
LEA ECX,[EBX + EBX*0x4]
LEA EAX,[EAX + ECX*0x8 + 0xffffff3c]
JS 0x50
MOV ECX,dword ptr [EDI + 0xc]
DEC EDX
MOV dword ptr [EAX],ECX
MOV dword ptr [EDI + 0xc],EAX
SUB EAX,0xc8
TEST EDX,EDX
JGE 0x3E
MOV ESI,dword ptr [EDI + 0xc]
MOV ECX,dword ptr [ESP + 0x10]
MOV EDX,dword ptr [ESP + 0x14]
MOV EAX,dword ptr [ESI]
LEA EBX,[ESI + 0x8]
MOV dword ptr [EDI + 0xc],EAX
XOR EAX,EAX
MOV dword ptr [ESI + 0x4],ECX
MOV ECX,0x30
MOV dword ptr [ESI],EDX
INC dword ptr [EDI + 0x8]
MOV EDI,EBX
STOSD.REP ES:EDI
XOR EDI,EDI
TEST EBX,EBX
JZ 0x97
PUSH 0x0
MOV ECX,EBX
PUSH 0x0
PUSH 0x0
PUSH 0x0
PUSH 0x0
PUSH 0x0
PUSH 0x0
PUSH 0x0
PUSH 0x0
PUSH 0x0
CALL 0x004198c0
ADD EBX,0xc0
MOV EAX,EDI
DEC EDI
TEST EAX,EAX
JNZ 0x78
MOV EAX,ESI
POP EDI
POP ESI
POP EBX
RET 0x8
*/
TimedEvent* TimedEvent::Create(void* callback, void* data) {
    if (this->field_c == 0) {
        int* pool = (int*)AllocateMemory_Wrapper(this->field_10 * 200 + 4);
        *pool = this->field_10;
        this->field_10 = (int)pool;
        for (int i = this->field_10 - 1; i >= 0; i--) {
            pool = (int*)((char*)pool + 200);
            *pool = this->field_c;
            this->field_c = (int)pool;
        }
    }

    TimedEvent* new_event = (TimedEvent*)this->field_c;
    this->field_c = *(int*)new_event;
    new_event->field_4 = (int)callback;
    new_event->field_0 = (int)data;
    this->field_8++;

    int* current = (int*)(new_event + 2);
    for (int i = 0; i < 0x30; i++) {
        current[i] = 0;
    }

    for (int j = 0; j < 1; j++) { // This loop seems to run only once
        if (current) {
            Message_Constructor(current, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        }
        current = (int*)((char*)current + 0xc0);
    }

    return new_event;
}

int TimedEvent::Update() {
    int time_remaining = this->field_c - this->timer.Update();

    switch (this->field_4) {
        case 0:
            if (time_remaining > 0) {
                return 0;
            }
            if (this->field_10) {
                TimedEvent* new_event = ((TimedEvent*)g_GameStruct2)->Create(g_GameStruct2[1], 0);
                *new_event = *(TimedEvent*)this->field_10;
                if (g_GameStruct2[1] == 0) {
                    *g_GameStruct2 = new_event;
                } else {
                    *(void**)g_GameStruct2[1] = new_event;
                }
                g_GameStruct2[1] = new_event;
            }
            if (this->field_10) {
                ((TimedEvent*)this->field_10)->~TimedEvent();
                FreeFromGlobalHeap((void*)this->field_10);
                this->field_10 = 0;
            }
            return 1;
        case 1:
            if (time_remaining > 0) {
                return 0;
            }
            if (this->field_10) {
                TimedEvent* new_event = ((TimedEvent*)g_GameStruct2)->Create(g_GameStruct2[1], 0);
                *new_event = *(TimedEvent*)this->field_10;
                if (g_GameStruct2[1] == 0) {
                    *g_GameStruct2 = new_event;
                } else {
                    *(void**)g_GameStruct2[1] = new_event;
                }
                g_GameStruct2[1] = new_event;
            }
            this->timer.Reset();
            return 0;
        case 2:
            _sprintf(DAT_00436960, "%3.3d:%2.2d", time_remaining / 60000, (time_remaining / 1000) % 60);
            FUN_0041c000(g_SoundManager, DAT_00436960, 0x208, 0x1c2, 10000);
            if (time_remaining > 0) {
                return 0;
            }
            if (this->field_10) {
                TimedEvent* new_event = ((TimedEvent*)g_GameStruct2)->Create(g_GameStruct2[1], 0);
                *new_event = *(TimedEvent*)this->field_10;
                if (g_GameStruct2[1] == 0) {
                    *g_GameStruct2 = new_event;
                } else {
                    *(void**)g_GameStruct2[1] = new_event;
                }
                g_GameStruct2[1] = new_event;
            }
            if (this->field_10) {
                ((TimedEvent*)this->field_10)->~TimedEvent();
                FreeFromGlobalHeap((void*)this->field_10);
                this->field_10 = 0;
            }
            return 1;
        default:
            ShowError("illegal type %d in TimedEvent::Update\n", this->field_4);
            return 0; // Should not be reached
    }
}

/*
Function: TimeOut_Start
Address: 0x419020

MOV EAX,dword ptr [ESP + 0x4]
PUSH ESI
MOV ESI,ECX
TEST EAX,EAX
JZ 0xE
MOV dword ptr [ESI + 0x4],EAX
CMP dword ptr [ESI + 0x4],0x0
JNZ 0x21
PUSH 0x436688
CALL 0x00419110
MOV ECX,dword ptr [ESI + 0x8]
MOV dword ptr [ESI],0x1
CALL 0x00418ef0
POP ESI
RET 0x4
*/
void TimeOut::Start(int timeout_val)
{
    if (timeout_val != 0) {
        this->timeout = timeout_val;
    }
    if (this->timeout == 0) {
        ShowError("TimeOut::Start() - Must Set a time value greater than 0");
    }
    this->active = 1;
    this->timer->Reset();
}
