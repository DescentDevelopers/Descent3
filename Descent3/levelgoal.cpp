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

/*
 * $Logfile: /DescentIII/Main/levelgoal.cpp $
 * $Revision: 50 $
 * $Date: 6/23/99 5:34p $
 * $Author: Jeff $
 *
 * Level goal stuff, I guess.
 *
 * $Log: /DescentIII/Main/levelgoal.cpp $
 *
 * 50    6/23/99 5:34p Jeff
 * For some reason Linux started complaining about Status function,
 * renaming it to GetStatus made it quiet.
 *
 * 49    5/23/99 8:39p Chris
 * Now multiple equal priority (primary and secondary) goals are shown on
 * the level goal tab
 *
 * 48    5/18/99 7:33p Jeff
 * fixed game message log...using out-dated variable that is no longer
 * used...switched to real buffer.  We should remove the out-dated
 * variable, but I don't want to break any code at this point.
 *
 * 47    5/12/99 6:02p Jeff
 * flip order of saving/loading objects and players
 *
 * 46    5/09/99 10:49p Jason
 * fixed some strings being truncuated in multiplayer games
 *
 * 45    5/08/99 4:55p Chris
 * Only add HUD completion message if the lgoal is enabled
 *
 * 44    5/07/99 7:57p Jeff
 * handle the multisafe condition of clearing the completed flag
 *
 * 43    5/02/99 5:09a Jeff
 * send levelgoal state to joining clients
 *
 * 42    5/01/99 1:41a Jeff
 * made levelgoals multi friendly
 *
 * 41    4/14/99 11:49a Matt
 * Made localizable the goal completed HUD message
 *
 * 40    4/05/99 1:07p Chris
 * Added a goal failed state
 *
 * 39    3/16/99 4:17p Matt
 * Changed goal complete message from "<goal>: Completed!" to "Completed:
 * <goal>"
 *
 * 38    3/10/99 2:25p Kevin
 * Save/Load and Demo file fixes
 *
 * 37    3/04/99 7:39p Matt
 * Added sound effects to FreeSpace-style persistent HUD messages.
 *
 * 36    3/04/99 6:12p Matt
 * Changed color of goal complete message
 *
 * 35    3/03/99 5:34p Matt
 * Added fade-out for goal complete messages
 *
 * 34    3/03/99 3:35a Chris
 * Fixed.
 *
 * 33    3/02/99 6:39p Jeff
 * level goals working in save/load game
 *
 * 32    3/02/99 5:44p Chris
 * Fixed the save game problem with levelgoals
 *
 * 31    2/26/99 8:33a Chris
 * : Completed! was added
 *
 * 30    2/23/99 1:03p Luke
 * Fixed a bug where all goals where completing automatically
 *
 * 29    2/22/99 8:11p Chris
 * Fixed a bug with saving goals
 *
 * 28    2/22/99 3:05p Chris
 * Complete - minus coop
 *
 * 27    2/22/99 1:43a Chris
 * Version 4 is near complete
 *
 * 26    2/21/99 7:49p Chris
 *
 * 25    2/21/99 7:47p Chris
 * Finished up with the anywhere on terrain and anywhere on mine items  :)
 *
 * 24    2/21/99 7:16p Chris
 * Added the EVT_LEVEL_GOAL_ITEM_COMPLETE event
 *
 * 23    2/21/99 6:37p Chris
 *
 * 22    2/21/99 5:44p Nate
 * Fixed a bug with GoalComplete being called directly from the
 * Level_goals class.  Oops.
 *
 * 21    2/21/99 4:47p Chris
 * More updates
 *
 * 20    2/21/99 4:31p Chris
 * Improving the level goal system...  Not done.
 *
 * 19    2/20/99 6:19p Chris
 *
 * 18    2/20/99 6:18p Chris
 * Improved levelgoals - not done
 *
 * 17    2/20/99 4:29p Chris
 * SOme updates to the system, not done
 *
 * 16    2/20/99 2:59p Matt
 * Made ShowGoalMessage() add the message to the game message buffer.
 *
 * 15    2/19/99 11:32a Chris
 * Added GoalFindId
 *
 * 14    2/18/99 5:29p Matt
 * Added ShowGoalMessage()
 *
 * 13    2/10/99 1:47p Matt
 * Changed object handle symbolic constants
 *
 * 12    1/31/99 7:26p Matt
 * Renamed a bunch of functions to have HUD capitalized
 *
 * 11    1/29/99 5:22p Jeff
 * localization
 *
 * 10    10/15/98 8:54a Matt
 * Don't call Endlevel() directly, but instead change the game state to
 * end-of-level
 *
 */

#include "levelgoal.h"
#include "mem.h"
#include "string.h"
#include "hud.h"
#include "game.h"
#include "gamesequence.h"
#include "stringtable.h"
#include <stdarg.h>
#include "pstring.h"
#include "hlsoundlib.h"
#include "sounds.h"
#include "osiris_dll.h"
#include "room.h"
#include "object.h"
#include "trigger.h"
#include "player.h"
#include "multi.h"
#include "demofile.h"
#include "osiris_share.h"
#include "multisafe.h"
#include "multi.h"
#include "multi_world_state.h"

#define GOAL_MESSAGE_TIME 10.0
#define GOAL_MESSAGE_COLOR GR_RGB(0, 242, 148)

levelgoals Level_goals;

// This is the load/save version of the level goals
#define LEVEL_GOAL_VERSION 4

void lgoal::GoalComplete(int handle, bool announce) {
  m_goal_completed = true;

  tOSIRISEventInfo ei;
  ei.evt_level_goal_complete.level_goal_index = handle;

  Osiris_CallLevelEvent(EVT_LEVEL_GOAL_COMPLETE, &ei);

  if (m_name && announce && (this->m_flags & LGF_ENABLED)) {
    // Draw on the HUD
    int y = Game_window_h / 4;
    //		AddPersistentHUDMessage(GOAL_MESSAGE_COLOR,HUD_MSG_PERSISTENT_CENTER,y,GOAL_MESSAGE_TIME,m_completion_message);
    char message[200];
    snprintf(message, sizeof(message), "%s %s", TXT_COMPLETED_HUD, m_name);
    if (Demo_flags == DF_RECORDING) {
      DemoWritePersistantHUDMessage(GOAL_MESSAGE_COLOR, HUD_MSG_PERSISTENT_CENTER, GOAL_MESSAGE_TIME, GOAL_MESSAGE_TIME,
                                    HPF_FADEOUT + HPF_FREESPACE_DRAW, SOUND_GOAL_COMPLETE, message);
    }
    if (Demo_flags != DF_PLAYBACK) {
      AddPersistentHUDMessage(GOAL_MESSAGE_COLOR, HUD_MSG_PERSISTENT_CENTER, y, GOAL_MESSAGE_TIME,
                              HPF_FADEOUT + HPF_FREESPACE_DRAW, SOUND_GOAL_COMPLETE, message);
    }

    // Add to the game message buffer
    AddGameMessage(message);
  }
}

int lgoal::AddItem() {
  if (m_num_items < MAX_GOAL_ITEMS) {
    if (m_num_items < 1) {
      m_item[m_num_items].m_type = LIT_OBJECT;
      m_item[m_num_items].m_handle = OBJECT_HANDLE_NONE;
      m_item[m_num_items].m_f_done = false;
    } else {
      m_item[m_num_items].m_type = m_item[0].m_type;
      m_item[m_num_items].m_handle = -1;
      m_item[m_num_items].m_f_done = false;
    }

    m_num_items++;
    return m_num_items - 1;
  } else {
    return -1;
  }
}

bool lgoal::DeleteItem(int index) {
  int i;

  if (index < 0 || index >= m_num_items)
    return false;

  for (i = index; i < m_num_items - 1; i++) {
    m_item[i] = m_item[i + 1];
  }

  m_num_items--;

  return true;
}

bool lgoal::ItemInfo(int index, char operation, char *type, int *handle, bool *done) {
  if (index < 0 || index >= m_num_items)
    return false;

  switch (operation) {
  case LO_SET_SPECIFIED:
    if (type) {
      m_item[index].m_type = *type;
    }
    if (handle) {
      m_item[index].m_handle = *handle;
    }
    if (done) {
      m_item[index].m_f_done = *done;
    }
    return true;
    break;

  case LO_GET_SPECIFIED:
    if (type) {
      *type = m_item[index].m_type;
    }
    if (handle) {
      *handle = m_item[index].m_handle;
    }
    if (done) {
      *done = m_item[index].m_f_done;
    }
    return true;
    break;
  }

  return false;
}

bool lgoal::SetName(int handle, char *name) {
  if (!name)
    return false;

  if (m_name)
    mem_free(m_name);

  m_name = (char *)mem_malloc(strlen(name) + 1);
  strcpy(m_name, name);
  m_modified = 1;

  if (Game_mode & GM_MULTI && Netgame.local_role != LR_CLIENT) {
    // send it off to the clients
    SendMultiUpdate(handle);
  }

  return true;
}

void lgoal::SendMultiUpdate(int handle) {
  ASSERT(Game_mode & GM_MULTI && Netgame.local_role == LR_SERVER);

  msafe_struct mstruct;

  mstruct.index = handle;
  GetName(mstruct.message, MSAFE_MESSAGE_LENGTH);
  mstruct.type = m_flags;
  mstruct.count = m_priority;

  msafe_CallFunction(MSAFE_MISC_LEVELGOAL, &mstruct);
}

bool lgoal::SetCompletionMessage(char *message) {
  if (!message)
    return false;

  if (m_completion_message)
    mem_free(m_completion_message);

  m_completion_message = (char *)mem_malloc(strlen(message) + 1);
  strcpy(m_completion_message, message);

  return true;
}

bool lgoal::SetItemName(char *iname) {
  if (!iname)
    return false;

  if (m_item_name)
    mem_free(m_item_name);

  m_item_name = (char *)mem_malloc(strlen(iname) + 1);
  strcpy(m_item_name, iname);

  return true;
}

bool lgoal::Priority(int handle, char operation, int *value) {
  if (!value)
    return false;

  switch (operation) {
  case LO_SET_SPECIFIED:
    m_priority = *value;
    m_modified = 1;
    if (Game_mode & GM_MULTI && Netgame.local_role != LR_CLIENT) {
      // send it off to the clients
      SendMultiUpdate(handle);
    }
    return true;
    break;
  case LO_GET_SPECIFIED:
    *value = m_priority;
    return true;
    break;
  }

  return false;
}

bool lgoal::GoalList(char operation, char *value) {
  if (!value)
    return false;

  switch (operation) {
  case LO_SET_SPECIFIED:
    if (*value < 0)
      *value = 0;
    else if (*value >= MAX_GOAL_LISTS)
      *value = MAX_GOAL_LISTS - 1;

    m_g_list = *value;
    return true;
    break;
  case LO_GET_SPECIFIED:
    *value = m_g_list;
    return true;
    break;
  }

  return false;
}

bool lgoal::GetStatus(int handle, char operation, int *value, bool f_save_load, bool announce) {
  if (!value)
    return false;

  switch (operation) {
  case LO_SET_SPECIFIED:
    m_flags |= *value;
    m_modified = 1;

    if (((*value) & LGF_COMPLETED) && !m_goal_completed) {
      if (!f_save_load)
        GoalComplete(handle, announce);
      m_goal_completed = true;
    }

    if (Game_mode & GM_MULTI && Netgame.local_role != LR_CLIENT) {
      // send it off to the clients
      SendMultiUpdate(handle);
    }
    return true;
    break;
  case LO_GET_SPECIFIED:
    *value = m_flags;
    return true;
    break;
  case LO_CLEAR_SPECIFIED:
    // multisafe will pass in 0xFFFFFFFF on a clear before actually setting flags,
    // so we need to ignore that case
    if ((*value) != 0xFFFFFFFF && m_goal_completed) {
      // check to see if we are marking this goal as "Not completed"
      if ((*value) & LGF_COMPLETED) {
        // unmark this m_goal_completed variable
        m_goal_completed = false;
      }
    }

    m_flags &= ~(*value);
    m_modified = 1;

    if (Game_mode & GM_MULTI && Netgame.local_role != LR_CLIENT) {
      // send it off to the clients
      SendMultiUpdate(handle);
    }
    return true;
    break;
  }

  return false;
}

bool lgoal::SetDesc(char *desc) {
  if (!desc)
    return false;

  if (m_desc)
    mem_free(m_desc);

  m_desc = (char *)mem_malloc(strlen(desc) + 1);
  strcpy(m_desc, desc);

  return true;
}

int lgoal::GetName(char *name, int buffer_size) {
  if (!name)
    return 0;

  if (buffer_size <= 0)
    return strlen(m_name);

  if (m_name == NULL) {
    name[0] = '\0';
    return 0;
  }

  strncpy(name, m_name, buffer_size);
  name[buffer_size - 1] = '\0';

  return strlen(m_name);
}

int lgoal::GetCompletionMessage(char *message, int buffer_size) {
  if (!message)
    return 0;

  if (buffer_size <= 0)
    return strlen(m_completion_message);

  if (m_completion_message == NULL) {
    message[0] = '\0';
    return 0;
  }

  strncpy(message, m_completion_message, buffer_size);
  message[buffer_size - 1] = '\0';

  return strlen(m_completion_message);
}

int lgoal::GetItemName(char *iname, int buffer_size) {
  if (!iname)
    return 0;

  if (buffer_size <= 0)
    return strlen(m_item_name);

  if (m_item_name == NULL) {
    iname[0] = '\0';
    return 0;
  }

  strncpy(iname, m_item_name, buffer_size);
  iname[buffer_size - 1] = '\0';

  return strlen(m_item_name);
}

int lgoal::GetDesc(char *desc, int buffer_size) {
  if (!desc)
    return 0;

  if (buffer_size <= 0)
    return strlen(m_desc);

  if (m_desc == NULL) {
    desc[0] = '\0';
    return 0;
  }

  strncpy(desc, m_desc, buffer_size);
  desc[buffer_size - 1] = '\0';

  return strlen(m_desc);
}

int lgoal::GetNumItems() { return m_num_items; }

void lgoal::Reset(bool f_from_editor) {
  if (m_name)
    mem_free(m_name);

  if (m_item_name)
    mem_free(m_item_name);

  if (m_desc)
    mem_free(m_desc);

  m_name = NULL;
  m_item_name = NULL;
  m_desc = NULL;
  m_num_items = 0;
  m_priority = 0;
  m_g_list = 0;
  m_goal_completed = false;

  if (f_from_editor)
    m_flags = LGF_ENABLED | LGF_TELCOM_LISTS | LGF_COMP_DALLAS;
  else
    m_flags = 0;
}

void lgoal::SendStateToPlayer(int index, int pnum) {
  if (!(Game_mode & GM_MULTI))
    return;

  if (m_modified == 0)
    return;

  ASSERT(Netgame.local_role == LR_SERVER);
  ASSERT(pnum >= 0 && pnum < MAX_NET_PLAYERS);

  if (!(NetPlayers[pnum].flags & NPF_CONNECTED && NetPlayers[pnum].sequence == NETSEQ_PLAYING))
    return;

  mprintf((0, "Sending modified LevelGoal %d to player %d\n", index, pnum));

  // now update the buddy handle list of the clients
  int count = 0;
  int size_offset;
  ubyte data[MAX_GAME_DATA_SIZE];

  size_offset = START_DATA(MP_WORLD_STATES, data, &count);

  MultiAddByte(WS_LEVELGOAL, data, &count);
  MultiAddUshort(index, data, &count);

  MultiAddInt(m_priority, data, &count);

  char name[256];
  GetName(name, 256);
  int len = strlen(name) + 1;
  MultiAddByte(len, data, &count);
  memcpy(&data[count], name, len);
  count += len;

  MultiAddInt(m_flags, data, &count);

  MultiAddByte(WS_END, data, &count);
  END_DATA(count, data, size_offset);

  // Send it out
  nw_SendReliable(NetPlayers[pnum].reliable_socket, data, count, false);
}

void lgoal::ResetModified(void) { m_modified = 0; }

bool levelgoals::GoalStatus(int index, char operation, int *flags, bool announce) {
  if (index < 0 || index >= m_num_goals)
    return false;

  return m_goal[index].GetStatus(index, operation, flags, false, announce);
}

bool levelgoals::GoalPriority(int index, char operation, int *flags) {
  if (index < 0 || index >= m_num_goals)
    return false;

  return m_goal[index].Priority(index, operation, flags);
}

bool levelgoals::GoalGoalList(int index, char operation, char *value) {
  if (index < 0 || index >= m_num_goals)
    return false;

  return m_goal[index].GoalList(operation, value);
}

int levelgoals::AddGoal(bool f_from_editor) {
  if (m_num_goals < MAX_LEVEL_GOALS) {
    m_goal[m_num_goals].Reset(f_from_editor);
    m_num_goals++;
    return m_num_goals - 1;
  } else {
    return -1;
  }
}

bool levelgoals::DeleteGoal(int index) {
  int i;

  if (index < 0 || index >= m_num_goals)
    return false;

  m_goal[index].Reset(false);

  for (i = index; i < m_num_goals - 1; i++) {
    m_goal[i] = m_goal[i + 1];
  }

  m_num_goals--;

  return true;
}

int levelgoals::GoalAddItem(int goal_index) {
  if (goal_index < 0 || goal_index >= m_num_goals)
    return -1;

  return m_goal[goal_index].AddItem();
}

bool levelgoals::GoalDeleteItem(int goal_index, int item_index) {
  if (goal_index < 0 || goal_index >= m_num_goals)
    return false;

  return m_goal[goal_index].DeleteItem(item_index);
}

bool levelgoals::GoalItemInfo(int goal_index, int index, char operation, char *type, int *handle, bool *done) {
  if (goal_index < 0 || goal_index >= m_num_goals)
    return false;

  return m_goal[goal_index].ItemInfo(index, operation, type, handle, done);
}

bool levelgoals::GoalSetName(int goal_index, char *name) {
  if (goal_index < 0 || goal_index >= m_num_goals)
    return false;

  return m_goal[goal_index].SetName(goal_index, name);
}

bool levelgoals::GoalSetItemName(int goal_index, char *iname) {
  if (goal_index < 0 || goal_index >= m_num_goals)
    return false;

  return m_goal[goal_index].SetItemName(iname);
}

bool levelgoals::GoalSetDesc(int goal_index, char *desc) {
  if (goal_index < 0 || goal_index >= m_num_goals)
    return false;

  return m_goal[goal_index].SetDesc(desc);
}

bool levelgoals::GoalSetCompletionMessage(int goal_index, char *message) {
  if (goal_index < 0 || goal_index >= m_num_goals)
    return false;

  return m_goal[goal_index].SetCompletionMessage(message);
}

int levelgoals::GoalGetName(int goal_index, char *name, int buffer_size) {
  if (goal_index < 0 || goal_index >= m_num_goals)
    return -1;

  return m_goal[goal_index].GetName(name, buffer_size);
}

int levelgoals::GoalGetCompletionMessage(int goal_index, char *message, int buffer_size) {
  if (goal_index < 0 || goal_index >= m_num_goals)
    return -1;

  return m_goal[goal_index].GetCompletionMessage(message, buffer_size);
}

int levelgoals::GoalGetItemName(int goal_index, char *iname, int buffer_size) {
  if (goal_index < 0 || goal_index >= m_num_goals)
    return -1;

  return m_goal[goal_index].GetItemName(iname, buffer_size);
}

int levelgoals::GoalGetDesc(int goal_index, char *desc, int buffer_size) {
  if (goal_index < 0 || goal_index >= m_num_goals)
    return -1;

  return m_goal[goal_index].GetDesc(desc, buffer_size);
}

int levelgoals::GoalGetNumItems(int goal_index) {
  if (goal_index < 0 || goal_index >= m_num_goals)
    return -1;

  return m_goal[goal_index].GetNumItems();
}

int levelgoals::GetNumGoals() { return m_num_goals; }

bool levelgoals::SaveLevelGoalInfo(CFILE *fptr) {
  int i;
  cf_WriteShort(fptr, LEVEL_GOAL_VERSION);

  cf_WriteShort(fptr, m_num_goals);
  for (i = 0; i < m_num_goals; i++) {
    char name[256];
    char iname[256];
    char desc[2560];
    char message[2560];
    int len;
    int num_items;
    int j;
    int priority = 0;
    char g_list = 0;
    int status = 0;

    Level_goals.GoalStatus(i, LO_GET_SPECIFIED, &status);
    cf_WriteInt(fptr, status);

    Level_goals.GoalPriority(i, LO_GET_SPECIFIED, &priority);
    cf_WriteInt(fptr, priority);

    Level_goals.GoalGoalList(i, LO_GET_SPECIFIED, &g_list);
    cf_WriteByte(fptr, g_list);

    Level_goals.GoalGetName(i, name, 256);
    Level_goals.GoalGetItemName(i, iname, 256);
    Level_goals.GoalGetDesc(i, desc, 2560);
    Level_goals.GoalGetCompletionMessage(i, message, 2560);

    len = strlen(name);
    cf_WriteShort(fptr, len);
    if (len) {
      cf_WriteBytes((unsigned char *)name, len, fptr);
    }

    len = strlen(iname);
    cf_WriteShort(fptr, len);
    if (len) {
      cf_WriteBytes((unsigned char *)iname, len, fptr);
    }

    len = strlen(desc);
    cf_WriteShort(fptr, len);
    if (len) {
      cf_WriteBytes((unsigned char *)desc, len, fptr);
    }

    len = strlen(message);
    cf_WriteShort(fptr, len);
    if (len) {
      cf_WriteBytes((unsigned char *)message, len, fptr);
    }

    num_items = Level_goals.GoalGetNumItems(i);
    cf_WriteShort(fptr, num_items);
    for (j = 0; j < num_items; j++) {
      char type = 0;
      int handle;
      bool f_done;

      Level_goals.GoalItemInfo(i, j, LO_GET_SPECIFIED, &type, &handle, &f_done);
      cf_WriteByte(fptr, type);
      cf_WriteInt(fptr, handle);
      cf_WriteByte(fptr, (char)f_done);
    }
  }

  cf_WriteInt(fptr, m_flags);

  return true;
}

bool levelgoals::LGStatus(char operation, int *value) {
  if (!value)
    return false;

  switch (operation) {
  case LO_SET_SPECIFIED:
    m_flags |= *value;
    return true;
    break;
  case LO_GET_SPECIFIED:
    *value = m_flags;
    return true;
    break;
  case LO_CLEAR_SPECIFIED:
    m_flags &= ~(*value);
    return true;
    break;
  }

  return false;
}

bool levelgoals::LoadLevelGoalInfo(CFILE *fptr) {
  CleanupAfterLevel();

  int i;
  short version = cf_ReadShort(fptr);
  int num_goals = cf_ReadShort(fptr);

  for (i = 0; i < num_goals; i++) {
    char name[256];
    char iname[256];
    char desc[2560];
    char message[2560];
    int len;
    int num_items;
    int j;
    int priority;
    char g_list;
    int status;

    int gi = AddGoal(false);
    ASSERT(gi == i);

    status = cf_ReadInt(fptr);
    m_goal[i].GetStatus(i, LO_SET_SPECIFIED, &status, true);

    priority = cf_ReadInt(fptr);
    Level_goals.GoalPriority(i, LO_SET_SPECIFIED, &priority);

    if (version < 4) {
      g_list = 0;
    } else {
      g_list = cf_ReadByte(fptr);
    }
    Level_goals.GoalGoalList(i, LO_SET_SPECIFIED, &g_list);

    len = cf_ReadShort(fptr);
    if (len) {
      cf_ReadBytes((unsigned char *)name, len, fptr);
    }
    name[len] = '\0';

    len = cf_ReadShort(fptr);
    if (len) {
      cf_ReadBytes((unsigned char *)iname, len, fptr);
    }
    iname[len] = '\0';

    len = cf_ReadShort(fptr);
    if (len) {
      cf_ReadBytes((unsigned char *)desc, len, fptr);
    }
    desc[len] = '\0';

    if (version < 3) {
      message[0] = '\0';
    } else {
      len = cf_ReadShort(fptr);
      if (len) {
        cf_ReadBytes((unsigned char *)message, len, fptr);
      }
      message[len] = '\0';
    }

    Level_goals.GoalSetName(i, name);
    Level_goals.GoalSetItemName(i, iname);
    Level_goals.GoalSetDesc(i, desc);
    Level_goals.GoalSetCompletionMessage(i, message);

    num_items = cf_ReadShort(fptr);
    for (j = 0; j < num_items; j++) {
      char type;
      int handle;
      bool f_done;

      int item_slot = GoalAddItem(i);
      ASSERT(item_slot == j);

      type = cf_ReadByte(fptr);
      handle = cf_ReadInt(fptr);
      f_done = cf_ReadByte(fptr) != 0;
      Level_goals.GoalItemInfo(i, j, LO_SET_SPECIFIED, &type, &handle, &f_done);
    }
  }

  if (version >= 2)
    m_flags = cf_ReadInt(fptr);
  else
    m_flags = 0;

  ResetModifiedFlags();

  return true;
}
/*
int levelgoals::GetCurrentPrimaryGoalIndex()
{
        int i;
        int cp;
        int bestp = -1;
        int bestg = -1;

        for(i = 0; i < m_num_goals; i++)
        {
                int flags;
                m_goal[i].GetStatus(i, LO_GET_SPECIFIED, &flags);

                if(!(flags & (LGF_SECONDARY_GOAL | LGF_COMPLETED)))
                {
                        if(flags & LGF_ENABLED)
                        {
                                m_goal[i].Priority(i,LO_GET_SPECIFIED, &cp);

                                if(cp > bestp)
                                {
                                        bestp = cp;
                                        bestg = i;
                                }
                        }
                }
        }

        return bestg;
}*/

// int m_num_active_primaries = 0;
// int m_active_primaries[MAX_LEVEL_GOALS];
// int m_num_active_secondaries = 0;
// int m_active_secondaries[MAX_LEVEL_GOALS];

int levelgoals::GetNumActivePrimaryGoals() {
  int x;
  m_num_active_primaries = 0;

  int i;
  int bestp[4];
  int cp;  // Current priority
  char cl; // Current list
  int temp_p[MAX_GOAL_LISTS][MAX_LEVEL_GOALS];
  char temp_g[MAX_GOAL_LISTS][MAX_LEVEL_GOALS];

  int temp_count[MAX_GOAL_LISTS];

  for (i = 0; i < MAX_GOAL_LISTS; i++) {
    bestp[i] = -1;
    temp_count[i] = 0;
  }

  // Determine the active number for the current list
  for (i = 0; i < m_num_goals; i++) {
    int flags;
    m_goal[i].GetStatus(i, LO_GET_SPECIFIED, &flags);

    if (!(flags & (LGF_COMPLETED | LGF_FAILED | LGF_SECONDARY_GOAL))) {
      if (flags & LGF_ENABLED) {
        m_goal[i].GoalList(LO_GET_SPECIFIED, &cl);
        m_goal[i].Priority(i, LO_GET_SPECIFIED, &cp);

        if (bestp[cl] == -1 || cp <= bestp[cl]) {
          bestp[cl] = cp;
          temp_p[cl][temp_count[cl]] = cp;
          temp_g[cl][(temp_count[cl])++] = i;
        }
      }
    }
  }

  for (x = 0; x < MAX_GOAL_LISTS; x++) {
    for (i = 0; i < temp_count[x]; i++) {
      if (temp_p[x][i] == bestp[x]) {
        m_active_primaries[m_num_active_primaries++] = temp_g[x][i];
      }
    }
  }

  return m_num_active_primaries;
}

int levelgoals::GetActivePrimaryGoal(int p_index) {
  if (p_index < 0 || p_index >= m_num_active_primaries)
    return -1;

  return m_active_primaries[p_index];
}

int levelgoals::GetNumActiveSecondaryGoals() {
  int x;
  m_num_active_secondaries = 0;

  int i;
  int bestp[4];
  int cp;  // Current priority
  char cl; // Current list
  int temp_p[MAX_GOAL_LISTS][MAX_LEVEL_GOALS];
  char temp_g[MAX_GOAL_LISTS][MAX_LEVEL_GOALS];

  int temp_count[MAX_GOAL_LISTS];

  for (i = 0; i < MAX_GOAL_LISTS; i++) {
    bestp[i] = -1;
    temp_count[i] = 0;
  }

  // Determine the active number for the current list
  for (i = 0; i < m_num_goals; i++) {
    int flags;
    m_goal[i].GetStatus(i, LO_GET_SPECIFIED, &flags);

    if (!(flags & (LGF_COMPLETED | LGF_FAILED))) {
      if (flags & LGF_ENABLED) {
        if (flags & LGF_SECONDARY_GOAL) {
          m_goal[i].GoalList(LO_GET_SPECIFIED, &cl);
          m_goal[i].Priority(i, LO_GET_SPECIFIED, &cp);

          if (bestp[cl] == -1 || cp <= bestp[cl]) {
            bestp[cl] = cp;
            temp_p[cl][temp_count[cl]] = cp;
            temp_g[cl][(temp_count[cl])++] = i;
          }
        }
      }
    }
  }

  for (x = 0; x < MAX_GOAL_LISTS; x++) {
    for (i = 0; i < temp_count[x]; i++) {
      if (temp_p[x][i] == bestp[x]) {
        m_active_secondaries[m_num_active_secondaries++] = temp_g[x][i];
      }
    }
  }

  return m_num_active_secondaries;
}

int levelgoals::GetActiveSecondaryGoal(int s_index) {
  if (s_index < 0 || s_index >= m_num_active_secondaries)
    return -1;

  return m_active_secondaries[s_index];
}

void levelgoals::DoFrame() {
  int i;

  // Check for auto-completion
  for (i = 0; i < m_num_goals; i++) {
    int flags;
    m_goal[i].GetStatus(i, LO_GET_SPECIFIED, &flags);

    if (!(flags & (LGF_COMPLETED | LGF_FAILED))) {
      if (flags & LGF_ENABLED) {
        char ctype;
        int chandle;
        bool cdone;
        bool f_done = true;

        m_goal[i].ItemInfo(0, LO_GET_SPECIFIED, &ctype, &chandle, &cdone);

        if (ctype == LIT_TERRAIN_CELL) {
          if (Game_mode & GM_MULTI) {
            int j;

            for (j = 0; j < MAX_PLAYERS; j++) {
              if ((NetPlayers[j].flags & NPF_CONNECTED) && (NetPlayers[j].sequence >= NETSEQ_PLAYING)) {
                object *player = &Objects[Players[j].objnum];
                if (!ROOMNUM_OUTSIDE(player->roomnum)) {
                  f_done = false;
                }
              }
            }
          } else {
            if (!ROOMNUM_OUTSIDE(Player_object->roomnum)) {
              f_done = false;
            }
          }
        } else if (ctype == LIT_ANY_MINE) {
          if (Game_mode & GM_MULTI) {
            int j;

            for (j = 0; j < MAX_PLAYERS; j++) {
              if ((NetPlayers[j].flags & NPF_CONNECTED) && (NetPlayers[j].sequence >= NETSEQ_PLAYING)) {
                object *player = &Objects[Players[j].objnum];
                if (ROOMNUM_OUTSIDE(player->roomnum)) {
                  f_done = false;
                }
              }
            }
          } else {
            if (ROOMNUM_OUTSIDE(Player_object->roomnum)) {
              f_done = false;
            }
          }
        } else {
          continue;
        }

        if (f_done) {
          int flags = LGF_COMPLETED;
          m_goal[i].GetStatus(i, LO_SET_SPECIFIED, &flags);
        }
      }
    }
  }

  if (GetNumActivePrimaryGoals() == 0 && !(m_flags & LF_ALL_PRIMARIES_DONE)) {
    m_flags |= LF_ALL_PRIMARIES_DONE;

    tOSIRISEventInfo ei;
    Osiris_CallLevelEvent(EVT_ALL_LEVEL_GOALS_COMPLETE, &ei);
  }
}

void levelgoals::CleanupAfterLevel() {
  int i;

  for (i = m_num_goals; i >= 0; i--) {
    DeleteGoal(i);
  }
}

void levelgoals::InitLevel() {
  int i, j;

  for (i = 0; i <= Highest_object_index; i++) {
    if (Objects[i].type != OBJ_NONE) {
      Objects[i].flags &=
          (~(OF_INFORM_PLAYER_COLLIDE_TO_LG | OF_INFORM_PLAYER_WEAPON_COLLIDE_TO_LG | OF_INFORM_DESTROY_TO_LG));
    }
  }

  for (i = 0; i <= Highest_room_index; i++) {
    if (Rooms[i].used) {
      Rooms[i].flags &= (~(RF_INFORM_RELINK_TO_LG));
    }
  }

  for (i = 0; i < Num_triggers; i++) {
    Triggers[i].flags &= (~(TF_INFORM_ACTIVATE_TO_LG));
  }

  for (i = 0; i < m_num_goals; i++) {
    int flags;
    m_goal[i].GetStatus(i, LO_GET_SPECIFIED, &flags);
    flags &= LGF_COMP_MASK;

    for (j = 0; j < m_goal[i].GetNumItems(); j++) {
      char type;
      int handle;
      bool done;

      m_goal[i].ItemInfo(j, LO_GET_SPECIFIED, &type, &handle, &done);

      switch (type) {
      case LIT_TERRAIN_CELL:
        break;
      case LIT_INTERNAL_ROOM:
        if (flags == LGF_COMP_ENTER) {
          if (handle >= 0 && handle <= Highest_room_index && Rooms[handle].used) {
            Rooms[handle].flags |= RF_INFORM_RELINK_TO_LG;
          }
        }
        break;
      case LIT_OBJECT:
        if (flags == LGF_COMP_PLAYER) {
          object *obj = ObjGet(handle);
          if (obj) {
            obj->flags |= OF_INFORM_PLAYER_COLLIDE_TO_LG;
          }
        } else if (flags == LGF_COMP_PLAYER_WEAPON) {
          object *obj = ObjGet(handle);
          if (obj) {
            obj->flags |= OF_INFORM_PLAYER_WEAPON_COLLIDE_TO_LG;
          }
        } else if (flags == LGF_COMP_DESTROY) {
          object *obj = ObjGet(handle);
          if (obj) {
            obj->flags |= OF_INFORM_DESTROY_TO_LG;
          }
        }
        break;
      case LIT_TRIGGER:
        if (handle >= 0 && handle < Num_triggers) {
          if (flags == LGF_COMP_ACTIVATE) {
            Triggers[handle].flags |= TF_INFORM_ACTIVATE_TO_LG;
          }
        }
        break;
      case LIT_ANY_MINE:
        break;
      }
    }
  }
}

void levelgoals::Inform(char type, int comp_type, int handle) {
  int i, j;

  for (i = 0; i < m_num_goals; i++) {
    int flags;
    m_goal[i].GetStatus(i, LO_GET_SPECIFIED, &flags);

    if (!(flags & LGF_ENABLED) && (comp_type == LGF_COMP_ACTIVATE || comp_type == LGF_COMP_ENTER ||
                                   comp_type == LGF_COMP_PLAYER_WEAPON || comp_type == LGF_COMP_PLAYER)) {
      continue;
    }

    int cur_comp_type = flags & LGF_COMP_MASK;
    int num_items = m_goal[i].GetNumItems();

    if (flags != LGF_COMP_DALLAS && cur_comp_type != comp_type) {
      continue;
    }

    for (j = 0; j < num_items; j++) {
      char ctype;
      int chandle;
      bool cdone;

      m_goal[i].ItemInfo(j, LO_GET_SPECIFIED, &ctype, &chandle, &cdone);

      if (cdone || type != ctype || chandle != handle) {
        continue;
      }

      bool done = true;
      m_goal[i].ItemInfo(j, LO_SET_SPECIFIED, NULL, NULL, &done);

      int k;
      bool f_done = true;

      for (k = 0; k < num_items; k++) {
        m_goal[i].ItemInfo(j, LO_SET_SPECIFIED, NULL, NULL, &done);
        if (!done) {
          f_done = false;
          break;
        }
      }

      if (f_done) {
        int flags = LGF_COMPLETED;
        m_goal[i].GetStatus(i, LO_SET_SPECIFIED, &flags);
      }

      // Make this after the goal complete is done so that designers can say, if this and not
      // goal complete
      tOSIRISEventInfo ei;
      ei.evt_level_goal_item_complete.level_goal_index = i;

      Osiris_CallLevelEvent(EVT_LEVEL_GOAL_ITEM_COMPLETE, &ei);
    }
  }
}

int levelgoals::GoalFindId(const char *goal_name) {
  int i;
  char cur_name[200];

  for (i = 0; i < m_num_goals; i++) {
    GoalGetName(i, cur_name, 200);
    if (stricmp(cur_name, goal_name) == 0) {
      return i;
    }
  }

  return -1;
}

void levelgoals::ResetModifiedFlags(void) {
  for (int i = 0; i < m_num_goals; i++) {
    m_goal[i].ResetModified();
  }
}

void levelgoals::MultiSendChangedGoals(int pnum) {
  for (int i = 0; i < m_num_goals; i++) {
    m_goal[i].SendStateToPlayer(i, pnum);
  }
}
