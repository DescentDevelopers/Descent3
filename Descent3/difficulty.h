#ifndef _DIFFICULTY_H_
#define _DIFFICULTY_H_

#include "game.h"
#include "pilot.h"
#include "difficulty_external.h"

extern ubyte ingame_difficulty;

// DAJ static global instead of subroutine call for speed
#define DIFF_LEVEL (((Game_mode & GM_MULTI)) ? Netgame.difficulty : ingame_difficulty)
//#define DIFF_LEVEL (((Game_mode & GM_MULTI))?Netgame.difficulty:dCurrentPilotDifficulty())

extern float Diff_ai_dodge_percent[5];   //
extern float Diff_ai_dodge_speed[5];     //
extern float Diff_ai_speed[5];           //
extern float Diff_ai_rotspeed[5];        //
extern float Diff_ai_circle_dist[5];     //
extern float Diff_ai_vis_dist[5];        //
extern float Diff_player_damage[5];      //
extern float Diff_ai_weapon_speed[5];    //
extern float Diff_homing_strength[5];    //
extern float Diff_robot_damage[5];       //
extern float Diff_general_scalar[5];     // Trainee = 2.50f HotShot = 1.0f
extern float Diff_general_inv_scalar[5]; // Trainee = 0.50f HotShot = 1.0f
extern float Diff_shield_energy_scalar[5];
extern float Diff_ai_turret_speed[5];
extern float Diff_ai_min_fire_spread[5];
#endif