#ifndef DAMAGE_EXTERNAL_H_
#define DAMAGE_EXTERNAL_H_

// Player Damage types.  Used only to make a sound.
#define PD_NONE 0             // Make no sound
#define PD_ENERGY_WEAPON 1    // Hit by laser, etc.
#define PD_MATTER_WEAPON 2    // Hit by missile, etc.
#define PD_MELEE_ATTACK 3     // Whacked by robot
#define PD_CONCUSSIVE_FORCE 4 // Hit by shockwave
#define PD_WALL_HIT 5         // Crashed into a wall
#define PD_VOLATILE_HISS 6    // Touched a volatile substance (such as acid)

// Generic damage types
#define GD_SCRIPTED 0      // Script is saying to do the damage
#define GD_ELECTRIC 1      // Electrical weapons
#define GD_CONCUSSIVE 2    // Concussive damage
#define GD_FIRE 3          // Fire and napalm like stuff
#define GD_MATTER 4        // Matter weapons
#define GD_ENERGY 5        // Energy weapons and fields
#define GD_PHYSICS 6       // Bumping into a wall or player too hard
#define GD_MELEE_ATTACK 7  // From a melee robot attack
#define GD_VOLATILE_HISS 8 // Touched a volatile substance (such as acid)

#endif