/*
 * $Logfile: /DescentIII/Main/grtext/grtextlib.h $
 * $Revision: 2 $
 * $Date: 4/27/98 3:46p $
 * $Author: Samir $
 *
 *
 *
 * $Log: /DescentIII/Main/grtext/grtextlib.h $
 *
 * 2     4/27/98 3:46p Samir
 * scaling fonts.
 *
 * 1     12/29/97 3:24p Samir
 * Initial revision
 *
 * $NoKeywords: $
 */

#ifndef GRTEXTLIB_H
#define GRTEXTLIB_H

#include "grtext.h"
#include "pstypes.h"

typedef struct tCharBlt {
  ushort ch;
  ushort clipped; // if =1, use sx,sy,sw,sh
  int x, y;
  float dsw, dsh; // scale vertical and horiz.
  int sx, sy;     // source rectangle within character in pixels.
  int sw, sh;
} tCharBlt;

//	render a character
int grfont_BltChar(int font, tCharBlt *cbi);

#endif