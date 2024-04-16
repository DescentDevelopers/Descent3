/*
 * $Logfile: /DescentIII/main/subtitles.h $
 * $Revision: 2 $
 * $Date: 8/28/98 4:20p $
 * $Author: Jeff $
 *
 * Subtitle system for cinematics
 *
 * $Log: /DescentIII/main/subtitles.h $
 *
 * 2     8/28/98 4:20p Jeff
 * got subtitles completly working
 *
 * $NoKeywords: $
 */

#ifndef __SUBTITLES_H_
#define __SUBTITLES_H_

// draw the subtitles for this frame
void SubtDrawSubtitles(int frame_num);
// Shutsdown the subtitle system
void SubtCloseSubtitles();
// Initializes the subtitles for a given movie file
void SubtInitSubtitles(const char *filename);

#endif