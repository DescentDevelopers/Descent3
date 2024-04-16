#ifndef MULTISAFE_H
#define MULTISAFE_H

#include "vecmat.h"
#include "osiris_share.h"

// LOCAL MULTISAFE FUNCTIONS
void msafe_CallFunction(ubyte type, msafe_struct *mstruct);
void msafe_DoPowerup(msafe_struct *mstruct);

// Gets a value for the calling party
void msafe_GetValue(int type, msafe_struct *mstruct);
//----------------------------------------------------------------------

// MULTPLAYER SPECIFIC STUFF
// MULTISAFE FUNCTIONS
void MultiSendMSafeFunction(ubyte type, msafe_struct *mstruct);
void MultiSendMSafePowerup(msafe_struct *mstruct);

#endif