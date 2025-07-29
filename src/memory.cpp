#include <windows.h>

extern "C" {
    void* AllocateMemory(size_t size, int flag);
    void* AllocateMemoryFromHeap(size_t size);
    int CallOutOfMemoryHandler(size_t size);
}

extern "C" {
    HANDLE DAT_0043eff0;
    int (*DAT_0043dfa8)(size_t);
}

/*
Function: AllocateMemory_Wrapper
Address: 0x4249C0

MOV EAX,dword ptr [ESP + 0x4]
PUSH 0x1
PUSH EAX
CALL 0x00428460
ADD ESP,0x8
RET
*/
void* __cdecl AllocateMemory_Wrapper(unsigned int param_1)
{
  return AllocateMemory(param_1,1);
}

/*
Function: AllocateMemory
Address: 0x428460

PUSH ESI
PUSH EDI
MOV ESI,dword ptr [ESP + 0xc]
CMP ESI,-0x20
JBE 0x10
XOR EAX,EAX
POP EDI
POP ESI
RET
TEST ESI,ESI
JNZ 0x19
MOV ESI,0x1
MOV EDI,dword ptr [ESP + 0x10]
PUSH ESI
CALL 0x004284a0
ADD ESP,0x4
TEST EAX,EAX
JNZ 0x3D
TEST EDI,EDI
JZ 0x3D
PUSH ESI
CALL 0x0042c5c0
ADD ESP,0x4
TEST EAX,EAX
JNZ 0x1D
XOR EAX,EAX
POP EDI
POP ESI
RET
*/
void* __cdecl AllocateMemory(size_t param_1, int param_2)
{
    if (param_1 > 0xffffffe0) {
        return NULL;
    }

    if (param_1 == 0) {
        param_1 = 1;
    }

    for (;;) {
        void* ptr = AllocateMemoryFromHeap(param_1);
        if (ptr != NULL) {
            return ptr;
        }
        if (param_2 == 0) {
            break;
        }
        if (CallOutOfMemoryHandler(param_1) == 0) {
            break;
        }
    }
    return NULL;
}

/*
Function: AllocateMemoryFromHeap
Address: 0x4284A0

MOV EAX,dword ptr [ESP + 0x4]
MOV ECX,dword ptr [0x0043eff0]
PUSH EAX
PUSH 0x0
PUSH ECX
CALL dword ptr [0x004403a8]
RET
*/
void* __cdecl AllocateMemoryFromHeap(size_t param_1)
{
    return HeapAlloc(DAT_0043eff0, 0, param_1);
}

/*
Function: CallOutOfMemoryHandler
Address: 0x42C5C0

MOV ECX,dword ptr [0x0043dfa8]
TEST ECX,ECX
JZ 0x1E
MOV EAX,dword ptr [ESP + 0x4]
PUSH EAX
CALL ECX
ADD ESP,0x4
TEST EAX,EAX
JZ 0x1E
MOV EAX,0x1
RET
XOR EAX,EAX
RET
*/
int __cdecl CallOutOfMemoryHandler(size_t param_1)
{
    if (DAT_0043dfa8 != NULL) {
        if (DAT_0043dfa8(param_1) != 0) {
            return 1;
        }
    }
    return 0;
}
