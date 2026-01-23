#include "Sample.h"
#include "globals.h"
#include "Sound.h"
#include "string.h"
#include <mss.h>
#include <stdio.h>

/* Function start: 0x41E460 */
Sample::Sample() {
  m_data = 0;
  m_size = 0;
  m_field8 = 0;
  m_sample = 0;
}

/* Function start: 0x41E470 */
void Sample::Unload() {
  if (m_data != 0) {
    Sample::~Sample();
    AIL_mem_free_lock(m_data);
    m_data = 0;
  }
}

/* Function start: 0x41E490 */
int Sample::Load(char *filename) {
  if (g_Sound_0043696c->digital_driver == 0) {
    return 1;
  }
  if (m_data != 0) {
    Unload();
  }
  m_size = 0;
  FILE *_File = OpenFileAndFindKey((char *)0x0, filename, "rb",
                                   (unsigned int *)&m_size);
  if ((_File != 0) && (m_size != 0)) {
    m_data = (char *)AIL_mem_alloc_lock(m_size);
    if (m_data == 0) {
      return 1;
    }
    fread(m_data, m_size, 1, _File);
    fclose(_File);
    return 0;
  }
  return 1;
}

/* Function start: 0x41E530 */
void Sample::Init(int volume) {
  if (m_sample == 0) {
    ShowError("Sample::Init() - Error! Sample Channel has not been set");
  }
  AIL_init_sample(m_sample);
  AIL_set_sample_file(m_sample, m_data, 0);
  m_size = *(int *)((char *)m_sample + 0xc);
  AIL_set_sample_volume(m_sample, volume);
}

/* Function start: 0x41E580 */
void Sample::Fade(int volume, unsigned int duration) {
  HSAMPLE sample = m_sample;
  if (sample == 0 || m_size != *(int *)((char *)sample + 0xc)) {
    return;
  }
  int current_volume = AIL_sample_volume(sample);
  int diff = current_volume - volume;
  int sign = diff >> 31;
  diff = (diff ^ sign) - sign;
  int step;
  if (volume < current_volume) {
    step = -1;
  } else {
    step = 1;
  }

  unsigned int delay;
  if (diff != 0) {
    delay = duration / diff;
  } else {
    delay = 0;
  }

  Timer timer;
  if (duration != 0 && diff > 0) {
    delay = (short)delay;
    do {
      current_volume += step;
      AIL_set_sample_volume(m_sample, current_volume);
      timer.Wait(delay);
      diff--;
    } while (diff != 0);
  }
  AIL_set_sample_volume(m_sample, volume);
}

/* Function start: 0x41E670 */
Sample::~Sample() {
  if (m_sample != 0 && m_size == *(int *)((char *)m_sample + 0xc)) {
    AIL_end_sample(m_sample);
  }
}

/* Function start: 0x41E690 */
void Sample::Stop() {
  if (m_sample != 0 && m_size == *(int *)((char *)m_sample + 0xc)) {
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
  m_sample = g_Sound_0043696c->FindFreeSampleHandle();
  if (m_sample != 0) {
    Init(volume);
    if (m_sample != 0) {
      AIL_set_sample_loop_count(m_sample, loop_count);
    }
    if (m_sample != 0 && m_size == *(int *)((char *)m_sample + 0xc)) {
      AIL_start_sample(m_sample);
    }
    return 0;
  }
  return 1;
}
