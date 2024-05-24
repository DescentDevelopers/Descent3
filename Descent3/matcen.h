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

#ifndef _MATCEN_H_
#define _MATCEN_H_

#include "cfile.h"
#include "vecmat.h"
#include "matcen_external.h"

#define MAX_MATCENS 60
#define MAX_MATCEN_NAME_LEN 32

#define MATCEN_LOADSAVE_VERSION 3

#define MAX_MATCEN_ALIVE_CHILDREN 32

extern int Num_matcens;
extern bool Matcen_created;

#define MATCEN_OUTSIDE_NEAR_DIST 150.0f

#define MATCEN_ACTIVE_CHECK_RATE 4.0f
#define MATCEN_ACTIVE_CHECK_VARIENCE 1.0f

#define CHECK_ACTIVE_RATE 3.0f
#define CHECK_ACTIVE_VARIENCE 1.0f

#define MAX_MATCEN_EFFECT_SATURATION 2

#ifdef EDITOR
extern char *MatcenEffectStrings[NUM_MATCEN_EFFECTS];
#endif

// Versions
// 1 - Initial
// 2 - Added matcen effects
// 3 - Added matcen active_sound_handle

class matcen {
private:
  // Static data -- only changes by OSIRIS
  char m_name[MAX_MATCEN_NAME_LEN];

  char m_num_prod_types;
  char m_control_type;
  char m_type;
  char m_creation_effect;
  int16_t m_creation_texture;
  uint8_t m_cur_saturation_count;

  int m_num_spawn_pnts;

  union {
    int m_roomnum;
    int m_objref;
  };

  vector m_create_pnt;
  int m_create_room;

  int m_spawn_pnt[MAX_SPAWN_PNTS];
  vector m_spawn_vec[MAX_SPAWN_PNTS];
  vector m_spawn_normal[MAX_SPAWN_PNTS];
  int16_t m_spawn_vis_effects[MAX_MATCEN_EFFECT_SATURATION][MAX_SPAWN_PNTS];

  int m_max_prod;

  int m_prod_type[MAX_PROD_TYPES];
  float m_prod_time[MAX_PROD_TYPES];
  int m_prod_priority[MAX_PROD_TYPES];
  int m_max_prod_type[MAX_PROD_TYPES];

  int16_t m_max_alive_children;
  int16_t m_num_alive;
  int *m_alive_list; // list of alive children

  float m_preprod_time;
  float m_postprod_time;

  int m_sounds[MAX_MATCEN_SOUNDS];

  float m_speed_multi;

  // Dynamic values that change without scripting
  char m_prod_mode;
  float m_prod_mode_time;

  int m_status;

  int m_num_prod;
  int m_last_prod_type_index;
  float m_last_prod_finish_time;

  int m_cached_prod_index;
  float m_cached_prod_time;

  int m_sound_active_handle;

  float m_next_active_check_time;
  bool m_last_active_check_result;

  int m_last_prod_objref;

  int m_num_prod_type[MAX_PROD_TYPES];

  // Private functions that are not available outside of the matcen internals
  bool StartObjProd();
  bool DoObjProd();
  bool FinishObjProd();

  bool ComputeNextProdInfo();
  bool ComputeCreatePnt();

  void CheckActivateStatus();
  bool DoAliveListFrame();
  bool AddToAliveList(int objref);

public:
  matcen();
  ~matcen();

  void SetCreationTexture(int16_t texnum);
  int16_t GetCreationTexture();

  char GetAttachType();
  bool SetAttachType(char type);

  char GetControlType();
  bool SetControlType(char type);

  int GetAttach();
  bool SetAttach(int attach);

  bool GetCreatePnt(vector *pnt);
  bool SetCreatePnt(vector *pnt);
  int GetCreateRoom();
  bool SetCreateRoom(int room);

  char GetNumSpawnPnts();
  bool SetNumSpawnPnts(char num_s);

  int GetSpawnPnt(char s_index);
  bool SetSpawnPnt(char s_index, int s_value);

  void SaveData(CFILE *fptr);
  void LoadData(CFILE *fptr);

  void GetName(char *);
  bool SetName(const char *);

  int GetMaxProd();
  bool SetMaxProd(int max_p);

  char GetNumProdTypes();
  bool SetNumProdTypes(char num_prod_types);

  bool GetProdInfo(char index, int *type_id, int *priority, float *time, int *max_prod);
  bool SetProdInfo(char index, int *type_id, int *priority, float *time, int *max_prod);

  float GetProdMultiplier();
  bool SetProdMultiplier(float multi);

  int GetStatus();
  bool SetStatus(int status, bool f_enable); // Not all flags are settable

  void DoThinkFrame();
  void DoRenderFrame();

  char GetCreationEffect();
  bool SetCreationEffect(char effect_index);

  int GetMaxAliveChildren();
  bool SetMaxAliveChildren(int max_alive);

  float GetPreProdTime();
  bool SetPreProdTime(float time);

  float GetPostProdTime();
  bool SetPostProdTime(float time);

  int GetSound(char sound_type);
  bool SetSound(char sound_type, int sound_index);

  void Reset();
};

extern matcen *Matcen[MAX_MATCENS];
int FindMatcenIndex(const char *name);
int CreateMatcen(const char *name, bool *f_name_changed);
void InitMatcens();

void DestroyAllMatcens();

void DestroyMatcen(int id, bool f_resort);

bool MatcenValid(int id);

void DoMatcensFrame();
void DoMatcensRenderFrame();
void InitMatcensForLevel();

#endif
