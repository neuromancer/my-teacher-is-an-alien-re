#include "Timer.h"
#include "string.h"

// Forward declarations for external functions and classes
extern "C" {
    void *TimedEvent__Create(void *param_1, void *param_2, int param_3);
    void TimedEvent__CopyConstructor(void *dest, void *src);
    void TimedEvent_dtor(void *event);
    void ShowError(const char *msg, ...);
    int _sprintf(char *buffer, const char *format, ...);
    void FUN_0041c000(void *soundManager, char *str, int, int, int);
    void FreeFromGlobalHeap(void *ptr);
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
void Timer::Update()
{
  this->field_c = timeGetTime();
  this->field_10 = (this->field_c - this->field_8) - this->field_0;
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
        for (int i = 0; i < 10; i++) {
            ((int*)this)[i] = 0;
        }
        this->timer.Reset();
    } catch(...) {
        // Matching SEH
    }
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

int TimedEvent::Update() {
    this->timer.Update();
    int time_remaining = this->field_c - this->timer.field_10;

    switch (this->field_4) {
        case 0:
            if (time_remaining > 0) {
                return 0;
            }
            if (this->field_10) {
                void* new_event = TimedEvent__Create(g_GameStruct2, g_GameStruct2[1], 0);
                TimedEvent__CopyConstructor((void*)((char*)new_event + 8), (void*)this->field_10);
                if (g_GameStruct2[1] == 0) {
                    *g_GameStruct2 = new_event;
                } else {
                    *(void**)g_GameStruct2[1] = new_event;
                }
                g_GameStruct2[1] = new_event;
            }
            if (this->field_10) {
                TimedEvent_dtor((void*)this->field_10);
                FreeFromGlobalHeap((void*)this->field_10);
                this->field_10 = 0;
            }
            return 1;
        case 1:
            if (time_remaining > 0) {
                return 0;
            }
            if (this->field_10) {
                void* new_event = TimedEvent__Create(g_GameStruct2, g_GameStruct2[1], 0);
                TimedEvent__CopyConstructor((void*)((char*)new_event + 8), (void*)this->field_10);
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
                void* new_event = TimedEvent__Create(g_GameStruct2, g_GameStruct2[1], 0);
                TimedEvent__CopyConstructor((void*)((char*)new_event + 8), (void*)this->field_10);
                if (g_GameStruct2[1] == 0) {
                    *g_GameStruct2 = new_event;
                } else {
                    *(void**)g_GameStruct2[1] = new_event;
                }
                g_GameStruct2[1] = new_event;
            }
            if (this->field_10) {
                TimedEvent_dtor((void*)this->field_10);
                FreeFromGlobalHeap((void*)this->field_10);
                this->field_10 = 0;
            }
            return 1;
        default:
            ShowError("illegal type %d in TimedEvent::Update\n", this->field_4);
            return 0; // Should not be reached
    }
}
