#include "Sound.h"
#include <string.h>

extern "C" {
    void _AIL_startup_0();
    void _AIL_set_preference_8(int, int);
    int FUN_0041e3d0(int, unsigned short, short);
    void WriteToMessageLog(const char*, ...);
    void _SmackSoundUseMSS_4(int);
    void FUN_0041e320(short*);
    void _AIL_digital_configuration_16(int, int, int, char*);
    int _AIL_allocate_sample_handle_4(int);
    int _AIL_sample_status_4(int);
    void _AIL_end_sample_4(int);
    int _AIL_waveOutOpen_16(int*, int, int, void*);
}

extern char DAT_00436970;
extern short _param_3;

/* Function start: 0x41E460 */
Sound::Sound()
{
    *(int*)this = 0;
    *(int*)((char*)this + 4) = 0;
    *(int*)((char*)this + 8) = 0;
    *(int*)((char*)this + 12) = 0;
}

int DAT_0043de30;
short DAT_0043de32;
int DAT_0043de34;
int DAT_0043de38;
int DAT_0043de3c;
short DAT_0043de3e;

/* Function start: 0x41E1F0 */
void* Sound::Init(int param_1, unsigned short param_2, short param_3)
{
    int* puVar4 = (int*)this;
    for (int i = 0; i < 15; i++) {
        *puVar4++ = 0;
    }

    _AIL_startup_0();
    if (*(char*)(DAT_00436970 + 0x46) == '\x02') {
        _AIL_set_preference_8(0xf, 0);
        int iVar3 = FUN_0041e3d0(param_1, param_2, param_3 + 1);
        this->field_0x38 = iVar3;
        if (iVar3 == 0) {
            _AIL_set_preference_8(0xf, 1);
        }
        if (this->field_0x38 == 0) {
            iVar3 = FUN_0041e3d0(param_1, param_2, param_3 + 1);
            this->field_0x38 = iVar3;
        }
        if (this->field_0x38 == 0) {
            const char* puVar2 = "Miles 32-bit";
            if (_param_3 == 0) {
                puVar2 = "Miles 16-bit";
            }
            WriteToMessageLog("Sound :: Cannot initialize sound %d bit %d hz %s", (int)param_2, param_1,
                puVar2);
        }
        else {
            _SmackSoundUseMSS_4(this->field_0x38);
            FUN_0041e320((short*)this);
            char auStack_80[128];
            auStack_80[0] = 0;
            _AIL_digital_configuration_16(this->field_0x38, 0, 0, auStack_80);
            const char* puVar2 = "Miles 32-bit";
            if (_param_3 == 0) {
                puVar2 = "Miles 16-bit";
            }
            WriteToMessageLog("Sound :: Initialized at %d bits, %d hz %s - %s", (int)param_2, param_1,
                puVar2, auStack_80);
        }
    }
    else {
        WriteToMessageLog("Sound :: Not Initialized (No Sound Card)");
    }
    return this;
}

/* Function start: 0x41E320 */
void Sound::AllocateSampleHandles()
{
    short sVar2 = 0;
    do {
        int iVar1 = _AIL_allocate_sample_handle_4(this->field_0x38);
        this->handles[sVar2] = iVar1;
        if (iVar1 == 0) break;
        sVar2 = sVar2 + 1;
    } while (sVar2 < 13);
    this->num_handles = sVar2;
}

/* Function start: 0x41E360 */
int Sound::FindFreeSampleHandle()
{
    short sVar2 = 0;
    int bVar3 = this->num_handles == 0;
    if (0 < this->num_handles) {
        do {
            int iVar1 = _AIL_sample_status_4(this->handles[sVar2]);
            if (iVar1 == 2) break;
            sVar2 = sVar2 + 1;
        } while (sVar2 < this->num_handles);
        bVar3 = this->num_handles == sVar2;
    }
    if (!bVar3) {
        return this->handles[sVar2];
    }
    return 0;
}

/* Function start: 0x41E3A0 */
void Sound::StopAllSamples()
{
    short sVar2 = 0;
    if (0 < this->num_handles) {
        do {
            _AIL_end_sample_4(this->handles[sVar2]);
            sVar2 = sVar2 + 1;
        } while (sVar2 < this->num_handles);
    }
}

/* Function start: 0x41E3D0 */
int FUN_0041e3d0(int param_1, unsigned short param_2, unsigned short param_3)
{
    int local_4;
    DAT_0043de32 = param_3;
    DAT_0043de30 = 1;
    DAT_0043de3c = param_3 * (param_2 >> 3);
    DAT_0043de38 = (unsigned int)param_3 * (unsigned int)(param_2 >> 3) * param_1;
    DAT_0043de3e = param_2;
    DAT_0043de34 = param_1;
    int iVar1 = _AIL_waveOutOpen_16(&local_4, 0, 0xffffffff, &DAT_0043de30);
    if (iVar1 != 0) {
        return 0;
    }
    return local_4;
}
