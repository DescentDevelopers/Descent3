/*
* Descent 3 
* Copyright (C) 2024 Parallax Software
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef ROBOTPAGE_H
#define ROBOTPAGE_H

#include "manage.h"
#include "robot.h"
#include "cfile.h"
#include "pstypes.h"
#include "objinfo.h"

struct mngs_robot_page {
  object_info robot_struct;
  char image_name[PAGENAME_LEN];
};

// Robot page functions
//---------------------------------------------------------------

// Given an open file pointer and a robot_page struct, writes that robotpage out
void mng_WriteRobotPage(CFILE *outfile, mngs_robot_page *robotpage);

// Reads a robot page from an open file.  Returns 0 on error.
int mng_ReadRobotPage(CFILE *infile, mngs_robot_page *robotpage);

// Given a robot handle, searches the table file and replaces the robot with the same name
// If local=1, then does it to the users local copy
// Returns 0 on error, else 1 if all is good
int mng_ReplaceRobotPage(char *name, int handle, int local);

// Given a robot name, finds it in the table file and deletes it
// If local is 1, deletes from the local table file
int mng_DeleteRobotPage(char *name, int local);

// Reads in the robotpage named "name" into robotpage struct
// Returns 0 on error or couldn't find, else 1 if all is good
int mng_FindSpecificRobotPage(char *name, mngs_robot_page *robotpage);

// Given a robot page, allocs a robot and calls AssignRobotPageToRobot to actually
// load model and values. Rturns robot handle on success, -1 if fail
int mng_SetAndLoadRobot(mngs_robot_page *robotpage);

// Given a robotpage and a robot handle, attempts to make robot n correspond to
// to the robotpage.
// Returns 1 on success, 0 otherwise
int mng_AssignRobotPageToRobot(mngs_robot_page *robotpage, int n);

// Copies values from a Robot into a robot_page
void mng_AssignRobotToRobotPage(int n, mngs_robot_page *robotpage);

// Reads in a robot page from the local table file, superseding any robot
// already in RAM with that same name
void mng_LoadLocalRobotPage(CFILE *);

// Reads in a page off the net
void mng_LoadNetRobotPage(CFILE *);

#endif
