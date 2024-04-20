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
 * $Logfile: /DescentIII/Main/editor/DoorwayDialog.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:37 $
 * $Author: kevinb $
 *
 * Doorway keypad
 *
 * $Log: not supported by cvs2svn $
 * 
 * 23    5/05/99 2:02a Matt
 * Added doorway flag to tell the Guide-Bot to ignore the locked state of
 * the door.
 * 
 * 22    4/03/99 7:55p Matt
 * Use new keypad update system instead of timer to update.
 * 
 * 21    3/05/99 1:51p Kevin
 * Fixed small crash bug
 * 
 * 20    3/01/99 10:37a Matt
 * Update doorway tab when the door object is renamed
 * 
 * 19    2/06/99 10:03p Matt
 * Added keys system
 * 
 * 18    2/04/99 2:05p Matt
 * Added blastable doors
 * 
 * 17    1/29/99 12:48p Matt
 * Rewrote the doorway system
 * 
 * 16    9/22/98 12:02p Matt
 * Fixed compile warnings
 * 
 * 15    9/08/98 12:06p Jason
 * added automatic reinitting of doors when rooms get deleted
 * 
 * 14    9/02/98 4:32p Matt
 * Added some error checking when placing a door.
 * 
 * 13    6/15/98 4:00p Jason
 * replaced monochromatic polymodel lighting with rgb lighting
 * 
 * 12    4/20/98 2:52p Jason
 * added doorway reinit function
 * 
 * 11    4/07/98 11:05a Kevin
 * For Samir, added m_Active to the if statement in the timer interrupt
 * 
 * 10    4/02/98 3:54p Jason
 * first pass in getting polymodel paging to work
 * 
 * 9     12/19/97 11:25a Samir
 * g3_StartFrame and g3_EndFrame replaced by EditorStartFrame and
 * EditorEndFrame
 * 
 * 8     10/03/97 12:24p Jason
 * added new keys for doorways
 * 
 * 7     9/29/97 12:09p Jason
 * added functionality to doorway system
 * 
 * 6     9/23/97 12:29p Jason
 * fixed m_Active bug
 * 
 * 5     9/17/97 1:01p Matt
 * Ripped out segment code
 * 
 * 4     9/11/97 5:46p Jason
 * first pass at getting doors to work with room engine
 * 
 * 3     7/24/97 6:11p Matt
 * Created symbolic constant for default zoom, and used it every place
 * that specifies zoom
 * 
 * 20    6/03/97 4:55p Mark
 * 
 *	20	6/03/97	4:51p	Jeff
 *	Added Context Sensitive Help
 *
 * 19    4/02/97 5:15p Samir
 * Added a Opened checkbox to mark a door as open or closed initially
 * 
 * 18    4/01/97 11:00p Matt
 * Changed editor to keep a viewer object (type camera) seperate from the
 * player object.  This camera, and not the player, is now moved by
 * slewing, the C key, etc.  When going into the game, the viewer position
 * & orientation are copied to the player.  When going back to the editor,
 * the player position is copied to the viewer, and the player object is
 * reset to its start location.
 * 
 * 17    3/31/97 5:57p Matt
 * Revamped mine update flags
 * 
 * 16    3/26/97 5:43p Samir
 * Fixed refresh of doorway selector when changing state of door page.
 * 
 * 15    3/25/97 6:31p Samir
 * Now you can select from different 3d doors hopefully.
 * 
 * 14    3/24/97 6:47p Samir
 * Fixed some place,remove  doorway probs with 3d doors.  Made some state
 * vars static for multiple instances of dialog.
 * 
 * 13    3/20/97 12:10p Samir
 * Fixed prob when going into fullscreen with doorway keypad visible.
 * 
 * 12    3/19/97 7:02p Samir
 * If adding door segment fails, don't register it as a door! And
 * deactivated moving doorways from one seg to another for now.
 * 
 * 11    3/18/97 11:35a Samir
 * Fixed user interface problems.
 *
 * $NoKeywords: $
 */

// DoorwayDialog.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "DoorwayDialog.h"

#include "door.h"
#include "HView.h"
#include "HRoom.h"
#include "polymodel.h"
#include "edoors.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int Current_door_room=-1;

/////////////////////////////////////////////////////////////////////////////
// CDoorwayDialog dialog


CDoorwayDialog::CDoorwayDialog(CWnd* pParent /*=NULL*/)
	: CKeypadDialog(CDoorwayDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDoorwayDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

//	m_SelectionType = DOORWAY_DOOR;
}


void CDoorwayDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDoorwayDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDoorwayDialog, CDialog)
	//{{AFX_MSG_MAP(CDoorwayDialog)
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_DOORWAY_PLACEDOOR, OnDoorwayPlaceDoorway)
	ON_BN_CLICKED(IDC_NEXT_DOOR, OnNextDoor)
	ON_BN_CLICKED(IDC_PREV_DOOR, OnPrevDoor)
	ON_BN_CLICKED(IDC_DOORWAY_LOCKED, OnDoorwayLocked)
	ON_BN_CLICKED(IDC_KEY1_CHECK, OnKey1Check)
	ON_BN_CLICKED(IDC_KEY2_CHECK, OnKey2Check)
	ON_BN_CLICKED(IDC_KEY3_CHECK, OnKey3Check)
	ON_BN_CLICKED(IDC_KEY4_CHECK, OnKey4Check)
	ON_BN_CLICKED(IDC_KEY5_CHECK, OnKey5Check)
	ON_BN_CLICKED(IDC_KEY6_CHECK, OnKey6Check)
	ON_BN_CLICKED(IDC_KEY7_CHECK, OnKey7Check)
	ON_BN_CLICKED(IDC_KEY8_CHECK, OnKey8Check)
	ON_BN_CLICKED(IDC_DOORWAY_ATTACHDOOR, OnDoorwayAttachDoor)
	ON_BN_CLICKED(IDC_DOORWAY_AUTO, OnDoorwayAuto)
	ON_EN_KILLFOCUS(IDC_DOORWAY_POS_EDIT, OnKillfocusDoorwayPosEdit)
	ON_EN_KILLFOCUS(IDC_DOORWAY_HITPOINT_EDIT, OnKillfocusDoorwayHitpointEdit)
	ON_BN_CLICKED(IDC_DOORWAY_KEY_ALL, OnDoorwayKeyAll)
	ON_BN_CLICKED(IDC_DOORWAY_KEY_ONLY_ONE, OnDoorwayKeyOnlyOne)
	ON_BN_CLICKED(IDC_DOORWAY_GB_IGNORE_LOCKED, OnDoorwayGBIgnoreLocked)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDoorwayDialog message handlers

void CDoorwayDialog::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	
	CKeypadDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}


void CDoorwayDialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	
	CKeypadDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CDoorwayDialog::OnSize(UINT nType, int cx, int cy) 
{
	CKeypadDialog::OnSize(nType, cx, cy);

	
}


void CDoorwayDialog::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	UpdateDialog();	
}


BOOL CDoorwayDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	UpdateDialog();
	return TRUE;
}


void CDoorwayDialog::OnOK()
{
	CWnd *focus = CWnd::GetFocus();
	if (focus) focus->GetNextWindow()->SetFocus();
}

void CDoorwayDialog::EnableCurrentItems(bool state)
{
	((CButton *) GetDlgItem(IDC_DOORWAY_ID))->EnableWindow(state);
	((CButton *) GetDlgItem(IDC_DOORWAY_LOCKED))->EnableWindow(state);
	((CButton *) GetDlgItem(IDC_DOORWAY_GB_IGNORE_LOCKED))->EnableWindow(state);
	((CButton *) GetDlgItem(IDC_DOORWAY_AUTO))->EnableWindow(state);
	((CButton *) GetDlgItem(IDC_DOORWAY_CURRENT_BOX))->EnableWindow(state);
	((CButton *) GetDlgItem(IDC_DOORWAY_POS_TEXT))->EnableWindow(state);
	((CButton *) GetDlgItem(IDC_DOORWAY_POS_EDIT))->EnableWindow(state);
	((CButton *) GetDlgItem(IDC_DOORWAY_HITPOINT_TEXT))->EnableWindow(state);
	((CButton *) GetDlgItem(IDC_DOORWAY_HITPOINT_EDIT))->EnableWindow(state);
	((CButton *) GetDlgItem(IDC_DOORWAY_KEYS_TEXT))->EnableWindow(state);
	((CButton *) GetDlgItem(IDC_DOORWAY_KEY_ALL))->EnableWindow(state);
	((CButton *) GetDlgItem(IDC_DOORWAY_KEY_ONLY_ONE))->EnableWindow(state);
	((CButton *) GetDlgItem(IDC_KEY1_CHECK))->EnableWindow(state);
	((CButton *) GetDlgItem(IDC_KEY2_CHECK))->EnableWindow(state);
	((CButton *) GetDlgItem(IDC_KEY3_CHECK))->EnableWindow(state);
	((CButton *) GetDlgItem(IDC_KEY4_CHECK))->EnableWindow(state);
	((CButton *) GetDlgItem(IDC_KEY5_CHECK))->EnableWindow(state);
	((CButton *) GetDlgItem(IDC_KEY6_CHECK))->EnableWindow(state);
	((CButton *) GetDlgItem(IDC_KEY7_CHECK))->EnableWindow(state);
	((CButton *) GetDlgItem(IDC_KEY8_CHECK))->EnableWindow(state);
}


//	---------------------------------------------------------------------------
//	Update doorway key functions functions
//	---------------------------------------------------------------------------

char last_name[OBJ_NAME_LEN +1];

void CDoorwayDialog::UpdateDialog()
{
	if (!m_Active) return;

	if (Num_doors<=0)
		return;
	if (Doors[D3EditState.current_door].used==0)
		D3EditState.current_door=GetNextDoor (D3EditState.current_door);

	int door_handle=D3EditState.current_door;
	
	CEdit *ebox=(CEdit *) GetDlgItem (IDC_DOORWAYSELEDIT);
	ebox->SetWindowText (Doors[door_handle].name);
	DrawDoorwayDoorPic();

	((CButton *) GetDlgItem(IDC_DOORWAY_ATTACHDOOR))->EnableWindow(Placed_door != -1);

	ebox=(CEdit *) GetDlgItem (IDC_DOORWAY_ID);

	if (Cur_object_index==-1 || Objects[Cur_object_index].type!=OBJ_DOOR)
	{
		ebox->SetWindowText ("No doorway selected");
		Current_door_room = -1;
		EnableCurrentItems(0);
	}
	else
	{
		char str[250];
		object *objp = &Objects[Cur_object_index];

		EnableCurrentItems(1);

		Current_door_room = objp->roomnum;

		ASSERT(Rooms[Current_door_room].flags & RF_DOOR);

		strcpy(last_name,Objects[Cur_object_index].name?Objects[Cur_object_index].name:"<no name>");
		sprintf (str,"Door:  %s        ",last_name);
		ebox->SetWindowText (str);

		doorway *dw = Rooms[Current_door_room].doorway_data;

		sprintf(str,"%.2f",dw->position);
		ebox=(CEdit *) GetDlgItem(IDC_DOORWAY_POS_EDIT);
  		ebox->SetWindowText(str);

		//Update key check boxes
		CButton *bbox = (CButton *)GetDlgItem(IDC_DOORWAY_LOCKED);
  		bbox->SetCheck(dw->flags & DF_LOCKED);

		bbox = (CButton *)GetDlgItem(IDC_DOORWAY_AUTO);
  		bbox->SetCheck(dw->flags & DF_AUTO);

		bbox = (CButton *)GetDlgItem(IDC_DOORWAY_GB_IGNORE_LOCKED);
  		bbox->SetCheck(dw->flags & DF_GB_IGNORE_LOCKED);

		bbox = (CButton *)GetDlgItem(IDC_KEY1_CHECK);
  		bbox->SetCheck(dw->keys_needed & KEY_FLAG(1));

		bbox = (CButton *)GetDlgItem(IDC_KEY2_CHECK);
  		bbox->SetCheck(dw->keys_needed & KEY_FLAG(2));

		bbox = (CButton *)GetDlgItem(IDC_KEY3_CHECK);
  		bbox->SetCheck(dw->keys_needed & KEY_FLAG(3));

		bbox = (CButton *)GetDlgItem(IDC_KEY4_CHECK);
  		bbox->SetCheck(dw->keys_needed & KEY_FLAG(4));

		bbox = (CButton *)GetDlgItem(IDC_KEY5_CHECK);
  		bbox->SetCheck(dw->keys_needed & KEY_FLAG(5));

		bbox = (CButton *)GetDlgItem(IDC_KEY6_CHECK);
  		bbox->SetCheck(dw->keys_needed & KEY_FLAG(6));

		bbox = (CButton *)GetDlgItem(IDC_KEY7_CHECK);
  		bbox->SetCheck(dw->keys_needed & KEY_FLAG(7));

		bbox = (CButton *)GetDlgItem(IDC_KEY8_CHECK);
  		bbox->SetCheck(dw->keys_needed & KEY_FLAG(8));

		//Update key radio buttons
		bbox = (CButton *)GetDlgItem(IDC_DOORWAY_KEY_ALL);
  		bbox->SetCheck(! (dw->flags & DF_KEY_ONLY_ONE));
		bbox = (CButton *)GetDlgItem(IDC_DOORWAY_KEY_ONLY_ONE);
  		bbox->SetCheck(dw->flags & DF_KEY_ONLY_ONE);

		//Update hitpoints editing
		door *door = &Doors[dw->doornum];
		bool blastable = ((door->flags & DF_BLASTABLE) != 0);
		((CButton *) GetDlgItem(IDC_DOORWAY_HITPOINT_TEXT))->EnableWindow(blastable);
		sprintf(str,"%d",(int) objp->shields);
		ebox=(CEdit *) GetDlgItem(IDC_DOORWAY_HITPOINT_EDIT);
 		ebox->SetWindowText(str);
		ebox->EnableWindow(blastable);
	}
}


//	update doorway picture

void CDoorwayDialog::DrawDoorwayDoorPic()
{
	int door_handle=D3EditState.current_door;
	vector zero_vector;
	vector view_vector={0,0,-20};
	matrix id_matrix;
	float norm_angles[30];
	int w, h, bm_handle, x, y;
	RECT rect;
	CWnd *wnd = GetDlgItem(IDC_DOORPIC);

	Desktop_surf->attach_to_window((unsigned)wnd->m_hWnd);

	wnd->GetWindowRect(&rect);
	ScreenToClient(&rect);

	w=rect.right-rect.left;
	h=rect.bottom-rect.top;

	m_DoorSurf.create(w, h, BPP_16);

	Desktop_surf->clear(0,0,w,h);
	
	bm_handle=GetDoorImage (door_handle);

	poly_model *pm=GetPolymodelPointer (bm_handle);

	vm_MakeZero (&zero_vector);
	vm_MakeIdentity (&id_matrix);

	for (int i=0;i<30;i++)
		norm_angles[i]=0;

	grViewport *vport=new grViewport (&m_DoorSurf);
	StartEditorFrame (vport,&view_vector,&id_matrix,D3_DEFAULT_ZOOM);
	DrawPolygonModel (&zero_vector,&id_matrix,bm_handle,norm_angles,0,1.0,1.0,1.0);
	EndEditorFrame();

	x = rect.left + ((rect.right-rect.left)/2) - m_DoorSurf.width()/2; 
	y = rect.top + ((rect.bottom-rect.top)/2) - m_DoorSurf.height()/2;
	Desktop_surf->blt(0, 0, &m_DoorSurf);

	delete vport;
	m_DoorSurf.free();
	Desktop_surf->attach_to_window((unsigned)NULL);	
}



//	---------------------------------------------------------------------------
//	Doorway editing functions
//	---------------------------------------------------------------------------

void CDoorwayDialog::OnDoorwayPlaceDoorway() 
{
	if (Num_doors<=0)
		return;

	if (D3EditState.current_door == -1) {
		OutrageMessageBox("You must have a current door for this operation");
		return;
	}

	if (Curroomp->faces[Curface].portal_num != -1) {
		OutrageMessageBox("There's already a connection at the current room:face.");
		return;
	}

	PlaceDoor(Curroomp,Curface,D3EditState.current_door);

	UpdateDialog();

	World_changed = 1;
}

/*BOOL CDoorwayDialog::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	// TODO: Add your message handler code here and/or call default
	WinHelp(HID_DOORWAYTAB,HELP_CONTEXT);
	return TRUE;
	//return CDialog::OnHelpInfo(pHelpInfo);
}*/

void CDoorwayDialog::OnNextDoor() 
{
	if (Num_doors<=0)
		return;

	D3EditState.current_door=GetNextDoor (D3EditState.current_door);
	UpdateDialog();
}

void CDoorwayDialog::OnPrevDoor() 
{
	if (Num_doors<=0)
		return;

	D3EditState.current_door=GetPrevDoor (D3EditState.current_door);
	UpdateDialog();

	
}

void CDoorwayDialog::OnDoorwayLocked() 
{
	if (Current_door_room == -1)
		return;

	doorway *dp = Rooms[Current_door_room].doorway_data;

	ASSERT(dp != NULL);

	int c=IsDlgButtonChecked(IDC_DOORWAY_LOCKED);

	if (c)
		dp->flags|=DF_LOCKED;
	else
		dp->flags&=~DF_LOCKED;
}

void CDoorwayDialog::UpdateKeypad(int mask)
{
	if (mask & (KUF_CUROBJ_CHANGED+KUF_CUROBJ_NAME_CHANGED))
		UpdateDialog();
}

void CDoorwayDialog::OnKeyCheck(int id,int keynum)
{
	if (Current_door_room == -1)
		return;

	doorway *dp = Rooms[Current_door_room].doorway_data;

	ASSERT(dp != NULL);

	if (IsDlgButtonChecked(id))
		dp->keys_needed |= KEY_FLAG(keynum);
	else
		dp->keys_needed &= ~KEY_FLAG(keynum);
}

void CDoorwayDialog::OnKey1Check() 
{
	OnKeyCheck(IDC_KEY1_CHECK,1);
}

void CDoorwayDialog::OnKey2Check() 
{
	OnKeyCheck(IDC_KEY2_CHECK,2);
}

void CDoorwayDialog::OnKey3Check() 
{
	OnKeyCheck(IDC_KEY3_CHECK,3);
}

void CDoorwayDialog::OnKey4Check() 
{
	OnKeyCheck(IDC_KEY4_CHECK,4);
}

void CDoorwayDialog::OnKey5Check() 
{
	OnKeyCheck(IDC_KEY5_CHECK,5);
}

void CDoorwayDialog::OnKey6Check() 
{
	OnKeyCheck(IDC_KEY6_CHECK,6);
}

void CDoorwayDialog::OnKey7Check() 
{
	OnKeyCheck(IDC_KEY7_CHECK,7);
}

void CDoorwayDialog::OnKey8Check() 
{
	OnKeyCheck(IDC_KEY8_CHECK,8);
}


void CDoorwayDialog::OnDoorwayAttachDoor() 
{
	ASSERT(Placed_door != -1);

	AttachRoom();

	UpdateDialog();
}

void CDoorwayDialog::OnDoorwayAuto() 
{
	if (Current_door_room == -1)
		return;

	doorway *dp = Rooms[Current_door_room].doorway_data;

	ASSERT(dp != NULL);

	int c=IsDlgButtonChecked(IDC_DOORWAY_AUTO);

	if (c)
		dp->flags |= DF_AUTO;
	else
		dp->flags &= ~DF_AUTO;
}

void CDoorwayDialog::OnKillfocusDoorwayPosEdit() 
{
	if (Current_door_room == -1)
		return;

	doorway *dp = Rooms[Current_door_room].doorway_data;

	ASSERT(dp != NULL);

	char str[100];

	CEdit *ebox=(CEdit *) GetDlgItem(IDC_DOORWAY_POS_EDIT);
	ebox->GetWindowText(str,sizeof(str));

	dp->position = dp->dest_pos = atof(str);

	if (dp->position < 0.0)
		dp->position = 0.0;
	if (dp->position > 1.0)
		dp->position = 1.0;

	dp->dest_pos = dp->position;

	DoorwayUpdateAnimation(&Rooms[Current_door_room]);

	World_changed = 1;
}

void CDoorwayDialog::OnKillfocusDoorwayHitpointEdit() 
{
	if (Current_door_room == -1)
		return;

	doorway *dp = Rooms[Current_door_room].doorway_data;

	ASSERT(dp != NULL);

	char str[100];

	CEdit *ebox=(CEdit *) GetDlgItem(IDC_DOORWAY_HITPOINT_EDIT);
	ebox->GetWindowText(str,sizeof(str));

	ASSERT(Objects[Cur_object_index].type == OBJ_DOOR);

	Objects[Cur_object_index].shields = atof(str);

	World_changed = 1;
}

void CDoorwayDialog::OnDoorwayKeyAll() 
{
	if (Current_door_room == -1)
		return;

	doorway *dp = Rooms[Current_door_room].doorway_data;

	ASSERT(dp != NULL);

	if (IsDlgButtonChecked(IDC_DOORWAY_KEY_ALL))
		dp->flags &= ~DF_KEY_ONLY_ONE;
	else
		dp->flags |= DF_KEY_ONLY_ONE;

	World_changed = 1;
	
	UpdateDialog();
}

void CDoorwayDialog::OnDoorwayKeyOnlyOne() 
{
	if (Current_door_room == -1)
		return;

	doorway *dp = Rooms[Current_door_room].doorway_data;

	ASSERT(dp != NULL);

	if (IsDlgButtonChecked(IDC_DOORWAY_KEY_ONLY_ONE))
		dp->flags |= DF_KEY_ONLY_ONE;
	else
		dp->flags &= ~DF_KEY_ONLY_ONE;

	World_changed = 1;
	
	UpdateDialog();
}

void CDoorwayDialog::OnDoorwayGBIgnoreLocked() 
{
	if (Current_door_room == -1)
		return;

	doorway *dp = Rooms[Current_door_room].doorway_data;

	ASSERT(dp != NULL);

	int c=IsDlgButtonChecked(IDC_DOORWAY_GB_IGNORE_LOCKED);

	if (c)
		dp->flags |= DF_GB_IGNORE_LOCKED;
	else
		dp->flags &= ~DF_GB_IGNORE_LOCKED;
}

