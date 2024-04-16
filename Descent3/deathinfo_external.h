/*
 * $Logfile: /DescentIII/main/deathinfo_external.h $
 * $Revision: 10 $
 * $Date: 4/26/99 11:39p $
 * $Author: Matt $
 *
 * I don't know why we need this file, but here it is.
 *
 * $Log: /DescentIII/main/deathinfo_external.h $
 *
 * 10    4/26/99 11:39p Matt
 * Added a flag to make fly-in-air deaths no tumble
 *
 * 9     4/25/99 10:19p Matt
 * Fixed multiplayer and demo problems will killing an object from script,
 * and cleaned up the death code a bit in the process.
 *
 * 8     4/23/99 12:32a Matt
 * Added a death info option to play the explosion sound at the start of
 * the delay, instead of at the end when the object actually dies.
 *
 * 7     4/21/99 3:01p Matt
 * Added a new type for dying objects that have AI, instead of keeping a
 * flag in the dying info.
 *
 * 6     4/02/99 2:46p Matt
 * Moved flags from deathinfo to deathinfo_external, because the arhive
 * builder only copies the latter into the archive.
 *
 * 5     4/02/99 11:23a Matt
 * Made KillObject not take a death_info struct, but rather the death info
 * as individual parameters.  Moved death_info into objinfo.h, since it's
 * only used for generic objects.  Took out fade-away death hack, now that
 * fade-away deaths can be explicitely set.
 *
 */

//
//  Object death info flags
//

// Not a bit flag, but an override value
#define DF_DEFAULT -1 // use the default death for this object

// Unused flag
#define DF_UNUSED 0x0000001

// Delay options
#define DF_DELAY_FROM_ANIM 0x0000002      // delay time from death animation
#define DF_DELAY_SPARKS 0x0000004         // delay with sparks
#define DF_DELAY_LOSES_ANTIGRAV 0x0000008 // object gets gravity during delay
#define DF_DELAY_SMOKES 0x0000010         // delay with smoke
#define DF_DELAY_FLYING 0x0100000         // delay with object flying up into the air
#define DF_DELAY_FIREBALL 0x0200000       // delay with fireballs
#define DF_DELAY_FADE_AWAY 0x0400000      // fade away
#define DF_DELAY_NO_TUMBLE_FLY 0x2000000  // don't tumble while flying up in the air

// Options for what happens on death
#define DF_FIREBALL 0x0000020       // there are fireballs when the object dies
#define DF_BREAKS_APART 0x0000040   // the object breaks into pieces when it dies
#define DF_BLAST_RING 0x0000080     // a blast ring is created when the object dies
#define DF_REMAINS 0x0000100        // the object does not go away when it does
#define DF_LOSES_ANTIGRAV 0x0000200 // object gets gravity on death
#define DF_FADE_AWAY 0x0800000      // fades away

// Explosion size if there is a death fireball
#define DF_EXPL_SMALL 0x0000000  // use a small explosion
#define DF_EXPL_MEDIUM 0x0000400 // use a medium explosion
#define DF_EXPL_LARGE 0x0000800  // use a large explosion
#define DF_EXPL_SIZE_MASK 0x0000c00
#define DF_EXPL_SIZE_SHIFT 10

// What happens when this object hits something during delay
#define DF_CONTACT_FIREBALL 0x0001000     // creates fireballs
#define DF_CONTACT_BREAKS_APART 0x0002000 // break apart
#define DF_CONTACT_BLAST_RING 0x0004000   // blast ring
#define DF_CONTACT_REMAINS 0x0008000      // stays around

// Whether the debris puts off smoke
#define DF_DEBRIS_SMOKES 0x0010000 // the debris that's created smokes

// What happens to the debris when it times out or hits something
#define DF_DEBRIS_FIREBALL 0x0020000   // creates fireballs
#define DF_DEBRIS_BLAST_RING 0x0040000 // blast ring
#define DF_DEBRIS_REMAINS 0x0080000    // stays around

// Sound option
#define DF_DELAY_SOUND 0x1000000 // play sound at start of fade
