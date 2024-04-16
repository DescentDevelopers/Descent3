/*
 * $Logfile: /DescentIII/main/AIGoal.h $
 * $Revision: 24 $
 * $Date: 3/30/99 4:32p $
 * $Author: Chris $
 *
 * AI Goal Header
 *
 * $Log: /DescentIII/main/AIGoal.h $
 *
 * 24    3/30/99 4:32p Chris
 * Moved subtype to the main goal sturct (from goal_info).  Made move
 * relative object vec finishable.  (Like get behind player)
 *
 * 23    1/25/99 7:43a Chris
 * Added the GUID (Goal Unique Id) and added the ability for weapon
 * batteries to always fire exactly forward.
 *
 * 22    12/14/98 1:07p Chris
 * Allowed OSIRIS to change AI types
 *
 * 21    12/13/98 9:18p Chris
 * Improved influence values for in-code goals (10000 to 1.0).  Added
 * GF_ORIENT stuff.  :)
 *
 * 20    12/01/98 5:52p Chris
 * Made the default influence 1.0
 *
 * 19    12/01/98 4:30p Chris
 * Checked in a massive amount of AI work.  Improved flocking code.  :)
 * (Still hacked lightly).  In addition, I am moving toward using the
 * composite dir.  :)
 *
 * 18    9/17/98 12:08p Chris
 * Improved support of AIGoalAddEnabler
 *
 * 17    9/17/98 11:11a Chris
 * Worked on goal system and improved integration with OSIRIS (better
 * EVT_AI_INIT event calling)
 *
 * 16    5/15/98 2:58p Chris
 * More on the big AI update
 *
 * 15    5/14/98 12:21p Chris
 * Updating AI system... Incomplete
 *
 * 14    5/11/98 4:39p Chris
 * Improved AI system (goals have notifies and are more flexable).
 *
 * 13    4/30/98 11:31a Chris
 * Massive upgrades to the AI system
 *
 * 12    4/22/98 1:15p Chris
 * Incremental work on paths and goals
 *
 * 11    4/21/98 11:25a Chris
 * Improving GET_TO_OBJ goal
 *
 * 10    4/20/98 3:19p Chris
 * Added more goal/path support
 *
 * 9     4/20/98 12:27p Chris
 * Made AIG_GET_TO_OBJECT work.
 *
 * 8     2/11/98 7:00p Chris
 * Removed priority (changed to influence) and added activation_level
 *
 * 7     1/14/98 7:57p Chris
 * Improving the awareness system
 *
 * 6     11/14/97 11:55p Chris
 * Dodge goals are semi-functional
 *
 * 5     7/15/97 7:29p Chris
 * Added a sound for helicopter blades.
 *
 * 4     7/15/97 5:35p Chris
 * New AI code
 *
 * 3     7/15/97 4:59p Chris
 * added support for AI and animations
 *
 * $NoKeywords: $
 */

#ifndef AIGOAL_H_
#define AIGOAL_H_

#include "object.h"

// Clears and removes all goals for a robot
void GoalClearAll(object *obj);

// Adds a new goal
int GoalAddGoal(object *obj, unsigned int goal_type, void *args, int level = 0, float influence = 1.0f, int f_goal = 0,
                int guid = -1, char subtype = 0);

// Adds a ending condition to a goal
int GoalAddDisabler(object *obj, int goal_index, ubyte ender_type, void *args, float percent = 1.0f,
                    float interval = 0.0f);

// Adds a enabler condition to a goal
int GoalAddEnabler(object *obj, int goal_index, ubyte enabler_type, void *arg_struct, float percent, float interval);

// Clears one goal
void GoalClearGoal(object *obj, goal *goal_ptr, int notify_reason = AI_INVALID_INDEX);

// Init's an AI to no goals
void GoalInitTypeGoals(object *obj, int ai_type);

// Does the goal related stuff for an object per frame
void GoalDoFrame(object *obj);

// Goal's path is complete.  What next?
void GoalPathComplete(object *obj);

// Is a used goal currently enabled?
bool GoalIsGoalEnabled(object *obj, int goal_index);

// Determines minimum distance to look for nearby objects
float GoalDetermineTrackDist(object *obj);

// Returns a pointer to the highest priority non-blended goal
goal *GoalGetCurrentGoal(object *obj);

#endif