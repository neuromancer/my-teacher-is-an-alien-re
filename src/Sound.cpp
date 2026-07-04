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

/* Function start: 0x424C50 */
Sound::Sound(int param_1, short param_2, int param_3) {
  memset(this, 0, 15 * sizeof(int));

  AIL_startup();
  if (g_GameConfig2_0046aa10->data.rawData[2] != '\x02') {
    WriteToMessageLog("Sound():: Not Initialized (No Sound Requested)");
  } else {
    int iVar3;
    if (g_DirectSoundFlag_0046a87c != 0) {
      AIL_set_preference(0xf, 0);
      iVar3 = Sound::OpenDigitalDriver(param_1, param_2, param_3 + 1);
      Sound::digital_driver = (HDIGDRIVER)iVar3;
    }
    if (Sound::digital_driver == 0) {
      AIL_set_preference(0xf, 1);
      iVar3 = Sound::OpenDigitalDriver(param_1, param_2, param_3 + 1);
      Sound::digital_driver = (HDIGDRIVER)iVar3;
    }
    if (Sound::digital_driver == 0) {
      const char *puVar2 = "yes";
      if (param_3 == 0) {
        puVar2 = "no";
      }
      WriteToMessageLog("Sound():: Cannot initialize sound driver at bits=%d frequency=%lu stereo=%s",
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

/* Function start: 0x424D90 */
void Sound::AllocateSampleHandles() {
  short sVar2;
  HSAMPLE *slot;

  for (sVar2 = 0; sVar2 < 13; sVar2++) {
    slot = &samples[(int)sVar2];
    *slot = AIL_allocate_sample_handle(digital_driver);
    if (*slot == 0)
      break;
  }
  num_samples = sVar2;
}

/* Function start: 0x424DD0 */
HSAMPLE Sound::FindFreeSampleHandle() {
  for (short i = 0; i < num_samples; i++) {
    if (AIL_sample_status(samples[i]) == 2) {
      return samples[i];
    }
  }
  return 0;
}

/* Function start: 0x424E40 */
int Sound::OpenDigitalDriver(int rate, unsigned short bits,
                      unsigned short channels) {
  HDIGDRIVER driver;

  g_PcmWaveFormat_00472dc8.wf.nChannels = channels;
  unsigned short shifted = bits >> 3;
  g_PcmWaveFormat_00472dc8.wf.nSamplesPerSec = rate;
  int avgBytesPerSec = (unsigned int)channels * (unsigned int)shifted * rate;
  g_PcmWaveFormat_00472dc8.wf.wFormatTag = 1;
  channels *= shifted;
  g_PcmWaveFormat_00472dc8.wf.nBlockAlign = channels;
  g_PcmWaveFormat_00472dc8.wBitsPerSample = bits;
  g_PcmWaveFormat_00472dc8.wf.nAvgBytesPerSec = avgBytesPerSec;

  if (AIL_waveOutOpen(&driver, 0, -1, &g_PcmWaveFormat_00472dc8)) {
    return 0;
  }
  return (int)driver;
}

/* Function start: 0x424E10 */
void Sound::StopAllSamples() {
    short i = 0;
    if (i < num_samples) {
        do {
            AIL_end_sample(samples[i]);
            i++;
        } while (i < num_samples);
    }
}
