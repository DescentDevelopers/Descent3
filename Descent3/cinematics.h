/*
 * $Logfile: /DescentIII/main/cinematics.h $
 * $Revision: 7 $
 * $Date: 3/31/99 11:53a $
 * $Author: Samir $
 *
 *	D3 Cinematics interface with movie player
 *
 * $Log: /DescentIII/main/cinematics.h $
 *
 * 7     3/31/99 11:53a Samir
 * slow opengl code for movies.
 *
 * 6     3/17/99 4:18p Samir
 * redid movie code.
 *
 * 5     2/04/98 12:12p Samir
 * The movie frame interface now has an option to not advance to next
 * frame.
 *
 * 4     2/03/98 6:42p Samir
 * Movies in 3dfx mode.
 *
 * 3     9/19/97 2:34p Samir
 * Added movie frame management support.
 *
 * 2     9/12/97 4:12p Samir
 * first movie library
 *
 * $NoKeywords: $
 */

#ifndef CINEMATICS_H
#define CINEMATICS_H

#include "renderer.h"
#include "bitmap.h"

//	Movie Cinematic
typedef struct tCinematic {
  intptr_t mvehandle;
  int filehandle;
  chunked_bitmap frame_chunk; // used internally, don't access.
} tCinematic;

bool InitCinematics();
void SetMovieProperties(int x, int y, int w, int h, renderer_type type);
bool PlayMovie(const char *moviename);
tCinematic *StartMovie(const char *moviename, bool looping = false);
bool FrameMovie(tCinematic *mve, int x, int y, bool sequence = true);
void EndMovie(tCinematic *mve);

#endif
