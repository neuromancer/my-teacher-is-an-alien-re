#include "BaseObject.h"
#include "SoundList.h"
#include "Sprite.h"
#include <string.h>
#include <new>

typedef int bool;
#define true 1
#define false 0

struct UnkStruct {
    char data[0xe0];
};

// Ghidra was unable to determine the class name, so we'll call it Screen
class Screen : public BaseObject {
public:
    char unknown_data[0x600]; // Just a large buffer for now

    Screen(BaseObject* hotspot_data);
    int CheckAndResolveCollision(BaseObject *colliding_object);
    void PlaySoundEffect(int sound_index);
};

extern "C" {
    void FUN_0041e870(SoundList* sound_list);
    void FUN_0041e6d0(void* sound_data, int volume, int flags);
    void* AllocateMemory_Wrapper(int size);
}

extern "C" int* DAT_00435a80;
extern "C" const char* s_audio_Snd0023_wav_00435248;
extern "C" const char* s_audio_Snd0024_wav_00435234;
extern "C" const char* s_audio_Snd0025_wav_00435220;

/*
Function: SCI::Screen::CheckAndResolveCollision
Address: 0x403040

PUSH EBX
PUSH ESI
MOV ESI,dword ptr [ESP + 0xc]
PUSH EDI
PUSH EBP
MOV EDI,ECX
PUSH ESI
CALL 0x004171b0
MOV ECX,dword ptr [ESI + 0xa4]
CMP dword ptr [EDI + 0xa0],ECX
JG 0x41
CMP dword ptr [EDI + 0xa8],ECX
JL 0x41
MOV EAX,dword ptr [ESI + 0xa8]
CMP dword ptr [EDI + 0xa4],EAX
JG 0x41
CMP dword ptr [EDI + 0xac],EAX
MOV EAX,0x1
JGE 0x43
XOR EAX,EAX
TEST EAX,EAX
JNZ 0x50
XOR EAX,EAX
POP EBP
POP EDI
POP ESI
POP EBX
RET 0x4
CMP dword ptr [ESI + 0xac],0x2
JL 0x1C5
XOR EBX,EBX
LEA EDX,[EDI + 0x190]
CMP dword ptr [EDX],0x0
JZ 0x8F
CMP dword ptr [EDX + -0x10],ECX
JG 0x89
CMP dword ptr [EDX + -0x8],ECX
JL 0x89
MOV EAX,dword ptr [ESI + 0xa8]
CMP dword ptr [EDX + -0xc],EAX
JG 0x89
CMP dword ptr [EDX + -0x4],EAX
MOV EAX,0x1
JGE 0x8B
XOR EAX,EAX
TEST EAX,EAX
JNZ 0xA0
ADD EDX,0xe0
INC EBX
CMP EBX,0x6
JL 0x65
JMP 0x1C5
PUSH EBX
MOV ECX,EDI
CALL 0x00403300
LEA EDX,[EBX*0x8 + 0x0]
SUB EDX,EBX
SHL EDX,0x5
MOV EAX,dword ptr [EDX + EDI*0x1 + 0xc8]
MOV dword ptr [ESI + 0x8],EAX
LEA ECX,[EDX + EDI*0x1]
MOV EBX,dword ptr [EDX + EDI*0x1 + 0xcc]
MOV dword ptr [ESI + 0xc],EBX
XOR EBX,EBX
MOV AL,byte ptr [EBX + ECX*0x1 + 0xd0]
INC EBX
CMP EBX,0x20
MOV byte ptr [EBX + ESI*0x1 + 0xf],AL
JC 0x0040310d
MOV EAX,dword ptr [ECX + 0xf0]
LEA EBP,[ESI + 0x38]
MOV dword ptr [ESI + 0x30],EAX
LEA EAX,[ECX + 0xf8]
MOV EBX,dword ptr [EAX]
MOV EAX,dword ptr [EAX + 0x4]
MOV dword ptr [EBP],EBX
XOR EBX,EBX
MOV dword ptr [EBP + 0x4],EAX
LEA EAX,[EDI + EBX*0x1]
INC EBX
CMP EBX,0x40
MOV AL,byte ptr [EAX + EDX*0x1 + 0x100]
MOV byte ptr [ESI + EBX*0x1 + 0x3f],AL
JC 0x0040313d
MOV EAX,dword ptr [ECX + 0x140]
MOV dword ptr [ESI + 0x80],EAX
MOV EDX,dword ptr [ECX + 0x148]
MOV dword ptr [ESI + 0x88],EDX
MOV EBX,dword ptr [ECX + 0x14c]
MOV dword ptr [ESI + 0x8c],EBX
MOV EAX,dword ptr [ECX + 0x150]
MOV dword ptr [ESI + 0x90],EAX
MOV EDX,dword ptr [ECX + 0x154]
MOV dword ptr [ESI + 0x94],EDX
MOV EBX,dword ptr [ECX + 0x158]
MOV dword ptr [ESI + 0x98],EBX
MOV EAX,dword ptr [ECX + 0x15c]
MOV dword ptr [ESI + 0x9c],EAX
MOV EDX,dword ptr [ECX + 0x160]
MOV dword ptr [ESI + 0xa0],EDX
MOV EBX,dword ptr [ECX + 0x164]
MOV dword ptr [ESI + 0xa4],EBX
MOV EAX,dword ptr [ECX + 0x168]
MOV dword ptr [ESI + 0xa8],EAX
MOV EDX,dword ptr [ECX + 0x16c]
MOV dword ptr [ESI + 0xac],EDX
MOV EBX,dword ptr [ECX + 0x170]
MOV dword ptr [ESI + 0xb0],EBX
MOV EAX,dword ptr [ECX + 0x174]
MOV dword ptr [ESI + 0xb4],EAX
MOV EDX,dword ptr [ECX + 0x178]
MOV dword ptr [ESI + 0xb8],EDX
MOV ECX,dword ptr [ECX + 0x17c]
MOV dword ptr [ESI + 0xbc],ECX
MOV EAX,0x1
POP EBP
POP EDI
POP ESI
POP EBX
RET 0x4
*/

struct Hotspot {
    int x1;
    int y1;
    int x2;
    int y2;
};

int Screen::CheckAndResolveCollision(BaseObject *colliding_object)
{
    VFunc5(colliding_object);

    int collision = 0;
    if (this->bounding_box_x1 <= colliding_object->bounding_box_x2) {
        if (this->bounding_box_x2 >= colliding_object->bounding_box_x1) {
            if (this->bounding_box_y1 <= colliding_object->bounding_box_y2) {
                if (this->bounding_box_y2 >= colliding_object->bounding_box_y1) {
                    collision = 1;
                }
            }
        }
    }

    if (!collision) {
        return 0;
    }

    if (colliding_object->object_type <= 1) {
        return 1;
    }

    Hotspot** hotspots = (Hotspot**)((char*)this + 0x190);
    for (int i = 0; i < 6; i++) {
        Hotspot *hotspot = hotspots[i];
        if (hotspot != 0) {
            if (colliding_object->bounding_box_x1 < hotspot->x2 &&
                colliding_object->bounding_box_x2 > hotspot->x1 &&
                colliding_object->bounding_box_y1 < hotspot->y2 &&
                colliding_object->bounding_box_y2 > hotspot->y1) {

                PlaySoundEffect(i);

                char *src = (char *)this + 0xc8 + i * 0xe0;
                char *dst = (char *)colliding_object;
                for (int j = 0; j < 0xe0; j++) {
                    dst[j] = src[j];
                }
                return 1;
            }
        }
    }

    return 1;
}

/*
Function: Screen::PlaySoundEffect
Address: 0x403300

PUSH ESI
MOV ESI,ECX
MOV ECX,dword ptr [ECX + 0x5f8]
CALL 0x0041e870
MOV EAX,dword ptr [ESP + 0x8]
CMP EAX,-0x1
JZ 0x37
LEA ECX,[EAX*0x8 + 0x0]
SUB ECX,EAX
SHL ECX,0x5
MOV ECX,dword ptr [ECX + ESI*0x1 + 0x194]
TEST ECX,ECX
JZ 0x37
PUSH 0x1
PUSH 0x64
CALL 0x0041e6d0
POP ESI
RET 0x4
*/
void Screen::PlaySoundEffect(int sound_index)
{
    SoundList* soundList = *(SoundList**)((char*)this + 0x5f8);
    FUN_0041e870(soundList);
    if (sound_index != -1) {
        void* sound_data = *(void**)((char*)this + 0x194 + sound_index * 0xe0);
        if (sound_data != 0) {
            FUN_0041e6d0(sound_data, 100, 1);
        }
    }
}

/*
Function: Screen
Address: 0x402ED0

MOV EAX,FS:[0x0]
PUSH EBP
MOV EBP,ESP
PUSH -0x1
PUSH 0x402fb3
PUSH EAX
MOV dword ptr FS:[0x0],ESP
MOV EAX,dword ptr [EBP + 0x8]
SUB ESP,0x4
PUSH ESI
PUSH EDI
MOV ESI,ECX
PUSH EAX
CALL 0x00417180
CMP dword ptr [0x00435a80],0x0
JZ 0x45
MOV EAX,[0x00435a80]
MOV ECX,dword ptr [ESI + 0xb0]
MOV EAX,dword ptr [EAX]
INC EAX
PUSH EAX
CALL 0x0041d190
LEA ECX,[ESI + 0x190]
MOV EAX,0x6
MOV dword ptr [ECX],0x1
ADD ECX,0xe0
DEC EAX
JNZ 0x50
PUSH 0x10
CALL 0x004249c0
MOV dword ptr [EBP + -0x10],EAX
ADD ESP,0x4
MOV dword ptr [EBP + -0x4],0x0
MOV EDI,EAX
MOV EAX,0x0
TEST EDI,EDI
JZ 0x87
PUSH 0x32
MOV ECX,EDI
CALL 0x0041e740
PUSH 0x435248
MOV ECX,EAX
MOV dword ptr [EBP + -0x4],0xffffffff
MOV dword ptr [ESI + 0x5f8],EAX
CALL 0x0041e8c0
PUSH 0x435234
MOV ECX,dword ptr [ESI + 0x5f8]
MOV dword ptr [ESI + 0x274],EAX
CALL 0x0041e8c0
PUSH 0x435220
MOV ECX,dword ptr [ESI + 0x5f8]
MOV dword ptr [ESI + 0x354],EAX
CALL 0x0041e8c0
POP EDI
MOV dword ptr [ESI + 0x434],EAX
MOV EAX,dword ptr [EBP + -0xc]
POP ESI
MOV FS:[0x0],EAX
MOV ESP,EBP
POP EBP
RET 0x4
*/
Screen::Screen(BaseObject* hotspot_data)
{
    Copy(hotspot_data);

    if (DAT_00435a80 != 0) {
        Sprite* sprite = *(Sprite**)((char*)this + 0xb0);
        sprite->SetState(*DAT_00435a80 + 1);
    }

    for (int i = 0; i < 6; i++) {
        *(int*)((char*)this + 0x190 + i * 0xe0) = 1;
    }

    SoundList* soundList = (SoundList*)AllocateMemory_Wrapper(0x10);
    if (soundList) {
        new (soundList) SoundList(50);
    }
    *(SoundList**)((char*)this + 0x5f8) = soundList;

    *(void**)((char*)this + 0x274) = soundList->Register(s_audio_Snd0023_wav_00435248);
    *(void**)((char*)this + 0x354) = soundList->Register(s_audio_Snd0024_wav_00435234);
    *(void**)((char*)this + 0x434) = soundList->Register(s_audio_Snd0025_wav_00435220);
}
