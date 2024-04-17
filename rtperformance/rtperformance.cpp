/*
 * $Logfile: /DescentIII/Main/rtperformance/rtperformance.cpp $
 * $Revision: 9 $
 * $Date: 10/21/99 3:21p $
 * $Author: Kevin $
 *
 * Run time performance lib
 *
 * $Log: /DescentIII/Main/rtperformance/rtperformance.cpp $
 *
 * 9     10/21/99 3:21p Kevin
 * Mac merge
 *
 * 8     5/07/99 6:21p Jeff
 * hooked up fvi to rtperformance
 *
 * 7     4/16/99 4:04a Jeff
 * fixed up for non-window's builds
 *
 * 6     4/16/99 1:59a Jeff
 * ifdef out windows.h for now windows builds
 *
 * 5     10/19/98 7:51p Kevin
 * performance testing
 *
 * 4     10/19/98 2:48p Jeff
 * changed define for rtp performance...just define USE_RTP in
 * rtperformance.h in order to enable it
 *
 * 3     10/17/98 6:05p Jeff
 * it helps to write the heading to file
 *
 * 2     10/17/98 5:58p Jeff
 * put in heading
 *
 * $NoKeywords: $
 */

#if defined(WIN32)
#include <windows.h>
#endif

#include "rtperformance.h"
#include "pstypes.h"
#include "mono.h"
#include "descent.h"
#include "manage.h"
#include "ddio.h"
#include "CFILE.H"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

float rtp_startlog_time;

// maximum number of samples before we stop and autoflush
#define MAX_RTP_SAMPLES 3800 // this is a little more than whats needed for 2 minutes at 30fps

//		Internal Global Vars
// ------------------------------
INT64 Runtime_performance_flags = RTI_FRAMETIME;
INT64 Runtime_performance_clockfreq = 0;
INT64 Runtime_performance_frame_counter = 0;
unsigned int Runtime_performance_counter = 0;
unsigned char Runtime_performance_enabled = 0;

tRTFrameInfo RTP_SingleFrame;
#ifdef USE_RTP
tRTFrameInfo RTP_FrameBuffer[MAX_RTP_SAMPLES];
#endif

/*
void rtp_WriteBufferLog
        Writes the buffer of frames to file...
*/
void rtp_WriteBufferLog(void) {
#ifdef USE_RTP
  Runtime_performance_enabled = 1; // make sure it's enabled for the macros
  // determine how many frames to write out
  unsigned int Num_frames;
  unsigned int counter;
  char buffer[4096];

  Num_frames = MIN(Runtime_performance_counter, MAX_RTP_SAMPLES);

  // Open the log file for writing
  ddio_MakePath(buffer, LocalD3Dir, "D3Performance.txt", NULL);
  CFILE *file = cfopen(buffer, "wt");

  if (file) {
    mprintf((0, "RTP: Recording Log\n"));

    strcpy(buffer, "FrameNum,FrameTime,RenderFrameTime,MultiFrameTime,MusicFrameTime,AmbientSoundTime,WeatherFrameTime,"
                   "PlayerFrameTime,DoorwayFrameTime,LevelGoalFrameTime,MatCenFrameTime,ObjectFrameTime,AIFrameAllTime,"
                   "ProcessKeysTime,REN:NumTexturesUploaded,REN:PolysDrawn,OBJ:CT_FlyingTime,OBJ:CT_AIDoFrameTime,OBJ:"
                   "CT_WeaponFrameTime,OBJ:CT_ExplosionFrameTime,OBJ:CT_DebrisFrameTime,OBJ:CT_SplinterFrameTime,OBJ:"
                   "MT_PhsyicsFrameTime,OBJ:MT_WalkingFrame,OBJ:MT_ShockWaveTime,OBJ:DoEffectTime,OBJ:MovePlayerTime,"
                   "OBJ:D3XIntervalTime,OBJ:ObjLightTime,FRAME:NormalEventTime,AnimCycle,VisEffectMoveAll,"
                   "DoPhysLinkedFrame,ObjDoFrame,NumFVICalls,FVITime");
    cf_WriteString(file, buffer);

    // Loop through all the frames, and write out the data for each frame
    for (counter = 0; counter < Num_frames; counter++) {
      tRTFrameInfo *fi = &RTP_FrameBuffer[counter];
      double renderframe_time;
      double multiframe_time;
      double musicframe_time;
      double ambsound_frame_time;
      double weatherframe_time;
      double playerframe_time;
      double doorframe_time;
      double levelgoal_time;
      double matcenframe_time;
      double objframe_time;
      double aiframeall_time;
      double processkeys_time;
      double ct_flying_time;
      double ct_aidoframe_time;
      double ct_weaponframe_time;
      double ct_explosionframe_time;
      double ct_debrisframe_time;
      double ct_splinterframe_time;
      double mt_physicsframe_time;
      double mt_walkingframe_time;
      double mt_shockwave_time;
      double obj_doeffect_time;
      double obj_move_player_time;
      double obj_d3xint_time;
      double obj_objlight_time;
      double normalevent_time;
      double cycle_anim;
      double vis_eff_move;
      double phys_link;
      double obj_do_frm;
      double fvi_time;

      RTP_CLOCKSECONDS(fi->renderframe_time, renderframe_time);
      RTP_CLOCKSECONDS(fi->multiframe_time, multiframe_time);
      RTP_CLOCKSECONDS(fi->musicframe_time, musicframe_time);
      RTP_CLOCKSECONDS(fi->ambsound_frame_time, ambsound_frame_time);
      RTP_CLOCKSECONDS(fi->weatherframe_time, weatherframe_time);
      RTP_CLOCKSECONDS(fi->playerframe_time, playerframe_time);
      RTP_CLOCKSECONDS(fi->doorframe_time, doorframe_time);
      RTP_CLOCKSECONDS(fi->levelgoal_time, levelgoal_time);
      RTP_CLOCKSECONDS(fi->matcenframe_time, matcenframe_time);
      RTP_CLOCKSECONDS(fi->objframe_time, objframe_time);
      RTP_CLOCKSECONDS(fi->aiframeall_time, aiframeall_time);
      RTP_CLOCKSECONDS(fi->processkeys_time, processkeys_time);
      RTP_CLOCKSECONDS(fi->ct_flying_time, ct_flying_time);
      RTP_CLOCKSECONDS(fi->ct_aidoframe_time, ct_aidoframe_time);
      RTP_CLOCKSECONDS(fi->ct_weaponframe_time, ct_weaponframe_time);
      RTP_CLOCKSECONDS(fi->ct_explosionframe_time, ct_explosionframe_time);
      RTP_CLOCKSECONDS(fi->ct_debrisframe_time, ct_debrisframe_time);
      RTP_CLOCKSECONDS(fi->ct_splinterframe_time, ct_splinterframe_time);
      RTP_CLOCKSECONDS(fi->mt_physicsframe_time, mt_physicsframe_time);
      RTP_CLOCKSECONDS(fi->mt_walkingframe_time, mt_walkingframe_time);
      RTP_CLOCKSECONDS(fi->mt_shockwave_time, mt_shockwave_time);
      RTP_CLOCKSECONDS(fi->obj_doeffect_time, obj_doeffect_time);
      RTP_CLOCKSECONDS(fi->obj_move_player_time, obj_move_player_time);
      RTP_CLOCKSECONDS(fi->obj_d3xint_time, obj_d3xint_time);
      RTP_CLOCKSECONDS(fi->obj_objlight_time, obj_objlight_time);
      RTP_CLOCKSECONDS(fi->normalevent_time, normalevent_time);
      RTP_CLOCKSECONDS(fi->cycle_anim, cycle_anim);
      RTP_CLOCKSECONDS(fi->vis_eff_move, vis_eff_move);
      RTP_CLOCKSECONDS(fi->phys_link, phys_link);
      RTP_CLOCKSECONDS(fi->obj_do_frm, obj_do_frm);
      RTP_CLOCKSECONDS(fi->fvi_time, fvi_time);

      sprintf(buffer,
              "%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%d,%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f "
              "%d %lld",
              (int)fi->frame_num, fi->frame_time, renderframe_time, multiframe_time, musicframe_time,
              ambsound_frame_time, weatherframe_time, playerframe_time, doorframe_time, levelgoal_time,
              matcenframe_time, objframe_time, aiframeall_time, processkeys_time, fi->texture_uploads, fi->polys_drawn,
              ct_flying_time, ct_aidoframe_time, ct_weaponframe_time, ct_explosionframe_time, ct_debrisframe_time,
              ct_splinterframe_time, mt_physicsframe_time, mt_walkingframe_time, mt_shockwave_time, obj_doeffect_time,
              obj_move_player_time, obj_d3xint_time, obj_objlight_time, normalevent_time, cycle_anim, vis_eff_move,
              phys_link, obj_do_frm, fi->fvi_calls, fi->fvi_time);

      cf_WriteString(file, buffer);
    }

    // Close the log file
    cfclose(file);
  } else
    mprintf((0, "RTP: Unable to open log for writing\n"));
#endif
}

/*
void rtp_RecordFrame
        Calling this will record the data of the frame into the internal log, and prepare for
        the next frame
*/
void rtp_RecordFrame(void) {
#ifdef USE_RTP
  if (Runtime_performance_enabled) {
    //	do our saving of information
    // --------------------------------

    RTP_SingleFrame.frame_num = Runtime_performance_frame_counter; // save the frame num

    // Copy everything into the buffer, and advance the buffer
    memcpy(&RTP_FrameBuffer[Runtime_performance_counter], &RTP_SingleFrame, sizeof(tRTFrameInfo));

    Runtime_performance_counter++;

    if (Runtime_performance_counter >= MAX_RTP_SAMPLES) {
      // uh oh, we hit the end of our samples, flush the buffer and stop
      rtp_StopLog();
    }

    //	reset our global struct to zero everything out
    // ------------------------------------------------
    memset(&RTP_SingleFrame, 0, sizeof(tRTFrameInfo));
  }

  // Update the frame counter to the next frame, always
  Runtime_performance_frame_counter++;
#endif
}

//		External Functions
// -----------------------------
/*
void rtp_Init
        This function initilizes the runtime performance system so it's ready to be used
*/
void rtp_Close(void);
void rtp_Init(void) {
#ifdef USE_RTP
  Runtime_performance_flags = 0;
  Runtime_performance_clockfreq = 0;
  Runtime_performance_frame_counter = 0;
  Runtime_performance_counter = 0;
  Runtime_performance_enabled = 0;

#ifdef MACINTOSH
  Runtime_performance_clockfreq = 1000000; // micoseconds
#else
  LARGE_INTEGER freq;
  if (!QueryPerformanceFrequency(&freq)) {
    // there is no hi-res clock available....ummmm
    mprintf((0, "RTP: No Hi-Resolution clock available on this system!!!!!!\n"));
    // well, this isn't good...what to do, what to do?
  }

  // Setup the clock freq....we'll need this info later I guess when converting to seconds
  Runtime_performance_clockfreq = freq.QuadPart;
#endif
  rtp_EnableFlags(RTI_FRAMETIME);

  atexit(rtp_Close);
#endif
}

/*
void rtp_Close
        This function shuts down the runtime performance system when the game is done
*/
void rtp_Close(void) {
#ifdef USE_RTP
  if (Runtime_performance_enabled) {
    // Save the log out, since it was currently logging
    rtp_StopLog();
  }
#endif
}

/*
void rtp_EnableFlags
        This function enables the performance profiling of whatever flags you pass in
*/
void rtp_EnableFlags(INT64 flags) {
#ifdef USE_RTP
  Runtime_performance_flags |= flags;
#endif
}

/*
void rtp_DisableFlags
        This function disables the performance profiling of whatever flags you pass in
*/
void rtp_DisableFlags(INT64 flags) {
#ifdef USE_RTP
  Runtime_performance_flags &= ~flags;
#endif
}

/*
void rtp_StartLog
        Calling this function will reset the log and start a new log, recording immediatly
*/
void rtp_StartLog(void) {
#ifdef USE_RTP
  mprintf((0, "RTP: Starting Log\n"));
  Runtime_performance_counter = 0;
  Runtime_performance_enabled = 1;
  memset(&RTP_SingleFrame, 0, sizeof(tRTFrameInfo));
  rtp_startlog_time = timer_GetTime();
#endif
}

/*
void rtp_StopLog
        Calling this function will stop the currently processing log and write it out to
        file.
*/
void rtp_StopLog(void) {
#ifdef USE_RTP
  mprintf((0, "Recorded performance for %f seconds\n", timer_GetTime() - rtp_startlog_time));
  mprintf((0, "RTP: Stopping Log\n"));

  // Save out the log now
  rtp_WriteBufferLog();

  Runtime_performance_enabled = 0;
#endif
}

/*
void rtp_PauseLog
        Calling this function will pause the log recording until rtp_ResumeLog is called
*/
void rtp_PauseLog(void) {
#ifdef USE_RTP
  mprintf((0, "RTP: Pausing Log\n"));
  Runtime_performance_enabled = 0;
#endif
}

/*
void rtp_ResumeLog
        Calling this will resume a paused log, starting at where it left off
*/
void rtp_ResumeLog(void) {
#ifdef USE_RTP
  mprintf((0, "RTP: Resuming Log\n"));
  Runtime_performance_enabled = 1;
#endif
}

//			Internal Functions
// -------------------------------
/*
INT64 rtp_GetClock
        Returns the current hi-resolution clock value...no checking for overflow
*/
INT64 rtp_GetClock(void) {
#ifdef USE_RTP
#ifdef MACINTOSH
  INT64 currentTimeUI = 0;

  // Get the current time in microseconds
  Microseconds((UnsignedWide *)(&currentTimeUI));
  return currentTimeUI;
#else
  LARGE_INTEGER t;
  QueryPerformanceCounter(&t);
  return (INT64)t.QuadPart;
#endif
#else
  return 0;
#endif
}
