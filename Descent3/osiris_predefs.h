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

 * $Logfile: /DescentIII/main/osiris_predefs.h $
 * $Revision: 47 $
 * $Date: 5/05/99 4:56p $
 * $Author: Chris $
 *
 * Header file for osiris predefined functions
 *
 * $Log: /DescentIII/main/osiris_predefs.h $
 *
 * 47    5/05/99 4:56p Chris
 * Added the RType powerup for the GB
 *
 * 46    4/29/99 7:15p Jeff
 * added initial velocity parameter to Obj_Create
 *
 * 45    4/24/99 2:19a Chris
 * Added another parameter to FollowPathSimple
 *
 * 44    4/24/99 12:09a Jeff
 * added path value functions
 *
 * 43    4/20/99 8:56p Chris
 * Fixed problem with robots not being able to open locked doors that a
 * player has the key for.
 *
 * 42    4/14/99 3:57a Jeff
 * fixed case mismatch in #includes
 *
 * 41    4/02/99 10:18a Chris
 * We can now mess with the Object_info anim stuff
 *
 * 40    3/17/99 11:47a Jeff
 * exported function to get what language we are using
 *
 * 39    3/03/99 3:01a Chris
 * Exported the difficulty stuff to OSIRIS
 *
 * 38    3/03/99 12:11a Chris
 * one more AI path check function
 *
 * 37    3/03/99 12:07a Chris
 * Added the two new OSIRIS predefs for AI path finding
 *
 * 36    3/01/99 7:20p Chris
 * Fixed dist problems with finding nearby objects
 *
 * 35    2/28/99 11:30p Chris
 * FindObjOfType and OSIRIS controllable deaths
 *
 * 34    2/22/99 8:22p Chris
 *
 * 33    2/22/99 8:17p Chris
 * Added the LGoal_Value() function to OSIRIS
 *
 * 32    2/21/99 5:51p Chris
 * FIxed a bug with OBJV_C_TYPE (It should have been OBJV_I_TYPE).  Added
 * Obj_FindType()
 *
 * 31    2/18/99 4:28p Jeff
 * added lookup functions for matcen, paths, level goals
 *
 * 30    2/11/99 2:56a Jeff
 * improvements to introcam
 *
 * 29    2/10/99 1:47p Matt
 * Changed object handle symbolic constants
 *
 * 28    2/08/99 3:06a Jeff
 * created a function to switch a player between ai and flying control
 * types (not exported to osiris...instead used in multisafe)
 *
 * 26    2/03/99 12:43a Chris
 * Added Obj_GetGroundPos
 *
 * 25    1/29/99 5:10p Chris
 * Added an optional parent handle check for FindObjOfType
 *
 * 24    1/25/99 8:38a Chris
 * The the GUID to AI_FollowPathSimple
 *
 * 23    1/25/99 7:43a Chris
 * Added the GUID (Goal Unique Id) and added the ability for weapon
 * batteries to always fire exactly forward.
 *
 * 22    1/24/99 8:17p Chris
 * Updated AI and OSIRIS.  Externalized fireball constants for spew and
 * sparks.  Added CreateRandomSparks to OSIRIS, renamed a bunch of AI
 * Notify names to use underscores.  Fixed a memory access leak in the
 * matcen effect code.
 *
 * 21    1/22/99 4:47p Jeff
 *
 * 20    1/20/99 4:16p Jeff
 * added ai notify child events and predefs to get ids of sound, room,
 * trigger, object names
 *
 * 19    1/20/99 1:01a Chris
 * Improved AI and OSIRIS intergration
 *
 * 18    1/15/99 5:59p Chris
 *
 * 17    1/13/99 5:20p Jeff
 * added 4 new file operation predefs
 *
 * 16    1/13/99 3:25a Chris
 * Added Obj_Burning and Obj_IsEffect to OSIRIS
 *
 * 15    1/13/99 2:29a Chris
 * Massive AI, OSIRIS update
 *
 * 14    1/11/99 2:14p Chris
 * Massive work on OSIRIS and AI
 *
 * 13    1/05/99 12:24p Chris
 * More OSIRIS improvements
 *
 * 12    1/04/99 6:43p Jeff
 * added support to get/set mission flag values
 *
 * 11    1/04/99 12:42p Chris
 * Updated OSIRIS with "xxxx_external.h" files and Obj_Value, and
 * Matcen_XXX functions
 *
 * 10    12/30/98 3:46p Chris
 * Incremental AI changes
 *
 * 9     12/23/98 12:10p Chris
 * Added Gametime and Frametime to OSIRIS
 *
 * 8     12/22/98 4:52p Chris
 * Added another defualt parameter to Objcreate
 *
 * 7     12/22/98 4:51p Chris
 * Added a ObjCreate predefined function
 *
 * 6     12/22/98 2:46p Chris
 * Added the AIValue predefined function
 *
 * 5     12/21/98 5:30p Samir
 * predef for determining if object is being rendered.
 *
 * 4     12/17/98 6:42p Samir
 * added functions to manipulate player controls
 *
 * 3     12/17/98 5:43p Jeff
 * created timer system for osiris and save restore events
 *
 * 2     12/16/98 10:14p Jeff
 * created osiris predefined function file
 *
 * $NoKeywords: $
 */

#ifndef __OSIRIS_PREDEF_H_
#define __OSIRIS_PREDEF_H_

#include "osiris_dll.h"
#include "cfile.h"

//	osipf_CallObjectEvent
//	Sends an event to an object.  Returns true if the default action should
//	continue to process.
bool osipf_CallObjectEvent(int objnum, int event, tOSIRISEventInfo *ei);

//	osipf_CallTriggerEvent
//	Sends an event to a trigger.  Returns true if the default action should
//	continue to process.
bool osipf_CallTriggerEvent(int trignum, int event, tOSIRISEventInfo *ei);

// Touches a sound file so it loads into memory
void osipf_SoundTouch(char *str);

// searches for an object id given its name
int osipf_ObjectFindID(const char *name);

int osipf_ObjectFindType(const char *name);

// searches through the weapons for a name and returns the id
int osipf_WeaponFindID(const char *name);

// returns how long an object has lived
float osipf_ObjectGetTimeLived(int objhandle);

void osipf_GetGunPos(int objhandle, int gun_number, vector *gun_pnt, vector *gun_normal = NULL);

// Returns room values (Index is optional and not always used)
void osipf_RoomValue(int roomnum, char op, char vhandle, void *ptr, int index = 0);
uint8_t osipf_IsRoomValid(int roomnum);

// Returns player only values (Index is optional and not always used)
void osipf_PlayerValue(int obj_handle, char op, char vhandle, void *ptr, int index = 0);
void osipf_ObjectCustomAnim(int handle, float start, float end, float time, char flags, int sound_handle = -1,
                            char next_anim_type = -1);

int osipf_GetAttachParent(int childhandle);
int osipf_GetNumAttachSlots(int objhandle);
int osipf_GetAttachChildHandle(int objhandle, char attachpoint);
int osipf_AttachObjectAP(int parenthandle, char parent_ap, int childhandle, char child_ap, uint8_t f_use_aligned);
int osipf_AttachObjectRad(int parenthandle, char parent_ap, int childhandle, float percent_rad);
void osipf_UnattachFromParent(int objhandle);
void osipf_UnattachChild(int objhandle, char parent_ap);
void osipf_UnattachChildren(int objhandle);

int osipf_RayCast(int objhandle, vector *p0, vector *p1, int start_roomnum, float rad, int flags, ray_info *ri);

// searches through GamePath index and returns index of path matching name
// returns -1 if not found
int osipf_AIGetPathID(char *string);
vector osipf_AIFindHidePos(int hideobjhandle, int viewobjhandle, float time, int *hide_room);
int osipf_AIFindObjOfType(int objhandle, int type, int id, bool f_ignore_init_room,
                          int parent_handle = OBJECT_HANDLE_NONE);
vector osipf_AIGetRoomPathPoint(int roomnum);
int osipf_AIFindEnergyCenter(int objhandle);
float osipf_AIGetDistToObj(int objhandle, int otherobjhandle);

void osipf_AISetType(int objhandle, int type);
int osipf_AIPowerSwitch(int objhandle, uint8_t f_power_on);

int osipf_AIGoalAdd(int objhandle, int goal_type, int level, float influence, int guid, int flags, ...);
int osipf_AIGoalAddEnabler(int objhandle, int goal_index, int enabler_type, float percent, float interval, void *ptr);
void osipf_AIGoalClear(int objhandle, int goal_index);
int osipf_AIGoalFollowPathSimple(int objhandle, int path_id, int guid, int flags, int slot = 3);
void osipf_AISetGoalCircleDist(int objhandle, int goal_handle, float dist);
int osipf_AISetGoalFlags(int objhandle, int goal_handle, int flags, uint8_t f_enable);

uint8_t osipf_AITurnTowardsVectors(int objhandle, vector *fvec, vector *uvec);
uint8_t osipf_AIMoveTowardsPosition(int objhandle, vector *pos, int *roomnum, float scalar, bool f_bline,
                                  bool f_bline_if_vis);

void osipf_AIValue(int objhandle, char op, char vtype, void *ptr);

// Reads the specified number of bytes from a file into the buffer
// DO NOT USE THIS TO READ STRUCTURES.  This function is for byte
// data, such as a string or a bitmap of 8-bit pixels.
// Returns the number of bytes read.
// Throws an exception of type (cfile_error *) if the OS returns an error on read
int osipf_CFReadBytes(uint8_t *buffer, int count, CFILE *cfp);

// The following functions read numeric vales from a CFILE.  All values are
// stored in the file in Intel (little-endian) format.  These functions
// will convert to big-endian if required.
// These funtions will throw an exception of if the value cannot be read,
// so do not call these if you don't require the data to be present.

// Read and return an integer (32 bits)
// Throws an exception of type (cfile_error *) if the OS returns an error on read
int osipf_CFReadInt(CFILE *cfp);

// Read and return a int16_t (16 bits)
// Throws an exception of type (cfile_error *) if the OS returns an error on read
int16_t osipf_CFReadShort(CFILE *cfp);

// Read and return a byte (8 bits)
// Throws an exception of type (cfile_error *) if the OS returns an error on read
int8_t osipf_CFReadByte(CFILE *cfp);

// Read and return a float (32 bits)
// Throws an exception of type (cfile_error *) if the OS returns an error on read
float osipf_CFReadFloat(CFILE *cfp);

// Read and return a double (64 bits)
// Throws an exception of type (cfile_error *) if the OS returns an error on read
double osipf_CFReadDouble(CFILE *cfp);

// Reads a string from a CFILE.  If the file is type binary, this
// function reads until a NULL or EOF is found.  If the file is text,
// the function reads until a newline or EOF is found.  The string is always
// written to the destination buffer null-terminated, without the newline.
// Parameters:  buf - where the string is written
//					n - the maximum string length, including the terminating 0
//					cfp - the CFILE pointer
// Returns the number of bytes in the string, before the terminator
// Does not generate an exception on EOF
int osipf_CFReadString(char *buf, size_t n, CFILE *cfp);

// Writes the specified number of bytes from a file into the buffer
// DO NOT USE THIS TO WRITE STRUCTURES.  This function is for byte
// data, such as a string or a bitmap of 8-bit pixels.
// Returns the number of bytes written.
// Throws an exception of type (cfile_error *) if the OS returns an error on write
int osipf_CFWriteBytes(const uint8_t *buf, int count, CFILE *cfp);

// Writes a null-terminated string to a file.  If the file is type binary,
// the string is terminated in the file with a null.  If the file is type
// text, the string is terminated with a newline.
// Parameters:	buf - pointer to the string
//					cfp = the CFILE pointer
// Returns the number of bytes written
// Throws an exception of type (cfile_error *) if the OS returns an error on write
int osipf_CFWriteString(const char *buf, CFILE *cfp);

// The following functions write numeric vales to a CFILE.  All values are
// stored to the file in Intel (little-endian) format.
// All these throw an exception if there's an error on write.

// Write an integer (32 bits)
// Throws an exception of type (cfile_error *) if the OS returns an error on write
void osipf_CFWriteInt(int i, CFILE *cfp);

// Write a int16_t (16 bits)
// Throws an exception of type (cfile_error *) if the OS returns an error on write
void osipf_CFWriteShort(int16_t s, CFILE *cfp);

// Write a byte (8 bits).  If the byte is a newline & the file is a text file, writes a CR/LF pair.
// Throws an exception of type (cfile_error *) if the OS returns an error on write
void osipf_CFWriteByte(int8_t b, CFILE *cfp);

// Write a float (32 bits)
// Throws an exception of type (cfile_error *) if the OS returns an error on write
void osipf_CFWriteFloat(float f, CFILE *cfp);

// Write a double (64 bits)
// Throws an exception of type (cfile_error *) if the OS returns an error on write
void osipf_CFWriteDouble(double d, CFILE *cfp);

// CONTROLLER predefs
//	enables or disables controls specified.
void osipf_SetAllControls(bool enabled);

// enable or disable specified function
void osipf_SetControls(int fn, bool enabled);

// Creates an object
int osipf_ObjCreate(uint8_t type, uint16_t id, int roomnum, vector *pos, const matrix *orient = NULL,
                    int parent_handle = OBJECT_HANDLE_NONE, vector *velocity = NULL);

// OBJECT Properties.
// is an object visible? (rendered current frame)
bool osipf_IsObjectVisible(object *obj);

float osipf_GameTime(void);
float osipf_FrameTime(void);
void osipf_ObjWBValue(int obj_handle, char wb_index, char op, char vtype, void *ptr, char g_index = 0);

void osipf_ObjectValue(int handle, char op, char vtype, void *ptr, int index = 0);
void osipf_MatcenValue(int handle, char op, char vtype, void *ptr, int index = 0);

void osipf_MatcenReset(int handle);
void osipf_MatcenCopy(int dhandle, int shandle);
int osipf_MatcenCreate(char *name);
int osipf_MatcenFindId(char *str);

// Sets/Clears mission flags
//	flag is which mission flag to set/clear (1-32)
//	value is 0 to clear, or 1 to set
void osipf_MissionFlagSet(int flag, uint8_t value);

// Gets a mission flag
//	flag is what mission flag to get.  Returns 1 if set, 0 if not.
int osipf_MissionFlagGet(int flag);

void osipf_PlayerAddHudMessage(int handle, const char *str);
void osipf_ObjGhost(int handle, bool f_ghost);
void osipf_ObjBurning(int handle, float time, float damage_per_second);
bool osipf_ObjIsEffect(int handle, int type_flag);

//	opens a cfile for reading/writing (works just like fopen())
void *osipf_CFopen(const char *filename, const char *mode);

//	closes a cfile (like fclose())
void osipf_CFclose(CFILE *file);

//	returns the current position of the file (like ftell())
int osipf_CFtell(CFILE *file);

//	returns 1 if the file is at the EOF, else 0 (like feof())
uint8_t osipf_CFeof(CFILE *file);

void osipf_SoundStop(int s_handle, bool f_immediately = false);
int osipf_SoundPlay2d(int obj_handle, int s_id, float volume = 1.0f);
int osipf_SoundPlay3d(int obj_handle, int s_id, float volume = 1.0f);
int osipf_SoundFindId(const char *s_name);

bool osipf_AIIsObjFriend(int obj_handle, int it_handle);
bool osipf_AIIsObjEnemy(int obj_handle, int it_handle);
void osipf_AIGoalValue(int obj_handle, char g_index, char op, char vtype, void *ptr, char index = 0);
int osipf_AIGetNearbyObjs(vector *pos, int init_roomnum, float rad, int *object_handle_list, int max_elements,
                          bool f_lightmap_only, bool f_only_players_and_ais = true,
                          bool f_include_non_collide_objects = false, bool f_stop_at_closed_doors = true);
char osipf_AIGetCurGoalIndex(int obj_handle);

int osipf_FindSoundName(const char *name);
int osipf_FindRoomName(const char *name);
int osipf_FindTriggerName(const char *name);
int osipf_FindObjectName(const char *name);
int osipf_GetTriggerRoom(int trigger_id);
int osipf_GetTriggerFace(int trigger_id);
int osipf_FindDoorName(const char *name);
int osipf_FindTextureName(const char *name);
int osipf_FindMatcenName(const char *name);
int osipf_FindPathName(const char *name);
int osipf_FindLevelGoalName(const char *name);

void osipf_CreateRandomSparks(int num_sparks, vector *pos, int roomnum, int which_index, float force_scalar);

void osipf_GetGroundPos(int objhandle, int ground_number, vector *ground_pnt, vector *ground_normal);

// disable/enable ship
void osipf_EnableShip(char *ship_name, bool enable);
// is ship enabled
bool osipf_IsShipEnabled(char *ship_name);

// turns the given player into AI mode or back to regular control mode
void osipf_SetPlayerControlMode(int pnum, bool set_to_ai);

// gets information about a path point
// pass NULL for parameters not needed.
//	pathid: path number
//	point: which path point
// returns true if operation was successful
bool osipf_PathGetInformation(int pathid, int point, vector *pos, int *room, matrix *orient);

void osipf_LGoalValue(char op, char vtype, void *ptr, int g_index = -1, int i_index = -1);
int osipf_ObjMakeListOfType(int objhandle, int type, int id, bool f_ignore_init_room, int parent_handle,
                            int max_recorded, int *handles);

void osipf_ObjKill(int handle, int killer_handle, float damage, int flags, float min_time, float max_time);
bool osipf_AIIsDestReachable(int handle, int room);
bool osipf_AIIsObjReachable(int handle, int target);
char osipf_GameGetDiffLevel(void);
int osipf_GetLanguageSetting(void);

// Sets/Gets information about a path.
// If you change is PV_ALL (or any of its individual components), pass in a pointer to an
// osiris_path_node_info struct.  For the others, you must pass in an appropriate pointer
// (i.e. an int* for PV_I_NUMNODES).  You can only set PV_ALL components.
//	for PV_I_NUMNODES, path_id MUST be specified, node_id is ignored
//	for PV_I_NUMPATHS, path_id and node_id are ignored
//	for PV_CS_NAME, path_id MUST be specified, node_id is ignored
//	for PV_ALL components, path_id and node_id MUST be valid.
void osipf_PathValue(int path_id, int node_id, char op, int changes, void *ptr);

#endif
