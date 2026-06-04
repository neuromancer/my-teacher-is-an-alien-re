#ifndef TEACHER_ANALYSIS_MSS_H
#define TEACHER_ANALYSIS_MSS_H

#include "windows.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void* HSAMPLE;
typedef void* HDIGDRIVER;

#define SMP_DONE 2
#define SMP_PLAYING 4

int AIL_startup(void);
void AIL_shutdown(void);
int AIL_set_preference(unsigned int, int);
int AIL_waveOutOpen(HDIGDRIVER*, void*, int, PCMWAVEFORMAT*);
void AIL_waveOutClose(HDIGDRIVER);
void AIL_digital_configuration(HDIGDRIVER, int*, int*, char*);
HSAMPLE AIL_allocate_sample_handle(HDIGDRIVER);
void AIL_release_sample_handle(HSAMPLE);
void AIL_init_sample(HSAMPLE);
int AIL_set_sample_file(HSAMPLE, void*, int);
void AIL_set_sample_volume(HSAMPLE, int);
int AIL_sample_volume(HSAMPLE);
void AIL_set_sample_loop_count(HSAMPLE, int);
void AIL_start_sample(HSAMPLE);
void AIL_end_sample(HSAMPLE);
int AIL_sample_status(HSAMPLE);
void* AIL_mem_alloc_lock(unsigned int);
void AIL_mem_free_lock(void*);

#ifdef __cplusplus
}
#endif

#endif
