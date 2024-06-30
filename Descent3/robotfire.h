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

#ifndef ROBOTFIRE_H_
#define ROBOTFIRE_H_

#include "robotfirestruct.h"
#include "objinfo.h"
#include "object.h"
#include "ship.h"

void FireOnOffWeapon(object *obj);

void WBSetupFireAnim(object *obj, otype_wb_info *static_wb, int wb_index);
void WBFireAnimFrame(object *obj, otype_wb_info *static_wb, int wb_index);

bool WBIsBatteryReady(object *obj, otype_wb_info *static_wb, int wb_index);
void WBFireBattery(object *obj, otype_wb_info *static_wb, int wb_index);
void WBFireBattery(object *obj, otype_wb_info *static_wb, int poly_wb_index, int dynamic_wb_index,
                   float damage_scalar = 1.0);

void WBClearInfo(object *obj);
void WBClearInfo(otype_wb_info static_wb[]);

void WBEnable(object *obj, int wb_index, bool f_enable);

#endif
