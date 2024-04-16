/*
 * $Logfile: /DescentIII/main/objinit.h $
 * $Revision: 8 $
 * $Date: 4/20/99 12:24p $
 * $Author: Matt $
 *
 * Header for ObjInit.cpp
 *
 * $Log: /DescentIII/main/objinit.h $
 *
 * 8     4/20/99 12:24p Matt
 * Re-revised the ObjInit() system.  ObjInit() again does all the
 * initialization stuff, but now it's passed a lot more information so
 * those fields can be set before the rest of the initialization takes
 * place.
 *
 * 7     4/10/99 5:56p Matt
 * Cleaned up object initialization code, including create object & load
 * object.
 *
 * 6     3/30/98 6:58p Matt
 * When reinitializing an object, don't bash its custom script.
 *
 * 5     3/30/98 6:20p Matt
 * Renamed ResetObject() to be ObjReInitAll()
 *
 * 4     1/18/98 9:06p Matt
 * Changed a comment, and deleted some unused code
 *
 * 3     8/21/97 5:56p Samir
 * Took out script specific stuff from ObjInit and moved to ObjScript
 *
 * 2     8/12/97 3:25p Samir
 * Added prototype to initialize scripts for objects.
 *
 * 2     4/04/97 2:57p Matt
 * Added code to initialize all the type-specific data for an object from
 * the page for that object type.  This means that we need to pass less
 * info to ObjCreate(), and that we save less info in the level save file.
 * It also makes it easy to reset all the objects when an object page has
 * changed.
 *
 * 1     4/04/97 11:38a Matt
 *
 * $NoKeywords: $
 */

#ifndef _OBJINIT_H
#define _OBJINIT_H

#include "object.h"

// Initializes a new object.  All fields not passed in set to defaults.
// Returns 1 if ok, 0 if error
int ObjInit(object *objp, int type, int id, int handle, vector *pos, float creation_time,
            int parent_handle = OBJECT_HANDLE_NONE);

// Re-copies data to each object from the appropriate page for that object type.
// Called after an object page has changed.
void ObjReInitAll(void);

#endif
