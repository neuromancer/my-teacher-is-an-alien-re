#include "SoundList.h"
#include "Memory.h"
#include "Sample.h"
#include "Sound.h"
#include "main.h"
#include "string.h"
#include "globals.h"
#include <mss.h>
#include <string.h>

/* Function start: 0x41E740 */
SoundList::SoundList(int count) {
  m_sounds = 0;
  m_count = 0;
  m_field8 = 0;
  m_fieldc = 0;
  m_pad = 0;
  m_count = count;
  m_sounds = new char*[count];
  m_field8 = new Sample*[m_count];
  if (m_field8 == 0 || m_sounds == 0) {
    ShowError("SoundList::SoundList() - Memory allocation error");
  }
  for (short i = 0; i < m_count; i++) {
    m_sounds[i] = 0;
    m_field8[i] = 0;
  }
}

/* Function start: 0x41E7D0 */
SoundList::~SoundList() {
  while (m_fieldc > 0) {
    m_fieldc--;
    Sample *sound = m_field8[m_fieldc];
    if (sound != 0) {
      sound->Unload();
      delete sound;
      m_field8[m_fieldc] = 0;
    }
    char *sound2 = m_sounds[m_fieldc];
    if (sound2 != 0) {
      delete[] sound2;
      m_sounds[m_fieldc] = 0;
    }
  }
  if (m_field8 != 0) {
    delete[] m_field8;
    m_field8 = 0;
  }
  if (m_sounds != 0) {
    delete[] m_sounds;
    m_sounds = 0;
  }
}

/* Function start: 0x41E870 */
void SoundList::StopAll() {
  Sample **field8_ptr = m_field8;

  for (short i = 0; i < m_fieldc; i++) {
    Sample *sample = field8_ptr[i];
    if (AIL_sample_status(sample->m_sample) == SMP_PLAYING) {
      sample->Unload();
    }
  }
}

/* Function start: 0x41E8C0 */
void *SoundList::Register(char *filename) {
  char local_54[64];
  short sVar9;

  if (g_Sound_0043696c->digital_driver != 0) {
    local_54[0] = '\0';
    ParsePath(filename, 0, 0, local_54, 0);
    if (local_54[0] != '\0') {
      sVar9 = 0;
      if (m_fieldc > 0) {
        do {
          if (strcmpi(m_sounds[sVar9], local_54) == 0) {
            break;
          }
          sVar9++;
        } while (sVar9 < m_fieldc);
      }

      if (sVar9 == m_fieldc) {
        if (m_count == m_fieldc) {
          ShowError("SoundList::Register() - Error! Can't register any more "
                    "sounds. max=%d",
                    m_fieldc);
        }

        int len = strlen(local_54);
        m_sounds[m_fieldc] = new char[len + 1];
        strcpy(m_sounds[m_fieldc], local_54);

        Sample *sound = new Sample();

        m_field8[m_fieldc] = sound;

        if (m_field8[m_fieldc]->Load(filename) == 0) {
          sVar9 = m_fieldc;
          m_fieldc++;
        }
      }

      if (sVar9 >= 0 && sVar9 < m_fieldc) {
        return m_field8[sVar9];
      }
    }
  }

  return 0;
}
