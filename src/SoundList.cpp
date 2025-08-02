#include "SoundList.h"
#include "Sample.h"
#include <string.h>

extern "C" {
    void* AllocateMemory_Wrapper(int size);
    void ParsePath(const char* path, char* drive, char* dir, char* fname, char* ext);
    void ShowError(const char*, ...);
    void FreeFromGlobalHeap(void*);
    void FUN_0041e470(void*);
    int _AIL_sample_status_4(int);
    void* FUN_0041e460(void*);
    int FUN_0041e490(void*, const char*);
}

/* Function start: 0x41E740 */
SoundList::SoundList(int count)
{
    m_sounds = 0;
    m_count = 0;
    m_field8 = 0;
    m_fieldc = 0;
    m_count = count;
    m_sounds = (void**)AllocateMemory_Wrapper(count * 4);
    m_field8 = (void**)AllocateMemory_Wrapper(m_count * 4);
    if (m_field8 != 0 && m_sounds != 0) {
        for (int i = 0; i < m_count; i++) {
            m_sounds[i] = 0;
            m_field8[i] = 0;
        }
    }
    else {
        ShowError("SoundList::SoundList() - Memory allocation error");
    }
}

/* Function start: 0x41E7D0 */
SoundList::~SoundList()
{
    while (m_fieldc > 0) {
        m_fieldc--;
        void* sound = m_field8[m_fieldc];
        if (sound != 0) {
            FUN_0041e470(sound);
            FreeFromGlobalHeap(sound);
            m_field8[m_fieldc] = 0;
        }
        void* sound2 = m_sounds[m_fieldc];
        if (sound2 != 0) {
            FreeFromGlobalHeap(sound2);
            m_sounds[m_fieldc] = 0;
        }
    }
    if (m_field8 != 0) {
        FreeFromGlobalHeap(m_field8);
        m_field8 = 0;
    }
    if (m_sounds != 0) {
        FreeFromGlobalHeap(m_sounds);
        m_sounds = 0;
    }
}

/* Function start: 0x41E870 */
void SoundList::StopAll()
{
    for (int i = 0; i < m_fieldc; i++) {
        Sample* sample = (Sample*)m_field8[i];
        if (_AIL_sample_status_4((int)sample->m_sample) == 4) {
            FUN_0041e470(sample);
        }
    }
}

/* Function start: 0x41E8C0 */
void* SoundList::Register(char* filename)
{
    char local_54[64];
    short sVar9 = -1;

    if (*(int*)(0x0043696c + 0x38) != 0) {
        local_54[0] = '\0';
        ParsePath(filename, 0, 0, local_54, 0);
        if (local_54[0] != '\0') {
            sVar9 = 0;
            while (sVar9 < m_fieldc) {
                if (_strcmpi((char*)m_sounds[sVar9], local_54) == 0) {
                    break;
                }
                sVar9++;
            }

            if (sVar9 == m_fieldc) {
                if (m_count == m_fieldc) {
                    ShowError("SoundList::Register() - Error! Can't register any more sounds. max=%d", m_fieldc);
                }

                int len = strlen(local_54);
                m_sounds[m_fieldc] = (void*)AllocateMemory_Wrapper(len + 1);
                strcpy((char*)m_sounds[m_fieldc], local_54);

                void* sound = AllocateMemory_Wrapper(0x10);
                if (sound != NULL) {
                    sound = FUN_0041e460(sound);
                }

                m_field8[m_fieldc] = sound;
                if (FUN_0041e490(m_field8[m_fieldc], filename) == 0) {
                    sVar9 = m_fieldc;
                    m_fieldc++;
                }
            }

            if (sVar9 > -1 && sVar9 < m_fieldc) {
                return m_field8[sVar9];
            }
        }
    }

    return 0;
}
