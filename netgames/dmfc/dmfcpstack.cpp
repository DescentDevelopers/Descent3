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

#include "DMFC.h"
#include "dmfcinternal.h"

PlayerStack::PlayerStack() { root = NULL; }
PlayerStack::~PlayerStack() { RemoveAll(); }
void PlayerStack::Add(char *callsign, network_address *addr, tDMFCPlayerStat *pstat, void *GameStatItem,
                      int GameStatItemsize) {
  if (!callsign)
    return;
  if (!addr)
    return;
  if (!pstat)
    return;

  DLLmprintf((0, "Pushing %s onto a PlayerStack %s\n", callsign, GameStatItem ? "(With Stats)" : "(No Stats)"));

  if (!root) {
    root = (tPlayerStackItem *)malloc(sizeof(tPlayerStackItem));
    if (!root)
      return;
    root->next = NULL;
    memcpy(&root->stat, pstat, sizeof(tDMFCPlayerStat));
    memcpy(&root->addr, addr, sizeof(network_address));
    root->callsign = (char *)malloc(strlen(callsign) + 1);
    root->timeadded = *basethis->DLLGametime;
    if (!root->callsign) {
      free(root);
      root = NULL;
      return;
    }
    if (GameStatItem) {
      root->GameStatItem = malloc(GameStatItemsize);
      if (!root->GameStatItem) {
        free(root->callsign);
        free(root);
        root = NULL;
        return;
      }
      memcpy(root->GameStatItem, GameStatItem, GameStatItemsize);
    } else
      root->GameStatItem = NULL;
    strcpy(root->callsign, callsign);
    return;
  } else {
    tPlayerStackItem *curr;
    curr = root;
    while (curr->next) {
      curr = curr->next;
    }
    curr->next = (tPlayerStackItem *)malloc(sizeof(tPlayerStackItem));
    if (!curr->next)
      return;
    curr->next->callsign = (char *)malloc(strlen(callsign) + 1);
    if (!curr->next->callsign) {
      free(curr->next);
      curr->next = NULL;
      return;
    }
    if (GameStatItem) {
      curr->next->GameStatItem = malloc(GameStatItemsize);
      if (!curr->next->GameStatItem) {
        free(curr->next->callsign);
        free(curr->next);
        curr->next = NULL;
        return;
      }
      memcpy(curr->next->GameStatItem, GameStatItem, GameStatItemsize);
    } else
      curr->next->GameStatItem = NULL;

    curr = curr->next;
    curr->next = NULL;
    memcpy(&curr->stat, pstat, sizeof(tDMFCPlayerStat));
    memcpy(&curr->addr, addr, sizeof(network_address));
    strcpy(curr->callsign, callsign);
    curr->timeadded = *basethis->DLLGametime;
  }
  return;
}
void PlayerStack::RemoveAll(void) {
  if (root) {
    tPlayerStackItem *curr, *next;
    curr = next = root;
    while (curr) {
      next = curr->next;
      Remove(curr);
      curr = next;
    }
  }
  root = NULL;
}

void PlayerStack::Remove(tPlayerStackItem *item) {
  if (!item)
    return;

  if (item->callsign)
    free(item->callsign);
  if (item->GameStatItem)
    free(item->GameStatItem);
  free(item);
}

bool PlayerStack::Find(char *callsign, network_address *addr, tDMFCPlayerStat *stat, bool remove, void *GameStatItem,
                       int GameStatItemsize) {
  if (!root) {
    DLLmprintf((0, "PlayerStack: No Items\n"));
    return false;
  }
  if (!addr) {
    return false;
  }
  if (!callsign) {
    return false;
  }
  if (!stat) {
    return false;
  }

  DLLmprintf((0, "PlayerStack: Looking for Player\n"));

  tPlayerStackItem *curr, *prev;
  curr = prev = root;

  while (curr) {
    if (!strcmp(callsign, curr->callsign)) {
      DLLmprintf((0, "Found a pilot by the name of %s...", callsign));
      if (!memcmp(addr, &curr->addr, sizeof(network_address))) {
        DLLmprintf((0, "Network address matches!\n"));
        memcpy(stat, &curr->stat, sizeof(tDMFCPlayerStat));
        if ((curr->GameStatItem) && (GameStatItem)) {
          memcpy(GameStatItem, curr->GameStatItem, GameStatItemsize);
        }
        if (remove) {
          if (curr != root) {
            tPlayerStackItem *temp;
            temp = curr->next;
            Remove(curr);
            prev->next = temp;
          } else {
            tPlayerStackItem *temp;
            temp = root->next;
            Remove(root);
            root = temp;
          }
        }
        return true;
      } else
        DLLmprintf((0, "But network address doesn't match!\n"));
    }

    prev = curr;
    curr = curr->next;
  }
  return false;
}

void PlayerStack::Remove(char *callsign, network_address *addr) {
  if (!root)
    return;
  if (!addr)
    return;
  if (!callsign)
    return;

  tPlayerStackItem *curr, *prev;
  curr = prev = root;

  while (curr) {
    if (!strcmp(callsign, curr->callsign)) {
      DLLmprintf((0, "Found a pilot by the name of %s...", callsign));
      if (!memcmp(addr, &curr->addr, sizeof(network_address))) {
        DLLmprintf((0, "Network address matches!\n"));
        DLLmprintf((0, "Removing %s from list\n", callsign));
        if (curr != root) {
          tPlayerStackItem *temp;
          temp = curr->next;
          Remove(curr);
          prev->next = temp;
        } else {
          tPlayerStackItem *temp;
          temp = root->next;
          Remove(root);
          root = temp;
        }
        return;
      } else
        DLLmprintf((0, "But network address doesn't match!\n"));
    }
    prev = curr;
    curr = curr->next;
  }
}

bool PlayerStack::Pop(char *callsign, int maxlength, tDMFCPlayerStat *stat, void *GameStatItem, int GameStatItemsize) {
  if (!root)
    return false;
  if (!callsign)
    return false;
  if (!stat)
    return false;

  tPlayerStackItem *curr, *prev;
  curr = prev = root;

  while (curr->next) {
    prev = curr;
    curr = curr->next;
  }

  // When this ends
  // Last node ==  curr
  // Second to last node == prev (if curr not at root)
  DLLmprintf((0, "Popping %s from list\n", curr->callsign));
  memcpy(stat, &curr->stat, sizeof(tDMFCPlayerStat));
  if ((curr->GameStatItem) && (GameStatItem)) {
    memcpy(GameStatItem, curr->GameStatItem, GameStatItemsize);
  }
  strncpy(callsign, curr->callsign, maxlength);
  curr->callsign[maxlength - 1] = '\0';

  if (curr != root) {
    Remove(curr);
    prev->next = NULL;
  } else {
    Remove(curr);
    root = NULL;
  }
  return true;
}

int PlayerStack::PeekLast(float time, int max, char *callsigns[], int maxlength, tDMFCPlayerStat *stats,
                          void *GameStatItem, int GameStatItemsize) {
  if (!root)
    return 0;
  if (!callsigns)
    return 0;
  if (!stats)
    return 0;

  tPlayerStackItem *curr, *next;
  curr = next = root;
  int count = 0;
  float cutofftime = *basethis->DLLGametime;

  while (curr) {
    next = curr->next;

    // see if this fits within our time
    if (curr->timeadded + time > cutofftime) {
      // we gots a winner
      memcpy(&stats[count], &curr->stat, sizeof(tDMFCPlayerStat));
      if ((curr->GameStatItem) && (GameStatItem)) {
        memcpy(((ubyte *)GameStatItem) + (GameStatItemsize * count), curr->GameStatItem, GameStatItemsize);
      }
      strncpy(callsigns[count], curr->callsign, maxlength);
      curr->callsign[maxlength - 1] = '\0';

      count++;
      if (count >= max) {
        return count;
      }
    }
    curr = next;
  }

  // we're done
  return count;
}