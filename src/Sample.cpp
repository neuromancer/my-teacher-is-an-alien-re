#include "Sample.h"
#include "Sound.h"

extern "C" {
    void ShowError(const char*);
    void _AIL_init_sample_4(int);
    void _AIL_set_sample_file_12(int, void*, int);
    void _AIL_set_sample_volume_8(int, int);
    int _AIL_sample_volume_4(int);
    void _AIL_end_sample_4(int);
    void _AIL_stop_sample_4(int);
    int _AIL_sample_status_4(int);
    void _AIL_set_sample_loop_count_8(int, int);
    void _AIL_start_sample_4(int);
    void FUN_0041e666();
}

extern Sound* g_sound;

/* Function start: 0x000000 */
Sample::Sample()
{
    m_data = 0;
    m_field4 = 0;
    m_field8 = 0;
    m_sample = 0;
}

/* Function start: 0x41E670 */
Sample::~Sample()
{
    if (m_sample != 0 && m_field4 == *(int*)((char*)m_sample + 0xc)) {
        _AIL_end_sample_4((int)m_sample);
    }
}

/* Function start: 0x41E530 */
void Sample::Init(int volume)
{
    if (m_sample == 0) {
        ShowError("Sample::Init Error Sample C");
    }
    _AIL_init_sample_4((int)m_sample);
    _AIL_set_sample_file_12((int)m_sample, m_data, 0);
    m_field4 = *(int*)((char*)m_sample + 0xc);
    _AIL_set_sample_volume_8((int)m_sample, volume);
}

/* Function start: 0x41E580 */
void Sample::Fade(int volume, unsigned int duration)
{
    int current_volume = _AIL_sample_volume_4((int)m_sample);
    if (m_sample != 0 && m_field4 == *(int*)((char*)m_sample + 0xc)) {
        int diff = current_volume - volume;
        int step = -1;
        if (diff < 0) {
            diff = -diff;
            step = 1;
        }

        unsigned int delay = 0;
        if (diff != 0) {
            delay = duration / diff;
        }

        Timer timer;
        if (duration != 0 && diff > 0) {
            do {
                current_volume += step;
                _AIL_set_sample_volume_8((int)m_sample, current_volume);
                timer.Wait(delay);
                diff--;
            } while (diff != 0);
        }
        _AIL_set_sample_volume_8((int)m_sample, volume);
        FUN_0041e666();
    }
}

/* Function start: 0x41E690 */
void Sample::Stop()
{
    if (m_sample != 0 && m_field4 == *(int*)((char*)m_sample + 0xc)) {
        while (_AIL_sample_status_4((int)m_sample) == 4) {
        }
        _AIL_stop_sample_4((int)m_sample);
    }
}


/* Function start: 0x41E6D0 */
int Sample::Play(int volume, int loop_count)
{
    if (m_data == 0) {
        return 1;
    }
    m_sample = g_sound->FindFreeSampleHandle();
    if (m_sample != 0) {
        Init(volume);
        if (m_sample != 0) {
            _AIL_set_sample_loop_count_8((int)m_sample, loop_count);
        }
        if (m_sample != 0 && m_field4 == *(int*)((char*)m_sample + 0xc)) {
            _AIL_start_sample_4((int)m_sample);
        }
        return 0;
    }
    return 1;
}
