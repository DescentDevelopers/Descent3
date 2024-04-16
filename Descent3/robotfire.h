#ifndef ROBOTFIRE_H_
#define ROBOTFIRE_H_

#include "robotfirestruct.h"
#include "objinfo.h"
#include "object.h"
#include "ship.h"

void FireOnOffWeapon (object *obj);

void WBSetupFireAnim(object *obj, otype_wb_info *static_wb, int wb_index);
void WBFireAnimFrame(object *obj, otype_wb_info *static_wb, int wb_index);

bool WBIsBatteryReady(object *obj, otype_wb_info *static_wb, int wb_index);
void WBFireBattery(object *obj, otype_wb_info *static_wb, int wb_index);
void WBFireBattery(object *obj, otype_wb_info *static_wb, int poly_wb_index, int dynamic_wb_index,float damage_scalar=1.0);

void WBClearInfo(object *obj);
void WBClearInfo(otype_wb_info static_wb[]);

void WBEnable(object *obj, int wb_index, bool f_enable);

#endif
