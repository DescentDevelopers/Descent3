#ifndef LIST_H
#define LIST_H

#include "pstypes.h"
#include "pserror.h"

typedef struct listnode 
{
	void            *data;
	struct listnode *next;
	struct listnode *prev;
} listnode, list;


// Allocates space for a new list node
listnode *NewListNode(void);

// Adds an item to a list
int AddListItem(list **listp, void *item);

// Removes an item from a list
int RemoveListItem(list **listp, void *item);

// Destroys all the nodes in a list
// The items must be freed in another routine
void DestroyList(list **listp);

// Returns the number of items in a list
int CountListItems(list **listp);

// returns a pointer the given item index in a list
// Returns NULL if point not found
void *GetListItem(list **listp, int index);

// Returns how far from the head of the list a given item is
// Returns -1 if not found
int GetListItemIndex(list **listp, void *item);

#endif