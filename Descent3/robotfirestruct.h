#ifndef ROBOTFIRESTRUCT_H_
#define ROBOTFIRESTRUCT_H_

#include "pstypes.h"
#include "vecmat.h"
#include "robotfirestruct_external.h"

// NOTE: CHANGE gunbattery.h (IN POFGEN PROJECT) if constants are changed
// NOTE: Robots are limited to the number of wb configurations of the player.  This seem like an
//       adequit number (currently 21)
// (MAX_PRIMARY_WEAPONS + MAX_SECONDARY_WEAPONS + 1/*Flare*/)
// NOTE: Cannot include weapon.h because of circular dependances.
#define MAX_WBS_PER_OBJ	    21 

// Attach to the polymodel
typedef struct poly_wb_info
{
	// Static Data  (Add to robot generic page)
	unsigned short num_gps;
	ubyte gp_index[MAX_WB_GUNPOINTS];

	// Turrets are listed from most important (greatest mobility) to least important
	ubyte num_turrets;
	unsigned short turret_index[MAX_WB_TURRETS];

} poly_wb_info;



//Next free WBF is 32

// Attach to a object type
typedef struct otype_wb_info
{
	unsigned short gp_weapon_index[MAX_WB_GUNPOINTS];
	unsigned short fm_fire_sound_index[MAX_WB_FIRING_MASKS];
	unsigned short aiming_gp_index;

	ubyte num_masks;
	ubyte gp_fire_masks[MAX_WB_FIRING_MASKS];
	float gp_fire_wait[MAX_WB_FIRING_MASKS];

	ubyte	gp_quad_fire_mask;
	
	ubyte num_levels;
	unsigned short gp_level_weapon_index[MAX_WB_UPGRADES];
	unsigned short gp_level_fire_sound_index[MAX_WB_UPGRADES];

	ubyte aiming_flags;
	float aiming_3d_dot; // These can be reused.
	float aiming_3d_dist;
	float aiming_XZ_dot;

	float		anim_start_frame[MAX_WB_FIRING_MASKS];
	float		anim_fire_frame[MAX_WB_FIRING_MASKS];
	float		anim_end_frame[MAX_WB_FIRING_MASKS];
	float		anim_time[MAX_WB_FIRING_MASKS];

	ushort	flags;

	float energy_usage,ammo_usage;
} otype_wb_info;

#define WB_MOVE_STILL  0
#define WB_MOVE_RIGHT  1
#define WB_MOVE_LEFT   2

// Goes with an individual robot's instance
typedef struct dynamic_wb_info
{
	// Dynamic Data
	float last_fire_time;
	unsigned char cur_firing_mask;
	
	float norm_turret_angle[MAX_WB_TURRETS];
	float turret_next_think_time[MAX_WB_TURRETS];
	ubyte turret_direction[MAX_WB_TURRETS];

	ubyte wb_anim_mask;
	float wb_anim_frame;

	vector cur_target;

	char upgrade_level;  // For multi-level weapons ( 0 to MAX_WB_UPGRADES-1)

	int flags;

} dynamic_wb_info;

#endif