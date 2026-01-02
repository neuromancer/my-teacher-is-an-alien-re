#include "SoundList.h"
#include "Memory.h"
#include "Sample.h"
#include "main.h"
#include "string.h"
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
  m_sounds = (void **)AllocateMemory(count * 4);
  m_field8 = (void **)AllocateMemory(m_count * 4);
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
    void *sound = m_field8[m_fieldc];
    if (sound != 0) {
      ((Sample *)sound)->Unload();
      FreeMemory(sound);
      m_field8[m_fieldc] = 0;
    }
    void *sound2 = m_sounds[m_fieldc];
    if (sound2 != 0) {
      FreeMemory(sound2);
      m_sounds[m_fieldc] = 0;
    }
  }
  if (m_field8 != 0) {
    FreeMemory(m_field8);
    m_field8 = 0;
  }
  if (m_sounds != 0) {
    FreeMemory(m_sounds);
    m_sounds = 0;
  }
}

/* Function start: 0x41E870 */
void SoundList::StopAll() {
  void **field8_ptr = m_field8;

  for (short i = 0; i < m_fieldc; i++) {
    Sample *sample = (Sample *)field8_ptr[i];
    if (AIL_sample_status(sample->m_sample) == SMP_PLAYING) {
      ((Sample *)sample)->Unload();
    }
  }
}

/* Function start: 0x41E8C0 */
void *SoundList::Register(char *filename) {
  char local_54[64];
  short sVar9;

  if (*(int *)(0x0043696c + 0x38) != 0) {
    local_54[0] = '\0';
    ParsePath(filename, 0, 0, local_54, 0);
    if (local_54[0] != '\0') {
      sVar9 = 0;
      if (m_fieldc > 0) {
        do {
          if (strcmpi((char *)m_sounds[sVar9], local_54) == 0) {
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
        m_sounds[m_fieldc] = (void *)AllocateMemory(len + 1);
        strcpy((char *)m_sounds[m_fieldc], local_54);

        Sample *sound = new Sample();

        m_field8[m_fieldc] = sound;

        if (((Sample *)m_field8[m_fieldc])->Load(filename) == 0) {
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
