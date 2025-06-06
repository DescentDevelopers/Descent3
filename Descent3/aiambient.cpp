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

#include <cstdint>
#include <stdlib.h>
#include "aiambient.h"
#include "string.h"
#include "objinfo.h"
#include "game.h"
#include "psrand.h"

ambient_life a_life;

#define AL_VERSION 1

void ambient_life::DoFrame(void) {}

void ambient_life::InitForLevel(void) {
  int8_t i;

  for (i = 0; i < MAX_AL_TYPES; i++) {
    ComputeNextSize(i);
    m_cur_num[i] = 0;
    m_next_do_time[i] = Gametime;
  }

  DoFrame();
}

void ambient_life::GetALValue(int8_t i, char field, void *ptr) {
  switch (field) {
  case ALI_TYPE:
    *((int *)ptr) = m_type[i];
    break;
  case ALI_TOTAL:
    *((uint8_t *)ptr) = m_total[i];
    break;
  case ALI_MAX:
    *((uint8_t *)ptr) = m_max[i];
    break;
  case ALI_MIN:
    *((uint8_t *)ptr) = m_min[i];
    break;
  case ALI_FLAGS:
    *((uint8_t *)ptr) = m_flags[i];
    break;
  }
}

void ambient_life::ComputeNextSize(int8_t i) {
  char diff = m_max[i] - m_min[i];
  if (diff > 0) {
    char offset = ps_rand() % diff;
    m_next_size[i] = m_min[i] + offset;
  } else {
    m_next_size[i] = m_max[i];
  }
}

void ambient_life::SetALValue(int8_t i, char field, void *ptr) {
  switch (field) {
  case ALI_TYPE:
    m_type[i] = *((int *)ptr);
    break;
  case ALI_TOTAL:
    m_total[i] = *((uint8_t *)ptr);
    break;
  case ALI_MAX:
    m_max[i] = *((uint8_t *)ptr);
    break;
  case ALI_MIN:
    m_min[i] = *((uint8_t *)ptr);
    break;
  case ALI_FLAGS:
    m_flags[i] = *((uint8_t *)ptr);
    break;
  }

  // Jeff(Linux): Commented out because m_total[] is unsigned, so never negative
  // if(m_total[i] < 0)
  //	m_total[i] = 0;
  if (m_total[i] > MAX_ALS_PER_TYPE)
    m_total[i] = MAX_ALS_PER_TYPE;

  // Jeff(Linux): Commented out because m_max[] is unsigned, so never negative
  // if(m_max[i] < 0)
  //	m_max[i] = 0;
  if (m_max[i] > m_total[i])
    m_max[i] = m_total[i];

  // Jeff(Linux): Commented out because m_min[] is unsigned, so never negative
  // if(m_min[i] < 0)
  //	m_min[i] = 0;
  if (m_min[i] > m_max[i])
    m_min[i] = m_max[i];

  ComputeNextSize(i);
}

void ambient_life::ALReset() {
  int i;
  int j;

  for (i = 0; i < MAX_AL_TYPES; i++) {
    m_type[i] = -1;

    for (j = 0; j < MAX_ALS_PER_TYPE; j++) {
      m_handle[i][j] = 0;
    }

    m_total[i] = 0;
    m_cur_num[i] = 0;
    m_flags[i] = 0;

    m_min[i] = 0;
    m_max[i] = 0;

    m_next_do_time[i] = 0;
    m_next_size[i] = 0;
  }
}

void ambient_life::SaveData(CFILE *fp) {
  int i, j;
  int16_t len;

  cf_WriteInt(fp, AL_VERSION);

  for (i = 0; i < MAX_AL_TYPES; i++) {
    int type = m_type[i];

    if (type >= 0) {
      len = strlen(Object_info[type].name) + 1; // Accounts for NULL character
      cf_WriteShort(fp, len);
      for (j = 0; j < len; j++) {
        cf_WriteByte(fp, Object_info[type].name[j]);
      }
    } else {
      cf_WriteShort(fp, 1);
      cf_WriteByte(fp, '\0');
    }

    cf_WriteByte(fp, m_total[i]);
    cf_WriteByte(fp, m_flags[i]);
    cf_WriteByte(fp, m_min[i]);
    cf_WriteByte(fp, m_max[i]);
    cf_WriteByte(fp, m_next_size[i]);
    cf_WriteFloat(fp, m_next_do_time[i]);
  }

  for (i = 0; i < MAX_AL_TYPES; i++) {
    cf_WriteByte(fp, m_cur_num[i]);

    for (j = 0; j < m_cur_num[i]; j++) {
      cf_WriteInt(fp, m_handle[i][j]);
    }
  }
}

void ambient_life::LoadData(CFILE *fp) {
  int version = cf_ReadInt(fp);
  int i, j;
  int len;
  char temp_name[256];

  if (version < 1)
    return;

  for (i = 0; i < MAX_AL_TYPES; i++) {
    len = cf_ReadShort(fp);
    for (j = 0; j < len; j++) {
      {
        temp_name[j] = cf_ReadByte(fp);
      }
    }

    m_type[i] = FindObjectIDName(temp_name);

    m_total[i] = cf_ReadByte(fp);
    m_flags[i] = cf_ReadByte(fp);
    m_min[i] = cf_ReadByte(fp);
    m_max[i] = cf_ReadByte(fp);
    m_next_size[i] = cf_ReadByte(fp);
    m_next_do_time[i] = cf_ReadFloat(fp);
  }

  for (i = 0; i < MAX_AL_TYPES; i++) {
    m_cur_num[i] = cf_ReadByte(fp);

    for (j = 0; j < m_cur_num[i]; j++) {
      m_handle[i][j] = cf_ReadInt(fp);
    }
  }
}