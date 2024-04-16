/*
 * $Logfile: /DescentIII/Main/mac/MACSTREAMAUDIO.CPP $
 * $Revision: 1.1.1.1 $
 * $Date: 2003/08/26 03:58:15 $
 * $Author: kevinb $
 *
 * Interface for streaming audio, compressed or not
 *
 * $Log: MACSTREAMAUDIO.CPP,v $
 * Revision 1.1.1.1  2003/08/26 03:58:15  kevinb
 * initial 1.5 import
 *
 *
 * 2     4/12/00 7:08p Matt
 * From Duane for 1.4
 *
 * 1     10/21/99 1:55p Kevin
 * Mac!
 *
 * 41    5/17/99 1:56p Ardussi
 * changed to compile on Mac
 *
 * 40    5/10/99 9:22p Samir
 * added code to allow for certain streams to be specified to start on
 * next sound frame.
 *
 * 39    4/29/99 3:03p Samir
 * took out mprintfs and if we're opening an opened stream, close it and
 * continue.
 *
 * 38    4/17/99 5:58a Jeff
 * Changes for linux
 *
 * 37    4/14/99 1:48a Jeff
 * fixed case mismatched #includes
 *
 * 36    4/10/99 5:09p Samir
 * beginning to add prioritization of sounds code.  currently non
 * functional, but allows me to change all calls to Play3dSound so that I
 * can test later.
 *
 * 35    3/22/99 5:12p Samir
 * on shutdown, CLOSE any open streams.
 *
 * 34    3/17/99 4:22p Samir
 * made pause and resume for streams work.
 *
 * 33    3/09/99 6:40p Jeff
 * put try/catch around StreamPlay
 *
 * 32    3/04/99 6:38p Samir
 * Saved stream state per frame.
 *
 * 31    3/03/99 5:08p Samir
 * added slew of debug code: hopefully the library will be revamped after
 * OEM.
 *
 * 30    3/01/99 8:11p Samir
 * fixed bug when audio stream was playing while sound system shutdown.
 *
 * 29    2/28/99 6:35p Samir
 * fixed streaming bugs for very small samples.
 *
 * 28    2/28/99 5:11p Nate
 * added playcount.
 *
 * 27    2/27/99 6:52p Samir
 * added function to get current loop count
 *
 * 26    2/26/99 8:34p Samir
 * may have fixed problem with really small streams.
 *
 * 25    2/26/99 5:31p Samir
 *
 * 24    2/25/99 9:20p Kevin
 * Semi fix for music in DS
 *
 * 23    2/17/99 8:35p Samir
 * use m_streamdone to determine whether play should reset the stream or
 * not.
 *
 * 22    12/13/98 6:50p Samir
 * fixed problems with looping and next.switching sync.
 *
 * 21    12/11/98 5:25p Samir
 * took out debug messages.
 *
 * 20    12/11/98 5:20p Samir
 * perhaps prevent unused buffers from playing.
 *
 * 19    12/11/98 3:27p Samir
 * add debug code.
 *
 * 18    12/10/98 7:12p Samir
 * fixed some bugs in file/playback buffer sequencing with new system.
 *
 * 17    12/10/98 10:11a Samir
 * newer streaming audio library.
 *
 * 11    11/20/98 5:20p Samir
 * correcting a lot of errors in AudioStream::Next
 *
 * 10    11/13/98 4:03p Nate
 * took out volume stuff from high level sound lib
 *
 * 9     11/13/98 2:25p Samir
 * added 'next' stream processing.
 *
 * 9     11/13/98 10:48a Samir
 * added 'next' stream capability.
 *
 * 8     10/23/98 7:04p Samir
 *
 * 7     10/23/98 6:45p Matt
 * Scale the streaming audio volume by the master volume.
 *
 * 6     8/10/98 5:54p Samir
 * added looping streams and soft measure stopping.
 *
 * 5     7/30/98 4:35p Dan
 * more error checking.
 *
 * 4     7/30/98 10:57a Sean
 * if no sound, then don't do anything.
 *
 * 3     7/24/98 5:18p Samir
 * use OSFArchive for stream file operations now.
 *
 * 2     7/09/98 8:36p Samir
 * fully implemented redo of Stream interface.
 *
 * 1     7/09/98 6:48p Samir
 *
 * MOVED FROM MAIN PROJECT
 *
 * 10    6/29/98 4:15p Chris
 * Streaming audio works
 *
 * 9     6/24/98 5:00p Jeff
 * set filehandle to null on close, short circuit read data if file handle
 * is NULL
 *
 * 8     6/24/98 4:41p Jeff
 * will kill the stream is the sound is killed
 *
 *
 * $NoKeywords: $
 */
#include <sound.h>
#include "streamaudio.h"
#include "pserror.h"
#include "CFILE.H"
#include "mem.h"
#include "Macros.h"
#include "ddio.h"

#include <stdlib.h>
#include <string.h>

#ifdef MACINTOSH
// #define __MACBUILD__
#include "adecode.h"
#endif
// #include "samirlog.h"
#define LOGFILE(_s)

#if defined(__LINUX__)
typedef struct {
  bool empty;
} AudioDecoder;
typedef void *(*ad_malloc)(unsigned size);
typedef void (*ad_free)(void *p);
#endif

#if defined(__LINUX__)
typedef unsigned ReadFunction(void *data, void *buf, unsigned qty);
AudioDecoder *Create_AudioDecoder(ReadFunction *reader, void *data, unsigned *pChannels, unsigned *pSampleRate,
                                  long *pSampleCount);
unsigned AudioDecoder_Read(AudioDecoder *ad, void *buf, unsigned qty);
void AudioDecoder_Close(AudioDecoder *ad);
void AudioDecoder_MallocFree(ad_malloc *fn_malloc, ad_free *fn_free);
#endif

void *MyDoubleBackProc(SndChannelPtr channel, SndDoubleBufferPtr dBuffer);

//	this stream is for everyone (used by the StreamPlay interface)
static AudioStream User_audio_stream;

llsSystem *AudioStream::m_ll_sndsys = NULL;
AudioStream *AudioStream::m_streams[STRM_LIMIT];
int AudioStream::m_thisid = -1;

#define SAMPLES_PER_STREAM_SHORT_BUF 22050
#define SAMPLES_PER_STREAM_BUF 44100

SndDoubleBackUPP gSndDoubleBackUPP = NULL;

//	sets the low-level sound object
void AudioStream::InitSystem(llsSystem *sndsys) {
  int i;
  OSErr err;

  if (AudioStream::m_ll_sndsys)
    return;

  AudioStream::m_ll_sndsys = sndsys;

  //	reserve sound slots for streams, they should never get replaced, and always should be played.
  for (i = 0; i < STRM_LIMIT; i++) {
    AudioStream::m_streams[i] = NULL;
  }
  if (gSndDoubleBackUPP == NULL)
    gSndDoubleBackUPP = NewSndDoubleBackProc(MyDoubleBackProc);
}

//	shutdsown
void AudioStream::Shutdown() {
  if (AudioStream::m_ll_sndsys) {
    int i;

    // stop all streams.
    for (i = 0; i < STRM_LIMIT; i++) {
      if (AudioStream::m_streams[i]) {
        AudioStream::m_streams[i]->Close();
        AudioStream::m_streams[i] = NULL;
      }
    }

    AudioStream::m_ll_sndsys = NULL;
  }
}

// allocates a stream slot for a stream
bool AudioStream::ActivateStream(AudioStream *stream) {
  int i;

  for (i = 0; i < STRM_LIMIT; i++) {
    if (AudioStream::m_streams[i] == NULL) {
      AudioStream::m_streams[i] = stream;
      stream->m_streamindex = i;
      return true;
    }
  }
  mprintf((1, "STRMAUD: AudioStream queue filled!\n"));
  return false;
}

void AudioStream::DeactivateStream(AudioStream *stream) {
  if (stream->m_streamindex > -1) {
    mprintf((1, "Deactivateing: STRM[%d]\n", stream->m_streamindex));
    AudioStream::m_streams[stream->m_streamindex] = NULL;
    stream->m_streamindex = -1;
  }
}

//	called by application to allow stream playback
void AudioStream::Frame() {
  int t;
  float time = timer_GetTime();

  for (t = 0; t < STRM_LIMIT; t++) {
    if (AudioStream::m_streams[t]) {
      AudioStream *strm = AudioStream::m_streams[t];

      if (strm->m_laststate != strm->m_state) {
        mprintf((1, "frame:STRM[%d]: NEW m_state=%d\n", strm->m_curid, strm->m_state));
      }

      if (strm->m_state == STRM_PLAYING) {
        //				mprintf((1, "frame:STRM[%d]: Playing\n", strm->m_curid));
        strm->m_measure_timer += (time - strm->m_last_frametime);
        if (strm->m_measure_timer >= strm->m_measure_time) {
          strm->m_curmeasure++;
        }

        strm->m_last_frametime = time;

        strm->UpdateData();
      }

      //			else if (strm->m_state == STRM_STOPPED) {
      //					if (CHECK_FLAG(strm->m_flags,STRM_OPNF_ONETIME))
      //						strm->Close();
      //			}

      else if (strm->m_state == STRM_STOPPING) {
        if (CHECK_FLAG((strm->doubleHeader.dbhBufferPtr[0])->dbFlags, dbLastBuffer) && !strm->IsPlaying()) {
          mprintf((1, "frame STRM[%d]: chan 0x%X StopING! Last Buffer read!!\n", strm->m_curid, strm->strm_channel));
          strm->Stop();
          if (CHECK_FLAG(strm->m_flags, STRM_OPNF_ONETIME)) {
            mprintf((1, "Frame: STRM[%d] stoped in onetime so closing\n", strm->m_curid));
            strm->Close();
          }
          strm->m_readahead = false;
        }

      } else if (strm->m_state == STRM_INVALID) {
        mprintf((1, "STRM[%d]: INVALID!!\n", strm->m_curid));
        strm->DeactivateStream(strm);
      } else if (strm->m_start_on_frame) {
        mprintf((1, "frame: STRM[%d]: start on frame playstream\n", strm->m_curid));
        strm->m_llshandle = strm->PlayStream(&strm->m_playinfo);

        if (strm->m_llshandle > -1) {
          mprintf((1, "frame: STRM[%d]:handle %d\n", strm->m_curid, strm->m_llshandle));
          // for one buffer samples, prepare to stop now so that the one and only one buffer gets called.
          //	looped one buffer samples dont do this though...
          if (strm->m_nbufs == 1 && !strm->m_start_on_frame_looped) {
            strm->m_state = STRM_STOPPING;
            mprintf((1, "frame: ONLY ONE BUFFERS WORTH nbufs == 1 & !looped\n"));
          } else {
            strm->m_state = STRM_PLAYING;
            mprintf((1, "frame: STRM[%d]: playing\n", strm->m_curid));
          }
        }

        strm->m_start_on_frame = false;
        strm->m_start_on_frame_looped = false;
      } else {
        //				mprintf((1, "frame: STRM[%d]: NOT DOING ANYTHING WHOOPS!!!\n", strm->m_curid));
      }

      strm->m_laststate = strm->m_state;
    }
  }
}

// called to pause all streams.
void AudioStream::PauseAll() {
  int t;
  for (t = 0; t < STRM_LIMIT; t++) {
    if (AudioStream::m_streams[t]) {
      AudioStream::m_streams[t]->Pause();
    }
  }
}

void AudioStream::ResumeAll() {
  int t;
  for (t = 0; t < STRM_LIMIT; t++) {
    if (AudioStream::m_streams[t]) {
      AudioStream::m_streams[t]->Resume();
    }
  }
}

//	AudioStream object Interface
//		allows dynamic playing of streams

AudioStream::AudioStream() {
  m_decoder = NULL; // audio decomp object
  m_llshandle = -1; // snd system handle
  m_sbufidx = 0;    // stream position markers for synching.
  m_fbufidx = 0;
  m_flags = 0;
  m_state = STRM_INVALID;
  m_streamindex = -1;
  m_curmeasure = -1;
  m_stopnextmeasure = false;
  m_stopflag = NULL;
  m_bytesleft = 0;
  m_readahead_finished_loop = false;
  m_playcount = 0;
  m_curid = -1;
  m_nbufs = 0;
  m_start_on_frame = false;

  strm_channel = NULL;
  OSErr err;
  err = SndNewChannel(&strm_channel, sampledSynth, initStereo + initNoInterp, NULL);
  if (err)
    mprintf((1, "unable to open stream channel\n"));
}

AudioStream::~AudioStream() {
  AudioStream::Close();
  if (strm_channel == 0)
    return;
  SndDisposeChannel(strm_channel, true);
  //	mprintf((0, "distroying: STRM[%d]:\n", this->m_curid));
}

// sets flags for playback (STRM_OPNF_XXX)
void AudioStream::SetFlags(int flags) { m_flags = flags; }

void AudioStream::SetLoopCount(int loop_count) {
  // if loop_count = 0, then m_loopcount= -1, infinite;
  // if loop_count = 1, then m_loopcount=0, no looping, etc.
  m_loopcount = loop_count - 1;
  mprintf((1, "SetLoopCount: %d\n", m_loopcount));
}

int AudioStream::GetLoopCount() const {
  // if loop_count = 0, then m_loopcount= -1, infinite;
  // if loop_count = 1, then m_loopcount=0, no looping, etc.
  return m_loopcount + 1;
}

void AudioStream::SetVolume(float vol) {
  m_volume = vol;
  SndCommand sndCommand;
  OSErr err;

  if (m_ll_sndsys) {
    if (strm_channel == 0)
      return;

    short one_side = vol * 0x0080;
    long both_side = (one_side << 16) | one_side;

    sndCommand.cmd = volumeCmd;
    sndCommand.param1 = 0;
    sndCommand.param2 = both_side;
    err = SndDoImmediate(strm_channel, &sndCommand);
    if (err)
      mprintf((0, "stream channel is corrupt: volume\n"));
  }
}

float AudioStream::GetVolume() { return m_volume; }

//	flags specify what type of stream it is.
bool AudioStream::Open(const char *filename, int open_flags) {
  // don't open a stream that's already open, or bogus filename
  if (m_state != STRM_INVALID) {
    AudioStream::Close();
    //	Int3();
    //	return false;
  }

  if (m_archive.Opened() || !filename)
    return false;

  if (!m_ll_sndsys)
    return false;

  m_state = STRM_INVALID;
  m_streamindex = -1;
  m_flags = open_flags;
  m_curmeasure = 0;
  //	m_volume = 1.0f;
  m_bufsize = SAMPLES_PER_STREAM_BUF * 4;
  m_measure_timer = 0.0f;
  m_measure_time = 0.0f;
  m_last_frametime = 0.0f;
  m_readahead_finished_loop = false;
  m_curid = -1;
  m_nbufs = 0;
  m_start_on_frame = m_start_on_frame_looped = false;

  mprintf((0, "Open new stream %s flags %d sof %d\n", filename, open_flags, m_start_on_frame));
  //	open up stream file
  if (m_archive.Open(filename)) {
    int i;
    int nbufs;

    ASSERT(m_archive.StreamType() == OSF_DIGITAL_STRM);

    m_thisid++;
    m_curid = m_thisid;

    for (i = 0; i < STRM_BUFCOUNT; i++) {
      m_buffer[i].flags = 0;
      m_buffer[i].nbytes = 0;
      m_buffer[i].data = NULL;
      m_buffer[i].id = -1; // marks stream buffer to be allocated.
    }

    SetVolume(m_volume);

    // gradual streams don't readahead yet.
    nbufs = CHECK_FLAG(m_flags, STRM_OPNF_GRADUAL) ? 1 : STRM_BUFCOUNT;

    if (!AudioStream::ReopenDigitalStream(0, nbufs)) {
      return false;
    }

    m_loopmutex.Create();
    AudioStream::SetLoopCount(1);
    m_laststate = m_state;
    mprintf((1, "opening: STRM[%d]: STOPPED sof %d\n", this->m_curid, m_start_on_frame));
    m_state = STRM_STOPPED;
    //		m_state = STRM_PENDING;

    return true;
  }

  return false;
}

//	deallocates all stream buffers and decoder.
void AudioStream::Close() {
  int i;

  if (m_archive.Opened()) {
    mprintf((0, "Close: STRM[%d]: \n", this->m_curid));
    // stop the stream, close the archive, close the decoder.
    AudioStream::Stop();
    m_archive.Close();
    m_loopmutex.Destroy();
    m_curid = -1;

    // free streaming buffers and decoder if we need to.
    for (i = 0; i < STRM_BUFCOUNT; i++) {
      if (m_buffer[i].data) {
        mem_free(m_buffer[i].data);
        m_buffer[i].data = NULL;
        m_buffer[i].nbytes = 0;
        m_buffer[i].flags = 0;
        m_buffer[i].id = -1;
      }
    }

    if (m_decoder) {
      AudioDecoder_Close(m_decoder);
      m_decoder = NULL;
    }
  }

  if (doubleHeader.dbhBufferPtr[0]) {
    mem_free(doubleHeader.dbhBufferPtr[0]);
    doubleHeader.dbhBufferPtr[0] = NULL;
  }
  if (doubleHeader.dbhBufferPtr[1]) {
    mem_free(doubleHeader.dbhBufferPtr[1]);
    doubleHeader.dbhBufferPtr[1] = NULL;
  }

  m_playcount = 0;
  m_nbufs = 0;
  m_state = STRM_INVALID;
}

// has stream finished with its readahead of current loop?
bool AudioStream::ReadAheadFinishedLoop() {
  if (m_readahead_finished_loop) {
    m_readahead_finished_loop = false;
    return true;
  }

  return false;
}

// are we still reading from disk?
bool AudioStream::ReadAhead() { return m_readahead; }

bool AudioStream::IsReady() {
  return true;
  if (m_state == STRM_STOPPED) {
    if (!m_readahead || m_fbufidx == (STRM_BUFCOUNT - 1)) {
      return true;
    }
  }
  return false;
}

//////////////////////////////////////////////////////////////////////////////

bool AudioStream::ReopenDigitalStream(ubyte fbufidx, int nbufs) {
  const tOSFDigiHdr *digihdr = (const tOSFDigiHdr *)m_archive.StreamHeader();
  int bytesize, granularity;
  long sample_count;
  unsigned channels;

  m_bytesleft = m_archive.StreamLength();
  m_readahead = false;
  mprintf((1, "reopen: %d total length %d\n", m_curid, m_bytesleft));
  // free current decoder.
  if (m_decoder) {
    AudioDecoder_Close(m_decoder);
    m_decoder = NULL;
  }

  // instatiate decompression facility or use raw source data
  if (m_archive.StreamComp() == OSF_DIGIACM_STRM) {
    unsigned sample_rate;

    m_decoder = Create_AudioDecoder(ADecodeFileRead, this, &channels, &sample_rate, &sample_count);
    if (!m_decoder) {
      AudioDecoder_Close(m_decoder);
      m_decoder = NULL;
      return false;
    }
    //		m_total_size = sample_count;
  } else {
    return false;
  }

  //	convert bufsize to true bufsize (bufsize = samples per measure for now.)
  switch (m_archive.StreamFormat()) {
  case SAF_8BIT_M:
    granularity = 1;
    break;
  case SAF_8BIT_S:
    granularity = 2;
    bytesize = digihdr->measure << 1;
    break;
  case SAF_16BIT_M:
    granularity = 2;
    bytesize = digihdr->measure << 1;
    break;
  case SAF_16BIT_S:
    granularity = 4;
    bytesize = digihdr->measure << 2;
    break;
  }

  //	bufsize is bytes per measure.  now scale according to memory requirements.
  //	clear out buffer list
  if (channels == 0 || channels > 2) {
    // weird, faulty osf
    mprintf((0, "STRM[%d]: Illegal OSF (no channels?): %d.\n", m_curid, channels));
    return false;
  }

  long bytes_per_buf = (SAMPLES_PER_STREAM_BUF * granularity);
  long filelen = (sample_count / channels) * granularity;
  int nbuffers = filelen / bytes_per_buf;
  if (nbuffers >= 0 && nbuffers <= 1) {
    if (filelen > 0) {
      bytes_per_buf = (SAMPLES_PER_STREAM_SHORT_BUF * granularity);
      nbuffers = filelen / bytes_per_buf;
      if (nbuffers == 0)
        nbuffers = 1;
    } else {
      mprintf((0, "STRM[%d]: Bad stream length %d\n", m_curid, filelen));
      return false;
    }
  }

  mprintf((1, "Reopen: nbuffers %d of %d each\n", nbuffers, bytes_per_buf));

  m_playbytestotal = filelen;
  m_playbytesleft = filelen;
  m_fbufidx = fbufidx;
  m_sbufidx = 0;

  // Get the file size, and figure out an ideal buffer size
  // so our looping sounds don't have an odd silence in the middle
  m_measure_time = (digihdr->measure / 22050.0f);
  m_bufsize = (filelen / nbuffers);

  // figure out if we still need to be 4-byte aligned.
  granularity = granularity >> 1;
  if (granularity) {
    if (m_bufsize % 4) {
      m_bufsize = m_bufsize + (4 - (m_bufsize % 4));
      if (m_bufsize % 4) {
        Int3();
      }
    }
  }
  mprintf((1, "reopen: %d Using buffer size of %d\n", m_curid, m_bufsize));

  // mark stream as not done.
  m_readahead = true;
  m_readahead_finished_loop = false;

  if (!nbufs) {
    // dont set m_readahead, because readahead only told this function whether it was going to do
    // the read of buffers now, or let UpdateData do it later.
    return true;
  }

  // fill buffers.
  nbufs--;

  while (!CHECK_FLAG(m_buffer[m_fbufidx].flags, STRM_BUFF_USED) && nbufs >= 0 && m_readahead) {
    // if our stream's current id does not match the streaming buffer's id, then we need to reallocate
    // the stream buffer with the new memory size
    if (m_buffer[m_fbufidx].id != (int)m_curid) {
      if (m_buffer[m_fbufidx].data) {
        mem_free(m_buffer[m_fbufidx].data);
      }
      m_buffer[m_fbufidx].data = (ubyte *)mem_malloc(m_bufsize);
    }
    m_buffer[m_fbufidx].nbytes = AudioStream::ReadFileData(m_fbufidx, m_bufsize);
    m_buffer[m_fbufidx].flags = 0;
    m_buffer[m_fbufidx].flags |= STRM_BUFF_USED;
    m_buffer[m_fbufidx].id = m_thisid;

    m_playbytesleft -= m_buffer[m_fbufidx].nbytes;

    mprintf((1, "reopen: STRM[%d] m_buffer[%d] 0x%X bytes readin 0x%x left %X\n", this->m_curid, m_fbufidx, m_bufsize,
             m_playbytesleft, m_buffer[m_fbufidx].flags));

    if (m_playbytesleft <= (m_bufsize / 4)) {
      mprintf((0, "STRM[%d]: ", m_curid));
      if (m_buffer[m_fbufidx].nbytes == 0) {
        memset(m_buffer[m_fbufidx].data, 0, 4);
        m_buffer[m_fbufidx].nbytes = 4;
        mprintf((0, "making empty buffer and"));
      }
      mprintf((0, "TERMINAL buffer.\n"));
      m_buffer[m_fbufidx].flags |= STRM_BUFF_TERMINAL;
      m_readahead = false;
      m_readahead_finished_loop = true;
    }
    m_nbufs++;

    m_fbufidx = (m_fbufidx + 1) % STRM_BUFCOUNT;
    // T2		m_fbufidx = (m_fbufidx)?0:1;
  }

  // readjust file buffer index down so that it matches the CURRENT file index, not the next one.
  if (m_fbufidx == 0)
    m_fbufidx = STRM_BUFCOUNT - 1;
  else
    m_fbufidx--;

  return true;
}

///////////////////////////////////////////////////////////////////////////////
//	plays a stream
bool AudioStream::Play(bool start_on_frame) {
  //	call low level stream manager. - samir
  int sflag = SIF_STREAMING_16_M, i;
  bool looped = false;

  mprintf((1, "Play: STRM[%d]: called w sof %d\n", this->m_curid, start_on_frame));

  if (m_state == STRM_INVALID) {
    return false;
  }

  //	play stream if there's room to.
  if (!ActivateStream(this))
    return false;

  switch (m_archive.StreamFormat()) {
  case SAF_8BIT_M:
    sflag = SIF_STREAMING_8_M;
    break;
  case SAF_8BIT_S:
    sflag = SIF_STREAMING_8_S;
    break;
  case SAF_16BIT_M:
    sflag = SIF_STREAMING_16_M;
    break;
  case SAF_16BIT_S:
    sflag = SIF_STREAMING_16_S;
    break;
  default:
    Int3();
    return false;
  }

  // reset soft stopping.
  m_stopnextmeasure = false;
  m_curmeasure = 0;
  m_stopflag = NULL;

  if (m_state != STRM_STOPPED) {
    mprintf((1, "Play: STRM[%d]: Not Opened stop???\n", this->m_curid));
    AudioStream::Stop();
  }
  if (m_playcount > 0) {
    mprintf((1, "Play: STRM[%d]: m_playcount > 0 resetting!!\n", this->m_curid));
    AudioStream::Reset();
  }

  m_playcount++;

  m_measure_timer = 0.0f;
  m_last_frametime = timer_GetTime();

  //	check for terminal and if loopcount != 0 then specify terminal as looping
  if (m_loopcount != 0) {
    for (i = 0; i < STRM_BUFCOUNT; i++) {
      if (CHECK_FLAG(m_buffer[i].flags, STRM_BUFF_USED)) {
        if (CHECK_FLAG(m_buffer[i].flags, STRM_BUFF_TERMINAL)) {
          if (m_loopcount == -1) {
            m_buffer[m_fbufidx].flags |= STRM_BUFF_LOOPEND;
            looped = true;
            AudioStream::Reset();
          } else if (m_loopcount > 0) {
            m_buffer[m_fbufidx].flags |= STRM_BUFF_LOOPEND;
            looped = true;
            AudioStream::Reset();
            m_loopcount--;
          }
          m_readahead_finished_loop = true;
        }
      }
    }
  }

  // invoke low level sound system.
  memset(&m_playinfo, 0, sizeof(m_playinfo));
#if 0
	m_playinfo.samples_per_22khz_sample = 1.0;
	m_playinfo.left_volume = m_playinfo.right_volume = m_volume;
#endif
  m_playinfo.sample_skip_interval = 0;
  m_playinfo.m_stream_cback = AudioStreamCB;
  m_playinfo.m_stream_data = m_buffer[m_sbufidx].data;
  m_playinfo.m_stream_format = sflag;
  m_playinfo.m_stream_size = m_buffer[m_sbufidx].nbytes;
  m_playinfo.m_stream_handle = m_streamindex;
  m_playinfo.m_stream_bufsize = m_bufsize;
  m_playinfo.user_data = this;
  m_playinfo.priority = SND_PRIORITY_CRITICAL; // this stream must play.

  m_start_on_frame = start_on_frame;
  m_start_on_frame_looped = looped;

  mprintf((1, "Play: STRM[%d]: PlayStream sof %d\n", this->m_curid, m_start_on_frame));
  if (!m_start_on_frame) {
    mprintf((1, "play: size %d bufsize %d\n", m_playinfo.m_stream_size, m_playinfo.m_stream_bufsize));
    m_llshandle = AudioStream::PlayStream(&m_playinfo);
    mprintf((1, "Play: STRM[%d]: PlayStream handle %d\n", this->m_curid, m_llshandle));

    if (m_llshandle > -1) {
      // for one buffer samples, prepare to stop now so that the one and only one buffer gets called.
      //	looped one buffer samples dont do this though...
      if (m_nbufs == 1 && !looped) {
        m_state = STRM_STOPPING;
        mprintf((1, " not looped so stoping IMEDIATLY\n"));
      } else {
        m_state = STRM_PLAYING;
        mprintf((1, "looped so start playing\n"));
      }
      return true;
    }
  }
  return true;
}

bool AudioStream::IsPlaying() {
  SCStatus channel_status;
  SndChannelStatus(strm_channel, sizeof(SCStatus), &channel_status);

  return (channel_status.scChannelBusy);
}

// pauses a stream
void AudioStream::Pause() {
  if (m_state != STRM_PLAYING) {
    return;
  }

  m_state = STRM_PAUSED;

  SndCommand sndCommand;
  OSErr err;

  sndCommand.cmd = pauseCmd;
  sndCommand.param1 = 0;
  sndCommand.param2 = 0L;
  err = SndDoImmediate(strm_channel, &sndCommand);
  if (err)
    mprintf((1, "stream channelis corrupt: pause\n"));

  mprintf((0, "Paused stream\n"));
}

// pauses a stream
void AudioStream::Resume() {
  if (m_state != STRM_PAUSED) {
    return;
  }

  SndCommand sndCommand;
  OSErr err;
  sndCommand.cmd = resumeCmd;
  sndCommand.param1 = 0;
  sndCommand.param2 = 0L;
  err = SndDoImmediate(strm_channel, &sndCommand);
  if (err)
    mprintf((1, "stream channelis corrupt: resume\n"));

  int sflag = SIF_STREAMING_16_M;
  switch (m_archive.StreamFormat()) {
  case SAF_8BIT_M:
    sflag = SIF_STREAMING_8_M;
    break;
  case SAF_8BIT_S:
    sflag = SIF_STREAMING_8_S;
    break;
  case SAF_16BIT_M:
    sflag = SIF_STREAMING_16_M;
    break;
  case SAF_16BIT_S:
    sflag = SIF_STREAMING_16_S;
    break;
  }

  memset(&m_playinfo, 0, sizeof(m_playinfo));
#if 0
	m_playinfo.samples_per_22khz_sample = 1.0;
 	m_playinfo.left_volume = m_playinfo.right_volume = m_volume;
#endif
  m_playinfo.sample_skip_interval = 0;
  m_playinfo.m_stream_cback = AudioStreamCB;
  m_playinfo.m_stream_data = m_buffer[m_sbufidx].data;
  m_playinfo.m_stream_format = sflag;
  m_playinfo.m_stream_size = m_buffer[m_sbufidx].nbytes;
  m_playinfo.m_stream_handle = m_streamindex;
  m_playinfo.m_stream_bufsize = m_bufsize;
  m_playinfo.user_data = this;
  m_playinfo.priority = SND_PRIORITY_CRITICAL; // this stream must play.

  mprintf((1, "play: size %d bufsize %d\n", m_playinfo.m_stream_size, m_playinfo.m_stream_bufsize));

  for (int i = 0; i <= 1; ++i) {
    doubleHeader.dbhBufferPtr[i]->dbUserInfo[0] = (long)&m_playinfo;
    AudioStream::MyDoubleBackStart(strm_channel, doubleHeader.dbhBufferPtr[i]); // initialize the buffers
  }
  err = SndPlayDoubleBuffer(strm_channel, &doubleHeader);

  m_state = STRM_PLAYING;
  mprintf((1, "Resumed stream (%d)\n", m_llshandle));
}

// causes a rewind to start of stream, if on_measure is true, stop occurs when measure ends
void AudioStream::Stop(bool on_measure, int *stop_flag) {
  SndCommand sndCommand;
  OSErr err;

  m_start_on_frame = false;

  if (strm_channel == 0)
    return;
  if (m_state == STRM_INVALID) {
    if (stop_flag) {
      *stop_flag = true;
    }
    return;
  }

  if (m_state != STRM_STOPPED) {
    if (!on_measure) {
      if (m_state == STRM_PLAYING || m_state == STRM_PAUSED || m_state == STRM_STOPPING) {

        sndCommand.cmd = flushCmd;
        sndCommand.param1 = 0;
        sndCommand.param2 = 0L;
        err = SndDoImmediate(strm_channel, &sndCommand);

        sndCommand.cmd = quietCmd;
        sndCommand.param1 = 0;
        sndCommand.param2 = 0L;
        err = SndDoImmediate(strm_channel, &sndCommand);

        m_state = STRM_STOPPED;
        mprintf((1, "Stop: STRM[%d]\n", m_curid));
        m_readahead = m_readahead_finished_loop = true;
        DeactivateStream(this);
        m_llshandle = -1;
        m_curmeasure = 0;
      }
    } else {
      //	handle soft stopping.
      m_stopflag = stop_flag;
      if (m_stopflag) {
        *m_stopflag = false;
      }
      m_stopnextmeasure = on_measure;
    }
  } else {
    DeactivateStream(this);
    m_llshandle = -1;
    m_curmeasure = 0;
    if (m_stopflag) {
      *m_stopflag = true;
    }
  }
}

// performs a clean stop and play when switching to next stream
void AudioStream::Reset() {
  m_archive.Rewind();
  m_bytesleft = m_archive.StreamLength();
  m_curmeasure = 0;
  m_readahead = true;
  m_playbytesleft = m_playbytestotal;

  if (m_decoder) {
    unsigned channels;
    unsigned sample_rate;
    long sample_count;

    AudioDecoder_Close(m_decoder);
    m_decoder = Create_AudioDecoder(ADecodeFileRead, this, &channels, &sample_rate, &sample_count);
  }
}

//////////////////////////////////////////////////////////////////////////////
// invoked by AudioStreamCB.
void *AudioStream::StreamCallback(int *size) {
  void *data = NULL;

  ubyte nextbuffer = (m_sbufidx + 1) % STRM_BUFCOUNT;
  //	mprintf((0, "callback: m_state %d flag %d %d\n", m_state,m_buffer[0].flags, m_buffer[1].flags));

  if (m_state == STRM_PAUSED) {
    //		mprintf((0, "callback: %d paused\n", m_curid));
    *size = 1;
    m_buffer[0].flags = STRM_BUFF_USED;
    m_buffer[1].flags = STRM_BUFF_USED;
    m_buffer[0].nbytes = m_bufsize;
    m_buffer[1].nbytes = m_bufsize;
    return NULL;
  }

  // we're not done yet.
  // adjust sound buffer to the next buffer
  if (m_state == STRM_STOPPED || m_state == STRM_STOPPING) {
    // mark played buffer as unused.
    m_buffer[m_sbufidx].flags = 0;
    m_buffer[m_sbufidx].nbytes = 0;
    *size = 0;
    return NULL;
  }

  if (!CHECK_FLAG(m_buffer[nextbuffer].flags, STRM_BUFF_USED)) {
    //		mprintf((0, "**> stream %d (nextbuffer %d) (flags %x) unused stoping!! \n", m_sbufidx, nextbuffer,
    // m_buffer[nextbuffer].flags)); 	mprintf((0, "STRM[%d]: Playing onetime buffer?\n",m_curid));

    m_state = STRM_STOPPED;
    *size = 0;
    return NULL;
  }

  // mark played buffer as unused.
  m_buffer[m_sbufidx].flags = 0;
  m_buffer[m_sbufidx].nbytes = 0;

  m_sbufidx = nextbuffer;

  //	increment measure count if we've entered a new measure.
  //	if stop measure has been flagged, don't return new valid data.
  //	make sure and lock mutex, because we're modifying these contents when the caller could
  //	do the same.

  // if we reached the last buffer in an opened stream, and we're not switching prepare to stop
  //	if we are switching, release the switch lock and continue
  //	otherwise just continue.
  data = (void *)m_buffer[m_sbufidx].data;
  *size = m_buffer[m_sbufidx].nbytes;
  //	mprintf((0, "cb size %x\n", m_buffer[m_sbufidx].nbytes));
  //	mprintf((0, "callback: m_buffer[%d] nbytes 0x%x data 0x%X\n", m_sbufidx, m_buffer[m_sbufidx].nbytes,
  // m_buffer[m_sbufidx].data));

  if (CHECK_FLAG(m_buffer[m_sbufidx].flags, STRM_BUFF_TERMINAL)) {
    //		mprintf((0, "STRMAUD: reached end of stream (%d bytes).\n", (*size)));
    if (!CHECK_FLAG(m_buffer[m_sbufidx].flags, STRM_BUFF_LOOPEND)) {
      m_state = STRM_STOPPING;
      //			mprintf((0, "callback: flags == STRM_BUFF_TERMINAL stoping NOT loopend\n"));
    }
    if (m_stopnextmeasure) {
      m_state = STRM_STOPPING;
      //			mprintf((0, "callback: flags == STRM_BUFF_TERMINAL stoping stopnexmesure\n"));
    }
  }

  if ((*size) == 0) {
    //	mprintf((0, "STRM[%d]: Used buffer has 0 bytes!\n", m_curid));
    m_state = STRM_STOPPING;
    data = NULL;
  }

  return data;
}

// reads in decompressed raw data.
int AudioStream::ReadFileData(int buf, int len) {
  if (!m_archive.Opened())
    return 0;

  if (m_decoder) {
    // We have a compressed stream
    return AudioDecoder_Read(m_decoder, m_buffer[buf].data, len);
  } else {
    // We have a non-compressed stream
    if (len > m_bytesleft) {
      len = m_bytesleft;
      m_bytesleft = 0;
    } else
      m_bytesleft -= len;

    return m_archive.Read(m_buffer[buf].data, len);
  }
}

// reads in decompressed raw data.
int AudioStream::ReadFileDirect(char *buf, int len) {
  if (!m_archive.Opened())
    return 0;

  if (m_decoder) {
    // We have a compressed stream
    return AudioDecoder_Read(m_decoder, buf, len);
  } else {
    // We have a non-compressed stream
    if (len > m_bytesleft) {
      len = m_bytesleft;
      m_bytesleft = 0;
    } else
      m_bytesleft -= len;

    return m_archive.Read((unsigned char *)buf, len);
  }
}

// TURN OFF OPTIMIZATIONS HERE.  Placement of instructions is VERY IMPORTANT here.  synched with StreamCallback,
//	which also has optimizations off.
// updates file buffers
void AudioStream::UpdateData() {

  int nextbuffer = ((m_fbufidx + 1) % STRM_BUFCOUNT);

  //	mprintf((1, "update: nextbuf %d flags %d\n", nextbuffer, m_buffer[nextbuffer].flags));
  //	mprintf((0, "UD proc_sw %d m_state %d flags %d\n", proc_sw, m_state, m_buffer[nextbuffer].flags));
  //	check if are on a measure boundary for current stream.  if so, then check if we have a next request pending
  if (CHECK_FLAG(m_buffer[nextbuffer].flags, STRM_BUFF_USED)) {
    return;
  }
  // quit out if we can.
  if (nextbuffer == m_sbufidx) {
    return;
  }

  // do read!
  //	READ DATA INTO BUFFER.  UPDATE BYTES LEFT PER MEASURE, ETC.
  if (m_readahead) {
    // ok update the next buffer with data
    m_fbufidx = nextbuffer;
    //	mprintf((0,"%c",m_fbufidx+'a'));

    // if our stream's current id does not match the streaming buffer's id, then we need to reallocate
    // the stream buffer with the new memory size
    if (m_buffer[m_fbufidx].id != (int)m_curid) {
      if (m_buffer[m_fbufidx].data) {
        mem_free(m_buffer[m_fbufidx].data);
      }
      m_buffer[m_fbufidx].data = (ubyte *)mem_malloc(m_bufsize);
    }

    m_buffer[m_fbufidx].nbytes = AudioStream::ReadFileData(m_fbufidx, m_bufsize);
    m_buffer[m_fbufidx].flags = 0;
    m_buffer[m_fbufidx].flags |= STRM_BUFF_USED;
    m_buffer[m_fbufidx].id = (int)m_curid;

    m_playbytesleft -= m_buffer[m_fbufidx].nbytes;

    mprintf((1, "update: %d m_buffer[%d] 0x%X bytes readin 0x%x left flag %d\n", m_curid, m_fbufidx, m_bufsize,
             m_playbytesleft, m_buffer[m_fbufidx].flags));

    if (m_playbytesleft <= (m_bufsize / 4)) {
      mprintf((1, "update: %d last buffer has %d left\n", m_curid, m_playbytesleft));
      if (m_buffer[m_fbufidx].nbytes == 0) {
        memset(m_buffer[m_fbufidx].data, 0, 4);
        m_buffer[m_fbufidx].nbytes = 4;
        mprintf((1, "update: %d PAD last buffer with %d\n", m_curid, m_buffer[m_fbufidx].nbytes));
      }
      m_buffer[m_fbufidx].flags |= STRM_BUFF_TERMINAL;
      m_readahead = false;
      m_readahead_finished_loop = true;
    }

    // looping?
    if (CHECK_FLAG(m_buffer[m_fbufidx].flags, STRM_BUFF_TERMINAL)) {
      mprintf((1, "update: loopcount %d\n", m_loopcount));
      if (m_loopcount == -1) {
        m_buffer[m_fbufidx].flags |= STRM_BUFF_LOOPEND;
        AudioStream::Reset();
      } else if (m_loopcount > 0) {
        m_buffer[m_fbufidx].flags |= STRM_BUFF_LOOPEND;
        AudioStream::Reset();
        m_loopcount--;
      }
      m_readahead_finished_loop = true;
    }
  }
}

int AudioStream::PlayStream(play_information *play_info) {
  ASSERT(play_info != NULL);
  SndDoubleBufferPtr doubleBuffer;

  OSStatus err;

  switch (play_info->m_stream_format) {
  case SIF_STREAMING_8_M:
    doubleHeader.dbhSampleSize = 8;
    doubleHeader.dbhNumChannels = 1;
    break;
  case SIF_STREAMING_8_S:
    doubleHeader.dbhSampleSize = 8;
    doubleHeader.dbhNumChannels = 2;
    break;
  case SIF_STREAMING_16_M:
    doubleHeader.dbhSampleSize = 16;
    doubleHeader.dbhNumChannels = 1;
    break;
  case SIF_STREAMING_16_S:
    doubleHeader.dbhSampleSize = 16;
    doubleHeader.dbhNumChannels = 2;
    break;
  }
  doubleHeader.dbhSampleRate = rate22050hz;
  doubleHeader.dbhCompressionID = notCompressed;
  doubleHeader.dbhPacketSize = 0;

  doubleHeader.dbhDoubleBack = gSndDoubleBackUPP;

  for (int i = 0; i <= 1; ++i) {
    doubleBuffer = (SndDoubleBufferPtr)mem_malloc(sizeof(SndDoubleBuffer) + play_info->m_stream_bufsize);
    if (doubleBuffer == nil)
      return 0;
    //			Debugger();

    doubleBuffer->dbNumFrames = 0;
    doubleBuffer->dbFlags = 0;
    doubleBuffer->dbUserInfo[0] = (long)play_info;

    mprintf((1, "PlayStream bufsize %d\n", play_info->m_stream_bufsize));
    AudioStream::MyDoubleBackStart(strm_channel, doubleBuffer); // initialize the buffers

    doubleHeader.dbhBufferPtr[i] = doubleBuffer;
  }
  err = SndPlayDoubleBuffer(strm_channel, &doubleHeader);
  mprintf((1, "PlayStream:STRM[%d]: channel 0x%X SndPlayDoubleBuffer 0x%X & 0x%X\n", this->m_curid, strm_channel,
           doubleHeader.dbhBufferPtr[0], doubleHeader.dbhBufferPtr[1]));

  strm_channel->userInfo = play_info->m_stream_handle;

  return (play_info->m_stream_handle);
}

void *MyDoubleBackProc(SndChannelPtr channel, SndDoubleBufferPtr dBuffer) {
  play_information *play_info;

  int bytesFromBuffer;
  Ptr dataPtr;

  play_info = (play_information *)dBuffer->dbUserInfo[0];

  if ((dBuffer->dbFlags) & dbLastBuffer)
    return NULL;

  dataPtr = (Ptr)play_info->m_stream_cback(play_info->user_data, play_info->m_stream_handle, &bytesFromBuffer);

  dBuffer->dbFlags = (dBuffer->dbFlags) | dbBufferReady;
  if (dataPtr) {

    memcpy(&dBuffer->dbSoundData[0], dataPtr, bytesFromBuffer);
    //		BlockMove (dataPtr, &dBuffer->dbSoundData[0], bytesFromBuffer);
    dBuffer->dbNumFrames = bytesFromBuffer;
    if (play_info->m_stream_format >= SIF_STREAMING_16_M)
      dBuffer->dbNumFrames = dBuffer->dbNumFrames >> 1;
    if (play_info->m_stream_format == SIF_STREAMING_16_S)
      dBuffer->dbNumFrames = dBuffer->dbNumFrames >> 1;

  } else if (bytesFromBuffer) {
    dBuffer->dbFlags = 0;
    dBuffer->dbNumFrames = 0;
  } else {
    dBuffer->dbFlags = (dBuffer->dbFlags) | dbLastBuffer;
    dBuffer->dbNumFrames = 0;
  }
  return dataPtr;
}

void *AudioStream::MyDoubleBackStart(SndChannelPtr channel, SndDoubleBufferPtr dBuffer) {
  int bytesToCopy;

  play_information *play_info;
  play_info = (play_information *)dBuffer->dbUserInfo[0];

  mprintf((1, "MyDoubleBackStart: bufsize %d\n", play_info->m_stream_bufsize));
  bytesToCopy = play_info->m_stream_bufsize / 2; // split the buffer in half

  memcpy(&dBuffer->dbSoundData[0], play_info->m_stream_data, bytesToCopy);
  //	BlockMove (play_info->m_stream_data, &dBuffer->dbSoundData[   0], bytesToCopy);

  play_info->m_stream_data = (void *)((int)(play_info->m_stream_data) + bytesToCopy);

  dBuffer->dbNumFrames = bytesToCopy;

  if (play_info->m_stream_format >= SIF_STREAMING_16_M)
    dBuffer->dbNumFrames = dBuffer->dbNumFrames / 2;
  if (play_info->m_stream_format == SIF_STREAMING_16_S)
    dBuffer->dbNumFrames = dBuffer->dbNumFrames / 2;

  dBuffer->dbFlags = (dBuffer->dbFlags) | dbBufferReady;

  return NULL;
}

//	Router for stream callbacks.
//		Invoked by sound system
void *AudioStreamCB(void *user_data, int handle, int *size) {
  AudioStream *stream = (AudioStream *)user_data;
  ASSERT(stream);
  //	ASSERT(stream->m_streamindex == handle);
  return stream->StreamCallback(size);
  //	return stream->NewStreamCallback(size);
}

static float Stream_volume = 1.0f;
static float Stream_master_volume = 1.0f;

void StreamVolume(float master_volume) {
  Stream_master_volume = master_volume;
  User_audio_stream.SetVolume(Stream_volume * master_volume);
}

///////////////////////////////////////////////////////////////////////////////
//	decoder
unsigned ADecodeFileRead(void *data, void *buf, unsigned qty) {
  AudioStream *stream = (AudioStream *)data;
  int iqty = (int)qty;

  if (iqty > stream->m_bytesleft) {
    iqty = stream->m_bytesleft;
    stream->m_bytesleft = 0;
  } else
    stream->m_bytesleft -= iqty;

  return stream->m_archive.Read((ubyte *)buf, iqty);
}

int voice_stream = -1;
//	these functions are the 'simplified' stream interface from Jeff (most of this code is from Jeff)
int StreamPlay(const char *filename, float volume, int flags) {
  int retval = -1;
  try {
    flags = 0;
    User_audio_stream.Close();
    User_audio_stream.Open(filename, STRM_OPNF_ONETIME);

    Stream_volume = volume;
    User_audio_stream.SetVolume(Stream_volume * Stream_master_volume);
    User_audio_stream.Play(true); // start this stream on the next frame.

    retval = User_audio_stream.m_streamindex;
    voice_stream = retval;
  } catch (...) {
    return -1;
  }
  return retval;
}

void StreamStop(int handle) {
  if (handle == 0 && voice_stream > -1) {
    if (AudioStream::m_streams[voice_stream])
      AudioStream::m_streams[voice_stream]->Stop();
  } else if (handle > -1) {
    if (AudioStream::m_streams[handle])
      AudioStream::m_streams[handle]->Stop();
  }
  User_audio_stream.Close();
}

int StreamGetSoundHandle(int handle) {
  if (handle > -1) {
    if (AudioStream::m_streams[handle])
      return AudioStream::m_streams[handle]->GetSoundHandle();
    else
      return -1;
  }

  return -1;
}
