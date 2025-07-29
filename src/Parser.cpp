/*
Function: CloseFile
Address: 0x418B00

PUSH ESI
MOV ESI,ECX
CMP dword ptr [ECX + 0x8],0x0
JZ 0x2D
MOV EAX,dword ptr [ESI + 0x80]
TEST EAX,EAX
JZ 0x2D
PUSH EAX
CALL 0x00425e70
MOV dword ptr [ESI + 0x80],0x0
MOV dword ptr [ESI + 0x8],0x0
ADD ESP,0x4
POP ESI
RET

*/
#include <stdio.h>
#include <string.h>
#include "string.h"
#include "BaseObject.h"

// Based on the assembly, the Parser class has members at offsets 0x8 and 0x80.
// The function at 0x00419110
extern "C" void FUN_00419110(const char* message, const char* filename);

// The string at 0x4364e0.
extern "C" const char* s_Parser__Open___File___s__not_ope_004364e0;
// The string at 0x4364dc
extern "C" const char* s_rb_;
// The string at 0x4364b0
extern "C" const char* s_Parser__Open___Unable_to_open_fi_004364b0;
// The string at 0x436538
extern "C" const char* s_Parser__SaveFilePosition_____una_00436538;
// The string at 0x436578
extern "C" const char* s_Parser__RestoreFilePosition_____u_00436578;
// The string at 0x4365c0
extern "C" const char* s_Parser__FindKey___Unable_to_find_004365c0;
// The format string at 0x435440
extern "C" const char* format__s;
// The global buffer at 0x00436960
extern "C" char g_key_buffer[256];


// The function at 0x00425e50
extern "C" FILE* FUN_00425e50(const char* filename, const char* mode);

// The function at 0x00424c00
extern "C" char* FUN_00424c00(const char* s1, const char* s2);

// The function at 0x00425fd0 is implemented in src/string.cpp
extern "C" char* FUN_00425fd0(char* dest, const char* src, size_t n);


extern "C" void* AllocateMemory(int size, int flags);
extern "C" void* FUN_004249c0(int size);

class Parser {
public:
    void CloseFile();
    void Copy(Parser* dst);
    void Open(const char* filename);
    void SaveFilePosition();
    int RestoreFilePosition();
    void FindKey(const char* key);
    int GetKey(const char* line);

private:
    char pad1[0x8];
    int field8_0x8; // at offset 0x8
    int key_found_flag; // at offset 0xc
    char key_buffer[32]; // at offset 0x10
    char pad3[8]; // at offset 0x30
    fpos_t FILE_pos; // at offset 0x38
    char filename[64]; // at offset 0x40
    FILE* pFile; // at offset 0x80
};

void Parser::CloseFile() {
    if (field8_0x8 != 0 && pFile != 0) {
        fclose(pFile);
        pFile = 0;
        field8_0x8 = 0;
    }
}

/*
Function: Copy
Address: 0x418A90

PUSH EBX
PUSH ESI
MOV EBX,dword ptr [ESP + 0xc]
PUSH EDI
PUSH EBP
MOV EAX,dword ptr [ECX + 0x80]
MOV EBP,ECX
CMP dword ptr [EBX + 0x80],EAX
JZ 0x1D
CALL 0x00418b00
LEA EDX,[EBP + 0x40]
LEA EDI,[EBX + 0x40]
MOV ECX,0xffffffff
SUB EAX,EAX
SCASB.REPNE ES:EDI
NOT ECX
SUB EDI,ECX
MOV EAX,ECX
SHR ECX,0x2
MOV ESI,EDI
MOV EDI,EDX
MOVSD.REP ES:EDI,ESI
MOV ECX,EAX
AND ECX,0x3
MOVSB.REP ES:EDI,ESI
MOV EAX,dword ptr [EBX + 0x80]
MOV dword ptr [EBP + 0x80],EAX
TEST EAX,EAX
JNZ 0x60
PUSH EDX
PUSH 0x4364e0
CALL 0x00419110
POP EBP
POP EDI
POP ESI
POP EBX
RET 0x4
*/

void Parser::Copy(Parser* dst) {
    if (dst->pFile != this->pFile) {
        this->CloseFile();
    }

    // The assembly uses repne scasb and rep movsd/movsb, which is a string copy.
    // Using strcpy should generate a closer match.
    strcpy(this->filename, dst->filename);

    this->pFile = dst->pFile;

    if (this->pFile == 0) {
        FUN_00419110(s_Parser__Open___File___s__not_ope_004364e0, this->filename);
    }
}

void Parser::Open(const char* filename) {
    this->CloseFile();
    strcpy(this->filename, filename);
    this->pFile = FUN_00425e50(this->filename, s_rb_);
    this->field8_0x8 = 1;
    if (this->pFile == 0) {
        FUN_00419110(s_Parser__Open___Unable_to_open_fi_004364b0, this->filename);
    }
}

/*
Function: SaveFilePosition
Address: 0x418B50

PUSH ESI
LEA EAX,[ECX + 0x38]
MOV ESI,ECX
PUSH EAX
MOV ECX,dword ptr [ESI + 0x80]
PUSH ECX
CALL 0x00425ee0
ADD ESP,0x8
TEST EAX,EAX
JZ 0x2B
ADD ESI,0x40
PUSH ESI
PUSH 0x436538
CALL 0x00419110
POP ESI
RET

*/
void Parser::SaveFilePosition() {
    if (fgetpos(this->pFile, &this->FILE_pos) != 0) {
        FUN_00419110(s_Parser__SaveFilePosition_____una_00436538, this->filename);
    }
}

int Parser::RestoreFilePosition() {
    if (fsetpos(this->pFile, &this->FILE_pos) != 0) {
        FUN_00419110(s_Parser__RestoreFilePosition_____u_00436578, this->filename);
    }
    return 0;
}

void Parser::FindKey(const char* key) {
    char line_buffer[256];

    if (key == 0) {
        return;
    }

    // Rewind file to the beginning (SEEK_SET = 0)
    fseek(this->pFile, 0, 0);

    for (;;) {
        if (fgets(line_buffer, 255, this->pFile) == 0) {
            // End of file, key not found
            FUN_00419110(s_Parser__FindKey___Unable_to_find_004365c0, this->filename);
            return; // FUN_00419110 likely doesn't return, but we add this for completeness
        }

        sscanf(line_buffer, format__s, g_key_buffer);

        // Manual comparison to match assembly more closely, per user feedback.
        const char* p1 = g_key_buffer;
        const char* p2 = key;
        int match = 0;
        while (*p1 != '\0' && *p1 == *p2) {
            p1++;
            p2++;
        }
        if (*p1 == *p2) {
            match = 1; // strings are equal
        }

        if (match == 1) {
            // Key found, file is positioned at the next line
            return;
        }
    }
}

int Parser::GetKey(const char* line) {
    char local_buffer[64];
    const char *start_ptr, *end_ptr;
    int length;
    int return_value = this->key_found_flag;

    start_ptr = FUN_00424c00(line, "[=");
    if (start_ptr != 0) {
        end_ptr = FUN_00424c00(start_ptr + 2, "]");
        if (end_ptr != 0) {
            length = end_ptr - (start_ptr + 2);
            if (length > 0) {
                FUN_00425fd0(local_buffer, start_ptr + 2, length);
                local_buffer[length] = '\0';

                if (this->key_found_flag == 0) {
                    strcpy(this->key_buffer, local_buffer);
                    this->key_found_flag = 1;
                    return_value = 1;
                } else {
                    // Manual strcmp to match assembly more closely
                    const char* p1 = this->key_buffer;
                    const char* p2 = local_buffer;
                    int match = 0;
                    while (*p1 != '\0' && *p1 == *p2) {
                        p1++;
                        p2++;
                    }
                    if (*p1 == *p2) {
                        match = 1; // strings are equal
                    }

                    if (match == 1) {
                        memset(this->key_buffer, 0, 32);
                        this->key_found_flag = 0;
                    }
                }
            }
        }
    }

    return return_value;
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

Decompiled:
void* FUN_004249c0(int size) {
    return AllocateMemory(size, 1);
}
*/
void* FUN_004249c0(int size) {
    return AllocateMemory(size, 1);
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

Decompiled:
// This is a guess based on the assembly. The actual implementation might be different.
// The function at 0x004284a0 and 0x0042c5c0 are unknown.
extern "C" void* FUN_004284a0(int size);
extern "C" void* FUN_0042c5c0(int size);

void* AllocateMemory(int size, int flags) {
    if (size <= -32) {
        return 0;
    }
    if (size == 0) {
        size = 1;
    }
    void* mem = FUN_004284a0(size);
    if (mem == 0 && flags != 0) {
        mem = FUN_0042c5c0(size);
    }
    return mem;
}

/*
Function: FreeVBuffer
Address: 0x41FE20

PUSH ESI
PUSH EDI
MOV ESI,dword ptr [ECX + 0x8]
MOV EDI,ECX
TEST ESI,ESI
JZ 0x22
MOV ECX,ESI
CALL 0x0041aa10
PUSH ESI
CALL 0x00424940
MOV dword ptr [EDI + 0x8],0x0
ADD ESP,0x4
MOV dword ptr [EDI + 0x18],0x0
POP EDI
POP ESI
RET
*/

class VBuffer;

extern "C" void VBuffer_dtor(VBuffer*);
extern "C" void FreeFromGlobalHeap(void*);

class Sprite {
public:
    void FreeVBuffer();
private:
    char pad[8];
    VBuffer* pVBuffer; // at offset 0x8
    char pad2[12];
    int field_18; // at offset 0x18
};

void Sprite::FreeVBuffer()
{
    VBuffer* pVBuff;
    Sprite* self = this;

    pVBuff = self->pVBuffer;
    if (pVBuff) {
        VBuffer_dtor(pVBuff);
        FreeFromGlobalHeap(pVBuff);
        self->pVBuffer = 0;
    }
    self->field_18 = 0;
}

// --- BaseObject Queue Implementation ---

extern "C" void* AllocateMemory_Wrapper(int size);
extern "C" void Queue_InsertNode_2(void* queue, void* node);
extern "C" void* FUN_0041cd10(void* dest, void* src);
extern "C" void FUN_0041c58c();
extern "C" void FreeFromGlobalHeap(void* p);


extern "C" const char * s_queue_fault_0101_00435120;
extern "C" const char * s_queue_fault_0112_0043516c;
extern "C" const char * s_queue_fault_0113_00435158;


/*
Function: Queue_InsertNodeByType
Address: 0x41C2C0

MOV EAX,FS:[0x0]
PUSH EBP
MOV EBP,ESP
PUSH -0x1
PUSH 0x41c582
PUSH EAX
MOV dword ptr FS:[0x0],ESP
MOV EAX,[0x00436974]
SUB ESP,0x14
PUSH EBX
PUSH ESI
PUSH EDI
MOV ESI,dword ptr [EAX + 0x18]
DEC ESI
MOV EAX,dword ptr [EAX + 0x14]
DEC EAX
XOR EDX,EDX
MOV dword ptr [EBP + -0x20],EDX
MOV dword ptr [EBP + -0x1c],EDX
MOV dword ptr [EBP + -0x18],EAX
MOV dword ptr [EBP + -0x14],ESI
MOV dword ptr [EBP + -0x4],EDX
MOV EAX,dword ptr [ECX + 0x98]
CMP EAX,0x1
JZ 0x73
CMP EAX,0x2
JZ 0x95
CMP EAX,0x3
JZ 0x1AA
MOV dword ptr [EBP + -0x4],0xffffffff
CALL 0x0041c58c
MOV EAX,dword ptr [EBP + -0xc]
POP EDI
MOV FS:[0x0],EAX
POP ESI
POP EBX
MOV ESP,EBP
POP EBP
RET 0x4
LEA EAX,[EBP + -0x20]
MOV ESI,dword ptr [EBP + 0x8]
MOV EBX,dword ptr [ESI]
PUSH EAX
MOV ECX,ESI
CALL dword ptr [EBX]
TEST ESI,ESI
JZ 0x55
PUSH ESI
MOV dword ptr [ESI],0x431050
CALL 0x00424940
ADD ESP,0x4
JMP 0x55
MOV EBX,dword ptr [ECX + 0xa0]
MOV ESI,dword ptr [EBP + 0x8]
TEST ESI,ESI
JNZ 0xAF
PUSH 0x435120
CALL 0x00419110
MOV ECX,dword ptr [EBX]
MOV dword ptr [EBX + 0x8],ECX
MOV EAX,dword ptr [EBX + 0xc]
CMP EAX,0x1
JZ 0xCB
CMP EAX,0x2
JZ 0xCB
PUSH ESI
MOV ECX,EBX
CALL 0x0041cb40
JMP 0x55
TEST ECX,ECX
JNZ 0xDC
PUSH ESI
MOV ECX,EBX
CALL 0x0041cb40
JMP 0x55
MOV EAX,dword ptr [EBX + 0x8]
MOV EDX,dword ptr [ESI + 0x4]
MOV ECX,dword ptr [EAX + 0x8]
CMP dword ptr [ECX + 0x4],EDX
JC 0x0041c3c4
CMP dword ptr [EBX + 0x4],EAX
JZ 0x111
TEST EAX,EAX
JZ 0xF9
MOV EAX,dword ptr [EAX + 0x4]
MOV dword ptr [EBX + 0x8],EAX
CMP dword ptr [EBX + 0x8],0x0
JNZ 0xDC
JMP 0x55
PUSH ESI
MOV ECX,EBX
CALL 0x0041cb40
JMP 0x55
TEST ESI,ESI
JNZ 0x122
PUSH 0x43516c
CALL 0x00419110
PUSH 0xc
CALL 0x004249c0
MOV dword ptr [EBP + -0x10],EAX
ADD ESP,0x4
MOV byte ptr [EBP + -0x4],0x1
MOV EDI,EAX
TEST EDI,EDI
JZ 0x145
PUSH ESI
MOV ECX,EDI
CALL 0x0041cd10
MOV ESI,EAX
JMP 0x147
XOR ESI,ESI
MOV byte ptr [EBP + -0x4],0x0
CMP dword ptr [EBX + 0x8],0x0
JNZ 0x157
MOV EAX,dword ptr [EBX + 0x4]
MOV dword ptr [EBX + 0x8],EAX
CMP dword ptr [EBX],0x0
JNZ 0x169
MOV dword ptr [EBX],ESI
MOV dword ptr [EBX + 0x4],ESI
MOV dword ptr [EBX + 0x8],ESI
JMP 0x55
MOV EAX,dword ptr [EBX + 0x4]
TEST EAX,EAX
JZ 0x176
CMP dword ptr [EAX + 0x4],0x0
JZ 0x183
PUSH 0x435158
CALL 0x00419110
MOV dword ptr [ESI + 0x4],0x0
MOV EAX,dword ptr [EBX + 0x4]
MOV dword ptr [ESI],EAX
MOV EAX,dword ptr [EBX + 0x4]
MOV dword ptr [EAX + 0x4],ESI
MOV dword ptr [EBX + 0x4],ESI
JMP 0x55
MOV EBX,dword ptr [ECX + 0xa0]
MOV ESI,dword ptr [EBP + 0x8]
TEST ESI,ESI
JNZ 0x1C4
PUSH 0x435120
CALL 0x00419110
MOV EAX,dword ptr [EBX]
MOV dword ptr [EBX + 0x8],EAX
MOV ECX,dword ptr [EBX + 0xc]
CMP ECX,0x1
JZ 0x1E3
CMP ECX,0x2
JZ 0x1E3
PUSH ESI
MOV ECX,EBX
CALL 0x0041cb40
JMP 0x55
TEST EAX,EAX
JNZ 0x1F4
PUSH ESI
MOV ECX,EBX
CALL 0x0041cb40
JMP 0x55
MOV EAX,dword ptr [EBX + 0x8]
MOV ECX,dword ptr [ESI + 0x4]
MOV EDX,dword ptr [EAX + 0x8]
CMP dword ptr [EDX + 0x4],ECX
JC 0x0041c4dc
CMP dword ptr [EBX + 0x4],EAX
JZ 0x229
TEST EAX,EAX
JZ 0x211
MOV EAX,dword ptr [EAX + 0x4]
MOV dword ptr [EBX + 0x8],EAX
CMP dword ptr [EBX + 0x8],0x0
JNZ 0x1F4
JMP 0x55
PUSH ESI
MOV ECX,EBX
CALL 0x0041cb40
JMP 0x55
TEST ESI,ESI
JNZ 0x23A
PUSH 0x43516c
CALL 0x00419110
PUSH 0xc
CALL 0x004249c0
MOV dword ptr [EBP + -0x10],EAX
ADD ESP,0x4
MOV byte ptr [EBP + -0x4],0x2
MOV EDI,EAX
TEST EDI,EDI
JZ 0x25D
PUSH ESI
MOV ECX,EDI
CALL 0x0041cd10
MOV ESI,EAX
JMP 0x25F
XOR ESI,ESI
MOV byte ptr [EBP + -0x4],0x0
CMP dword ptr [EBX + 0x8],0x0
JNZ 0x26F
MOV EAX,dword ptr [EBX + 0x4]
MOV dword ptr [EBX + 0x8],EAX
CMP dword ptr [EBX],0x0
JNZ 0x281
MOV dword ptr [EBX],ESI
MOV dword ptr [EBX + 0x4],ESI
MOV dword ptr [EBX + 0x8],ESI
JMP 0x55
MOV EAX,dword ptr [EBX + 0x4]
TEST EAX,EAX
JZ 0x28E
CMP dword ptr [EAX + 0x4],0x0
JZ 0x29B
PUSH 0x435158
CALL 0x00419110
MOV dword ptr [ESI + 0x4],0x0
MOV EAX,dword ptr [EBX + 0x4]
MOV dword ptr [ESI],EAX
MOV EAX,dword ptr [EBX + 0x4]
MOV dword ptr [EAX + 0x4],ESI
MOV dword ptr [EBX + 0x4],ESI
JMP 0x55
*/
void BaseObject::Queue_InsertNodeByType(void* pNode) {
    // This is a rough translation, it will need refinement.
    // The try/catch structure is inferred from the FS:[0] manipulation.
    try {
        if (this->object_type == 1) {
            // This part is tricky. It seems to be calling a virtual function.
            // and then freeing the object.
            //(*(code **)*param_1)(&local_24);
            if (pNode != 0) {
                //*(undefined4 **)pNode = &PTR_FUN_00431050;
                FreeFromGlobalHeap(pNode);
            }
        } else if (this->object_type == 2 || this->object_type == 3) {
            QueueNode* queue = (QueueNode*)this->queue1;
            if (pNode == 0) {
                ShowError(s_queue_fault_0101_00435120);
            }
            queue->current = queue->head;
            if (queue->type == 1 || queue->type == 2) {
                if (queue->head == 0) {
                    Queue_InsertNode_2(queue, pNode);
                } else {
                    do {
                        ListNode* current_node = (ListNode*)queue->current;
                        // This is a guess based on *(uint *)(*(int *)(iVar1 + 8) + 4) < (uint)param_1[1]
                        if (*(unsigned int*)((char*)current_node->data + 4) < *(unsigned int*)((char*)pNode + 4)) {
                            Queue_InsertNode_2(queue, pNode);
                            break;
                        }
                        if (queue->tail == current_node) {
                            if (pNode == 0) {
                                ShowError(s_queue_fault_0112_0043516c);
                            }
                            void* mem = AllocateMemory_Wrapper(0xc);
                            ListNode* new_node = (ListNode*)FUN_0041cd10(mem, pNode);
                            if (queue->current == 0) {
                                queue->current = queue->tail;
                            }
                            if (queue->head == 0) {
                                queue->head = new_node;
                                queue->tail = new_node;
                                queue->current = new_node;
                            } else {
                                if (queue->tail == 0 || ((ListNode*)queue->tail)->next != 0) {
                                    ShowError(s_queue_fault_0113_00435158);
                                }
                                new_node->next = 0;
                                new_node->prev = (ListNode*)queue->tail;
                                ((ListNode*)queue->tail)->next = new_node;
                                queue->tail = new_node;
                            }
                            break;
                        }
                        if (current_node != 0) {
                            queue->current = current_node->next;
                        }
                    } while (queue->current != 0);
                }
            } else {
                Queue_InsertNode_2(queue, pNode);
            }
        }
    } catch(...) {
        FUN_0041c58c();
    }
}
