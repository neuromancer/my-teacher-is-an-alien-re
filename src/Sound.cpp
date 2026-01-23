#include "globals.h"
#include "Sound.h"
#include "string.h"
#include <mss.h>
#include <smack.h>
#include <sys/types.h>
#include <string.h>
#include <mmsystem.h>
#include "GameConfig.h"

extern short _param_3;

PCMWAVEFORMAT g_pcm;

/* Function start: 0x41E1F0 */
Sound::Sound(int param_1, short param_2, int param_3) {
  int *puVar4 = (int *)this;
  for (int i = 0; i < 15; i++) {
    *puVar4++ = 0;
  }

  AIL_startup();
  if (g_GameConfig_00436970->data.rawData[2] == '\x02') {
    ShowMessage("Sound():: Not Initialized (No Sound Requested)");
  } else {
    AIL_set_preference(0xf, 0);
    int iVar3 = OpenDigitalDriver(param_1, param_2, param_3 + 1);
    Sound::digital_driver = (HDIGDRIVER)iVar3;
    if (iVar3 == 0) {
      AIL_set_preference(0xf, 1);
    }
    if (Sound::digital_driver == 0) {
      iVar3 = OpenDigitalDriver(param_1, param_2, param_3 + 1);
      Sound::digital_driver = (HDIGDRIVER)iVar3;
    }
    if (Sound::digital_driver == 0) {
      const char *puVar2 = "yes";
      if (param_3 == 0) {
        puVar2 = "no";
      }
      ShowMessage("Sound():: Cannot initialize sound driver at bits=%d frequency=%lu stereo=%s",
                        (int)param_2, param_1, puVar2);
    } else {
      SmackSoundUseMSS(Sound::digital_driver);
      Sound::AllocateSampleHandles();
      char auStack_80[128];
      auStack_80[0] = 0;
      AIL_digital_configuration(Sound::digital_driver, 0, 0, auStack_80);
      const char *puVar2 = "yes";
      if (param_3 == 0) {
        puVar2 = "no";
      }
      WriteToMessageLog("Sound():: Initialized at bits=%d frequency=%lu stereo=%s using\n    %s",
                        (int)param_2, param_1, puVar2, auStack_80);
    }
  }
}

/* Function start: 0x41E320 */
void Sound::AllocateSampleHandles() {
  short sVar2 = 0;
  while (sVar2 < 13) {
    int index = sVar2;
    HSAMPLE *slot = &samples[index];
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
int __stdcall OpenDigitalDriver(int rate, unsigned short bits,
                      unsigned short channels) {
  HDIGDRIVER driver;

  g_pcm.wf.nChannels = channels;
  unsigned short shifted = bits >> 3;
  unsigned int product = (unsigned int)channels * (unsigned int)shifted;
  g_pcm.wf.wFormatTag = 1;
  g_pcm.wf.nBlockAlign = channels * shifted;
  product = product * rate;
  g_pcm.wBitsPerSample = bits;
  g_pcm.wf.nSamplesPerSec = rate;
  g_pcm.wf.nAvgBytesPerSec = product;

  if (AIL_waveOutOpen(&driver, 0, -1, &g_pcm)) {
    return 0;
  }
  return (int)driver;
}
