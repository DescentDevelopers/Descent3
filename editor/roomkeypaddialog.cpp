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

 * $Logfile: /DescentIII/Main/editor/roomkeypaddialog.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:38 $
 * $Author: kevinb $
 *
 *	Room keypad
 *
 * $Log: not supported by cvs2svn $
 *
 * 68    10/21/99 1:45p Jeff
 * fixed dialog update bug
 *
 * 67    10/21/99 1:33p Jeff
 * added checkbox to prevent certain rooms from lighting
 *
 * 66    4/29/99 5:44p Chris
 * Added the check for bad center points
 *
 * 65    4/29/99 1:59a Chris
 * Added the portal blockage support
 *
 * 64    4/28/99 1:39p Chris
 * Added the ability to block portals
 *
 * 63    4/24/99 4:32p Jason
 * fixed secret room checkbox
 *
 * 62    4/16/99 1:44p Jason
 * fixed selection of special rooms as goal rooms
 *
 * 61    4/15/99 12:18p Jason
 * made mirrors more robust and able to have multiple mirrored faces in
 * the same room (as long as they are all on the same plane)
 *
 * 60    4/14/99 7:21p Jason
 * added secret room checkbox
 *
 * 59    4/03/99 7:55p Matt
 * Use new keypad update system instead of timer to update.
 *
 * 58    3/31/99 5:15p Matt
 * Added checkboxes on the room tab to control rendering of faces based on
 * shell flag.
 *
 * 57    3/31/99 11:34a Matt
 * Got rid of unused forcefield checkbox on room tab
 *
 * 56    3/30/99 6:25p Matt
 * Use new error system for combine faces function.
 *
 * 55    2/03/99 5:48p Matt
 * Added room damage system
 *
 * 54    1/21/99 11:15p Jeff
 * pulled out some structs and defines from header files and moved them
 * into separate header files so that multiplayer dlls don't require major
 * game headers, just those new headers.  Side effect is a shorter build
 * time.  Also cleaned up some header file #includes that weren't needed.
 * This affected polymodel.h, object.h, player.h, vecmat.h, room.h,
 * manage.h and multi.h
 *
 * 53    1/15/99 10:50a Matt
 * Added rename room to Room menu & added shortcut key
 *
 * 52    1/08/99 6:32p Samir
 *
 * 51    1/08/99 5:37p Samir
 * reverb values per room.
 *
 * 50    12/30/98 6:50p Matt
 * Don't add rooms with no name (which may be placed doors) to the room
 * palette list
 *
 * 49    12/22/98 2:03p Matt
 * Added room names, and made rooms not compress so that room numbers are
 * suitable for persistant uses.
 *
 * 48    11/11/98 2:15p Jason
 *
 * 47    11/11/98 2:15p Jason
 * save current room directory
 *
 * 46    10/06/98 6:08p Jason
 * added RF_TRIANGULATE so that special rooms (with non-planar UVS) can be
 * drawn correctly
 *
 * 45    10/04/98 2:35p Matt
 * Added debug options to limit rendering
 *
 * 44    9/28/98 6:09p Jason
 * Corrected appending ORF to rooms
 *
 * 43    9/18/98 8:13p Jason
 * automatically append .orf extension when saving rooms
 *
 * 42    9/07/98 8:04p Matt
 * After combining faces, set curface to marked face.  (Old curface gets
 * deleting when joined.)
 *
 * 41    9/01/98 12:04p Matt
 * Ripped out multi-face portal code
 *
 * 40    8/28/98 6:21p Matt
 * Added a function to flip a face.
 *
 * 39    8/27/98 7:16p Jason
 * changes to dialog
 *
 * 38    8/20/98 12:17p Matt
 * When building ambient sound pattern list, ignore patterns that have
 * been marked as deleted.
 *
 * 37    8/20/98 12:07p Matt
 * Rebuild the ambient sound pattern list whenever we update, since the
 * list can change while the program is running.
 *
 * 36    8/17/98 6:40p Matt
 * Added ambient sound system
 *
 * 35    8/10/98 7:31p Jeff
 * fixed bug with special flag 6
 *
 * 34    7/20/98 4:46p Jason
 * added BOA vis table stuff
 *
 * 33    7/16/98 12:06p Jason
 * added special flags to room structure
 *
 * 32    4/21/98 2:31p Matt
 * When showing what vertex selected, show vert number in addition to
 * index
 *
 * 31    4/10/98 4:12p Jason
 * fixed room loading bug
 *
 * 30    4/10/98 11:52a Jason
 * fixed face splintering bug
 *
 * 29    3/30/98 5:49p Jason
 * fixed selection of faces
 *
 * 28    3/16/98 6:41p Jason
 * added goal room stuff
 *
 * 27    3/06/98 3:23p Jason
 * added lighting from satellites to indoor rooms
 *
 * 26    2/10/98 7:46p Matt
 * Added roompad check boxes for fuelcen and goals
 *
 * 25    2/10/98 1:12p Jason
 * added forcefields and saturating walls
 *
 * 24    1/21/98 12:33p Matt
 * Revamped viewer system
 *
 * 23    1/20/98 4:05p Matt
 * Move BuildListOfPortalVerts() from roomkeypaddialog.cpp to erooms.cpp
 *
 * 22    12/23/97 1:33p Samir
 * Added pserror.h
 *
 * 21    12/02/97 5:31p Samir
 * New file dialog interface implemented.
 *
 * 20    11/05/97 9:31p Jason
 * made render_external_rooms button work
 *
 * 19    10/01/97 7:49p Matt
 * Added "External Room" checkbox on room keypad
 *
 * 18    9/15/97 12:26p Matt
 * When a face is deleted (via combining faces) update markedface if it's
 * changed
 *
 * 17    9/10/97 5:13p Matt
 * Made small portal change to room keypad
 *
 * 16    9/06/97 10:53p Matt
 * Added portal stuff to room keypad
 *
 * 15    9/06/97 3:14p Matt
 * Added button to go to next portal
 * Added separate face & vert count displays for current room & cur
 * palette room
 * Use new PrintToDsgItem() function
 *
 * 14    9/02/97 12:08p Samir
 * Fixed scrolling.
 *
 * 13    8/29/97 3:57p Samir
 * This should add scrolling support.
 *
 * 12    8/21/97 5:59p Matt
 * Added next vertex & combine faces functions
 *
 * 11    8/04/97 12:48p Matt
 * Use SetMarkedRoom()
 *
 * 10    8/01/97 4:56p Jason
 * added face moving
 *
 * 9     8/01/97 12:50p Jason
 * added code to support scaling of rooms/faces/edges
 *
 * 8     7/31/97 3:31p Jason
 * added functions to rotate portal rooms
 *
 * 7     7/29/97 12:45p Matt
 * Force window redraws when loading or switching rooms
 *
 * 6     7/28/97 5:32p Matt
 * When loading a room, update the keypad dialog, and force the editor
 * into room view
 *
 * 6     6/30/97 1:30p Jason
 * added netherspace stuff
 *
 * 5     6/27/97 4:34p Jason
 * fixed silly bug
 *
 * 4     6/27/97 4:15p Jason
 * added more room functions
 *
 * 3     6/27/97 3:10p Jason
 * added groovy stuff for rooms
 *
 * 2     6/27/97 12:20p Samir
 * Added RCS comment header.
 *
 * $NoKeywords: $
 */

#include "stdafx.h"
#include "editor.h"
#include "RoomKeypadDialog.h"
#include "ddio.h"
#include "erooms.h"
#include "mainfrm.h"
#include "rotateroom.h"
#include "boa.h"
#include "HRoom.h"
#include "pserror.h"
#include "HView.h"
#include "ambient.h"
#include "render.h"
#include "EditLineDialog.h"
#include "hlsoundlib.h"
#include "mem.h"
#include "mono.h"
#include "damage_external.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// list shold match hlsoundlib.h constants.
const char *EnvAudio_PresetNames[N_ENVAUDIO_PRESETS] = {
    "<none>",       "Padded Cell", "Room",    "Bathroom",  "Living Room", "Stone Room", "Auditorium",
    "Concert Hall", "Cave",        "Arena",   "Hangar",    "Carpet Hall", "Hall",       "Stone Corridor",
    "Alley",        "Forest",      "City",    "Mountains", "Quarry",      "Plain",      "Parkinglot",
    "Sewer",        "Underwater",  "Drugged", "Dizzy",     "Psychotic"};

/////////////////////////////////////////////////////////////////////////////
// CRoomKeypadDialog dialog

int CRoomKeypadDialog::m_Instances = 0; // number of instances of this keypad.

CRoomKeypadDialog::CRoomKeypadDialog(CWnd *pParent /*=NULL*/) : CKeypadDialog(CRoomKeypadDialog::IDD, pParent) {
  //{{AFX_DATA_INIT(CRoomKeypadDialog)
  // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
}

void CRoomKeypadDialog::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CRoomKeypadDialog)
  // NOTE: the ClassWizard will add DDX and DDV calls here
  //}}AFX_DATA_MAP
}

void CRoomKeypadDialog::RunKeypadFunction(int code) {}

//	These functions save information about this dialog's current UI state to the database
void CRoomKeypadDialog::LoadSettings() {}

void CRoomKeypadDialog::SaveSettings() {}

BEGIN_MESSAGE_MAP(CRoomKeypadDialog, CDialog)
//{{AFX_MSG_MAP(CRoomKeypadDialog)
ON_BN_CLICKED(IDC_LOAD_ROOM, OnLoadRoom)
ON_BN_CLICKED(IDC_SAVE_ROOM_LOCALLY, OnSaveRoomLocally)
ON_CBN_SELENDOK(IDC_ROOM_PULLDOWN, OnSelendokRoomPulldown)
ON_WM_PAINT()
ON_WM_HELPINFO()
ON_BN_CLICKED(IDC_MARK_ROOM, OnMarkRoom)
ON_BN_CLICKED(IDC_ROOMPAD_ROTATE_LEFT, OnRoompadRotateLeft)
ON_BN_CLICKED(IDC_ROOMPAD_ROTATE_RIGHT, OnRoompadRotateRight)
ON_BN_CLICKED(IDC_ROOMPAD_PITCH_FORWARD, OnRoompadPitchForward)
ON_BN_CLICKED(IDC_ROOMPAD_PITCH_BACKWARD, OnRoompadPitchBackward)
ON_BN_CLICKED(IDC_ROOMPAD_BANK_RIGHT, OnRoompadBankRight)
ON_BN_CLICKED(IDC_ROOMPAD_BANK_LEFT, OnRoompadBankLeft)
ON_BN_CLICKED(IDC_ROOMPAD_NEXT_EDGE, OnRoompadNextEdge)
ON_BN_CLICKED(IDC_ROOMPAD_EXPAND_ROOM, OnRoompadExpandRoom)
ON_BN_CLICKED(IDC_ROOMPAD_CONTRACT_ROOM, OnRoompadContractRoom)
ON_BN_CLICKED(IDC_ROOMPAD_EXPAND_FACE, OnRoompadExpandFace)
ON_BN_CLICKED(IDC_ROOMPAD_CONTRACT_FACE, OnRoompadContractFace)
ON_BN_CLICKED(IDC_ROOMPAD_EXPAND_EDGE, OnRoompadExpandEdge)
ON_BN_CLICKED(IDC_ROOMPAD_CONTRACT_EDGE, OnRoompadContractEdge)
ON_BN_CLICKED(IDC_ROOMPAD_MOVE_FORWARD, OnRoompadMoveForward)
ON_BN_CLICKED(IDC_ROOMPAD_MOVE_BACKWARD, OnRoompadMoveBackward)
ON_BN_CLICKED(IDC_ROOMPAD_MOVE_RIGHT, OnRoompadMoveRight)
ON_BN_CLICKED(IDC_ROOMPAD_MOVE_LEFT, OnRoompadMoveLeft)
ON_BN_CLICKED(IDC_ROOMPAD_MOVE_UP, OnRoompadMoveUp)
ON_BN_CLICKED(IDC_ROOMPAD_MOVE_DOWN, OnRoompadMoveDown)
ON_BN_CLICKED(IDC_ROOMPAD_NEXT_VERTEX, OnRoompadNextVertex)
ON_BN_CLICKED(IDC_ROOMPAD_COMBINE_FACES, OnRoompadCombineFaces)
ON_WM_HSCROLL()
ON_WM_VSCROLL()
ON_WM_SIZE()
ON_BN_CLICKED(IDC_ROOMPAD_NEXT_PORTAL, OnRoompadNextPortal)
ON_BN_CLICKED(IDC_RENDER_PORTAL_FACES, OnRenderPortalFaces)
ON_BN_CLICKED(IDC_ENABLE_PORTAL_FLYTHROUGH, OnEnablePortalFlythrough)
ON_BN_CLICKED(IDC_EXTERNAL_ROOM, OnExternalRoom)
ON_BN_CLICKED(IDC_RENDER_EXTERNAL_ROOMS, OnRenderExternalRooms)
ON_BN_CLICKED(IDC_NEXT_FACE, OnNextFace)
ON_BN_CLICKED(IDC_ROOMPAD_GOAL1, OnRoompadGoal1)
ON_BN_CLICKED(IDC_ROOMPAD_GOAL2, OnRoompadGoal2)
ON_BN_CLICKED(IDC_ROOMPAD_REFUELING_CENTER, OnRoompadRefuelingCenter)
ON_BN_CLICKED(IDC_TOUCHES_OUTSIDE, OnTouchesOutside)
ON_BN_CLICKED(IDC_ROOMPAD_GOAL3, OnRoompadGoal3)
ON_BN_CLICKED(IDC_ROOMPAD_GOAL4, OnRoompadGoal4)
ON_BN_CLICKED(IDC_GOALFACE_CHECK, OnGoalfaceCheck)
ON_BN_CLICKED(IDC_SPECIAL_1, OnSpecial1)
ON_BN_CLICKED(IDC_SPECIAL_2, OnSpecial2)
ON_BN_CLICKED(IDC_SPECIAL_3, OnSpecial3)
ON_BN_CLICKED(IDC_SPECIAL_4, OnSpecial4)
ON_BN_CLICKED(IDC_SPECIAL_5, OnSpecial5)
ON_BN_CLICKED(IDC_SPECIAL_6, OnSpecial6)
ON_BN_CLICKED(IDC_BOA_VIS_BUTTON, OnBoaVisButton)
ON_CBN_SELENDOK(IDC_ROOM_AMBIENT, OnSelendokRoomAmbient)
ON_BN_CLICKED(IDC_MIRROR_BUTTON, OnMirrorButton)
ON_BN_CLICKED(IDC_NO_MIRROR_BUTTON, OnNoMirrorButton)
ON_BN_CLICKED(IDC_ROOMPAD_FLIP_FACE, OnRoompadFlipFace)
ON_BN_CLICKED(IDC_ROOMPAD_NO_DRAW_TERRAIN, OnRoompadNoDrawTerrain)
ON_BN_CLICKED(IDC_ROOMPAD_ONE_ROOM_ONLY, OnRoompadOneRoomOnly)
ON_BN_CLICKED(IDC_TRIANGULATE, OnTriangulate)
ON_BN_CLICKED(IDC_EDIT_NAME, OnEditName)
ON_CBN_SELCHANGE(IDC_ROOM_ENVAUDIO, OnSelchangeRoomEnvaudio)
ON_EN_KILLFOCUS(IDC_ROOMPAD_DAMAGE, OnKillfocusRoompadDamage)
ON_CBN_SELCHANGE(IDC_ROOMPAD_DAMAGETYPE, OnSelchangeRoompadDamageType)
ON_BN_CLICKED(IDC_RENDER_SHELL, OnRenderShell)
ON_BN_CLICKED(IDC_RENDER_NON_SHELL, OnRenderNonShell)
ON_BN_CLICKED(IDC_SECRET_CHECK, OnSecretCheck)
ON_BN_CLICKED(IDC_PORTAL_BLOCKED_CHECK, OnPortalBlockedCheck)
ON_BN_CLICKED(IDC_PORTAL_BLOCKED_CAN_GO_AWAY_CHECK, OnPortalBlockedCanGoAwayCheck)
ON_BN_CLICKED(IDC_BAD_CENTERPOINTS, OnBadCenterpoints)
ON_BN_CLICKED(IDC_ROOM_SKIP_LIGHTING, OnRoomSkipLighting)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRoomKeypadDialog message handlers

void CRoomKeypadDialog::OnLoadRoom() {
  char filename[255];
  int c = 1, finding_name = 1;

  CString filter = "Descent III room files (*.orf)|*.orf||";

  if (!OpenFileDialog(this, (LPCSTR)filter, filename, Current_room_dir, sizeof(Current_room_dir)))
    return;

  char name[255];
  char path[255];
  char extension[255];
  char roomname[255];
  int find_new = 0;

  // Find a unique name for this room
  ddio_SplitPath(filename, path, name, extension);
  int done = 0;
  int count = 1;

  strcpy(Current_room_dir, path);

  sprintf(roomname, "%s.ORF", name);

  int val = FindRoomName(roomname);

  if (val != -1) {
    int answer =
        MessageBox("Do you wish to overwrite the room in memory that already has this name?", roomname, MB_YESNO);
    if (answer == IDYES)
      FreeRoom(&Rooms[val]);
    else
      find_new = 1;
  }

  //	Okay, we selected a file. Lets do what needs to be done here.
  int n = AllocLoadRoom(filename);

  if (n < 0) {
    OutrageMessageBox("Couldn't load that room for some reason.");
    return;
  }

  while (!done && find_new) {
    val = FindRoomName(roomname);
    if (val == -1) {
      done = 1;
      continue;
    }
    count++;
    sprintf(roomname, "%s%d.ORF", name, count);
  }

  ASSERT(Rooms[n].name == NULL);
  Rooms[n].name = mem_strdup(roomname);

  D3EditState.current_room = n;

  // Update keypad
  UpdateDialog();

  // Force into room view
  ResetRoomViewer();
  SetViewMode(VM_ROOM);

  // Force window updates
  State_changed = 1;
}

void CRoomKeypadDialog::OnSaveRoomLocally() {
  char filename[255];

  if (Curroomp == NULL || Curroomp->used == 0) {
    OutrageMessageBox("No valid room selected for saving!");
    return;
  }

  CString filter = "Descent III room files (*.orf)|*.orf||";

  if (!SaveFileDialog(this, (LPCSTR)filter, filename, Current_weapon_dir, sizeof(Current_weapon_dir)))
    return;

  int len = strlen(filename);
  if (filename[len - 4] != '.' || filename[len - 3] != 'o' || filename[len - 2] != 'r' || filename[len - 1] != 'f')
    strcat(filename, ".orf");

  // Save it out to disk (locally)
  SaveRoom(Curroomp - Rooms, filename);
  OutrageMessageBox("Room saved.");
}

void CRoomKeypadDialog::OnSelendokRoomPulldown() {
  int i, cur;
  char name[200];

  cur = SendDlgItemMessage(IDC_ROOM_PULLDOWN, CB_GETCURSEL, 0, 0);
  SendDlgItemMessage(IDC_ROOM_PULLDOWN, CB_GETLBTEXT, cur, (LPARAM)(LPCTSTR)name);

  i = FindRoomName(name);

  if (i == -1) {
    mprintf(0, "Possible corrupted room list, but probably nothing.");
    UpdateDialog();
  }

  D3EditState.current_room = i;
  UpdateDialog();

  ResetRoomViewer();

  // Force window updates
  State_changed = 1;
}

#define NULL_NAME "<none>"

void CRoomKeypadDialog::UpdateDialog() {
  int i;

  // Show the number of verts, faces, & portals in the current room
  PrintToDlgItem(this, IDC_ROOM_NAME, "Name:\t%s", Curroomp->name ? Curroomp->name : "<none>");
  PrintToDlgItem(this, IDC_VERTEX_COUNT, "Vertices:\t%d", Curroomp->num_verts);
  PrintToDlgItem(this, IDC_FACE_COUNT, "Faces:\t%d", Curroomp->num_faces);
  PrintToDlgItem(this, IDC_PORTAL_COUNT, "Portals:\t%d", Curroomp->num_portals);

  // Show flags
  ((CButton *)GetDlgItem(IDC_EXTERNAL_ROOM))->SetCheck(Curroomp->flags & RF_EXTERNAL);
  ((CButton *)GetDlgItem(IDC_ROOM_SKIP_LIGHTING))->SetCheck((Curroomp->flags & RF_NO_LIGHT) ? 1 : 0);
  ((CButton *)GetDlgItem(IDC_ROOMPAD_REFUELING_CENTER))->SetCheck(Curroomp->flags & RF_FUELCEN);
  ((CButton *)GetDlgItem(IDC_ROOMPAD_GOAL1))->SetCheck(Curroomp->flags & RF_GOAL1);
  ((CButton *)GetDlgItem(IDC_ROOMPAD_GOAL2))->SetCheck(Curroomp->flags & RF_GOAL2);
  ((CButton *)GetDlgItem(IDC_ROOMPAD_GOAL3))->SetCheck(Curroomp->flags & RF_GOAL3);
  ((CButton *)GetDlgItem(IDC_ROOMPAD_GOAL4))->SetCheck(Curroomp->flags & RF_GOAL4);
  ((CButton *)GetDlgItem(IDC_SPECIAL_1))->SetCheck(Curroomp->flags & RF_SPECIAL1);
  ((CButton *)GetDlgItem(IDC_SPECIAL_2))->SetCheck(Curroomp->flags & RF_SPECIAL2);
  ((CButton *)GetDlgItem(IDC_SPECIAL_3))->SetCheck(Curroomp->flags & RF_SPECIAL3);
  ((CButton *)GetDlgItem(IDC_SPECIAL_4))->SetCheck(Curroomp->flags & RF_SPECIAL4);
  ((CButton *)GetDlgItem(IDC_SPECIAL_5))->SetCheck(Curroomp->flags & RF_SPECIAL5);
  ((CButton *)GetDlgItem(IDC_SPECIAL_6))->SetCheck(Curroomp->flags & RF_SPECIAL6);
  ((CButton *)GetDlgItem(IDC_TRIANGULATE))->SetCheck(Curroomp->flags & RF_TRIANGULATE);
  ((CButton *)GetDlgItem(IDC_TOUCHES_OUTSIDE))->SetCheck(Curroomp->flags & RF_TOUCHES_TERRAIN);

  if (Curroomp->flags & RF_SECRET)
    ((CButton *)GetDlgItem(IDC_SECRET_CHECK))->SetCheck(1);
  else
    ((CButton *)GetDlgItem(IDC_SECRET_CHECK))->SetCheck(0);

  // Show flags
  ((CButton *)GetDlgItem(IDC_EXTERNAL_ROOM))->SetCheck(Curroomp->flags & RF_EXTERNAL);
  ((CButton *)GetDlgItem(IDC_ROOMPAD_NO_DRAW_TERRAIN))->SetCheck(Render_inside_only);
  ((CButton *)GetDlgItem(IDC_ROOMPAD_ONE_ROOM_ONLY))->SetCheck(Render_one_room_only);
  ((CButton *)GetDlgItem(IDC_ROOM_SKIP_LIGHTING))->SetCheck((Curroomp->flags & RF_NO_LIGHT) ? 1 : 0);

  // set forcefield check
  //@@((CButton *) GetDlgItem(IDC_FORCEFIELD_CHECK))->SetCheck(Curroomp->faces[Curface].flags & FF_FORCEFIELD);
  ((CButton *)GetDlgItem(IDC_GOALFACE_CHECK))->SetCheck(Curroomp->faces[Curface].flags & FF_GOALFACE);
  PrintToDlgItem(this, IDC_CURRENT_FACE, "Current face: %d", Curface);

  // Enable/disable the next portal button
  ((CButton *)GetDlgItem(IDC_ROOMPAD_NEXT_PORTAL))
      ->EnableWindow(((Curportal == -1) && (Curroomp->num_portals > 0)) || (Curroomp->num_portals > 1));

  // Enable/disable the portal items
  ((CButton *)GetDlgItem(IDC_CURRENT_PORTAL))->EnableWindow(Curportal != -1);
  ((CButton *)GetDlgItem(IDC_RENDER_PORTAL_FACES))->EnableWindow(Curportal != -1);
  ((CButton *)GetDlgItem(IDC_PORTAL_BLOCKED_CHECK))->EnableWindow(Curportal != -1);
  ((CButton *)GetDlgItem(IDC_PORTAL_BLOCKED_CAN_GO_AWAY_CHECK))->EnableWindow(Curportal != -1);

  // Set shell render checkboxes
  ((CButton *)GetDlgItem(IDC_RENDER_SHELL))->SetCheck(!(Shell_render_flag & SRF_NO_SHELL));
  ((CButton *)GetDlgItem(IDC_RENDER_NON_SHELL))->SetCheck(!(Shell_render_flag & SRF_NO_NON_SHELL));

  // Update the portal number & checkboxes
  if (Curportal != -1) {
    portal *pp = &Curroomp->portals[Curportal];

    ASSERT(Curportal < Curroomp->num_portals);

    PrintToDlgItem(this, IDC_CURRENT_PORTAL, "Portal:\t%d", Curportal);
    ((CButton *)GetDlgItem(IDC_RENDER_PORTAL_FACES))->SetCheck(pp->flags & PF_RENDER_FACES);
    ((CButton *)GetDlgItem(IDC_ENABLE_PORTAL_FLYTHROUGH))->SetCheck(pp->flags & PF_RENDERED_FLYTHROUGH);
    ((CButton *)GetDlgItem(IDC_PORTAL_BLOCKED_CHECK))->SetCheck(pp->flags & PF_BLOCK);
    ((CButton *)GetDlgItem(IDC_PORTAL_BLOCKED_CAN_GO_AWAY_CHECK))->SetCheck(pp->flags & PF_BLOCK_REMOVABLE);

    // if render faces is on, enable the rendered flythrough checkbox
    ((CButton *)GetDlgItem(IDC_ENABLE_PORTAL_FLYTHROUGH))->EnableWindow(pp->flags & PF_RENDER_FACES);
    ((CButton *)GetDlgItem(IDC_PORTAL_BLOCKED_CAN_GO_AWAY_CHECK))->EnableWindow(pp->flags & PF_BLOCK);
  } else {
    ((CButton *)GetDlgItem(IDC_ENABLE_PORTAL_FLYTHROUGH))->EnableWindow(0);
    ((CButton *)GetDlgItem(IDC_RENDER_PORTAL_FACES))->SetCheck(0);
    ((CButton *)GetDlgItem(IDC_ENABLE_PORTAL_FLYTHROUGH))->SetCheck(0);
    ((CButton *)GetDlgItem(IDC_PORTAL_BLOCKED_CHECK))->SetCheck(0);
    ((CButton *)GetDlgItem(IDC_PORTAL_BLOCKED_CAN_GO_AWAY_CHECK))->SetCheck(0);
  }

  // Build the ambient sound list
  SendDlgItemMessage(IDC_ROOM_AMBIENT, CB_RESETCONTENT, 0, 0);
  SendDlgItemMessage(IDC_ROOM_AMBIENT, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)NULL_NAME);
  for (i = 0; i < Num_ambient_sound_patterns; i++)
    if (Ambient_sound_patterns[i].name[0])
      SendDlgItemMessage(IDC_ROOM_AMBIENT, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)AmbientSoundPatternName(i));
  SendDlgItemMessage(IDC_ROOM_AMBIENT, CB_SELECTSTRING, 0, (LPARAM)(LPCTSTR)NULL_NAME);

  // Set the current ambient sound pattern
  SendDlgItemMessage(IDC_ROOM_AMBIENT, CB_SELECTSTRING, 0,
                     (LPARAM)(LPCTSTR)((Curroomp->ambient_sound == -1)
                                           ? NULL_NAME
                                           : AmbientSoundPatternName(Curroomp->ambient_sound)));

  // Build environmental presets list
  ((CComboBox *)GetDlgItem(IDC_ROOM_ENVAUDIO))
      ->SelectString(-1, EnvAudio_PresetNames[(Curroomp->env_reverb < N_ENVAUDIO_PRESETS) ? Curroomp->env_reverb : 0]);

  // Update Mirror Stuff
  char str[255];
  if (Curroomp->mirror_face == -1)
    strcpy(str, "None");
  else
    sprintf(str, "%d", Curroomp->mirror_face);

  PrintToDlgItem(this, IDC_MIRROR_FACE_TEXT, "Current mirror face:\t%s", str);

  // Update the damage
  CEdit *box = (CEdit *)GetDlgItem(IDC_ROOMPAD_DAMAGE);
  sprintf(str, "%.2f", Curroomp->damage);
  box->SetWindowText(str);

  // Update the damage type
  CComboBox *cbox = (CComboBox *)GetDlgItem(IDC_ROOMPAD_DAMAGETYPE);
  int n = cbox->GetCount();
  for (i = 0; i < n; i++) {
    if (cbox->GetItemData(i) == Curroomp->damage_type) {
      cbox->SetCurSel(i);
      break;
    }
  }
  ASSERT(i < n);

  // Get current room in palette
  n = D3EditState.current_room;
  if ((n == -1) || !Rooms[n].used)                 // no current room?
    n = D3EditState.current_room = GetNextRoom(n); //..try to get one

  // Update palette room stuff
  if (n != -1) {

    // Show the number of verts & faces in the current palette room
    PrintToDlgItem(this, IDC_PALETTE_VERTEX_COUNT, "Vertices:\t%d", Rooms[n].num_verts);
    PrintToDlgItem(this, IDC_PALETTE_FACE_COUNT, "Faces:\t%d", Rooms[n].num_faces);

    // Update the list of palette rooms
    SendDlgItemMessage(IDC_ROOM_PULLDOWN, CB_RESETCONTENT, 0, 0);

    for (int i = FIRST_PALETTE_ROOM; i < FIRST_PALETTE_ROOM + MAX_PALETTE_ROOMS; i++) {
      if (Rooms[i].used && Rooms[i].name) // if a placed door, won't have a name
        SendDlgItemMessage(IDC_ROOM_PULLDOWN, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)Rooms[i].name);
    }
    SendDlgItemMessage(IDC_ROOM_PULLDOWN, CB_SELECTSTRING, 0, (LPARAM)(LPCTSTR)Rooms[n].name);
  }
}

void CRoomKeypadDialog::OnPaint() {
  CPaintDC dc(this); // device context for painting

  UpdateDialog();

  // Do not call CDialog::OnPaint() for painting messages
}

BOOL CRoomKeypadDialog::OnHelpInfo(HELPINFO *pHelpInfo) {
  // TODO: Add your message handler code here and/or call default
  WinHelp(HID_ROOMTAB, HELP_CONTEXT);
  return TRUE;
  // return CDialog::OnHelpInfo(pHelpInfo);
}

void CRoomKeypadDialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar) {
  CKeypadDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CRoomKeypadDialog::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar) {
  CKeypadDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CRoomKeypadDialog::OnSize(UINT nType, int cx, int cy) { CKeypadDialog::OnSize(nType, cx, cy); }

/////////////////////////////////////////////////////////////////////////////
// helper functions

// Goes through all the vertices in the room and scales them by scale_factor
void SizeRoomVertices(room *rp, float scale_factor) {
  vector room_center;
  vector vec;
  int portal_list[MAX_VERTS_PER_ROOM];
  int i, t, ok_to_scale;
  int num_portal_verts;

  ASSERT(rp->used > 0);

  num_portal_verts = BuildListOfPortalVerts(rp, portal_list);

  ComputeRoomCenter(&room_center, rp);

  for (i = 0; i < rp->num_verts; i++) {
    for (ok_to_scale = 1, t = 0; t < num_portal_verts; t++) {
      if (portal_list[t] == i) {
        ok_to_scale = 0;
        break;
      }
    }

    if (ok_to_scale) {
      vec = rp->verts[i] - room_center;
      vec *= scale_factor;
      rp->verts[i] = vec + room_center;
    }
  }
}

// Goes through all the vertices in a face and scales them by scale_factor
void SizeFaceVertices(room *rp, int facenum, float scale_factor) {
  vector face_center;
  vector vec;
  int portal_list[MAX_VERTS_PER_ROOM];
  int face_list[MAX_FACES_PER_ROOM];
  int i, t, ok_to_scale, k;
  int num_portal_verts;
  int facecount = 0;
  face *fp = &rp->faces[facenum];

  ASSERT(rp->used > 0);

  num_portal_verts = BuildListOfPortalVerts(rp, portal_list);

  ComputeCenterPointOnFace(&face_center, rp, facenum);

  for (i = 0; i < fp->num_verts; i++) {
    int vertnum = fp->face_verts[i];

    for (ok_to_scale = 1, t = 0; t < num_portal_verts; t++) {
      if (portal_list[t] == vertnum) {
        ok_to_scale = 0;
        break;
      }
    }

    if (ok_to_scale) {
      vec = rp->verts[vertnum] - face_center;
      vec *= scale_factor;
      rp->verts[vertnum] = vec + face_center;
    }
  }

  // Build a list of faces that share vertices with this face
  for (i = 0; i < rp->num_faces; i++) {
    for (t = 0; t < rp->faces[i].num_verts; t++) {
      for (k = 0; k < fp->num_verts; k++) {
        if (fp->face_verts[k] == rp->faces[i].face_verts[t]) {
          face_list[facecount] = i;
          facecount++;
        }
      }
    }
  }

  // Now fixup any concave/nonplanar faces
  FixConcaveFaces(rp, face_list, facecount);
}

void SizeFaceEdge(room *rp, int facenum, int edge, float scale_factor) {
  vector line_center;
  vector vec;
  int portal_list[MAX_VERTS_PER_ROOM];
  int face_list[MAX_FACES_PER_ROOM];
  int i, t, ok_to_scale;
  int num_portal_verts;
  int facecount = 0;
  face *fp = &rp->faces[facenum];
  int nextedge, vert, nextvert;

  nextedge = (edge + 1) % fp->num_verts;
  vert = fp->face_verts[edge];
  nextvert = fp->face_verts[nextedge];

  ASSERT(rp->used > 0);

  num_portal_verts = BuildListOfPortalVerts(rp, portal_list);

  line_center = ((rp->verts[nextvert] - rp->verts[vert]) / 2) + rp->verts[vert];

  for (ok_to_scale = 1, t = 0; t < num_portal_verts; t++)
    if (portal_list[t] == vert)
      ok_to_scale = 0;

  if (ok_to_scale) {
    vec = rp->verts[vert] - line_center;
    vec *= scale_factor;
    rp->verts[vert] = vec + line_center;
  }

  for (ok_to_scale = 1, t = 0; t < num_portal_verts; t++)
    if (portal_list[t] == nextvert)
      ok_to_scale = 0;

  if (ok_to_scale) {
    vec = rp->verts[nextvert] - line_center;
    vec *= scale_factor;
    rp->verts[nextvert] = vec + line_center;
  }

  // Build a list of faces that share vertices with this edge
  for (i = 0; i < rp->num_faces; i++) {
    for (t = 0; t < rp->faces[i].num_verts; t++) {
      if (rp->faces[i].face_verts[t] == vert || rp->faces[i].face_verts[t] == nextvert) {
        face_list[facecount] = i;
        facecount++;
      }
    }
  }

  // Now fixup any concave/nonplanar faces
  FixConcaveFaces(rp, face_list, facecount);
}

// moves a face of a room by a delta amount
void MoveRoomFace(room *rp, int facenum, float dx, float dy, float dz) {
  matrix facematrix;
  vector norm = -rp->faces[facenum].normal;

  int face_list[MAX_FACES_PER_ROOM];
  int portal_list[MAX_VERTS_PER_ROOM];
  int i, t, ok_to_scale, k, touches_portal = 0;
  int num_portal_verts;
  int facecount = 0;
  face *fp = &rp->faces[facenum];

  ASSERT(rp->used > 0);
  ASSERT(facenum >= 0 && facenum < rp->num_faces);

  num_portal_verts = BuildListOfPortalVerts(rp, portal_list);

  vm_VectorToMatrix(&facematrix, &norm, NULL, NULL);

  for (i = 0; i < fp->num_verts; i++) {
    for (ok_to_scale = 1, t = 0; t < num_portal_verts; t++) {
      if (portal_list[t] == fp->face_verts[i]) {
        ok_to_scale = 0;
        touches_portal = 1;
      }
    }

    if (ok_to_scale) {
      rp->verts[fp->face_verts[i]] += (facematrix.rvec * dx);
      rp->verts[fp->face_verts[i]] += (facematrix.uvec * dy);
      rp->verts[fp->face_verts[i]] += (facematrix.fvec * dz);
    }
  }

  // Build a list of faces that share vertices with this edge
  for (i = 0; i < rp->num_faces; i++) {
    if (i == facenum && touches_portal == 0)
      continue;

    for (t = 0; t < rp->faces[i].num_verts; t++) {
      for (k = 0; k < fp->num_verts; k++) {
        if (fp->face_verts[k] == rp->faces[i].face_verts[t]) {
          face_list[facecount] = i;
          facecount++;
        }
      }
    }
  }

  // Now fixup any concave/nonplanar faces
  FixConcaveFaces(rp, face_list, facecount);
}

void CRoomKeypadDialog::OnMarkRoom() { SetMarkedRoom(); }

void CRoomKeypadDialog::OnRoompadRotateLeft() { RotateRooms(0, (angle)-512, 0); }

void CRoomKeypadDialog::OnRoompadRotateRight() { RotateRooms(0, (angle)512, 0); }

void CRoomKeypadDialog::OnRoompadPitchForward() { RotateRooms((angle)512, 0, 0); }

void CRoomKeypadDialog::OnRoompadPitchBackward() { RotateRooms((angle)-512, 0, 0); }

void CRoomKeypadDialog::OnRoompadBankRight() { RotateRooms(0, 0, (angle)512); }

void CRoomKeypadDialog::OnRoompadBankLeft() { RotateRooms(0, 0, (angle)-512); }

void CRoomKeypadDialog::OnRoompadNextEdge() {
  Curedge++;
  if (Curedge >= Curroomp->faces[Curface].num_verts)
    Curedge = 0;

  EditorStatus("Edge %d selected.", Curedge);

  State_changed = 1;
}

void CRoomKeypadDialog::OnRoompadNextVertex() {
  if (++Curvert >= Curroomp->faces[Curface].num_verts)
    Curvert = 0;

  EditorStatus("Vertex %d (%d) selected.", Curvert, Curroomp->faces[Curface].face_verts[Curvert]);

  State_changed = 1;
}

void CRoomKeypadDialog::OnRoompadExpandRoom() {
  if (Curroomp == NULL)
    return;

  SizeRoomVertices(Curroomp, 1.05f);
  World_changed = 1;
}

void CRoomKeypadDialog::OnRoompadContractRoom() {
  if (Curroomp == NULL)
    return;

  SizeRoomVertices(Curroomp, .95f);
  World_changed = 1;
}

void CRoomKeypadDialog::OnRoompadExpandFace() {
  if (Curroomp == NULL || Curface < 0 || Curface >= Curroomp->num_faces)
    return;

  SizeFaceVertices(Curroomp, Curface, 1.05f);
  World_changed = 1;
}

void CRoomKeypadDialog::OnRoompadContractFace() {
  if (Curroomp == NULL || Curface < 0 || Curface >= Curroomp->num_faces)
    return;

  SizeFaceVertices(Curroomp, Curface, .95f);
  World_changed = 1;
}

void CRoomKeypadDialog::OnRoompadExpandEdge() {

  if (Curroomp == NULL || Curface < 0 || Curface >= Curroomp->num_faces)
    return;

  SizeFaceEdge(Curroomp, Curface, Curedge, 1.05f);
  World_changed = 1;
}

void CRoomKeypadDialog::OnRoompadContractEdge() {
  if (Curroomp == NULL || Curface < 0 || Curface >= Curroomp->num_faces)
    return;

  SizeFaceEdge(Curroomp, Curface, Curedge, .95f);
  World_changed = 1;
}

void CRoomKeypadDialog::OnRoompadMoveForward() {
  MoveRoomFace(Curroomp, Curface, 0, 0, 1);
  World_changed = 1;
}

void CRoomKeypadDialog::OnRoompadMoveBackward() {
  MoveRoomFace(Curroomp, Curface, 0, 0, -1);
  World_changed = 1;
}

void CRoomKeypadDialog::OnRoompadMoveRight() {
  MoveRoomFace(Curroomp, Curface, 1, 0, 0);
  World_changed = 1;
}

void CRoomKeypadDialog::OnRoompadMoveLeft() {
  MoveRoomFace(Curroomp, Curface, -1, 0, 0);
  World_changed = 1;
}

void CRoomKeypadDialog::OnRoompadMoveUp() {
  MoveRoomFace(Curroomp, Curface, 0, 1, 0);
  World_changed = 1;
}

void CRoomKeypadDialog::OnRoompadMoveDown() {
  MoveRoomFace(Curroomp, Curface, 0, -1, 0);
  World_changed = 1;
}

void CRoomKeypadDialog::OnRoompadCombineFaces() {
  if (Markedroomp == NULL) {
    OutrageMessageBox("You must have a marked face for this operation.");
    return;
  }

  if (Markedroomp != Curroomp) {
    OutrageMessageBox("The marked face and current face must be in the same room for this operation.");
    return;
  }

  if (Markedface == Curface) {
    OutrageMessageBox("The marked face and current face cannot be the same for this operation.");
    return;
  }

  // Combine the faces
  if (!CombineFaces(Curroomp, Markedface, Curface)) {
    OutrageMessageBox("Cannot combine faces: %s", GetErrorMessage());
    return;
  }

  // Fix marked face, if the number has changed
  if (Curface < Markedface)
    Markedface--;

  // Reset curface, which should no longer exist
  Curface = Markedface;
  Curedge = Curvert = 0;
}

void CRoomKeypadDialog::OnRoompadNextPortal() {
  if (Curroomp->num_portals) {
    Curportal = (Curportal + 1) % Curroomp->num_portals;
    portal *pp = &Curroomp->portals[Curportal];
    Curface = pp->portal_face;
    EditorStatus("Portal %d selected.  Face %d selected.", Curportal, Curface);
    State_changed = 1;
    UpdateDialog();
  }
}

BOOL CRoomKeypadDialog::OnInitDialog() {
  CDialog::OnInitDialog();

  int i;

  // set up reverb dropdown listbox
  CComboBox *cbox = (CComboBox *)GetDlgItem(IDC_ROOM_ENVAUDIO);
  cbox->ResetContent();
  for (i = 0; i < N_ENVAUDIO_PRESETS; i++) {
    cbox->AddString(EnvAudio_PresetNames[i]);
  }

  // Set up damage type dialog
  CComboBox *box = (CComboBox *)GetDlgItem(IDC_ROOMPAD_DAMAGETYPE);
  i = box->AddString("None");
  box->SetItemData(i, PD_NONE);
  i = box->AddString("Energy Weapon");
  box->SetItemData(i, PD_ENERGY_WEAPON);
  i = box->AddString("Matter Weapon");
  box->SetItemData(i, PD_MATTER_WEAPON);
  i = box->AddString("Melee Attach");
  box->SetItemData(i, PD_MELEE_ATTACK);
  i = box->AddString("Concussive Force");
  box->SetItemData(i, PD_CONCUSSIVE_FORCE);
  i = box->AddString("Wall Hit");
  box->SetItemData(i, PD_WALL_HIT);

  return TRUE; // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
}

void CRoomKeypadDialog::UpdateKeypad(int mask) {
  if (mask & (KUF_CURROOM_CHANGED + KUF_CURFACE_CHANGED))
    UpdateDialog();
}

void CRoomKeypadDialog::OnRenderPortalFaces() {
  CButton *btn;
  portal *pp = &Curroomp->portals[Curportal];

  btn = (CButton *)GetDlgItem(IDC_RENDER_PORTAL_FACES);

  if (btn->GetCheck())
    pp->flags |= PF_RENDER_FACES;
  else
    pp->flags &= ~PF_RENDER_FACES;

  World_changed = 1;

  UpdateDialog();
}

void CRoomKeypadDialog::OnEnablePortalFlythrough() {
  CButton *btn;
  portal *pp = &Curroomp->portals[Curportal];

  btn = (CButton *)GetDlgItem(IDC_ENABLE_PORTAL_FLYTHROUGH);

  if (btn->GetCheck())
    pp->flags |= PF_RENDERED_FLYTHROUGH;
  else
    pp->flags &= ~PF_RENDERED_FLYTHROUGH;

  World_changed = 1;
}

void CRoomKeypadDialog::OnExternalRoom() {
  CButton *btn;
  portal *pp = &Curroomp->portals[Curportal];

  btn = (CButton *)GetDlgItem(IDC_EXTERNAL_ROOM);

  if (btn->GetCheck())
    Curroomp->flags |= RF_EXTERNAL;
  else
    Curroomp->flags &= ~RF_EXTERNAL;

  World_changed = 1;

  UpdateDialog();
}

void CRoomKeypadDialog::OnRoompadRefuelingCenter() {
  CButton *btn;

  btn = (CButton *)GetDlgItem(IDC_ROOMPAD_REFUELING_CENTER);

  if (btn->GetCheck())
    Curroomp->flags |= RF_FUELCEN;
  else
    Curroomp->flags &= ~RF_FUELCEN;

  World_changed = 1;

  UpdateDialog();
}

void CRoomKeypadDialog::OnRoompadGoal1() {
  CButton *btn;

  btn = (CButton *)GetDlgItem(IDC_ROOMPAD_GOAL1);

  if (btn->GetCheck())
    Curroomp->flags |= RF_GOAL1;
  else
    Curroomp->flags &= ~RF_GOAL1;

  World_changed = 1;

  UpdateDialog();
}

void CRoomKeypadDialog::OnRoompadGoal2() {
  CButton *btn;

  btn = (CButton *)GetDlgItem(IDC_ROOMPAD_GOAL2);

  if (btn->GetCheck())
    Curroomp->flags |= RF_GOAL2;
  else
    Curroomp->flags &= ~RF_GOAL2;

  World_changed = 1;

  UpdateDialog();
}

void CRoomKeypadDialog::OnRenderExternalRooms() {
  CButton *btn;

  btn = (CButton *)GetDlgItem(IDC_RENDER_EXTERNAL_ROOMS);

  if (btn->GetCheck())
    Render_all_external_rooms = 1;
  else
    Render_all_external_rooms = 0;

  State_changed = 1;

  UpdateDialog();
}

void CRoomKeypadDialog::OnNextFace() {
  if (++Curface >= Curroomp->num_faces)
    Curface = 0;

  EditorStatus("Face %d selected.", Curface);
  UpdateDialog();

  State_changed = 1;
}

void CRoomKeypadDialog::OnTouchesOutside() {
  CButton *btn;

  btn = (CButton *)GetDlgItem(IDC_TOUCHES_OUTSIDE);

  if (btn->GetCheck())
    Curroomp->flags |= RF_TOUCHES_TERRAIN;
  else
    Curroomp->flags &= ~RF_TOUCHES_TERRAIN;

  World_changed = 1;

  UpdateDialog();
}

void CRoomKeypadDialog::OnRoompadGoal3() {
  CButton *btn;

  btn = (CButton *)GetDlgItem(IDC_ROOMPAD_GOAL3);

  if (btn->GetCheck())
    Curroomp->flags |= RF_GOAL3;
  else
    Curroomp->flags &= ~RF_GOAL3;

  World_changed = 1;

  UpdateDialog();
}

void CRoomKeypadDialog::OnRoompadGoal4() {
  CButton *btn;

  btn = (CButton *)GetDlgItem(IDC_ROOMPAD_GOAL4);

  if (btn->GetCheck())
    Curroomp->flags |= RF_GOAL4;
  else
    Curroomp->flags &= ~RF_GOAL4;

  World_changed = 1;

  UpdateDialog();
}

void CRoomKeypadDialog::OnGoalfaceCheck() {
  CButton *btn;

  btn = (CButton *)GetDlgItem(IDC_GOALFACE_CHECK);

  if (btn->GetCheck()) {
    if (!(Curroomp->flags & GOALROOM | RF_SPECIAL1 | RF_SPECIAL2 | RF_SPECIAL3 | RF_SPECIAL4 | RF_SPECIAL5 |
          RF_SPECIAL6)) {
      OutrageMessageBox("This room needs to marked as a goal first.");
    } else
      Curroomp->faces[Curface].flags |= FF_GOALFACE;
  } else
    Curroomp->faces[Curface].flags &= ~FF_GOALFACE;

  World_changed = 1;

  UpdateDialog();
}

void CRoomKeypadDialog::OnSpecial1() {
  CButton *btn;

  btn = (CButton *)GetDlgItem(IDC_SPECIAL_1);

  if (btn->GetCheck())
    Curroomp->flags |= RF_SPECIAL1;
  else
    Curroomp->flags &= ~RF_SPECIAL1;

  World_changed = 1;

  UpdateDialog();
}

void CRoomKeypadDialog::OnSpecial2() {
  CButton *btn;

  btn = (CButton *)GetDlgItem(IDC_SPECIAL_2);

  if (btn->GetCheck())
    Curroomp->flags |= RF_SPECIAL2;
  else
    Curroomp->flags &= ~RF_SPECIAL2;

  World_changed = 1;

  UpdateDialog();
}

void CRoomKeypadDialog::OnSpecial3() {
  CButton *btn;

  btn = (CButton *)GetDlgItem(IDC_SPECIAL_3);

  if (btn->GetCheck())
    Curroomp->flags |= RF_SPECIAL3;
  else
    Curroomp->flags &= ~RF_SPECIAL3;

  World_changed = 1;

  UpdateDialog();
}

void CRoomKeypadDialog::OnSpecial4() {
  CButton *btn;

  btn = (CButton *)GetDlgItem(IDC_SPECIAL_4);

  if (btn->GetCheck())
    Curroomp->flags |= RF_SPECIAL4;
  else
    Curroomp->flags &= ~RF_SPECIAL4;

  World_changed = 1;

  UpdateDialog();
}

void CRoomKeypadDialog::OnSpecial5() {
  CButton *btn;

  btn = (CButton *)GetDlgItem(IDC_SPECIAL_5);

  if (btn->GetCheck())
    Curroomp->flags |= RF_SPECIAL5;
  else
    Curroomp->flags &= ~RF_SPECIAL5;

  World_changed = 1;

  UpdateDialog();
}

void CRoomKeypadDialog::OnSpecial6() {
  CButton *btn;

  btn = (CButton *)GetDlgItem(IDC_SPECIAL_6);

  if (btn->GetCheck())
    Curroomp->flags |= RF_SPECIAL6;
  else
    Curroomp->flags &= ~RF_SPECIAL6;

  World_changed = 1;

  UpdateDialog();
}

void CRoomKeypadDialog::OnBoaVisButton() {
  int answer = MessageBox("Do you wish compute the BOA vis table?", "Question", MB_YESNO);

  if (answer == IDNO)
    return;

  MakeBOAVisTable();
}

void CRoomKeypadDialog::OnSelendokRoomAmbient() {
  int cur;
  char name[200];

  cur = SendDlgItemMessage(IDC_ROOM_AMBIENT, CB_GETCURSEL, 0, 0);
  SendDlgItemMessage(IDC_ROOM_AMBIENT, CB_GETLBTEXT, cur, (LPARAM)(LPCTSTR)name);

  Curroomp->ambient_sound = FindAmbientSoundPattern(name);

  World_changed = 1;
}

void CRoomKeypadDialog::OnMirrorButton() {
  if (Curroomp == NULL)
    return;
  Curroomp->mirror_face = Curface;
  ConsolidateMineMirrors();
  UpdateDialog();
}

void CRoomKeypadDialog::OnNoMirrorButton() {
  if (Curroomp == NULL)
    return;
  Curroomp->mirror_face = -1;
  ConsolidateMineMirrors();
  UpdateDialog();
}

void CRoomKeypadDialog::OnRoompadFlipFace() {
  if (Curroomp->faces[Curface].portal_num != -1) {
    OutrageMessageBox("You cannot flip a portal face.");
    return;
  }

  if (OutrageMessageBox(MBOX_YESNO, "Are you sure you want to flip the current face?") != IDYES)
    return;

  // Flip the face
  FlipFace(Curroomp, Curface);
}

void CRoomKeypadDialog::OnRoompadNoDrawTerrain() {
  CButton *btn = (CButton *)GetDlgItem(IDC_ROOMPAD_NO_DRAW_TERRAIN);

  Render_inside_only = (btn->GetCheck() != 0);

  State_changed = 1;

  UpdateDialog();
}

void CRoomKeypadDialog::OnRoompadOneRoomOnly() {
  CButton *btn = (CButton *)GetDlgItem(IDC_ROOMPAD_ONE_ROOM_ONLY);

  Render_one_room_only = (btn->GetCheck() != 0);

  State_changed = 1;

  UpdateDialog();
}

void CRoomKeypadDialog::OnTriangulate() {
  CButton *btn;

  btn = (CButton *)GetDlgItem(IDC_TRIANGULATE);

  if (btn->GetCheck())
    Curroomp->flags |= RF_TRIANGULATE;
  else
    Curroomp->flags &= ~RF_TRIANGULATE;

  World_changed = 1;

  UpdateDialog();
}

void CRoomKeypadDialog::OnEditName() {
  extern void RenameRoom();

  RenameRoom();

  UpdateDialog();
}

///////////////////////////////////////////////////////////////////////

void CRoomKeypadDialog::OnSelchangeRoomEnvaudio() {
  CComboBox *cbox = (CComboBox *)GetDlgItem(IDC_ROOM_ENVAUDIO);
  CString selection;
  int i;

  cbox->GetLBText(cbox->GetCurSel(), selection);

  for (i = 0; i < N_ENVAUDIO_PRESETS; i++) {
    if (selection == EnvAudio_PresetNames[i]) {
      Curroomp->env_reverb = i;
      break;
    }
  }
}

void CRoomKeypadDialog::OnKillfocusRoompadDamage() {
  char buf[128];

  CEdit *box = (CEdit *)GetDlgItem(IDC_ROOMPAD_DAMAGE);

  box->GetWindowText(buf, sizeof(buf));

  Curroomp->damage = atof(buf);

  World_changed = 1;

  UpdateDialog();
}

void CRoomKeypadDialog::OnSelchangeRoompadDamageType() {
  CComboBox *box = (CComboBox *)GetDlgItem(IDC_ROOMPAD_DAMAGETYPE);

  Curroomp->damage_type = (int8_t)box->GetItemData(box->GetCurSel());
}

void CRoomKeypadDialog::OnRenderShell() {
  Shell_render_flag ^= SRF_NO_SHELL;

  State_changed = 1;

  UpdateDialog();
}

void CRoomKeypadDialog::OnRenderNonShell() {
  Shell_render_flag ^= SRF_NO_NON_SHELL;

  State_changed = 1;

  UpdateDialog();
}

void CRoomKeypadDialog::OnSecretCheck() {
  CButton *btn;

  btn = (CButton *)GetDlgItem(IDC_SECRET_CHECK);

  if (btn->GetCheck())
    Curroomp->flags |= RF_SECRET;
  else
    Curroomp->flags &= ~RF_SECRET;

  World_changed = 1;

  UpdateDialog();
}

void CRoomKeypadDialog::OnPortalBlockedCheck() {
  CButton *btn;
  portal *pp = &Curroomp->portals[Curportal];

  btn = (CButton *)GetDlgItem(IDC_PORTAL_BLOCKED_CHECK);

  portal *np = &Rooms[Curroomp->portals[Curportal].croom].portals[Curroomp->portals[Curportal].cportal];

  if (btn->GetCheck()) {
    pp->flags |= PF_BLOCK;
    np->flags |= PF_BLOCK;
  } else {
    pp->flags &= ~PF_BLOCK;
    np->flags &= ~PF_BLOCK;
  }

  World_changed = 1;
}

void CRoomKeypadDialog::OnPortalBlockedCanGoAwayCheck() {
  CButton *btn;
  portal *pp = &Curroomp->portals[Curportal];

  btn = (CButton *)GetDlgItem(IDC_PORTAL_BLOCKED_CAN_GO_AWAY_CHECK);

  if (btn->GetCheck())
    pp->flags |= PF_BLOCK_REMOVABLE;
  else
    pp->flags &= ~PF_BLOCK_REMOVABLE;

  World_changed = 1;
}

void CRoomKeypadDialog::OnBadCenterpoints() {
  char message[10000];
  message[0] = '\0';

  mprintf(0, "Computing bad centers\n");
  BOA_ComputePathPoints(message, 10000);
  mprintf(0, "Done computing bad centers... List in clipboard.\n");
  DumpTextToClipboard(message);
}

void CRoomKeypadDialog::OnRoomSkipLighting() {
  CButton *btn = (CButton *)GetDlgItem(IDC_ROOM_SKIP_LIGHTING);

  if (btn->GetCheck()) {
    Curroomp->flags |= RF_NO_LIGHT;
  } else {
    Curroomp->flags &= ~RF_NO_LIGHT;
  }

  World_changed = 1;
  UpdateDialog();
}
