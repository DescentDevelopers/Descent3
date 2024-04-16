/*
 * Header for slew.c
 *
 */

#ifndef _SLEW_H
#define _SLEW_H

#ifndef RELEASE

#include "object.h"

#define SLEW_MOVE 1
#define SLEW_KEY 2
#define SLEW_ROTATE 4

void SlewResetOrient(object *obj); // Resets the object's orientation
int SlewFrame(object *obj,
              int movement_limitations = 0); // Moves the object for one frame  (returns flags.  1=MOVED 2=KEY_ACTION
int SlewStop(object *obj);                   // Stop the object
void SlewControlInit();                      // initializes controller system for slewer

extern float Slew_key_speed;
// If there's an active joystick, this is the stick number.  Else, this is -1
extern int Joystick_active;

#else

#define SlewResetOrient(obj)
#define SlewFrame(obj)
#define SlewStop(obj)
#define SlewControlInit()

#endif

#endif
