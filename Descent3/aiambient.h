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

#ifndef _AIAMBIENT_H_
#define _AIAMBIENT_H_

#include "cfile.h"

#define MAX_AL_TYPES 6
#define MAX_ALS_PER_TYPE 130

#define ALF_INSIDE 0x01

#define ALI_TYPE 0
#define ALI_TOTAL 1
#define ALI_MAX 2
#define ALI_MIN 3
#define ALI_FLAGS 4

#define CHECK_INTERVAL_MIN 5.0f
#define CHECK_INTERVAL_MAX 10.0f

class ambient_life {
  // Editor settable values
  int m_type[MAX_AL_TYPES];
  unsigned char m_total[MAX_AL_TYPES];
  unsigned char m_max[MAX_AL_TYPES];
  unsigned char m_min[MAX_AL_TYPES];
  unsigned char m_flags[MAX_AL_TYPES];

  // These are never set or gotten from outside of the class
  unsigned char m_cur_num[MAX_AL_TYPES];
  int m_handle[MAX_AL_TYPES][MAX_ALS_PER_TYPE];

  // Don't save these...
  unsigned char m_next_size[MAX_AL_TYPES];
  float m_next_do_time[MAX_AL_TYPES];

  void ComputeNextSize(char index);

public:
  ambient_life() { ALReset(); }

  void GetALValue(char index, char field, void *ptr);
  void SetALValue(char index, char field, void *ptr);
  void SaveData(CFILE *fptr);
  void LoadData(CFILE *fptr);

  void DoFrame();
  void InitForLevel(void);
  void ALReset();
};

extern ambient_life a_life;

#endif
