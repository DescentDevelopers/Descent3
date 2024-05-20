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

--- HISTORICAL COMMENTS FOLLOW ---

 * $Logfile: /DescentIII/Main/editor/group.h $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:38 $
 * $Author: kevinb $
 *
 * Header for group.cpp
 *
 * $Log: not supported by cvs2svn $
 * 
 * 8     9/15/99 1:56p Matt
 * Added the option to allow rooms or groups placed on the terrain to
 * either align with the terrain or with gravity.
 * 
 * 7     5/08/99 1:39a Matt
 * Added a function to delete all objects of a certain type, and support
 * for placing and attaching groups to the terrain.
 * 
 * 6     12/01/98 11:22p Matt
 * Made copy/paste work with doors.
 * 
 * 5     9/22/97 2:31p Matt
 * Made group code copy & paste objects & triggers
 * 
 * 4     9/04/97 4:43p Matt
 * Removed include of room.h to reduce compile times
 * 
 * 3     9/02/97 6:42p Matt
 * Got paste & group save/load working
 * 
 * 2     8/29/97 5:45p Matt
 * Converted group code to work with rooms (unfinished)
 * 
 * 4     6/13/97 11:52a Matt
 * Deleted now-unused RotateWorld() function
 * 
 * 3     5/05/97 3:57p Matt
 * Added code to rotate the mine (& the objects in it) to join with the
 * terrain.
 * 
 * 2     3/12/97 3:25p Matt
 * Added funcs for cut, copy, paste, & delete, and to save and load
 * groups.
 * 
 * 1     3/11/97 10:48p Matt
 *
 * $NoKeywords: $
 */

#ifndef _GROUP_H
#define _GROUP_H

//Define room, so we don't have to include room.h
struct room;
struct object;
struct trigger;

//A group.  Used for cut, paste, copy, etc.
struct group {
	int  		nrooms;			//number of rooms in this group
	room 		*rooms;			//pointer to list of rooms
	int  		attachroom;		//which room is attached when pasting
	int  		attachface;		//which face is attached when pasting
	int		nobjects;		//how many objects
	int		ndoors;			//how many doors
	object	*objects;		//pointer to list of objects
	int		ntriggers;		//how many triggers
	trigger	*triggers;		//pointer to list of triggers
};


//Free a group.
//Parameters:	g - the group to be freed 
void FreeGroup(group *g);

//Copy the given list of rooms to a group
//Parameters:	nrooms - the number of rooms in list
//					roomnums - pointer to list of room numbers
//					attachroom, attachface - where group attaches when pasted 
//Returns:		pointer to group
group *CopyGroup(int nrooms,int *roomnums,int attachroom,int attachface);

//Delete the given list of rooms
//Parameters:	nrooms - the number of rooms in list
//					roomnums - pointer to list of room numbers
void DeleteGroup(int nrooms,int *roomnums);

//Place the given group at the specified room face
//The function merely causes the group to be drawn in the editor, allowing the user to line it up
//before attaching it.  The function AttachGroup() must be called to do the actual attachment.
//Parameters:	destroomp, destside - where to place the group
//					g - the group to place
void PlaceGroup(room *destroomp,int destface,group *g);

//Place the given group at the specified terrain cell
//The function merely causes the group to be drawn in the editor, allowing the user to line it up
//before attaching it.  The function AttachGroup() must be called to do the actual attachment.
//Parameters:	cellnum - where to place the group
//					g - the group to place
void PlaceGroupTerrain(int cellnum,group *g,bool align_to_terrain);

//Attach the already-placed group
void AttachGroup();

//Saves a group to disk in the given filename
void SaveGroup(char *filename,group *g);

//Loads a group from disk
//Returns:	pointer to the group loaded
group *LoadGroup(char *filename);

#endif	//ifdef _GROUP_H

