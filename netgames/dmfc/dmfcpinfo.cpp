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

--- HISTORICAL COMMENTS FOLLOW ---

 * $Logfile: /DescentIII/Main/Dmfc/dmfcpinfo.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003/08/26 03:57:21 $
 * $Author: kevinb $
 *
 * DMFC Playerinfo class
 *
 * $Log: dmfcpinfo.cpp,v $
 * Revision 1.1.1.1  2003/08/26 03:57:21  kevinb
 * initial 1.5 import
 *
 *
 * 21    7/09/99 2:54p Jeff
 * handle gametime better (pause it when needed) if the server is 'waiting
 * for players'
 *
 * 20    7/07/99 6:10p Jeff
 * don't count the time that a player is in observer mode for death/kill
 * messages
 *
 * 19    5/13/99 4:55p Ardussi
 * changes for compiling on the Mac
 *
 * 18    5/12/99 11:05p Jeff
 * dmfc and multiplayer games now have endian friendly packets (*whew*)
 *
 * 17    4/25/99 7:19p Jeff
 * added code to handle suicides in pinfo
 *
 * 16    3/17/99 12:25p Jeff
 * converted DMFC to be COM interface
 *
 * 15    2/11/99 12:51a Jeff
 * changed names of exported variables
 *
 * 13    9/30/98 3:50p Jeff
 * general improvements (many)
 *
 * 12    9/29/98 3:04p Jeff
 * added time in game and start_time support
 *
 * 11    9/25/98 8:02p Jason
 * added checks for jeff
 *
 * 10    9/25/98 7:56p Jeff
 * fixed the hosing of the data in UnpackData
 *
 * 9     9/25/98 7:25p Jeff
 *
 * 8     9/25/98 4:50p Jeff
 *
 * 7     9/24/98 5:52p Jeff
 * starting adding statistical death messages, checked in for testing
 *
 * 6     9/21/98 7:11p Jeff
 * made InputCommand interface API and moved existing input commands to
 * the interface.  Changed mprintf/ASSERT so they are valid in DMFC
 *
 * 5     8/04/98 3:29p Jeff
 * completion of player records.  Moved Pinfo to player records...anyone
 * can save detailed stats of the game now
 *
 * 4     7/28/98 2:04p Jeff
 *
 * 3     7/22/98 12:13p Jeff
 * keep track of who you kill now...plus save memory with a name table
 *
 * 2     7/19/98 7:23p Jeff
 * added support to keep track of who kills who
 *
 * $NoKeywords: $
 */

#include "gamedll_header.h"
#include "DMFC.h"
#include "dmfcinternal.h"
#include <string.h>

PInfo::PInfo(int slot) {
  m_slot = slot;

  extra_info.last_kill_time = 0;
  extra_info.last_death_time = 0;
  extra_info.kill_time = 0;
  extra_info.death_time = 0;
  extra_info.observer_time = 0;
  extra_info.kills_in_a_row = 0;
  extra_info.deaths_in_a_row = 0;
  extra_info.last_kill_num = -1;
  extra_info.last_death_num = -1;
  extra_info.got_revenge = 0;

  killer_list = NULL;
  curr = NULL;

  curr = CreateKiller(slot); // Setup Suicide
  if (curr)
    curr->kills = 0;
}

PInfo::~PInfo(void) {
  tPKillerInfo *curr, *next;
  curr = next = killer_list;

  while (curr) {
    next = curr->next;
    free(curr);
    curr = next;
  }
}

tPKillerInfo *PInfo::GetKillerInfo(int slot) {
  tPKillerInfo *info;
  info = FindKiller(slot);
  return info;
}

void PInfo::Update(int slot, tPKillerInfo *amount) {
  tPKillerInfo *info;

  info = FindKiller(slot);
  if (!info) {
    // new one
    info = CreateKiller(slot);
    if (!info)
      return;
  }
  info->kills += amount->kills;
}

void PInfo::HandleKill(int victim_num) {
  extra_info.last_kill_time = extra_info.kill_time;
  extra_info.kill_time = basethis->GetRealGametime();
  extra_info.kills_in_a_row++;
  extra_info.deaths_in_a_row = 0;
  extra_info.last_kill_num = victim_num;
}

void PInfo::HandleDeath(int killer_num) {
  extra_info.got_revenge = 0;
  extra_info.last_death_time = extra_info.death_time;
  extra_info.death_time = basethis->GetRealGametime();
  extra_info.deaths_in_a_row++;
  extra_info.kills_in_a_row = 0;
  extra_info.last_death_num = killer_num;
}

void PInfo::HandleSuicide(void) {
  extra_info.got_revenge = 1; // mark it so they don't get a revenge message
  extra_info.last_death_time = extra_info.death_time;
  extra_info.death_time = basethis->GetRealGametime();
  extra_info.deaths_in_a_row++;
  extra_info.kills_in_a_row = 0;
}

tPExtraInfo *PInfo::GetExtraInfo(void) { return &extra_info; }

tPKillerInfo *PInfo::GetFirstKiller(void) {
  curr = killer_list;
  return killer_list;
}

tPKillerInfo *PInfo::GetNextKiller(void) {
  if (!curr)
    return NULL;
  if (!curr->next)
    return NULL;
  curr = curr->next;
  return curr;
}

tPKillerInfo *PInfo::FindKiller(int slot, tPKillerInfo **wprev) {
  tPKillerInfo *curr;
  tPKillerInfo *prev;
  curr = killer_list;
  prev = NULL;

  while (curr) {

    if (slot == curr->slot) {
      // we found a match
      break;
    }

    prev = curr;
    curr = curr->next;
  }

  if (wprev)
    *wprev = prev;
  return curr;
}

tPKillerInfo *PInfo::CreateKiller(int slot) {
  tPKillerInfo *curr, *prev;
  curr = FindKiller(slot, &prev);

  if (!curr) {
    if (prev) {
      // the name wasn't in the list so we can tack it on here at the end
      curr = (tPKillerInfo *)malloc(sizeof(tPKillerInfo));
      if (!curr)
        return NULL;
      prev->next = curr;
    } else {
      // no names in the list
      curr = killer_list = (tPKillerInfo *)malloc(sizeof(tPKillerInfo));
      if (!killer_list)
        return NULL;
    }

    curr->next = NULL;
    curr->kills = 0;
    curr->slot = slot;
  }

  return curr;
}

void PInfo::RemoveKiller(int slot) {
  tPKillerInfo *curr, *prev;
  curr = FindKiller(slot, &prev);

  if (curr) {
    if (prev) {
      prev->next = curr->next;
      free(curr);
    } else {
      // we're removing root (odd...this is yourself)
      ASSERT(!prev); // Get Jeff
      killer_list = curr->next;
      free(curr);
    }
  }
}

void PInfo::ResetAll(void) {
  tPKillerInfo *c, *next;
  c = next = killer_list;

  while (c) {
    next = c->next;
    free(c);
    c = next;
  }
  curr = killer_list = NULL;

  extra_info.last_kill_time = 0;
  extra_info.last_death_time = 0;
  extra_info.kill_time = 0;
  extra_info.death_time = 0;
  extra_info.observer_time = 0;
  extra_info.kills_in_a_row = 0;
  extra_info.deaths_in_a_row = 0;
  extra_info.last_kill_num = -1;
  extra_info.last_death_num = -1;
  extra_info.got_revenge = 0;
}

int PInfo::GetExtraInfoSize(void) {
  int size = 0;

  size += sizeof(float);    // extrainfo.last_kill_time
  size += sizeof(float);    // extrainfo.last_death_time
  size += sizeof(float);    // extra_info.observer_time
  size += sizeof(float);    // extrainfo.kill_time
  size += sizeof(float);    // extrainfo.death_time
  size += sizeof(uint16_t); // extrainfo.kills_in_a_row
  size += sizeof(uint16_t); // extrainfo.deaths_in_a_row
  size += sizeof(int8_t);   // extrainfo.last_kill_num
  size += sizeof(int8_t);   // extrainfo.last_death_num
  size += sizeof(char);     // extrainfo.got_revenge

  return size;
}

int PInfo::GetSizeOfData(void) {
  tPKillerInfo *c;
  c = killer_list;
  int size = 0;

  size = 0;
  while (c) {
    size += sizeof(int); // kills
    size++;              // slot
    c = c->next;
  }

  size += GetExtraInfoSize();

  return size;
}

void PInfo::PackData(uint8_t *buffer) {
  if (!buffer)
    return;
  tPKillerInfo *c;
  c = killer_list;
  int count = 0;

  MultiAddFloat(extra_info.last_kill_time, buffer, &count);
  MultiAddFloat(extra_info.last_death_time, buffer, &count);
  MultiAddFloat(extra_info.kill_time, buffer, &count);
  MultiAddFloat(extra_info.death_time, buffer, &count);
  MultiAddFloat(extra_info.observer_time, buffer, &count);
  MultiAddUshort(extra_info.kills_in_a_row, buffer, &count);
  MultiAddUshort(extra_info.deaths_in_a_row, buffer, &count);
  MultiAddByte(extra_info.last_kill_num, buffer, &count);
  MultiAddByte(extra_info.last_death_num, buffer, &count);
  MultiAddByte(extra_info.got_revenge, buffer, &count);

  while (c) {
    ASSERT(c->slot >= 0 && c->slot < 64);
    MultiAddByte(c->slot, buffer, &count); // slot
    MultiAddInt(c->kills, buffer, &count);
    c = c->next;
  }
}

bool PInfo::UnpackData(uint8_t *buffer, int buffsize) {
  if (!buffer)
    return false;
  int count = 0;
  int slot, kills;
  bool done = false;
  tPKillerInfo *k;
  ResetAll();

  // buffsize is taking into account extra info, which is guaranteed to be in there
  extra_info.last_kill_time = MultiGetFloat(buffer, &count);
  extra_info.last_death_time = MultiGetFloat(buffer, &count);
  extra_info.kill_time = MultiGetFloat(buffer, &count);
  extra_info.death_time = MultiGetFloat(buffer, &count);
  extra_info.observer_time = MultiGetFloat(buffer, &count);
  extra_info.kills_in_a_row = MultiGetUshort(buffer, &count);
  extra_info.deaths_in_a_row = MultiGetUshort(buffer, &count);
  extra_info.last_kill_num = MultiGetByte(buffer, &count);
  extra_info.last_death_num = MultiGetByte(buffer, &count);
  extra_info.got_revenge = MultiGetByte(buffer, &count);

  int t_size = sizeof(int) + GetExtraInfoSize() + 1;
  if (buffsize < t_size) // not even 1 slot in here
    done = true;

  while (!done) {
    slot = MultiGetByte(buffer, &count); // slot
    kills = MultiGetInt(buffer, &count); // kills

    if (count >= buffsize)
      done = true;

    ASSERT(slot >= 0 && slot < MAX_PLAYER_RECORDS);
    k = CreateKiller(slot);
    if (k) {
      k->kills = kills;
    }
  }

  return true;
}

void PInfo::EnterObserverMode(void) { extra_info.observer_time = basethis->GetRealGametime(); }

void PInfo::ExitObserverMode(void) {
  ASSERT(extra_info.observer_time > 0);
  float observe_time = (basethis->GetRealGametime()) - extra_info.observer_time;
  extra_info.observer_time = 0;

  if (extra_info.last_kill_time > 0)
    extra_info.last_kill_time += observe_time;

  if (extra_info.last_death_time > 0)
    extra_info.last_death_time += observe_time;

  if (extra_info.kill_time > 0)
    extra_info.kill_time += observe_time;

  if (extra_info.death_time > 0)
    extra_info.death_time += observe_time;
}
