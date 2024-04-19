
#ifndef WEAPONPAGE_H
#define WEAPONPAGE_H

#include "manage.h"
#include "weapon.h"
#include "CFILE.H"
#include "pstypes.h"

typedef struct {
  weapon weapon_struct;
  char hud_image_name[PAGENAME_LEN];
  char fire_image_name[PAGENAME_LEN];
  char explode_image_name[PAGENAME_LEN];
  char smoke_image_name[PAGENAME_LEN];
  char scorch_image_name[PAGENAME_LEN];
  char icon_name[PAGENAME_LEN];
  char spawn_name[PAGENAME_LEN];
  char alternate_spawn_name[PAGENAME_LEN];
  char particle_name[PAGENAME_LEN];
  char robot_spawn_name[PAGENAME_LEN];
  char sound_name[MAX_WEAPON_SOUNDS][PAGENAME_LEN];
} mngs_weapon_page;

// Weapon page functions
//---------------------------------------------------------------

// Given an open file pointer and a weapon_page struct, writes that weaponpage out
void mng_WriteWeaponPage(CFILE *outfile, mngs_weapon_page *weaponpage);

// Reads a weapon page from an open file.  Returns 0 on error.
int mng_ReadWeaponPage(CFILE *infile, mngs_weapon_page *weaponpage);

// Given an open file pointer and a weapon_page struct, writes that weaponpage out
void mng_WriteNewWeaponPage(CFILE *outfile, mngs_weapon_page *weaponpage);

// Reads a weapon page from an open file.  Returns 0 on error.
int mng_ReadNewWeaponPage(CFILE *infile, mngs_weapon_page *weaponpage);

// Reads in the weaponpage named "name" into weaponpage struct
// Returns 0 on error or couldn't find, else 1 if all is good
int mng_FindSpecificWeaponPage(char *name, mngs_weapon_page *weaponpage, int offset = 0);
int mng_FindSpecificWeaponPage(char *name, mngs_weapon_page *weaponpage);

// Given a weapon page, allocs a weapon and calls AssignWeaponPageToWeapon to actually
// load model and values. Rturns weapon handle on success, -1 if fail
int mng_SetAndLoadWeapon(mngs_weapon_page *weaponpage, CFILE *infile);

// Given a weaponpage and a weapon handle, attempts to make weapon n correspond to
// to the weaponpage.
// Returns 1 on success, 0 otherwise
int mng_AssignWeaponPageToWeapon(mngs_weapon_page *weaponpage, int n, CFILE *infile = NULL);

// Copies values from a Weapon into a weapon_page
void mng_AssignWeaponToWeaponPage(int n, mngs_weapon_page *weaponpage);

// Reads in a weapon page from the local table file, superseding any weapon
// already in RAM with that same name
void mng_LoadLocalWeaponPage(CFILE *);

// Reads in a page off the net
void mng_LoadNetWeaponPage(CFILE *, bool overlay = false);

int mng_GetGuaranteedWeaponPage(char *name, CFILE *infile = NULL);

#endif