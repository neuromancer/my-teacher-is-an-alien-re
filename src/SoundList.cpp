#include "SoundList.h"
#include "Sample.h"

extern "C" {
    void* AllocateMemory_Wrapper(int size);
    void ShowError(const char*);
    void FreeFromGlobalHeap(void*);
    void FUN_0041e470(void*);
    int _AIL_sample_status_4(int);
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
