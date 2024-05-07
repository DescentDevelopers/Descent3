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

#include "difficulty.h"

// Notes:
//
// 1.  Don't mess with target leading (other than algorithm type) as it will make robots
//     turn strangely (or at least never scale it up beyond 1.0)

const float Diff_ai_dodge_percent[5] = {0.04f, 0.10f, 1.00f, 1.00f, 1.50f};
const float Diff_ai_dodge_speed[5] = {0.20f, 0.30f, 1.00f, 1.25f, 1.50f};
const float Diff_ai_speed[5] = {0.70f, 0.80f, 1.00f, 1.10f, 1.20f};
const float Diff_ai_rotspeed[5] = {0.70f, 0.80f, 1.00f, 1.10f, 1.20f};
const float Diff_ai_circle_dist[5] = {1.10f, 1.00f, 1.00f, 1.00f, 1.00f};
const float Diff_ai_vis_dist[5] = {0.80f, 0.90f, 1.00f, 1.10f, 1.20f};
const float Diff_player_damage[5] = {0.30f, 0.60f, 1.00f, 1.50f, 2.00f};
const float Diff_ai_weapon_speed[5] = {0.50f, 0.75f, 1.00f, 1.20f, 1.40f};
const float Diff_homing_strength[5] = {0.20f, 0.70f, 1.00f, 1.20f, 1.40f};
const float Diff_robot_damage[5] = {2.75f, 1.50f, 1.00f, 0.80f, 0.60f};
const float Diff_general_scalar[5] = {2.50f, 1.75f, 1.00f, 0.75f, 0.50f};
const float Diff_general_inv_scalar[5] = {0.50f, 0.75f, 1.00f, 1.75f, 2.50f};
const float Diff_shield_energy_scalar[5] = {2.25f, 1.5f, 1.0f, 0.75f, 0.5f};
const float Diff_ai_turret_speed[5] = {0.6f, 0.7f, 1.0f, 1.0f, 1.0f};
const float Diff_ai_min_fire_spread[5] = {.30f, .15f, 0.0f, 0.0f, 0.0f};
