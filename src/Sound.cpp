#include "globals.h"
#include "Sound.h"
#include "string.h"
#include <mss.h>
#include <smack.h>
#include <sys/types.h>
#include <string.h>
#include "GameConfig.h"

extern short _param_3;

// Forward declaration for 0x41E3D0
int OpenDigitalDriver(int param_1, unsigned short param_2,
                      unsigned short param_3);

int DAT_0043de30;
short DAT_0043de32;
int DAT_0043de34;
int DAT_0043de38;
int DAT_0043de3c;
short DAT_0043de3e;

/* Function start: 0x41E1F0 */
Sound::Sound(int param_1, unsigned short param_2, short param_3) {
  int *puVar4 = (int *)this;
  for (int i = 0; i < 15; i++) {
    *puVar4++ = 0;
  }

  AIL_startup();
  if (g_GameConfig_00436970->data.rawData[2] != '\x02') {
    AIL_set_preference(0xf, 0);
    int iVar3 = OpenDigitalDriver(param_1, param_2, param_3 + 1);
    digital_driver = (HDIGDRIVER)iVar3;
    if (iVar3 == 0) {
      AIL_set_preference(0xf, 1);
    }
    if (digital_driver == 0) {
      iVar3 = OpenDigitalDriver(param_1, param_2, param_3 + 1);
      digital_driver = (HDIGDRIVER)iVar3;
    }
    if (digital_driver == 0) {
      const char *puVar2 = "Miles 32-bit";
      if (_param_3 == 0) {
        puVar2 = "Miles 16-bit";
      }
      WriteToMessageLog("Sound :: Cannot initialize sound %d bit %d hz %s",
                        (int)param_2, param_1, puVar2);
    } else {
      SmackSoundUseMSS(digital_driver);
      AllocateSampleHandles();
      char auStack_80[128];
      auStack_80[0] = 0;
      AIL_digital_configuration(digital_driver, 0, 0, auStack_80);
      const char *puVar2 = "Miles 32-bit";
      if (_param_3 == 0) {
        puVar2 = "Miles 16-bit";
      }
      WriteToMessageLog("Sound :: Initialized at %d bits, %d hz %s - %s",
                        (int)param_2, param_1, puVar2, auStack_80);
    }
  } else {
    WriteToMessageLog("Sound :: Not Initialized (No Sound Card)");
  }
}

/* Function start: 0x41E320 */
void Sound::AllocateSampleHandles() {
  short sVar2 = 0;
  while (sVar2 < 13) {
    int index = sVar2;
    HSAMPLE *slot = (HSAMPLE *)((char *)this + 4 + index * 4);
    HSAMPLE handle = AIL_allocate_sample_handle(digital_driver);
    *slot = handle;
    if (handle == 0)
      break;
    sVar2++;
  }
  num_samples = sVar2;
}

/* Function start: 0x41E360 */
HSAMPLE Sound::FindFreeSampleHandle() {
  for (short i = 0; i < num_samples; i++) {
    if (AIL_sample_status(samples[i]) == 2) {
      return samples[i];
    }
  }
  return 0;
}

/* Function start: 0x41E3A0 */
void Sound::StopAllSamples() {
  for (short i = 0; i < num_samples; i++) {
    AIL_end_sample(samples[i]);
  }
}

/* Function start: 0x41E3D0 */
int OpenDigitalDriver(int param_1, unsigned short param_2,
                      unsigned short param_3) {
  int local_4;

  // Preserve register/stack order and exact assignments to match original
  // assembly
  DAT_0043de32 = param_3;
  DAT_0043de30 = 1;
  DAT_0043de3c = param_3 * (param_2 >> 3);
  DAT_0043de38 = (unsigned int)param_3 * (unsigned int)(param_2 >> 3) * param_1;
  DAT_0043de3e = param_2;
  DAT_0043de34 = param_1;

  int result = AIL_waveOutOpen((HDIGDRIVER *)&local_4, 0, 0xffffffff,
                               (PCMWAVEFORMAT *)&DAT_0043de30);
  if (result != 0) {
    return 0;
  }
  return local_4;
}
