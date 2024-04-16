#ifndef VIBE_H_
#define VIBE_H_

#include "application.h"
#include "vecmat_external.h"
#include "controls.h"

// Initialize IntelliVIBE
void VIBE_Init(oeApplication *app);

// Shutdown IntelliVIBE
void VIBE_Close(void);

// Control functions
// Informs us of a quater-frame tick
void VIBE_DoQuaterFrame(bool first_frame);
void VIBE_WeaponFire(int weapon_index);
void VIBE_DoControls(game_controls *controls);
void VIBE_PlayerRespawn(void);
void VIBE_PlayerDeath(void);
void VIBE_DoForce(vector *force_vec);
void VIBE_DoPlayerDamage(float damage_amount);
void VIBE_DoLevelEnd(void);

#endif
