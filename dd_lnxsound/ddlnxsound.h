#ifndef __LNX__DD_SOUND_H_
#define __LNX__DD_SOUND_H_

//#include <pthread.h>
#include "SDL_thread.h"

// Sound Library Internal Error Codes
#define SSL_OK 0
#define SSL_ERROR_GENERIC -1
#define SSL_ERROR_SAMPLE_NODATA -2
#define SSL_ERROR_STREAMMIXER -3

// Sound Status
#define SSF_UNUSED 0
#define SSF_PLAY_NORMAL 1
#define SSF_PLAY_LOOPING 2
#define SSF_PAUSED 4
#define SSF_PLAY_STREAMING 8
#define SSF_BUFFERED_LOOP 64
#define SSF_BUFFERED_STRM 128

#define SBT_PRIMARY 0
#define SBT_2D 1
#define SBT_3D 2

// looping methods
#define DSLOOP_SMART_METHOD 0
#define DSLOOP_BUFFER_METHOD 1
#define DSLOOP_STREAM_METHOD 2

#define DSBUFLOOP_INIT_STEP -1
#define DSBUFLOOP_LOOP_STEP 0
#define DSBUFLOOP_FINISH_STEP 1

// used to time threads.
#define DSPB_TICK_INTERVAL .01 // Primary buffer update rate (in seconds)
#define DSPB_TICK_MILLISECONDS (DSPB_TICK_INTERVAL * 1000)

typedef struct LNXSTREAMTAG {
  //	pthread_t thread_id;
  SDL_Thread *thread_id;
  unsigned long thread_handle;
  volatile bool thread_request_kill;
  volatile bool thread_alive;
  volatile bool thread_waiting_for_death;

  int *sound_device;

  void (*fp_SetError)(int code);
  void (*fp_ErrorText)(char *fmt, ...);
  int *p_error_code;
} LNXSTREAM;

#endif
