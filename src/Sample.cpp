#include "Sample.h"
#include "Sound.h"
#include <mss.h>

extern "C" {
void ShowError(const char *);
void FUN_0041e666();
}

extern Sound *g_sound;

// Constructor wrapper for external callers
Sample *Sample_Ctor(Sample *p) {
  p->Sample::Sample();
  return p;
}

/* Function start: 0x41E460 */
Sample::Sample() {
  m_data = 0;
  m_field4 = 0;
  m_field8 = 0;
  m_sample = 0;
}

/* Function start: 0x41E530 */
void Sample::Init(int volume) {
  if (m_sample == 0) {
    ShowError("Sample::Init() - Error! Sample Channel has not been set");
  }
  AIL_init_sample(m_sample);
  AIL_set_sample_file(m_sample, m_data, 0);
  m_field4 = *(int *)((char *)m_sample + 0xc);
  AIL_set_sample_volume(m_sample, volume);
}

/* Function start: 0x41E580 */
void Sample::Fade(int volume, unsigned int duration) {
  int current_volume = AIL_sample_volume(m_sample);
  if (m_sample != 0 && m_field4 == *(int *)((char *)m_sample + 0xc)) {
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
        AIL_set_sample_volume(m_sample, current_volume);
        timer.Wait(delay);
        diff--;
      } while (diff != 0);
    }
    AIL_set_sample_volume(m_sample, volume);
    FUN_0041e666();
  }
}

/* Function start: 0x41E670 */
Sample::~Sample() {
  if (m_sample != 0 && m_field4 == *(int *)((char *)m_sample + 0xc)) {
    AIL_end_sample(m_sample);
  }
}

/* Function start: 0x41E690 */
void Sample::Stop() {
  if (m_sample != 0 && m_field4 == *(int *)((char *)m_sample + 0xc)) {
    while (AIL_sample_status(m_sample) == SMP_PLAYING) {
    }
    AIL_stop_sample(m_sample);
  }
}

/* Function start: 0x41E6D0 */
int Sample::Play(int volume, int loop_count) {
  if (m_data == 0) {
    return 1;
  }
  m_sample = g_sound->FindFreeSampleHandle();
  if (m_sample != 0) {
    Init(volume);
    if (m_sample != 0) {
      AIL_set_sample_loop_count(m_sample, loop_count);
    }
    if (m_sample != 0 && m_field4 == *(int *)((char *)m_sample + 0xc)) {
      AIL_start_sample(m_sample);
    }
    return 0;
  }
  return 1;
}
