#include "SoundList.h"
#include "stubs.h"
#include "Memory.h"
#include "Sample.h"
#include "Sound.h"
#include "main.h"
#include "string.h"
#include "globals.h"
#include <mss.h>
#include <string.h>

/* Function start: 0x425170 */ /* ~98% match */
SoundList::SoundList(int count) {
  memset(this, 0, 0x10);
  m_count = count;
  m_sounds = (char**)operator new(count * 4);
  m_field8 = (Sample**)operator new(m_count * 4);
  if (m_field8 == 0 || m_sounds == 0) {
    ShowError("SoundList::SoundList() - Memory allocation error");
  }
  for (short i = 0; i < m_count; i++) {
    m_sounds[i] = 0;
    m_field8[i] = 0;
  }
}

/* Function start: 0x425200 */
SoundList::~SoundList() {
  while (m_fieldc > 0) {
    m_fieldc--;
    Sample *sound = m_field8[m_fieldc];
    if (sound != 0) {
      sound->Unload();
      operator delete(sound);
      m_field8[m_fieldc] = 0;
    }
    char *sound2 = m_sounds[m_fieldc];
    if (sound2 != 0) {
      operator delete(sound2);
      m_sounds[m_fieldc] = 0;
    }
  }
  if (m_field8 != 0) {
    operator delete(m_field8);
    m_field8 = 0;
  }
  if (m_sounds != 0) {
    operator delete(m_sounds);
    m_sounds = 0;
  }
}

#include "Sample.h"

/* Function start: 0x4252A0 */
void SoundList::StopAll() {
  for (short i = 0; i < m_fieldc; i++) {
    if (AIL_sample_status(m_field8[i]->m_sample) == SMP_PLAYING) {
      m_field8[i]->Stop();
    }
  }
}

/* Function start: 0x4252F0 */ /* ~98% match */
void *SoundList::Register(char *filename) {
  char local_54[64];
  short sVar9;

  if (g_EngineSound_0046aa0c->digital_driver != 0) {
    local_54[0] = '\0';
    ParsePath(filename, 0, 0, local_54, 0);
    if (local_54[0] != '\0') {
      for (sVar9 = 0; sVar9 < m_fieldc; sVar9++) {
        if (strcmpi(m_sounds[sVar9], local_54) == 0) {
          break;
        }
      }

      if (sVar9 == m_fieldc) {
        if (m_count == m_fieldc) {
          ShowError("SoundList::Register() - Error! Can't register any more "
                    "sounds. max=%d",
                    m_fieldc);
        }

        m_sounds[m_fieldc] = new char[strlen(local_54) + 1];
        strcpy(m_sounds[m_fieldc], local_54);

        Sample *sound = new Sample();

        m_field8[m_fieldc] = sound;

        if (m_field8[m_fieldc]->Load(filename) == 0) {
          sVar9 = m_fieldc;
          m_fieldc = sVar9 + 1;
        }
      }

      if (sVar9 >= 0 && sVar9 < m_fieldc) {
        return m_field8[sVar9];
      }
    }
  }

  return 0;
}
