/*
 * $Logfile: /DescentIII/Main/ddgr_win32/ddgrWin32.h $
 * $Revision: 3 $
 * $Date: 4/23/98 6:38p $
 * $Author: Jason $
 *
 *	DDGR v2.0 Win32 Implementation
 *
 * $Log: /DescentIII/Main/ddgr_win32/ddgrWin32.h $
 *
 * 3     4/23/98 6:38p Jason
 * made bitmaps use 1555 format
 *
 * 2     9/12/97 4:13p Samir
 * Added some private data access functions and more DirectX
 * functionality.
 *
 * 1     6/12/97 6:33p Samir
 * Initial revision
 *
 * $NoKeywords: $
 */

#ifndef DDGRWIN32_H
#define DDGRWIN32_H

#include "ddgr.h"
#include "pstypes.h"

/*	Subsystem constants

        as of v2.0
*/
const int DDGR_GDI_SUBSYSTEM = 1, // GDI subsystem
    DDGR_GDIX_SUBSYSTEM = 2,      // GDIX subsystem (GDI+DirectX)
    DDGR_DX_SUBSYSTEM = 3;        // DIRECTX subsystem

/*	Internal Library Data

*/
typedef struct tDDGRInternalData {
  bool init;     // Is library currently initialized
  int subsystem; // Current active subsystem
} tDDGRInternalData;

/*	Global externs
 */
extern tDDGRInternalData DDGR_lib_data;
extern char *DDGR_subsystem_names[];
extern int DDGR_subsystems[];

/*	Macros
 */
#define LIB_DATA(_c) DDGR_lib_data._c

inline unsigned DDGR_MAKERGB16(ddgr_color c) {
  unsigned char r, g, b;
  r = (unsigned char)((c & 0x00ff0000) >> 16);
  g = (unsigned char)((c & 0x0000ff00) >> 8);
  b = (unsigned char)(c & 0x000000ff);

  return (((r >> 3) << 10) + ((g >> 3) << 5) + (b >> 3));
}

inline int DDGR_COLOR_RED(ddgr_color c) {
  unsigned r = (unsigned char)((c & 0x00ff0000) >> 16);
  return (int)r;
}

inline int DDGR_COLOR_GREEN(ddgr_color c) {
  unsigned g = (unsigned char)((c & 0x0000ff00) >> 8);
  return (int)g;
}

inline int DDGR_COLOR_BLUE(ddgr_color c) {
  unsigned b = (unsigned char)(c & 0x000000ff);
  return (int)b;
}

/*	Functions Internal
 */

void ddgr_FatalError(char *fmt, ...);
void ddgr_PushError(char *fmt, ...);

#endif