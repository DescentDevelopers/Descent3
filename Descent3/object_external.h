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

/*
 * $Logfile: /DescentIII/main/object_external.h $
 * $Revision: 37 $
 * $Date: 10/26/99 10:32a $
 * $Author: Jeff $
 *
 * Object defines usable by both the main code & the DLLs
 *
 * $Log: /DescentIII/main/object_external.h $
 *
 * 37    10/26/99 10:32a Jeff
 * no red guidebot in non-Windows versions
 *
 * 36    10/20/99 5:40p Chris
 * Added the Red Guidebot
 *
 * 35    10/12/99 11:06a Jeff
 * added object effect flags for negative lighting and virus infection
 *
 * 34    6/08/99 1:01p Jason
 * changes for bumpmapping
 *
 * 33    4/21/99 3:01p Matt
 * Added a new type for dying objects that have AI, instead of keeping a
 * flag in the dying info.
 *
 * 32    4/20/99 8:14p Chris
 * Added support for object's that hit the ceiling and for making the
 * level always check for the ceiling (inside and outside the mine)
 *
 * 31    4/20/99 5:39p Matt
 * Added macro to check if an object is a robot.  Does the
 * type-is-robot-or-type-is-building-with-AI check.
 *
 * 30    4/18/99 10:55p Chris
 * Added ignore own concussive blasts
 *
 * 29    4/18/99 8:13p Chris
 * Fixed the floating flare problems (where windows where broken out and
 * the flare remained)
 *
 * 28    4/05/99 10:54a Matt
 * Added auto-waypoint system
 *
 * 27    3/31/99 3:59p Chris
 * made the MC_ stuff externalized to OSIRIS.
 *
 * 26    3/28/99 5:56p Matt
 * Added sparking effect for objects
 *
 * 25    3/26/99 3:26p Jeff
 * option to display hud message when cloaking
 *
 * 24    3/11/99 6:31p Jeff
 * numerous fixes to demo system in multiplayer games (when
 * recording/playback a demo in a multiplayer game)
 *
 * 23    2/25/99 11:01a Matt
 * Added new explosion system.
 *
 * 22    2/22/99 11:38p Matt
 * Deleted static debris objects, since they were never used
 *
 * 21    2/21/99 4:35p Chris
 * Improving the level goal system...  Not done.
 *
 * 20    2/21/99 4:20p Matt
 * Added SoundSource objects (and reformatted parts of the object header
 * files).
 *
 * 19    2/13/99 12:36a Jeff
 * new object flag.  set for when an object is currently in a player's
 * inventory
 *
 * 18    2/12/99 3:38p Jason
 * added client side interpolation
 *
 * 17    2/11/99 6:25p Chris
 * Added PF_NO_COLLIDE_DOORS
 *
 * 16    2/10/99 2:49p Matt
 * Renamed OBJECT_HANDLE_INVALID to OBJECT_HANDLE_BAD
 *
 * 15    2/10/99 1:47p Matt
 * Changed object handle symbolic constants
 *
 * 14    2/08/99 5:26p Jeff
 * removed all calls to MultiSendRemoveObject, incorportated into
 * SetObjectDeadFlag.  Fixes sequencing issues in multiplayer
 *
 * 13    2/05/99 1:26p Matt
 * Added a macro to check if a type is one of the generic types
 *
 * 12    1/27/99 6:08p Jason
 * first pass at markers
 *
 * 11    1/21/99 11:15p Jeff
 * pulled out some structs and defines from header files and moved them
 * into seperate header files so that multiplayer dlls don't require major
 * game headers, just those new headers.  Side effect is a shorter build
 * time.  Also cleaned up some header file #includes that weren't needed.
 * This affected polymodel.h, object.h, player.h, vecmat.h, room.h,
 * manage.h and multi.h
 *
 * 10    1/21/99 3:35p Jason
 * added liquid code
 *
 * 9     1/18/99 8:07p Chris
 * Added the no-collide same flag (for flocks and nests)
 *
 * 8     1/18/99 2:46p Matt
 * Combined flags & flags2 fields in object struct
 *
 * 7     1/13/99 3:25a Chris
 * Added Obj_Burning and Obj_IsEffect to OSIRIS
 *
 * 6     1/13/99 2:29a Chris
 * Massive AI, OSIRIS update
 *
 * 5     1/11/99 2:14p Chris
 * Massive work on OSIRIS and AI
 *
 * 4     1/05/99 5:09p Jason
 * added permissable server networking (ala Quake/Unreal) to Descent3
 *
 * 3     1/05/99 4:16p Matt
 * Added SourceSafe header
 *
 */

#ifndef OBJECT_EXTERNAL_H_
#define OBJECT_EXTERNAL_H_

// Use this handle when you want a handle that will never be a valid object
#define OBJECT_HANDLE_BAD 0

// Use this handle when you want a handle that will never be a valid object
#define OBJECT_HANDLE_NONE -1

// Object types
#define OBJ_NONE 255        // unused object
#define OBJ_WALL 0          // A wall... not really an object, but used for collisions
#define OBJ_FIREBALL 1      // a fireball, part of an explosion
#define OBJ_ROBOT 2         // an evil enemy
#define OBJ_SHARD 3         // a piece of glass
#define OBJ_PLAYER 4        // the player on the console
#define OBJ_WEAPON 5        // a laser, missile, etc
#define OBJ_VIEWER 6        // a viewed object in the editor
#define OBJ_POWERUP 7       // a powerup you can pick up
#define OBJ_DEBRIS 8        // a piece of robot
#define OBJ_CAMERA 9        // a camera object in the game
#define OBJ_SHOCKWAVE 10    // a shockwave
#define OBJ_CLUTTER 11      // misc objects
#define OBJ_GHOST 12        // what the player turns into when dead
#define OBJ_LIGHT 13        // a light source, & not much else
#define OBJ_COOP 14         // a cooperative player object.
#define OBJ_MARKER 15       // a map marker
#define OBJ_BUILDING 16     // a building
#define OBJ_DOOR 17         // a door
#define OBJ_ROOM 18         // a room, visible on the terrain
#define OBJ_PARTICLE 19     // a particle
#define OBJ_SPLINTER 20     // a splinter piece from an exploding object
#define OBJ_DUMMY 21        // a dummy object, ignored by everything
#define OBJ_OBSERVER 22     // an observer in a multiplayer game
#define OBJ_DEBUG_LINE 23   // something for debugging, I guess.  I sure wish people would add comments.
#define OBJ_SOUNDSOURCE 24  // an object that makes a sound but does nothing else
#define OBJ_WAYPOINT 25     // a object that marks a waypoint
#define MAX_OBJECT_TYPES 26 // Update this when adding new types
// NOTE: if you add a type here, you must add the name to Object_type_names[]

// Condition to check if the specified type in a generic type
#define IS_GENERIC(type)                                                                                               \
  ((type == OBJ_CLUTTER) || (type == OBJ_BUILDING) || (type == OBJ_ROBOT) || (type == OBJ_POWERUP))

// Condition to check if the specified object is a robot (checks for buildings with AI)
#define IS_ROBOT(objp) ((objp->type == OBJ_ROBOT) || ((objp->type == OBJ_BUILDING) && objp->ai_info))

// Effect type flags
#define EF_CLOAKED 1            // This object is cloaked
#define EF_DEFORM 2             // This object is deforming
#define EF_COLORED 4            // This object is slowing fading from a color
#define EF_NAPALMED 8           // This object is on fire
#define EF_VOLUME_LIT 16        // This object has volumetric lighting on it
#define EF_VOLUME_CHANGING 32   // This object is morphing from one volume color to another
#define EF_FREEZE 64            // This object is being slowed down
#define EF_LINE_ATTACH 128      // This object has a grapple between it and another object
#define EF_SPECULAR 256         // This object has specular lighting
#define EF_FADING_IN 512        // Object is fading in
#define EF_FADING_OUT 1024      // Object is fading out
#define EF_LIQUID 2048          // Object has the FOV liquid effect
#define EF_CLOAK_WITH_MSG 4096  // This object (which is OBJ_PLAYER) needs to print a HUD message when cloak wears off
#define EF_SPARKING 8192        // This object is sparking
#define EF_BUMPMAPPED 16384     // This object is sparking
#define EF_NEGATIVE_LIGHT 32768 // This object subtracts light that it gives off
#define EF_VIRUS_INFECTED 65536 // This object shows signs of being infected

// Control types - what tells this object what do do
#define CT_NONE 0          // doesn't move (or change movement)
#define CT_AI 1            // driven by AI
#define CT_EXPLOSION 2     // explosion sequencer
#define CT_FLYING 4        // the player is flying
#define CT_SLEW 5          // slewing
#define CT_FLYTHROUGH 6    // the flythrough system
#define CT_WEAPON 9        // laser, etc.
#define CT_DEBRIS 12       // this is a piece of debris
#define CT_POWERUP 13      // animating powerup blob
#define CT_SOAR 14         // Soar object
#define CT_PARTICLE 15     // Particle
#define CT_SPLINTER 16     // Splinter
#define CT_SOUNDSOURCE 17  // SoundSource
#define CT_DYING 18        // slowly dying
#define CT_DYING_AND_AI 19 // dying with AI

// Movement types
#define MT_NONE 0    // Doesn't move
#define MT_PHYSICS 1 // Moves by physics
#define MT_WALKING 2 // Uses physics data structure, but uses a different physics code pipe
#define MT_AT_REST 3
#define MT_SHOCKWAVE                                                                                                   \
  4                     // Moves like a shockwave
                        // (actually this is for space conservation -
                        //  it could be more logically used as a
                        //  control type)
#define MT_OBJ_LINKED 5 // Allows sticky objects to link to polymodel objects

// Movement classes
#define MC_STANDING 0
#define MC_FLYING 1
#define MC_ROLLING 2
#define MC_WALKING 3
#define MC_JUMPING 4

// Attach types
#define AT_RAD 0
#define AT_ALIGNED 1
#define AT_UNALIGNED 2

// Render types
#define RT_NONE 0          // does not render
#define RT_POLYOBJ 1       // a polygon model
#define RT_FIREBALL 2      // a fireball
#define RT_WEAPON 3        // a non-polygonal weapon
#define RT_LINE 4          // a line
#define RT_PARTICLE 5      // render as particle type
#define RT_SPLINTER 6      // render as a splinter
#define RT_ROOM 7          // rendered as a room, not an object
#define RT_EDITOR_SPHERE 8 // renderd as a sphere in the editor, else not rendered
#define RT_SHARD 9         // bits of broken glass

// misc object flags
#define OF_FORCE_CEILING_CHECK 0x00000001 // this object is exploding
#define OF_DEAD 0x00000002                // this object is dead, so next time we can, we should delete this object.
#define OF_DESTROYED 0x00000004           // this has been killed, and is showing the dead version
#define OF_STOPPED_THIS_FRAME 0x00000008
#define OF_ATTACHED 0x00000010 // this object is a fireball attached to another object
#define OF_MOVED_THIS_FRAME 0x00000020
#define OF_AI_DO_DEATH 0x00000040    // this is so objects can be vulerable, but the death is scripted out
#define OF_USES_LIFELEFT 0x00000080  // this object's lifeleft is valid
#define OF_SAFE_TO_RENDER 0x00000100 // this object can be seen this frame
#define OF_OUTSIDE_MINE 0x00000200   // this object has slewed outside of the mine (EDITOR ONLY)
#define OF_DESTROYABLE 0x00000400    // this object can be destroyed
#define OF_BIG_OBJECT 0x00000800     // this object is classified as being BIG
#define OF_POLYGON_OBJECT 0x00001000 // This object is a bonafide polygon object
#define OF_DYING 0x00002000          // This object is going through its death throes
#define OF_USE_DESTROYED_POLYMODEL                                                                                     \
  0x00004000 // This object should be drawn with its destroyed model (usual a lightmapped building)
#define OF_RENDERED 0x00008000
#define OF_NO_OBJECT_COLLISIONS 0x00010000
#define OF_STUCK_ON_PORTAL 0x00020000
#define OF_TEMP_GRAVITY 0x00040000
#define OF_CLIENT_KNOWS 0x00080000       // Client knows about this object
#define OF_SERVER_SAYS_DELETE 0x00100000 // Server tells me to delete this object
#define OF_SERVER_OBJECT 0x00200000      // Server told me to create this object
#define OF_PING_ACCELERATE 0x00400000    // Server is telling me to accelerate this object
#define OF_AI_DEATH 0x00800000
#define OF_SEND_MULTI_REMOVE_ON_DEATH                                                                                  \
  0x01000000 // when the object is being deleted on the server, send a MultiSendRemoveObject call
#define OF_SEND_MULTI_REMOVE_ON_DEATHWS                                                                                \
  0x02000000                    // same as above, but a sound should be played on the clients along with the remove
#define OF_PREDICTED 0x04000000 // This object moves with client-side prediction
#define OF_INPLAYERINVENTORY 0x08000000 // This object is in a player's inventory
#define OF_INFORM_PLAYER_COLLIDE_TO_LG 0x10000000
#define OF_INFORM_PLAYER_WEAPON_COLLIDE_TO_LG 0x20000000
#define OF_INFORM_DESTROY_TO_LG 0x40000000
#define OF_CLIENTDEMOOBJECT                                                                                            \
  0x80000000 // This object was sent to the client via
             // MultiSendObject() and so it should
             // recorded when writing a demo when it is
             // created and destroyed

/// Which object flags get saved with the level
#define OBJECT_SAVE_LOAD_FLAGS (OF_OUTSIDE_MINE)

// physics flags
#define PF_TURNROLL 0x01                // Roll when turning
#define PF_LEVELING 0x02                // Level object with closest side
#define PF_BOUNCE 0x04                  // Bounce (not slide) when hit will
#define PF_WIGGLE 0x08                  // Wiggle while flying
#define PF_STICK 0x10                   // Object sticks (stops moving) when hits wall
#define PF_PERSISTENT 0x20              // Object keeps going even after it hits another object (eg, fusion cannon)
#define PF_USES_THRUST 0x40             // This object uses its thrust
#define PF_GRAVITY 0x80                 // Effected by gravity
#define PF_IGNORE_OWN_CONC_FORCES 0x100 // Effected by magnetism
#define PF_WIND 0x200                   // Effected by wind
#define PF_USES_PARENT_VELOCITY 0x400
#define PF_FIXED_VELOCITY 0x800
#define PF_FIXED_ROT_VELOCITY 0x1000
#define PF_NO_COLLIDE_PARENT 0x2000 // this object cannot collide with its parent
#define PF_HITS_SIBLINGS                                                                                               \
  0x4000 // this object can collide with its siblings (like a bomb) // chrishack -- add flag to weapon page
#define PF_REVERSE_GRAVITY 0x8000 // this object flys upward with gravity
#define PF_GRAVITY_MASK (PF_REVERSE_GRAVITY | PF_GRAVITY)
#define PF_NO_COLLIDE 0x10000          // No collisions AND NO RELINKS -- DANGEROUS TO USE if not used correctly
#define PF_NO_ROBOT_COLLISIONS 0x20000 // No collisions occur with robots
#define PF_POINT_COLLIDE_WALLS 0x40000 // When colliding with walls, make our radius zero
#define PF_HOMING 0x80000              // This object (weapon) homes
#define PF_GUIDED 0x100000             // This object is guided
#define PF_IGNORE_CONCUSSIVE_FORCES 0x200000
#define PF_DESTINATION_POS 0x400000
#define PF_LOCK_X 0x800000
#define PF_LOCK_Y 0x1000000
#define PF_LOCK_Z 0x2000000
#define PF_LOCK_P 0x4000000
#define PF_LOCK_H 0x8000000
#define PF_LOCK_B 0x10000000
#define PF_LOCK_MASK (PF_LOCK_X | PF_LOCK_Y | PF_LOCK_Z | PF_LOCK_P | PF_LOCK_H | PF_LOCK_B)
#define PF_NEVER_USE_BIG_SPHERE 0x20000000
#define PF_NO_SAME_COLLISIONS 0x40000000
#define PF_NO_DOOR_COLLISIONS 0x80000000

// Generic Sound indices
#define GSI_AMBIENT 0
#define GSI_EXPLODE 1

// Static Robot ids
#define ROBOT_GUIDEBOT 0 // NOTE: this must match GENOBJ_GUIDEBOT
#define ROBOT_GUIDEBOTRED 2 // NOTE: this must match GENOBJ_GUIDEBOTRED

#endif
