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

#ifndef LIST_H
#define LIST_H

#include "pstypes.h"
#include "pserror.h"

struct listnode {
  void *data;
  struct listnode *next;
  struct listnode *prev;
};// listnode, list;

// Allocates space for a new list node
listnode *NewListNode(void);

// Adds an item to a list
int AddListItem(listnode **listp, void *item);

// Removes an item from a list
int RemoveListItem(listnode **listp, void *item);

// Destroys all the nodes in a list
// The items must be freed in another routine
void DestroyList(listnode **listp);

// Returns the number of items in a list
int CountListItems(listnode **listp);

// returns a pointer the given item index in a list
// Returns NULL if point not found
void *GetListItem(listnode **listp, int index);

// Returns how far from the head of the list a given item is
// Returns -1 if not found
int GetListItemIndex(listnode **listp, void *item);

#endif
