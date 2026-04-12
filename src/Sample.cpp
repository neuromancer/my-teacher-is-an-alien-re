#include "Sample.h"
#include "globals.h"
#include "Sound.h"
#include "string.h"
#include <mss.h>
#include <stdlib.h>
#include <stdio.h>

/* Function start: 0x424ED0 */
Sample::Sample() {
  m_data = 0;
  m_size = 0;
  m_field8 = 0;
  m_sample = 0;
}

Sample::~Sample() {
  Stop();
}

/* Function start: 0x424EE0 */
void Sample::Unload() {
  if (m_data != 0) {
    Sample::Stop();
    AIL_mem_free_lock(m_data);
    m_data = 0;
  }
}

extern char* __cdecl ResolveAssetPath(char*, ...);
extern int __cdecl GetFileSize(char*);
extern "C" FILE* fsopen(const char*, const char*);

/* Function start: 0x424F00 */
int Sample::Load(char *filename) {
  FILE* file;

  if (g_Sound_0043696c->digital_driver == 0) {
    return 1;
  }
  if (Sample::m_data != 0) {
    Sample::Unload();
  }
  ResolveAssetPath(filename);
  Sample::m_field8 = GetFileSize(filename);
  if (Sample::m_field8 == 0) {
    return 1;
  }
  file = fsopen(filename, "rb");
  if (file == 0) {
    return 1;
  }
  Sample::m_data = (char *)AIL_mem_alloc_lock(Sample::m_field8);
  if (Sample::m_data == 0) {
    return 1;
  }
  fread(Sample::m_data, Sample::m_field8, 1, file);
  fclose(file);
  return 0;
}

/* Function start: 0x424FA0 */ /* ~96% match */
void Sample::Init(int volume) {
  if (m_sample == 0) {
    ShowError("Sample::Init() - Error! Sample Channel has not been set");
  }
  AIL_init_sample(m_sample);
  AIL_set_sample_file(m_sample, m_data, 0);
  m_size = *(int *)((char *)m_sample + 0xc);
  AIL_set_sample_volume(m_sample, volume);
}

/* Function start: 0x424FF0 */
void Sample::Fade(int volume, unsigned int duration) {
  HSAMPLE sample = m_sample;
  if (sample == 0 || m_size != *(int *)((char *)sample + 0xc)) {
    return;
  }
  if (duration == 0) {
    goto set_final;
  }
  {
    int current_volume = AIL_sample_volume(sample);
    int diff = abs(current_volume - volume);
    int step;
    if (current_volume > volume) {
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
    if (diff > 0) {
      delay = (short)delay;
      do {
        current_volume += step;
        AIL_set_sample_volume(m_sample, current_volume);
        timer.Wait(delay);
        diff--;
      } while (diff != 0);
    }
  }
set_final:
  AIL_set_sample_volume(m_sample, volume);
}

/* Function start: 0x4250E0 */
void Sample::Stop() {
  if (m_sample != 0 && m_size == *(int *)((char *)m_sample + 0xc)) {
    AIL_end_sample(m_sample);
  }
}

/* Function start: 0x425100 */
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
