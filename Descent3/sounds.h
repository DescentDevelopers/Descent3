/*
 * $Logfile: /DescentIII/main/sounds.h $
 * $Revision: 22 $
 * $Date: 5/08/99 3:30p $
 * $Author: Matt $
 *
 * Constants for sounds
 *
 * $Log: /DescentIII/main/sounds.h $
 * 
 * 22    5/08/99 3:30p Matt
 * Added weapon-hit-water sound.
 * 
 * 21    4/30/99 3:28p Matt
 * Weapons now make a special sound when colliding with lava & volatile
 * surfaces, instead of the standard hit-wall sound.
 * 
 * 20    4/29/99 3:50p Matt
 * Added hiss sound for volatile surface damage
 * 
 * 19    4/29/99 1:10p Jeff
 * added sounds for inventory and countermeasure switch
 * 
 * 18    4/11/99 5:03p Matt
 * Changed static sound remapping code to use simpler & more
 * space-efficient scheme.  Also deleted an unused entry in the static
 * sound list.
 * 
 * 17    3/04/99 10:57a Matt
 * Added goal message notification sound
 * 
 * 16    2/22/99 10:24p Matt
 * Added a static sound for debris explosion
 * 
 * 15    2/18/99 2:49p Matt
 * Added goal complete sound
 * 
 * 14    11/23/98 1:50p Jason
 * added thruster sounds
 * 
 * 13    10/21/98 4:53p Jeff
 * added some sounds to sounds array
 * 
 * 12    10/20/98 2:09p Jeff
 * added 2 more sounds to static sounds
 * 
 * 11    10/20/98 12:49p Matt
 * Removed obsolete sounds from static sound list.
 * 
 * 10    10/19/98 7:18p Matt
 * Added system to support different types of damage to the player and
 * have these different types make different sounds.
 * 
 * 9     10/14/98 1:02p Jason
 * fixed FindSoundName issues
 * 
 * 8     9/21/98 8:19p Chris
 * Improved volatile and forcefield hits
 * 
 * 7     9/18/98 1:27p Matt
 * Added afterburner release sound
 * 
 * 6     5/22/98 11:59a Chris
 * Fixed improper uses of FindSoundName and made the proper static sounds
 * 
 * 5     5/03/98 10:22p Matt
 * Added breaking glass sound
 * 
 * 4     2/16/98 4:55p Chris
 * Added default sounds for explosions
 * 
 * 3     12/22/97 6:19p Chris
 * Moved weapon battery firing sound off the projectile (weapon) and into
 * the weapon battery.
 * 
 * 2     7/15/97 7:29p Chris
 * Added a sound for helicopter blades.
 * 
 * 1     5/13/97 10:32p Matt
 * Symbols for sounds referenced in code
 *
 * $NoKeywords: $
 */

//THESE SOUNDS MUST MATCH THE Static_sound_names[] array in SoundLoad.cpp

#define SOUND_NONE_INDEX				0		//the not-defined sound
#define SOUND_REFUELING					1
#define SOUND_ROBOT_EXPLODE_1			2
#define SOUND_ROBOT_EXPLODE_2			3
#define SOUND_BUILDING_EXPLODE		4
#define SOUND_BREAKING_GLASS			5
#define SOUND_MELEE_HIT_0				6 	
#define SOUND_MELEE_HIT_1				7 	
#define SOUND_AFTERBURNER				8 	
#define SOUND_ENERGY_DRAIN				9		
#define SOUND_MISSLE_LOCK				10		
#define SOUND_POWERUP_PICKUP			11
#define SOUND_DO_NOT_HAVE_IT			12		
#define SOUND_CHANGE_PRIMARY			13		
#define SOUND_CHANGE_SECONDARY		14		
#define SOUND_PLAYER_HIT_WALL			15		
#define SOUND_AFTERBURNER_TAIL		16
#define SOUND_FORCEFIELD_BOUNCE		17	
#define SOUND_HEADLIGHT					18
#define SOUND_CHEATER					19
#define SOUND_ENERGY_CONVERTER		20
#define SOUND_COCKPIT					21
#define SOUND_METALLIC_DOOR_HIT		22
#define SOUND_WALL_FADE					23
#define SOUND_RAINDROP						24	
#define SOUND_LIGHTNING						25
#define SOUND_HIT_BY_ENERGY_WEAPON 		26
#define SOUND_HIT_BY_MATTER_WEAPON		27
#define SOUND_HIT_BY_CONCUSSIVE_FORCE	28
#define SOUND_PLAYER_BURNING				29
#define SOUND_MENU_SOUND_CLICK			30
#define SOUND_HOSTAGE_PICKUP				31
#define SOUND_BRIEF_STARTUP				32
#define SOUND_BRIEF_STATIC					33
#define SOUND_BRIEF_MONITOROFF			34
#define SOUND_BRIEF_RUNNING				35
#define SOUND_BRIEF_BULLB					36
#define SOUND_BRIEF_TYPE					37
#define SOUND_SHIP_IDLE						38
#define SOUND_SHIP_FORWARD_THRUST		39
#define SOUND_SHIP_FORWARD_RELEASE		40
#define SOUND_GOAL_COMPLETE				41
#define SOUND_DEBRIS_EXPLODE				42
#define SOUND_GAME_MESSAGE					43
#define SOUND_CHANGE_COUNTERMEASURE			44
#define SOUND_CHANGE_INVENTORY				45
#define SOUND_VOLATILE_HISS				46
#define SOUND_WEAPON_HIT_LAVA				47
#define SOUND_WEAPON_HIT_WATER			48
#define NUM_STATIC_SOUNDS					49		//update this every time you add a new sound

