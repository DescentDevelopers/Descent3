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

 * $Logfile: /DescentIII/Main/LoadLevel.h $
 * $Revision: 130 $
 * $Date: 10/21/99 3:27p $
 * $Author: Gwar $
 *
 * Header for LoadLevel.cpp
 *
 * $Log: /DescentIII/Main/LoadLevel.h $
 *
 * 130   10/21/99 3:27p Gwar
 * changes to keep NEWEDITOR compatibility with D3 v1.2 and earlier (for
 * now)
 *
 * 129   10/14/99 1:08p Chris
 * Added another custom bounce force field texture slot
 *
 * 128   10/13/99 2:19p Chris
 * Added the ability to disable wind for powerups on a level
 *
 * 127   10/13/99 10:41a Chris
 * Added 'special forcefields' that have custom bounce factors
 *
 * 126   10/08/99 4:28p Chris
 * Added the forcefield and glass breaking override options
 *
 * 125   8/12/99 5:26p Matt
 * Added a chunk to store room wind vectors.
 *
 * 124   5/23/99 3:06a Jason
 * save editor lighting settings
 *
 * 123   5/18/99 11:13a Matt
 * Added variable ceiling height.
 *
 * 122   5/03/99 3:28p Jason
 * save out lightmap spacing
 *
 * 121   4/29/99 1:59a Chris
 * Added the portal blockage support
 *
 * 120   4/26/99 10:32p Chris
 * Improving the BNode system
 *
 * 119   4/26/99 11:11a Chris
 * Updated Bnode system
 *
 * 118   4/21/99 12:40p Chris
 * Added the FVI_always_check_ceiling flag
 *
 * 117   4/18/99 5:39a Chris
 * Vastly improved the path node system
 *
 * 116   4/14/99 3:56a Jeff
 * fixed case mismatch in #includes
 *
 * 115   4/05/99 3:34p Matt
 * Cleaned up player start flags
 *
 * 114   3/27/99 5:36p Matt
 * Save sounds by name instead of index
 *
 * 113   3/24/99 5:53p Jason
 * added per room ambience settings
 *
 * 112   3/10/99 7:12p Jason
 * added smooth specular shading for curved surfaces
 *
 * 111   2/28/99 9:28p Matt
 * Cleaned up load level error handing and version checking.
 *
 * 110   2/26/99 3:26p Jason
 * made satellites clip properly
 *
 * 109   2/21/99 4:20p Matt
 * Added SoundSource objects (and reformatted parts of the object header
 * files).
 *
 * 108   2/21/99 12:29p Matt
 * Added terrain sound system
 *
 * 107   2/19/99 12:10p Jason
 * fixed dumb (but major) bug with last rev
 *
 * 106   2/19/99 12:04p Jason
 * took out sky band
 *
 * 105   2/18/99 12:32p Jason
 * added room multiplier
 *
 * 104   2/14/99 3:27p Chris
 * FIxed a load/save bug... hehe
 *
 * 103   2/10/99 1:49p Matt
 * Don't have parent_handle, since it's always none.
 *
 * 102   2/09/99 9:58a Chris
 * Massive BOA update  :)  Terrain happy now.  Vis happy now.  Sound happy
 * now.
 *
 * 101   2/04/99 2:05p Matt
 * Added blastable doors
 *
 * 100   2/03/99 5:48p Matt
 * Added room damage system
 *
 * 99    2/03/99 12:15p Jason
 * added multiplayer vis optimizations
 *
 * 98    1/29/99 12:48p Matt
 * Rewrote the doorway system
 *
 * 97    1/22/99 8:53p Jeff
 * added custom-default script overrides
 *
 * 96    1/22/99 3:59p Jason
 * added 256x256 textures to help with terrain skies
 *
 * 95    1/21/99 11:34a Matt
 * Got rid of portal triggers.  Since we don't have multi-face portals, a
 * face trigger works fine for a portal.  Also fixed a few editor/trigger
 * bugs.
 *
 * 94    1/20/99 10:50a Jason
 * added new terrain
 *
 * 93    1/18/99 2:46p Matt
 * Combined flags & flags2 fields in object struct
 *
 * 92    1/15/99 3:14p Jason
 * added combinable portals
 *
 * 91    1/14/99 11:06a Matt
 * Added names to triggers
 *
 * 90    1/08/99 5:37p Samir
 * reverb values per room.
 *
 * 89    1/08/99 2:55p Samir
 * Ripped out OSIRIS1.
 *
 * 88    12/22/98 2:03p Matt
 * Added room names, and made rooms not compress so that room numbers are
 * suitable for persistant uses.
 *
 * 87    12/21/98 11:30p Matt
 * Added names for objects
 *
 * 86    12/21/98 6:00p Matt
 * Changed the object handle bit allocation to handle object numbers up to
 * 2047.
 * Increased MAX_OBJECTS to 1500.
 *
 * 85    12/18/98 5:40p Chris
 * Ambient life is now all sequenced up
 *
 * 84    12/11/98 1:55p Chris
 * Reduced BOA's size and changed how it works with no path situations
 *
 * 83    12/02/98 5:06p Jason
 * First pass at getting mega-lightmaps wprking
 *
 * 82    11/30/98 5:26p Jason
 * more lightmap changes
 *
 * 81    11/30/98 3:59p Jason
 * changed dynamic lighting to be better
 *
 * 80    11/18/98 4:30p Jason
 * added some functionality to terrain fog
 *
 * 79    11/11/98 11:41a Jason
 * added sunlight damage
 *
 * 78    11/02/98 6:15p Chris
 * Room AABBs get saved with the level and the sort_face and dec_sort_face
 * list s have been removed
 *
 * 77    10/19/98 5:40p Samir
 * added loading level callback.
 *
 * 76    10/17/98 11:11p Matt
 * Added room memory allocation system
 *
 * 75    10/05/98 2:42a Chris
 * Level goals are relatively functional
 *
 * 74    9/24/98 5:41p Chris
 * Designers can save out gravity changes
 *
 * 73    9/18/98 7:25p Matt
 * Put in code to handle a bunch of renamed doors.
 *
 * 72    9/04/98 12:29p Matt
 * Added marked edge & vertex in the editor
 *
 * 71    9/01/98 12:04p Matt
 * Ripped out multi-face portal code
 *
 * 70    8/27/98 5:19p Jason
 * added first rev of reflected surfaces
 *
 * 69    8/17/98 6:40p Matt
 * Added ambient sound system
 *
 * 68    8/10/98 11:25a Jason
 * added better specular lighting
 *
 * 67    7/20/98 6:07p Jason
 * Save BOA vis checksum to file
 *
 * 66    7/20/98 12:04p Jason
 * added per level satellite lighting
 *
 * 65    6/10/98 12:23p Jason
 * added new skies
 *
 * 64    6/05/98 5:22p Jason
 * added volumetric fog
 *
 * 63    6/04/98 6:17p Jason
 * added terrain occlusion system
 *
 * 62    6/02/98 6:03p Jason
 * added specular lightmaps
 *
 * 61    5/26/98 7:07p Samir
 * write out is_custom element of script structure for obejcts and
 * triggers.
 *
 * 60    5/22/98 5:12p Jason
 * save out sky radius
 *
 * 59    5/22/98 3:27p Jason
 * added specular lighting
 *
 * 58    5/15/98 5:41p Jason
 * implemented volume lighting system
 *
 * 57    5/14/98 12:56p Jason
 * changes to help lower memory usage
 *
 * 56    5/04/98 1:12a Chris
 * Added parenting bug checking
 *
 * 55    4/23/98 7:48p Jason
 * made older levels work with new 1555 format
 *
 * 54    4/22/98 12:38p Chris
 * Added path points to portals and rooms.  Improved BOA auto-making
 * process.
 *
 * 53    4/20/98 12:46p Jason
 * added level checksumming for multiplayer games
 *
 * 52    4/03/98 10:23a Chris
 * BOA cost array is saved out
 *
 * 51    4/02/98 12:24p Jason
 * trimmed some fat from our structures
 *
 * 50    3/29/98 11:58p Jason
 * saved some memory with object lightmaps
 *
 * 49    3/27/98 7:29p Jason
 * changed lightmap normals to work correctly
 *
 * 48    3/26/98 2:14p Jason
 * added dynamic lighting for objects
 *
 * 47    2/27/98 3:15p Jason
 * Don't save terrain pixel error stuff
 *
 * 46    2/17/98 8:58p Jason
 * took out loading/saving of useless rgbl data
 *
 * 45    2/15/98 8:01p Matt
 * Save wireframe view info in level file
 *
 * 44    2/13/98 12:52p Jason
 * made lighting multipliers work in quarter steps instead of whole steps
 *
 * 43    2/10/98 3:49p Jason
 * added pulsing walls
 *
 * 42    2/05/98 4:25p Chris
 * Fixed a few bugs in the path system, improved the insert node function,
 * and made the fvec and uvec of the node work (is even loaded and saved)
 * correctly.
 *
 * 41    2/04/98 8:25p Jason
 * added light multiplier for faces
 *
 * 40    2/04/98 6:09p Matt
 * Changed object room number to indicate a terrain cell via a flag.  Got
 * rid of the object flag which used to indicate terrain.
 *
 * 39    2/02/98 4:21p Samir
 * Now we save trigger script parameter info properly.
 *
 * 38    1/21/98 12:32p Matt
 * Revamped viewer system
 *
 * 37    1/20/98 4:12p Samir
 * Load and save script parameters.
 *
 * 36    1/19/98 10:04a Matt
 * Added new object handle system
 *
 * 35    1/16/98 8:06p Jason
 * added selectable halos and atmosphere blends to satellites
 *
 * 34    1/07/98 4:19p Jason
 * added dome to terrain sky
 *
 * 33    12/31/97 12:51p Jason
 * debugged and implementd BSP tree for lighting
 *
 * 32    12/17/97 5:26p Jason
 * added support for selectable wraparound sky
 *
 * 31    11/25/97 12:28p Jason
 * another pass at shadows
 *
 * 30    11/10/97 5:22p Jason
 * made lightmap spacing use floats instead of ubytes
 *
 * 29    11/03/97 1:52p Jason
 * compress lightmaps when reading/writing
 *
 * 28    10/21/97 11:08a Jason
 * added check for OF_POLYGON_OBJECT
 *
 * 27    10/03/97 12:42p Jason
 * save "keys_needed" info about doorways
 *
 * 26    10/02/97 12:09p Jason
 * made object lightmaps save and load
 *
 * 25    10/01/97 7:51p Matt
 * Added code for external rooms
 *
 * 24    9/29/97 12:09p Jason
 * added functionality to doorway system
 *
 * 23    9/25/97 3:49p Jason
 * sped up terrain reading/writing
 *
 * 22    9/24/97 6:18p Samir
 * Use script names instead of script id values to identify scripts.
 *
 * 21    9/19/97 2:52p Jason
 * changes to fix lightmap seam problem
 *
 * 20    9/16/97 4:22p Matt
 * Don't save/load the (now deleted) static_light field in a room
 *
 * 19    9/12/97 5:38p Jason
 * got doors working
 *
 * 18    9/12/97 2:35p Matt
 * Save/load face flags as a word
 *
 * 17    9/10/97 1:59p Samir
 * Added level version for script code storage in levels.
 *
 * 16    9/09/97 6:15p Jason
 * Added groovy dynamic terrain lighting system
 *
 * 15    9/08/97 4:13p Jason
 * added colored lighting for the terrain
 *
 * 14    9/08/97 12:16p Matt
 * Save & Load portal number as byte
 *
 * 13    9/02/97 6:40p Matt
 * Fixed code & added functions for group save/load
 *
 * 12    9/02/97 5:17p Jason
 * changes for dynamic lighting
 *
 * 11    9/02/97 11:56a Jason
 * saves alpha component
 *
 * 10    9/02/97 11:47a Matt
 * Changed externs for read/write segment to read/write room
 *
 * 9     8/22/97 2:42p Jason
 * added sky saving to loadlevel routines
 *
 * 8     8/13/97 11:52a Jason
 * now saves lightmap info
 *
 * 7     8/12/97 10:29a Samir
 * Read in a script handle per object in level.
 *
 * 6     8/11/97 1:53p Matt
 * Ripped out robot & powerup pages, and added generic page
 *
 * 5     8/07/97 3:19p Jason
 * replaced old terrain tmap system with new one
 *
 * 4     8/01/97 3:59p Jason
 * added loading and saving of terrain chunks
 *
 * 3     7/24/97 3:23p Matt
 * Saved several additional editor state variables
 *
 * 5     6/18/97 11:58a Jason
 * first pass at getting new room engine stuff reading in
 *
 * 4     5/22/97 5:16p Jason
 * added tmap2 capability to the terrain
 *
 * 3     5/13/97 11:58a Matt
 * Made group file code save & load the level file version number to use
 * when reading segments
 *
 * 2     3/12/97 3:23p Matt
 * Make ReadSegment() and WriteSegment() public, and removed a bunch of
 * version number passing that wasn't being used.
 *
 * 1     2/10/97 11:24a Matt
 *
 * $NoKeywords: $
 */

#include "cfile.h"
#include "room.h"

// Chunk types
#define CHUNK_TEXTURE_NAMES "TXNM"
#define CHUNK_GENERIC_NAMES "GNNM"
#define CHUNK_ROBOT_NAMES "RBNM"
#define CHUNK_POWERUP_NAMES "PWNM"
#define CHUNK_DOOR_NAMES "DRNM"
#define CHUNK_ROOMS "ROOM"
#define CHUNK_ROOM_WIND "RWND"
#define CHUNK_OBJECTS "OBJS"
#define CHUNK_TERRAIN "TERR"
#define CHUNK_EDITOR_INFO "EDIT"
#define CHUNK_SCRIPT "SCPT"
#define CHUNK_TERRAIN_HEIGHT "TERH"
#define CHUNK_TERRAIN_TMAPS_FLAGS "TETM"
#define CHUNK_TERRAIN_LINKS "TLNK"
#define CHUNK_TERRAIN_SKY "TSKY"
#define CHUNK_TERRAIN_END "TEND"
#define CHUNK_SCRIPT_CODE "CODE"
#define CHUNK_TRIGGERS "TRIG"
#define CHUNK_LIGHTMAPS "LMAP"
#define CHUNK_BSP "CBSP"
#define CHUNK_OBJECT_HANDLES "OHND"
#define CHUNK_GAME_PATHS "PATH"
#define CHUNK_BOA "CBOA"
#define CHUNK_NEW_BSP "CNBS"
#define CHUNK_LEVEL_INFO "INFO"
#define CHUNK_PLAYER_STARTS "PSTR"
#define CHUNK_MATCEN_DATA "MTCN"
#define CHUNK_LEVEL_GOALS "LVLG"
#define CHUNK_ROOM_AABB "AABB"
#define CHUNK_NEW_LIGHTMAPS "NLMP"
#define CHUNK_ALIFE_DATA "LIFE"
#define CHUNK_TERRAIN_SOUND "TSND"
#define CHUNK_BNODES "NODE"
#define CHUNK_OVERRIDE_SOUNDS "OSND"
#define CHUNK_FFT_MOD "FFTM"

#ifndef NEWEDITOR // for compatibility with Descent 3 v1.2 and earlier
#define LEVEL_FILE_VERSION 132
#else
#define LEVEL_FILE_VERSION 128
#endif

// This is the oldest version the code will read
#define LEVEL_FILE_OLDEST_COMPATIBLE_VERSION 13

// Version numbers of specific changes
#define LEVEL_FILE_SCRIPTING 18
#define LEVEL_FILE_NEWSCRIPTING 26
#define LEVEL_FILE_SCRIPTNAMES 31
#define LEVEL_FILE_SCRIPTPARMS 46
#define LEVEL_FILE_TRIGPARMS 48
#define LEVEL_FILE_SCRIPTCHECK 70
#define LEVEL_FILE_OSIRIS1DEAD 97

// Version number changes:
// 0 -> 1	Save curseg & markedseg to file
// 1 -> 2 Save texture names to file & remap when load
// 2 -> 3	Save selected list
// 3 -> 4	Save triggers and doorways
// 4 -> 5 Save segnums as ints, not shorts.  Also, save terrain height array
// 5 -> 6 Save & xlate names for robots, powerups, & doors
// 6 -> 7 Instead of saving a bunch of type-specific data, read it from the data page
// 7 -> 8 Now saves terrain sky data
// 8 -> 9 Now saves the mine/terrain links
// 9 ->10 Changes for floating segments
// 10->11 Added tmap2 textures to terrain
// 11->12 save u,v coords for terrain
// 12->13 New chunk-based file format to save room data
// 13->14 Save some more data in editor chunk
// 14->15 Saves terrain info in chunk format
// 15->16 Changes UV terrain chunk format
// 16->17 Generic objects replace robots & powerups
// 17->18 Custom script handle read and written for objects 8-11-97
// 18->19 Now saves lightmap info with room faces
// 19->20 Now saves mine/terrain links and sky info
// 20->21 Now saves alpha component per vertex
// 21->22 Now saves upper left vertex
// 22->23 Save portal num as byte
// 23->24 Saves RGB lighting for terrain
// 24->25 Saves terrain dynamic lighting table
// 25->26 If level isn't version 26 or above, then we need to ignore any script chunk, create a new one.
// 26->27 Save face flags as short
// 27->28 Store doorway information
// 28->29 Killed static light field in room struct
// 29->30 Store new lightmap_info information
// 30->31 Read in script names instead of handles for all objects.
// 31->32	Do RLE compression for terrain data
// 32->33	Save more info about doorways
// 33->34 Save object id as short
// 34->35 Save info about object lightmaps
// 35->36 Save "keys_needed" field for doorways
// 36->37 Do tricks to restore OF_POLYGON_OBJECT flag
// 37->38 Do RLE compression for lightmaps
// 38->39 Store lightmap spacing as floats,not ubytes
// 39->40 Save/load shadow rooms/faces
// 40->41 Save horizon texture info
// 41->42	Save BSP info for the mine
// 42->43 Save extra texture pieces for terrain sky
// 43->44	Save terrain satellite flags
// 44->45 Objects no longer compressed, so save objnum (handle,actually) with each object
// 45->46 Objects and triggers have optional script parameters.  Also, compressed script info in object.
// 46->47 Only save one viewer id, not two
// 47->48 Read in trigger parameters too.
// 48->49	Objects now store terrain flag as part of roomnum
// 49->50 Store light multiplier per face
// 50->51 Store fvec/uvec of path nodes
// 51->52 Now saves/loads wall pulsing data
// 52->53 Face light multiple now works in quarter steps
// 54->55 Add wireframe view info to editor chunk
// 55->56 Don't read/write useless face info such as rgba
// 56->57 Don't read/write pixel error or terrain distance
// 57->58 Read uvec,rvec for lightmap infos
// 58->59 Store lightmap normals
// 59->60 Don't store lightmap normals for dynamic objects
// 60->61 Trimmed some fat from rooms,faces structures
// 61->62 BOA now saves out the portal cost array
// 62->63 We now save path_pnts with rooms and portals
// 62->64 Translate old 565 lightmaps into 1555 format
// 64->65 Added object parent_handles
// 65->66 Store lightmap spacing as bytes, not floats
// 66->67 Save volume lighting for rooms
// 67->68	Save specular lighting for rooms
// 68->69 Save terrain sky radius
// 69->70 Read in whether attached script is default or custom.
// 70->71 Threw out vertex based specularity and went with lightmap based specularity
// 71->72 Added terrain occlusion data
// 72->73 Now store volumetric fog info with room
// 73->74 Support for new banded-dome skies
// 74->75 Now supports satellite lighting values
// 75->76	Saves BOA_vis_checksum to file
// 76->77 Saves multiple specular values
// 77->78 Added ambient sound field to room
// 78->79	Added mirrored faces
// 79->80	New single-face portals
// 80->81	Save marked face & vert
// 81->82	Door name translation
// 82->83 Added gravity to level info
// 83->84 Added level goal information
// 84->85 Save the amount of memory needed by the rooms
// 85->86 Added room AABB information to save
// 86->87 Added damage per second for terrain sky
// 87->88 Add fog scalar for adjustable fog distance
// 88->89 Took out dynamic light data saving
// 89->90 Save out more than one lightmap_info per lightmap
// 90->91 Save out lightmap_info x1,y1 origin in megalightmap
// 91->92 Reduced boa save size
// 92->93 Ambeint Life
// 93->94 Changed object handle bit allocation
// 94->95 Added object names
// 95->96 Added room names, & took out room compression (so saved list can have holes)
// 96->97 Ripped out OSIRIS v1.0
// 97->98 ???
// 98->99 Added trigger names
// 99->100 Add combinable portals
// 100->101 Changed object flags from short to int, and only read certain flags
// 101->102 Changed terrain textures system
// 102->103 Removed portal triggers
// 103->104 Changed terrain sky system
// 104->105 Added custom_default_script_name and custom_default_module_name to objects
// 105->106 Rewrote doorway system
// 106->107 Save out BOA terrain info (temp, for vis and multiplay only)
// 107->108 Added room damage
// 108->109 Added blastable doors
// 109->110 Update BOA to its final form
// 109->111 Don't save an object's parent handle
// 111->112 Added BOA_connect information
// 112->113 Save room multipliers out with editor chunk
// 113->114 Took out band textures
// 114->115 Added soundsource objects
// 115->116 Save satellite sizes
// 116->117 added smooth specular mapping
// 117->118 added room ambience settings
// 118->119 Save sound names, not indices
// 119->120 Save the number of player starts to the file
// 120->121 Added the BOA Node chunk
// 121->122 Added the ability for a level to always have the ceiling checked
// 122->123 Added the bnode_index to portals...
// 122->124 Added the bnode verify flag
// 124->125 Removed the portal field from BNode edges as the room portals now contain bnode info (so it isn't necessary)
// 125->126 Save lightmap spacing info
// 126->127 Added ceiling value
// 127->128 Save rest of lighting parameters
// 128->129 Add the Override sound chunk
// 129->130 Added the modified force field bounce textures
// 130->131 Added the powerups ignore gravity checkbox
// 131->132 Added another ff bounce texture for Dan (his last day wish)

// Load a level file
// Returns 1 if file read ok, else 0
// cb_fn returns current chunk, parm1 = # bytes in chunk, parm2 = number of bytes in file
//	parm1 = -1, for 1st just opened level
//	parm2 = -2, for done loading.
int LoadLevel(char *filename, void (*cb_fn)(const char *, int, int) = NULL);

// Save a level file
// Returns 1 if file saved ok, else 0
int SaveLevel(char *filename, bool f_save_room_AABB = true);

// Reads a room from a disk file
// Parameters:	ifile - file to read from
//					rp - room to read
//					version - version number of file
// Returns:		1 if read ok, else 0
int ReadRoom(CFILE *ifile, room *rp, int version);

// Writes a room to a disk file
// Parameters:	ofile - file to write to
//					rp - room to write
// Returns:		1 if written ok, else 0
int WriteRoom(CFILE *ofile, room *rp);

// Write the texture names for remapping when level is loaded
void WriteTextureList(CFILE *ofile);

// Read the texture names & build the xlate table
void ReadTextureList(CFILE *ifile);

// Primarily for multiplayer, makes sure the client and server levels are the same

#include "../md5/md5.h"
extern MD5 *Level_md5;
inline void RestartLevelMD5() {
  if (Level_md5)
    delete Level_md5;
  Level_md5 = new MD5();
}

inline void AppendToLevelChecksum(int val) {
  if (!Level_md5) {
    return;
  }
  Level_md5->update(val);
}

inline void AppendToLevelChecksum(uint32_t val) {
  if (!Level_md5) {
    return;
  }
  Level_md5->update(val);
}

inline void AppendToLevelChecksum(uint16_t val) {
  if (!Level_md5) {
    return;
  }
  Level_md5->update(val);
}

inline void AppendToLevelChecksum(int16_t val) {
  if (!Level_md5) {
    return;
  }
  Level_md5->update(val);
}

inline void AppendToLevelChecksum(float val) {
  if (!Level_md5) {
    return;
  }
  Level_md5->update(val);
}

inline void AppendToLevelChecksum(vector val) {
  if (!Level_md5) {
    return;
  }
  Level_md5->update(val.x);
  Level_md5->update(val.y);
  Level_md5->update(val.z);
}

inline void AppendToLevelChecksum(uint8_t val) {
  if (!Level_md5) {
    return;
  }
  Level_md5->update(val);
}

inline void GetLevelMD5Sum(uint8_t digest[16]) {
  if (!Level_md5) {
    for (int i = 0; i < 16; i++)
      digest[i] = 0;
    return;
  }
  Level_md5->digest(digest);
}

