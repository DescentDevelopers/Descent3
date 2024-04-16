/*
 * $Logfile: /DescentIII/Main/entropy/EntropyRoom.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003/08/26 03:57:41 $
 * $Author: kevinb $
 *
 * <insert description of file here>
 *
 * $Log: EntropyRoom.cpp,v $
 * Revision 1.1.1.1  2003/08/26 03:57:41  kevinb
 * initial 1.5 import
 *
 *
 * 15    10/21/99 9:28p Jeff
 * B.A. Macintosh code merge
 *
 * 14    5/19/99 3:24p Jason
 * fixed wrong ordering of InitObjectScripts and MultiSendObject
 *
 * 13    5/19/99 2:42a Jeff
 * call correct NewUIGameWindow functions
 *
 * 12    5/12/99 11:28a Jeff
 * added sourcesafe comment block
 *
 * $NoKeywords: $
 */

#include "gamedll_header.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include "idmfc.h"
#include "Entropystr.h"
#include "EntropyAux.h"

extern IDMFC *DMFCBase;
extern room *dRooms;
extern object *dObjects;

#ifdef MACINTOSH
#pragma export on
#endif

bool PaintRoomWithTexture(int text, int roomnum) {
  if (ROOMNUM_OUTSIDE(roomnum)) {
    Int3();
    return false;
  }

  ASSERT((roomnum >= 0) && (roomnum <= DMFCBase->GetHighestRoomIndex()));
  if ((roomnum < 0) || (roomnum > DMFCBase->GetHighestRoomIndex()))
    return false;

  if (dRooms[roomnum].flags & RF_EXTERNAL)
    return false;

  bool has_goalfaces = false;
  int t;
  for (t = 0; t < dRooms[roomnum].num_faces; t++) {
    if (dRooms[roomnum].faces[t].flags & FF_GOALFACE) {
      has_goalfaces = true;
      break;
    }
  }

  texture *GameTextures = DMFCBase->GetGameTextures();
  dRooms[roomnum].room_change_flags |= RCF_TEXTURE;

  for (t = 0; t < dRooms[roomnum].num_faces; t++) {
    texture *texp = &GameTextures[dRooms[roomnum].faces[t].tmap];
    if ((!(texp->flags & TF_ALPHA)) && texp->r < .01 && texp->g < .01 && texp->b < .01) {
      if (!has_goalfaces || (dRooms[roomnum].faces[t].flags & FF_GOALFACE)) {
        dRooms[roomnum].faces[t].tmap = text;
        dRooms[roomnum].faces[t].flags |= FF_TEXTURE_CHANGED;
      }
    }
  }
  return true;
}

int SpewObjectInRoom(int type, int id, int roomnum) {
  if (DMFCBase->GetLocalRole() != LR_SERVER)
    return -1;

  vector vpos;
  int objnum;

  DLLComputeRoomCenter(&vpos, &dRooms[roomnum]);
  objnum = DLLObjCreate(type, id, roomnum, &vpos, NULL, OBJECT_HANDLE_NONE);
  if (objnum == -1)
    return -1;

  object *obj = &dObjects[objnum];

  ASSERT(obj->movement_type == MT_PHYSICS);
  if (obj->movement_type != MT_PHYSICS)
    return -1;

  // Set random velocity for powerups
  obj->mtype.phys_info.velocity.x = ((float)rand() / RAND_MAX) * 20.0;
  obj->mtype.phys_info.velocity.z = ((float)rand() / RAND_MAX) * 20.0;
  obj->mtype.phys_info.velocity.y = ((float)rand() / RAND_MAX) * 20.0;

  DLLMultiSendObject(obj, 0, true);

  DLLInitObjectScripts(obj, true);

  SendVirusCreate(obj);

  return objnum;
}

void DoServerConfigureDialog(void) {
  if (DMFCBase->GetLocalRole() != LR_SERVER)
    return;

  char tempbuffer[20];
  bool exit_menu = false;

  void *start_game_text_off = DLLCreateNewUITextItem(TXT_STARTGAME, GR_WHITE, -1);
  void *start_game_text_on = DLLCreateNewUITextItem(TXT_STARTGAME, GR_RED, -1);
  void *virus_pk_text = DLLCreateNewUITextItem(TXT_VIRUSCREDIT, GR_WHITE, -1);

  void *main_wnd = DLLNewUIGameWindowCreate(0, 0, 256, 256, UIF_PROCESS_ALL | UIF_CENTER);

  int cury = 20;
  void *virus_pk_edit = DLLEditCreate(main_wnd, 13, 40, cury, 80, 15, 0);
  cury += 35;
  void *start_game_hs =
      DLLHotSpotCreate(main_wnd, UID_OK, K_ENTER, start_game_text_off, start_game_text_on, 40, cury, 130, 15, 0);
  cury += 20;

  sprintf(tempbuffer, "%d", 2);
  DLLEditSetText(virus_pk_edit, tempbuffer);

  DLLNewUIGameWindowOpen(main_wnd);

  while (!exit_menu) {
    int res;
    res = DLLPollUI();

    if (res == -1)
      continue;

    // handle all UI results.
    switch (res) {
    case NEWUIRES_FORCEQUIT:
      exit_menu = true;
      break;
    case 13: // virus_pk edit
      break;
    case UID_OK: // start game
      exit_menu = true;
      DLLEditGetText(virus_pk_edit, tempbuffer, 20);
      int newpk = atoi(tempbuffer);
      DLLmprintf((0, "New Virus/Kill = %d\n", newpk));
      break;
    };
  }
  DLLNewUIGameWindowClose(main_wnd);
  DLLNewUIGameWindowDestroy(main_wnd);

  DLLRemoveUITextItem(start_game_text_on);
  DLLRemoveUITextItem(start_game_text_off);
  DLLRemoveUITextItem(virus_pk_text);

  DLLDeleteUIItem(main_wnd);
  DLLDeleteUIItem(virus_pk_edit);
  DLLDeleteUIItem(start_game_hs);
}
#ifdef MACINTOSH
#pragma export off
#endif
