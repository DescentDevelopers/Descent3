#include "list.h"
//#include <malloc.h>
#include "mem.h"


// Allocates space for a new list node, returning the pointer to it
listnode *NewListNode(void)
{
	listnode	*node;

   node = (listnode *) mem_malloc(sizeof(listnode));
   if(node == NULL) 
	{
		mprintf ((0,"Not enough memory for a new listnode!\n"));
		Int3();
      return NULL;
   }

   
	node->data = NULL;
	node->next = NULL;
	node->prev = NULL;
	
	return node;
}


// Adds an item to a list
// Returns 1 if everything is ok, else 0
int AddListItem(list **listp, void *item)
{
	listnode	*newnode, *curr;

	newnode = NewListNode();

	if(newnode == NULL) 
	{
		mprintf ((0,"There was a problem mallocing list node memory!\n"));
		Int3();
		return 0;
	}
        

	if(*listp == NULL)		// If this list is empty, construct a new head
	{
		newnode->data = item;
		*listp = newnode;
		return	1;
	} 
	else 
	{
		// Go through until the end of the list and add the new item there
		for(curr=*listp; curr->next!=NULL; curr=curr->next) 
		{
			if(curr->data == item)
			{
				Int3();
				return 0;
			}
		}

			newnode->data = item;
			curr->next = newnode;
			newnode->next = NULL;
			newnode->prev = curr;
	}

	return 1;
}

// Removes an item from a list
int RemoveListItem(list **listp, void *item)
{
	listnode	*curr, *node;
	int inlist = 0;

	for(curr=*listp; curr!=NULL; curr=curr->next) 
	{
		if(curr->data == item) 
		{
			inlist = 1;
			node = curr;
			break;
      }
	}

	if(!inlist) 
	{
		mprintf((0,"RemoveItem: Warning, item not found in list\n"));
		return 0;
	}

	// Head of list is a special case
	if(node == *listp) 
	{
		*listp = node->next;
		if(*listp != NULL)
			(*listp)->prev = NULL;
		mem_free(node);
		return 1;
	} 
	else 
	{
		if(node->prev != NULL)
			node->prev->next = node->next;
		if(node->next != NULL)
			node->next->prev = node->prev;
		mem_free(node);
	}
	return 1;
}


// Destroys all the nodes in a list
// The items must be freed in another routine
void DestroyList(list **listp)
{
	listnode	*node, *next;

	for(node=*listp; node!=NULL; node=next) 
	{
		next = node->next;
		mem_free(node);
	}
}

// Returns the number of items in a list
int CountListItems(list **listp)
{
	listnode	*node;
	int	count = 0;

	for(node=*listp; node!=NULL; node=node->next)
		count++;

	return count;
}

// returns a pointer the given item index in a list
// Returns NULL if point not found
void *GetListItem(list **listp, int index)
{
	listnode	*node;

	for(node=*listp; node!=NULL; node=node->next) 
	{
		if(index-- == 0)
			return node->data;
	}

	return NULL;
}

// Returns how far from the head of the list a given item is
// Returns -1 if not found
int GetListItemIndex(list **listp, void *item)
{
	listnode	*node;
	int	index = 0;

	for(node=*listp; node!=NULL; node=node->next) 
	{
		if(node->data == item)
			return index;
		index++;
	}

	return -1;
}
