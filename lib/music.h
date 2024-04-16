/*
 * $Logfile: /DescentIII/Main/Lib/music.h $
 * $Revision: 21 $
 * $Date: 3/18/99 10:13a $
 * $Author: Samir $
 *
 * Event driven music system
 *
 * $Log: /DescentIII/Main/Lib/music.h $
 *
 * 21    3/18/99 10:13a Samir
 * msuic update.
 *
 * 20    3/03/99 5:08p Samir
 * added slew of debug code: hopefully the library will be revamped after
 * OEM.
 *
 * 19    2/27/99 8:23p Samir
 * fixes to music system to act nicely to sudden and frequent region
 * changes.
 *
 * 18    2/27/99 6:52p Samir
 * added function to get current loop count
 *
 * 17    2/27/99 4:37p Samir
 * return name of loop currently playing.
 *
 * 16    2/26/99 5:26p Samir
 * fixes to streaming audio to reflect fix in direct sound mixer.
 *
 * 15    2/22/99 3:23p Luke
 * upped size of music buffers
 *
 * 14    1/28/99 2:22p Samir
 * simplified music system for D3.
 *
 * 13    12/15/98 10:22a Samir
 * upped max labels.
 *
 * 12    12/11/98 4:03p Samir
 * made stream public for oms_stream.
 *
 * 11    12/10/98 10:12a Samir
 * uses newer streaming audio library
 *
 * 10    12/07/98 11:44a Samir
 * added new themes.
 *
 * 9     12/03/98 12:51p Samir
 * changed WasStopped to HasStopped.
 *
 * 8     11/20/98 5:21p Samir
 * added SEND_STRM_NEXT
 *
 * 7     11/13/98 2:29p Samir
 * added oms_tracklist and newer music code.
 *
 * 6     8/14/98 7:37p Matt
 * Since PSFILENAME_LEN is now considered to be the max length of the
 * string itself (not counting the terminating null), buffers for file
 * name need to be PSFILENAME_LEN+1 bytes long.
 *
 * 5     8/10/98 5:53p Samir
 * improved switching between streams and basic background/combat music.
 *
 * 4     7/28/98 5:43p Samir
 * reorg of music system.
 *
 * 3     7/24/98 5:20p Samir
 * first real music checkin.
 *
 * 2     7/08/98 6:26p Samir
 * took out obsolete code.
 *
 * 2     6/09/97 4:16p Samir
 * RCS check in
 *
 * $NoKeywords: $
 */

#ifndef MUSIC_H
#define MUSIC_H

#include "pstypes.h"
#include "pserror.h"
#include "psclass.h"
#include "streamaudio.h"

///////////////////////////////////////////////////////////////////////////////
//	These are internal constants for the music sequencer

#define OMS_NUM_STRM 2
#define OMS_STRM_QSIZE 8

//	OMS Theme Types
#define OMS_THEME_TYPES 8
#define OMS_THEME_TYPE_NONE 0
#define OMS_THEME_TYPE_INTRO 1
#define OMS_THEME_TYPE_IDLE 2
#define OMS_THEME_TYPE_COMBAT 3
#define OMS_THEME_TYPE_DEATH 4
#define OMS_THEME_TYPE_IDLE_TO_COMBAT 5
#define OMS_THEME_TYPE_COMBAT_TO_IDLE 6

//	OMS Stream commands
#define OMS_STRM_FREE 255  // indicates free stream.
#define OMS_STRM_STOP 0    // stop current stream.
#define OMS_STRM_PLAY 1    // play current stream.
#define OMS_STRM_FADEOUT 2 // fades out current stream in (parm secs).
#define OMS_STRM_FADEIN 3  // plays current stream, fade it in. at (parm secs)
#define OMS_STRM_DELAY 4   // standard delay (parm = time)
#define OMS_STRM_LOAD 5    // loads song (parm = song type).
#define OMS_STRM_PARM 6    // extra parameter for last stream command (parm = parameter)
#define OMS_STRM_SWITCH 7  // switches on next measure of this calling stream to the dominant one.
#define OMS_STRM_NEXT 8    // specifies setting a 'next' request on an opened stream.

//	////////////////////////////////////////////////////////////////////////////
//	generic event item for the queues.
struct oms_q_evt {
  ubyte cmd;
  union {
    float f;
    int i;
    void *p;
  } parm;
};

class OutrageMusicSeq;

//	internal class to the sequencer, handles command-based streams.
class oms_stream {
  friend class OutrageMusicSeq;

  OutrageMusicSeq *m_seq;                // parent sequencer.
  tQueue<oms_q_evt, OMS_STRM_QSIZE> m_q; // queue of stream commands.
  float m_timer, m_timer_init;           // timer managed by user.
  float m_maxvol;                        // sustained max volume of stream (used in fading).
  oms_q_evt m_status;                    // current command status (operation q item)
  int m_stopcount;                       // number of times stopped.

  union {
    int i;
    float f;
    void *p;
  } m_data; // data for internal ops.
  union {
    int i;
    float f;
    void *p;
  } m_data2; // data for internal ops.

private:
  inline void STREAM_COMMANDP(ubyte cmd, void *p = NULL) {
    m_status.cmd = cmd;
    m_status.parm.p = p;
  }
  inline void STREAM_COMMANDF(ubyte cmd, float cmdparm = 0) {
    m_status.cmd = cmd;
    m_status.parm.f = cmdparm;
  };
  inline void STREAM_COMMANDI(ubyte cmd, int cmdparm = 0) {
    m_status.cmd = cmd;
    m_status.parm.f = cmdparm;
  };
  inline void STREAM_TIMER(float time_init) {
    m_timer = time_init;
    m_timer_init = time_init;
  };

  bool processCommand();                // processes current command on stream
  void processQLoad(const char *fname); // processes load song q event.

public:
  oms_stream();
  ~oms_stream();

  AudioStream m_stream; // current audio stream playing

public:
  void Process(float frmtime);             // processes a stream's events.
  void Send(oms_q_evt *evt);               // sends an event to the stream.
  void Reset(OutrageMusicSeq *seq = NULL); // reset stream.
  void SetVolume(float vol);               // volume.
  int GetPlayRequestCount();               // tells how many play events send are pending processing.

  // event senders
public:
  void SEND_STRM_LOAD(const char *fname);
  void SEND_STRM_FADEOUT(float time);
  void SEND_STRM_FADEIN(float time);
  void SEND_STRM_STOP();
  void SEND_STRM_PLAY(float vol, ubyte count);
  void SEND_STRM_FREE();
  void SEND_STRM_SWITCH(bool *switch_flag); // when stream is done, changes value of flag.
  void SEND_STRM_NEXT(const char *fname, float vol, ubyte count,
                      bool *switch_flag); // specifies next stream to play in current stream.

  // return values (after process is called, this is updated.)
public:
  bool m_valid_result; // is this a valid returned result?
  oms_q_evt m_result;  // result event returned from process.
};

class oms_tracklist {
  char **m_fnamelist; // filename list
  char **m_symlist;   // symbolic list
  short m_numtracks;  // number of tracks
  short m_maxtracks;
  bool m_init; // determines if initialized

public:
  oms_tracklist();
  ~oms_tracklist();

  void init(short maxtracks); // initializes track list system
  void free();                // frees track list system
  void reset();               // resets track list to 0

  bool add(const char *fname, const char *sym);
  const char *get(const char *sym); // returns a track filename.
};

//	////////////////////////////////////////////////////////////////////////////
//	The Outrage Music Sequencer v1.0
//		playback songs with special FX.
//		this is an intermediate between song playback and event driven music.
//		the calling application must act as the 'brain' in most cases.

typedef short tMusicVal; // used for paramaters

inline float NORMALIZE_MUSICVAL(tMusicVal val) { return ((float)val) / 255.0f; }
inline tMusicVal QUANTIZE_MUSICVAL(float fval) { return (fval * 255); }

#define MAX_MUSIC_INSTRUCTIONS 1280
#define MAX_MUSIC_STRLEN 2048
#define N_MUSIC_REGS 16

//	OMS Events
#define OMSEVT_MAKE(_evt, _priority) (((_evt) << 2) + (_priority & 0x03))
#define OMSEVT_PRIORITY(_evt) ((_evt)&0x3)

#define OMS_EVT_NONE OMSEVT_MAKE(0, 0)
#define OMS_EVT_SONGENDED OMSEVT_MAKE(32, 0)  // music completed (parm tells us what theme)
#define OMS_EVT_LOOPENDING OMSEVT_MAKE(31, 0) // a loop is ending.

// class
class OutrageMusicSeq {
  friend class oms_stream; // stream should know about data here.

  //	music scripting system.
  struct music_ins {
    ubyte cmd; // OMFCMD_
    union {
      tMusicVal num;
      char *str;
    } opr; // operand is either numeric or a string
  };

  struct music_stream {
    oms_stream *strm;     // attached stream.
    oms_stream *old_strm; // previous stream.
    const char *loop_name;
    const char *pending_loop_name; // loop name preparing to play.
    short region;                  // region id.
    short type;
    music_ins *ins; // intro, combat, etc. section
    short last_ip;  // last ip value
    short ip;
    tMusicVal b_reg;       // branch register
    tMusicVal c_reg;       // compare regs
    tMusicVal p_reg;       // play count register.
    tMusicVal i_reg;       // iteration register
    const char *ln_reg;    // loopname register
    bool request_stop;     // we're requesting to stop at end of song.
    bool ifi_block;        // are we executing an ifi block?
    bool immediate_switch; // immediate switch to new song.
    bool stream_idle;      // are we in a next switch?
    bool error;
    ubyte pad[3];
  };

  // data
  tMusicVal m_registers[N_MUSIC_REGS]; // registers
  music_stream *m_active_song, *m_pending_song, *m_playing_song;
  oms_stream m_strm[OMS_NUM_STRM]; // stream attached to it.

  bool m_sequencer_run;  // do we run the sequencer.
  bool m_sequencer_init; // is sequencer initialized.
  int m_dominant_strm;   // which stream is dominant.
  float m_timer;         // master timer.
  float m_mastervol;     // master volume.

  oms_tracklist m_tracklist;        // sequencer track list.
  tList<music_stream> m_music_list; // list of tracks.

  short m_curregion; // current region.

private:
  //	Music list management
  music_stream *AddToList(short region, short theme_type, int n_inst, const music_ins *ins);
  void FreeList();
  music_stream *GetSong(short region, short theme_type);
  bool LoadTheme(const char *file); // takes a filename containing oms data.

  void ExecScript(music_stream *strm);

  inline int DOMINANT_STRM_ADJUST() {
    int nstrm = m_dominant_strm + 1;
    if (nstrm == OMS_NUM_STRM)
      nstrm = 0;
    return nstrm;
  };

private:
  music_ins *m_ins_buffer; // music instruction buffer
  music_ins *m_ins_curptr; // current pointer to free instruction buffer.
  char *m_str_buffer;      // string buffer.
  char *m_str_curptr;      // current pointer to free string

  music_ins *alloc_and_copy_ins(int len, const music_ins *ins);
  char *alloc_and_copy_str(const char *str);

public:
  OutrageMusicSeq();
  ~OutrageMusicSeq();

  //	takes a OMS theme file.
  bool Init(const char *theme_file);
  void Shutdown();

  bool IsSequencerRunning() const { return m_sequencer_init && m_sequencer_run; }

  //	starts the sequencer (usually at the beginning of a level)
  void Start();

  //	stops the sequencer, flushes events
  void Stop();

  //	do a frame
  void Frame(float frame_time);

  //	set volume of sequencer
  void SetVolume(float vol);

  // start a song, stopping the old either cleanly (on measure) or abruptly.
  void StartSong(int song, bool clean_switch);

  // stops song, cold.
  void StopSong();

  //	pauses
  void Pause();

  // resumes
  void Resume();

  // get and set user defined (theme file) parameters
  void SetRegister(int parm, tMusicVal val);
  tMusicVal GetRegister(int parm);

  // region setting
  void SetCurrentRegion(short region);
  short GetCurrentRegion() const;

  // gets current region PLAYING, not PENDING like above.
  short GetPlayingRegion() const;

  // get current loop playing
  const char *GetCurrentLoopName(int *loop_count);

public:
  tQueue<oms_q_evt, 4> m_output_q; // output events
};

#endif