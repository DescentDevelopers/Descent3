/*
 * $Logfile: /DescentIII/Main/aiterrain.h $
 * $Revision: 5 $
 * $Date: 8/05/97 12:17p $
 * $Author: Chris $
 *
 * Terrain specific AI stuff
 *
 * $Log: /DescentIII/Main/aiterrain.h $
 *
 * 5     8/05/97 12:17p Chris
 *
 * 4     7/30/97 1:31p Chris
 * Made helicopters slightly more interesting.
 *
 * 3     7/29/97 12:20p Chris
 * Incremental improvements.  Fixed a memory bug.
 *
 * 2     7/28/97 1:19p Chris
 * Expanding the AI system
 *
 * $NoKeywords: $
 */

#ifndef AITERRAIN_H_
#define AITERRAIN_H_

typedef struct ground_information {
  float highest_y;
  float lowest_y;
} ground_information;

bool ait_GetGroundInfo(ground_information *ground_info, vector *p0, vector *p1, float rad = 0.0f, angle fov = 0);
void ait_Init(void);

#endif