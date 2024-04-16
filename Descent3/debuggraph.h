/*
 * $Logfile: /DescentIII/Main/debuggraph.h $
 * $Revision: 2 $
 * $Date: 4/04/99 8:15p $
 * $Author: Jeff $
 *
 * Debug graph visual log
 *
 * $Log: /DescentIII/Main/debuggraph.h $
 *
 * 2     4/04/99 8:15p Jeff
 * added debug graph stuff
 *
 * $NoKeywords: $
 */

#ifndef __DEBUGGRAPH_H_
#define __DEBUGGRAPH_H_
#include "grdefs.h"

#define DGF_MULTIPLAYER 0x01  // used in multiplayer
#define DGF_SINGLEPLAYER 0x02 // used in single player

int DebugGraph_Add(float min, float max, char *name, int flags = DGF_MULTIPLAYER | DGF_SINGLEPLAYER);
int DebugGraph_Add(int min, int max, char *name, int flags = DGF_MULTIPLAYER | DGF_SINGLEPLAYER);

void DebugGraph_Update(int id, int value);
void DebugGraph_Update(int id, float value);
void DebugGraph_Render(void);

void DebugGraph_Disable(int id);
void DebugGraph_Enable(int id);

void DebugGraph_DisplayOptions(void);

void DebugGraph_Initialize(void);

#endif