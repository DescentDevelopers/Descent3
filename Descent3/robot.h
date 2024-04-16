#ifndef ROBOT_H
#define ROBOT_H

#include "pstypes.h"
#include "manage.h"
#include "object.h"
#include "objinfo.h"

#define MAX_ROBOTS 100

extern int Num_robots;
extern object_info Robots[MAX_ROBOTS];
extern char *Movement_class_names[];
extern char *Anim_state_names[];

// Sets all robots to unused
void InitRobots();

// Allocs a robot for use, returns -1 if error, else index on success
int AllocRobot();

// Frees robot index n
void FreeRobot(int n);

// Gets next robot from n that has actually been alloced
int GetNextRobot(int n);

// Gets previous robot from n that has actually been alloced
int GetPrevRobot(int n);

// Searches thru all robots for a specific name, returns -1 if not found
// or index of robot with name
int FindRobotName(char *name);

// Given a filename, loads either the model or vclip found in that file.  If type
// is not NULL, sets it to 1 if file is model, otherwise sets it to zero
int LoadRobotImage(char *filename);

// Given a robot handle, returns that robots image for framenum
int GetRobotImage(int handle);

// Given an object, renders the representation of this robot
// Currently only handles bitmap types, not poly models
void DrawRobotObject(object *obj);

// Given a robot name, assigns that robot to a specific index into
// the Robots array.  Returns -1 if the named robot is not found, 0 if the robot
// is already in its place, or 1 if successfully moved
int MatchRobotToIndex(char *name, int dest_index);

// Moves a robot from a given index into a new one (above MAX_STATIC_POWERUPS)
// returns new index
int MoveRobotFromIndex(int index);

// This is a very confusing function.  It takes all the robots that we have loaded
// and remaps then into their proper places (if they are static).
void RemapRobots();

// goes thru every entity that could possible have a robot index (ie objects, robots, etc)
// and changes the old index to the new index
void RemapAllRobotObjects(int old_index, int new_index);

#endif
