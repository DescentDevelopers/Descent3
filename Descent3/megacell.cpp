/*
* $Logfile: /descent3/main/megacell.cpp $
* $Revision: 3 $
* $Date: 6/06/97 5:25p $
* $Author: Mark $
*
* $Log: /descent3/main/megacell.cpp $
 *
 * 3     6/06/97 5:25p Mark
 * FROM JASON:Fixed megacell bug
 *
 * 2     6/05/97 2:52p Jason
 * added megacell functions
 *
 * 1     6/05/97 10:56a Jason
 *

*
* $NoKeywords: $
*/

#include "pstypes.h"
#include "pserror.h"
#include "terrain.h"
#include "texture.h"
#include "gametexture.h"
#include "megacell.h"
#include "bitmap.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

megacell Megacells[MAX_MEGACELLS];
int Num_megacells = 0;

// Sets all megacells to unused
void InitMegacells() {
  for (int i = 0; i < MAX_MEGACELLS; i++) {
    Megacells[i].used = 0;
    Megacells[i].name[0] = 0;
  }
  Num_megacells = 0;
}

// Allocs a megacell for use, returns -1 if error, else index on success
int AllocMegacell() {
  for (int i = 0; i < MAX_MEGACELLS; i++) {
    if (Megacells[i].used == 0) {
      memset(&Megacells[i], 0, sizeof(megacell));
      Megacells[i].used = 1;
      Megacells[i].width = DEFAULT_MEGACELL_WIDTH;
      Megacells[i].height = DEFAULT_MEGACELL_HEIGHT;
      Num_megacells++;
      return i;
    }
  }

  Int3(); // No megacells free!
  return -1;
}

// Frees megacell index n
void FreeMegacell(int n) {
  ASSERT(Megacells[n].used > 0);

  Megacells[n].used = 0;
  Megacells[n].name[0] = 0;
  Num_megacells--;
}

// Gets next megacell from n that has actually been alloced
int GetNextMegacell(int n) {
  int i;

  ASSERT(n >= 0 && n < MAX_MEGACELLS);

  if (Num_megacells == 0)
    return 0;

  for (i = n + 1; i < MAX_MEGACELLS; i++)
    if (Megacells[i].used)
      return i;
  for (i = 0; i < n; i++)
    if (Megacells[i].used)
      return i;

  // this is the only one

  return n;
}

// Gets previous megacell from n that has actually been alloced
int GetPrevMegacell(int n) {
  int i;

  ASSERT(n >= 0 && n < MAX_MEGACELLS);

  if (Num_megacells == 0)
    return 0;

  for (i = n - 1; i >= 0; i--) {
    if (Megacells[i].used)
      return i;
  }
  for (i = MAX_MEGACELLS - 1; i > n; i--) {
    if (Megacells[i].used)
      return i;
  }

  // this is the only one
  return n;
}
// Searches thru all megacells for a specific name, returns -1 if not found
// or index of megacell with name
int FindMegacellName(char *name) {
  int i;

  ASSERT(name != NULL);

  for (i = 0; i < MAX_MEGACELLS; i++)
    if (Megacells[i].used && !stricmp(name, Megacells[i].name))
      return i;

  return -1;
}
